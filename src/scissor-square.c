/* Copyright (C) 2017 Ben Asselstine
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <argp.h>
#include <string.h>
#include <gmp.h>
#include <unistd.h>
#include "magicsquareutil.h"

struct fv_app_scissor_square_t
{
  FILE *infile;
  int num_args;
  void (*display_square) (mpz_t s[3][3], FILE *out);
  int (*read_tuple) (FILE *, mpz_t *, char **, size_t *);
  mpz_t match[3];
  int no_warbird;
};


static int
has_same_diff (mpz_t *a, mpz_t *b)
{
  mpz_t diff1, diff2;
  mpz_inits (diff1, diff2, NULL);
  mpz_sub (diff1, a[1], a[0]);
  mpz_sub (diff2, b[1], b[0]);
  int ret = mpz_cmp (diff1, diff2) == 0;
  //it might still be okay if they are divisible
  if (!ret)
    {
      if (mpz_cmp (diff1, diff2) > 0)
        ret = mpz_divisible_p (diff1, diff2);
      else
        ret = mpz_divisible_p (diff2, diff1);
    }
  mpz_clears (diff1, diff2, NULL);
  return ret;
}

static void
multiply_square (mpz_t sq[3][3], int mult)
{
  mpz_mul_ui (sq[1][0], sq[1][0], mult);
  mpz_mul_ui (sq[0][2], sq[0][2], mult);
  mpz_mul_ui (sq[2][1], sq[2][1], mult);
  mpz_mul_ui (sq[1][2], sq[1][2], mult);
  mpz_mul_ui (sq[2][0], sq[2][0], mult);
  mpz_mul_ui (sq[0][1], sq[0][1], mult);
  mpz_mul_ui (sq[1][1], sq[1][1], mult);
  return;
}

static int
generate_warbird_square1 (mpz_t sq[3][3], mpz_t *a, mpz_t *b)
{
  if (!has_same_diff (a, b))
    return 0;

  mpz_set (sq[2][0], a[0]);
  mpz_set (sq[1][1], a[1]);
  mpz_set (sq[0][2], a[2]);
  mpz_set (sq[1][2], b[0]);
  mpz_set (sq[0][0], b[1]);
  mpz_set (sq[2][1], b[2]);
  // +------+------+------+
  // |  X2  |      |  N3  |
  // +------+------+------+
  // |      |  N2  |  X1  |
  // +------+------+------+
  // |  N1  |  X3  |      |
  // +------+------+------+

  int cm = mpz_cmp (a[1], b[1]);
  if (cm == 0)
    return 0;

  mpz_t sum;
  mpz_init (sum);
  mpz_add (sum, sq[2][0], sq[1][1]);
  mpz_add (sum, sum, sq[0][2]);

  mpz_sub (sq[2][2], sum, sq[2][0]);
  mpz_sub (sq[2][2], sq[2][2], sq[2][1]);

  mpz_sub (sq[0][1], sum, sq[0][0]);
  mpz_sub (sq[0][1], sq[0][1], sq[0][2]);

  mpz_sub (sq[1][0], sum, sq[1][1]);
  mpz_sub (sq[1][0], sq[1][0], sq[1][2]);

  mpz_clear (sum);
  //if (is_magic_square (sq, 1))
   // check_diff_divisors (a, b, sq);
  return 1;
}

static int
generate_warbird_square2 (mpz_t sq[3][3], mpz_t *a, mpz_t *b)
{
  if (!has_same_diff (a, b))
    return 0;

  mpz_set (sq[2][0], b[0]);
  mpz_set (sq[1][1], b[1]);
  mpz_set (sq[0][2], b[2]);
  mpz_set (sq[1][2], a[0]);
  mpz_set (sq[0][0], a[1]);
  mpz_set (sq[2][1], a[2]);
  // +------+------+------+
  // |  X2  |      |  N3  |
  // +------+------+------+
  // |      |  N2  |  X1  |
  // +------+------+------+
  // |  N1  |  X3  |      |
  // +------+------+------+

  int cm = mpz_cmp (a[1], b[1]);
  if (cm == 0)
    return 0;

  mpz_t sum;
  mpz_init (sum);
  mpz_add (sum, sq[2][0], sq[1][1]);
  mpz_add (sum, sum, sq[0][2]);

  mpz_sub (sq[2][2], sum, sq[2][0]);
  mpz_sub (sq[2][2], sq[2][2], sq[2][1]);

  mpz_sub (sq[0][1], sum, sq[0][0]);
  mpz_sub (sq[0][1], sq[0][1], sq[0][2]);

  mpz_sub (sq[1][0], sum, sq[1][1]);
  mpz_sub (sq[1][0], sq[1][0], sq[1][2]);

  mpz_clear (sum);
  //if (is_magic_square (sq, 1))
   // check_diff_divisors (a, b, sq);
  return 1;
}
static int
generate_scissor_square (mpz_t sq[3][3], mpz_t *a, mpz_t *b)
{
  if (!has_same_diff (a, b))
    return 0;

  mpz_set (sq[1][0], a[2]);
  mpz_set (sq[0][2], a[1]);
  mpz_set (sq[2][1], a[0]);
  mpz_set (sq[1][2], b[0]);
  mpz_set (sq[2][0], b[1]);
  mpz_set (sq[0][1], b[2]);
  // +------+------+------+
  // |      |  X3  |  N2  |
  // +------+------+------+
  // |  N3  |      |  X1  |
  // +------+------+------+
  // |  X2  |  N1  |      |
  // +------+------+------+

  int cm = mpz_cmp (a[1], b[1]);
  if (cm > 0)
    {
      mpz_sub (sq[1][1], a[1], b[1]);
      if (mpz_odd_p (sq[1][1]))
        multiply_square (sq, 4);
      mpz_cdiv_q_ui (sq[1][1], sq[1][1], 2);
      mpz_add (sq[1][1], sq[1][1], sq[2][0]);
    }
  else if (cm < 0)
    {
      mpz_sub (sq[1][1], b[1], a[1]);
      if (mpz_odd_p (sq[1][1]))
        multiply_square (sq, 4);
      mpz_cdiv_q_ui (sq[1][1], sq[1][1], 2);
      mpz_add (sq[1][1], sq[1][1], sq[0][2]);
    }
  else
    return 0;

  mpz_t sum;
  mpz_init (sum);
  mpz_add (sum, sq[2][0], sq[1][1]);
  mpz_add (sum, sum, sq[0][2]);

  mpz_sub (sq[0][0], sum, sq[0][2]);
  mpz_sub (sq[0][0], sq[0][0], sq[0][1]);

  mpz_sub (sq[2][2], sum, sq[2][0]);
  mpz_sub (sq[2][2], sq[2][2], sq[2][1]);
  mpz_clear (sum);
  return 1;
}

static int
pair_search (struct fv_app_scissor_square_t *app, mpz_t *target, FILE *in, FILE *out)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t b[3];
  mpz_t sq[3][3];

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_init (sq[i][j]);
  for (int i = 0; i < 3; i++)
    mpz_init (b[i]);

  while (1)
    {
      read = app->read_tuple (in, b, &line, &len);
      if (read == -1)
        break;
      if (generate_scissor_square (sq, target, b))
        app->display_square (sq, out);
      if (!app->no_warbird)
        {
          if (generate_warbird_square1 (sq, target, b))
            app->display_square (sq, out);
          if (generate_warbird_square2 (sq, target, b))
            app->display_square (sq, out);
        }
    }

  for (int i = 0; i < 3; i++)
    mpz_clear (b[i]);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_clear (sq[i][j]);

  if (line)
    free (line);
  return 0;
}

static void
_pair_search_file (struct fv_app_scissor_square_t *app, mpz_t *a, FILE *out)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  rewind (app->infile);
  mpz_t b[3];
  mpz_t sq[3][3];
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_init (sq[i][j]);
  for (int i = 0; i < 3; i++)
    mpz_init (b[i]);

  while (1)
    {
      read = app->read_tuple (app->infile, b, &line, &len);
      if (read == -1)
        break;
      if (generate_scissor_square (sq, a, b))
        app->display_square (sq, out);
      if (!app->no_warbird)
        {
          if (generate_warbird_square1 (sq, a, b))
            app->display_square (sq, out);
          if (generate_warbird_square2 (sq, a, b))
            app->display_square (sq, out);
        }
    }

  for (int i = 0; i < 3; i++)
    mpz_clear (b[i]);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_clear (sq[i][j]);
  if (line)
    free (line);
  return;
}

static int
pair_search_file (struct fv_app_scissor_square_t *app, FILE *in, FILE *out)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t a[3];
  for (int i = 0; i < 3; i++)
    mpz_init (a[i]);

  while (1)
    {
      read = app->read_tuple (in, a, &line, &len);
      if (read == -1)
        break;
      _pair_search_file (app, a, out);
    }

  for (int i = 0; i < 3; i++)
    mpz_clear (a[i]);
  if (line)
    free (line);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_scissor_square_t *app = (struct fv_app_scissor_square_t *) state->input;
  switch (key)
    {
    case 'n':
      app->no_warbird = 1;
      break;
    case 'i':
      app->read_tuple = binary_read_three_numbers_from_stream;
      break;
    case 'o':
      app->display_square = display_binary_square_record;
      break;
    case ARGP_KEY_ARG:
      if (app->num_args == 3 || app->infile)
        argp_error (state, "too many arguments");
      else
        {
          if (app->num_args == 0 && access (arg, R_OK) == 0)
            app->infile = fopen (arg, "r");
          else
            {
              char *comma = strchr (arg, ',');
              if (comma)
                *comma = '\0';
              mpz_init_set_str (app->match[app->num_args], arg, 10);
            }
          app->num_args++;
        }
      break;
    case ARGP_KEY_NO_ARGS:
      argp_error (state, "missing argument");
      break;
    case ARGP_KEY_INIT:
      setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
      break;
    case ARGP_KEY_FINI:
      if (app->num_args > 3 && !app->infile)
        argp_error (state, "too many arguments");
      else if (app->num_args < 3 && !app->infile)
        argp_error (state, "too few arguments");
      else if (!app->infile && app->num_args != 3)
        argp_error (state, "missing argument");
      break;
    }
  return 0;
}

int
fituvalu_scissor_square (struct fv_app_scissor_square_t *app, FILE *in, FILE *out)
{
  if (!app->infile)
    return pair_search (app, app->match, in, out);
  else
    return pair_search_file (app, in, out);
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "no-warbird", 'n', 0, 0, "Only try the scissor square method"},
  { 0 }
};

static struct argp
argp =
{
  options, parse_opt, "N1, N2, N3,\nFILE",
  "Try to create a 3x3 of magic square from two progressions of three squares that have the same difference between the squares, or the differences must be divisible.  This program tries create a magic squares of type 6:12, and 6:13.  The first progression is given as the arguments N1, N2, N3.  The second progression is passed in on the standard input.\vThe three values must be perfect squares, separated by a comma and terminated by a newline, and must be in ascending order.  This program also generates 3x3 nearly-magic squares.  The progressions are laid out as follows:\n\
+------+------+------+     +------+------+------+\n\
|      |  X3  |  N2  |     |  X2  |      |  N3  |\n\
+------+------+------+     +------+------+------+\n\
|  N3  |      |  X1  | and |      |  N2  |  X1  |\n\
+------+------+------+     +------+------+------+\n\
|  X2  |  N1  |      |     |  N1  |  X3  |      |\n\
+------+------+------+     +------+------+------+",
  0
};

int
main (int argc, char **argv)
{
  struct fv_app_scissor_square_t app;
  memset (&app, 0, sizeof (app));
  app.display_square = display_square_record;
  app.read_tuple = read_three_numbers_from_stream;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_scissor_square (&app, stdin, stdout);
}
