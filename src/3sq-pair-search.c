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

struct fv_app_3sq_pair_search_t
{
  FILE *infile;
  int flipflops;
  int num_args;
  int tries;
  mpz_t advsq;
  void (*display_square) (mpz_t s[3][3], FILE *out);
  int (*read_tuple) (FILE *, mpz_t *, char **, size_t *);
  mpz_t match[3];
};

static void
create_morgenstern_type_4 (mpz_t *a, mpz_t *b, mpz_t s[3][3])
{
  /*
    -----------
     -  B^2  - 
    D^2  -  F^2
    G^2  -  I^2
    -----------
         4     
  */
  mpz_set (s[0][1], a[0]);
  mpz_set (s[2][0], a[1]);
  mpz_set (s[1][2], a[2]);
  mpz_set (s[2][2], b[1]);
  mpz_set (s[1][0], b[2]);

  mpz_add (s[0][0], s[1][2], s[2][2]);
  mpz_sub (s[0][0], s[0][0], s[0][1]);
  mpz_add (s[0][2], s[1][0], s[2][0]);
  mpz_sub (s[0][2], s[0][2], s[0][1]);
  mpz_add (s[2][1], s[1][0], s[1][2]);
  mpz_sub (s[2][1], s[2][1], s[0][1]);
  mpz_add (s[1][1], s[0][0], s[0][2]);
  mpz_sub (s[1][1], s[1][1], s[2][1]);

}

static void
create_morgenstern_type_3 (mpz_t *a, mpz_t *b, mpz_t s[3][3])
{
/*
    ----------- 
    A^2 B^2  -
     -  E^2  -
     -  H^2 I^2
    -----------
         3
*/
  mpz_set (s[1][1], a[1]);
  mpz_set (s[0][0], a[0]);
  mpz_set (s[2][2], a[2]);
  mpz_set (s[0][1], b[0]);
  mpz_set (s[2][1], b[2]);

  mpz_add (s[0][2], s[2][1], s[1][1]);
  mpz_sub (s[0][2], s[0][2], s[0][0]);
  mpz_add (s[1][0], s[2][1], s[2][2]);
  mpz_sub (s[1][0], s[1][0], s[0][0]);
  mpz_add (s[1][2], s[0][1], s[0][0]);
  mpz_sub (s[1][2], s[1][2], s[2][2]);
  mpz_add (s[2][0], s[0][1], s[1][1]);
  mpz_sub (s[2][0], s[2][0], s[2][2]);
}

static void
create_morgenstern_type_2 (mpz_t *a, mpz_t *b, mpz_t s[3][3])
{
  /*
    -----------
     -  B^2  - 
    D^2 E^2 F^2
     -  H^2  - 
    -----------
         2     
  */
  mpz_set (s[1][1], a[1]);
  mpz_set (s[0][1], a[0]);
  mpz_set (s[2][1], a[2]);
  mpz_set (s[1][0], b[0]);
  mpz_set (s[1][2], b[2]);

  mpz_add (s[0][0], s[1][2], s[2][1]);
  mpz_cdiv_q_ui (s[0][0], s[0][0], 2);
  mpz_add (s[0][2], s[1][0], s[2][1]);
  mpz_cdiv_q_ui (s[0][2], s[0][2], 2);
  mpz_add (s[2][0], s[1][2], s[0][1]);
  mpz_cdiv_q_ui (s[2][0], s[2][0], 2);
  mpz_add (s[2][2], s[1][0], s[0][1]);
  mpz_cdiv_q_ui (s[2][2], s[2][2], 2);
}

static void
create_morgenstern_type_1 (mpz_t *a, mpz_t *b, mpz_t s[3][3])
{
  /*
    -----------
    A^2  -  C^2
     -  E^2  - 
    G^2  -  I^2
    -----------
         1     
  */
  mpz_t sum;
  mpz_init (sum);
  mpz_set (s[1][1], a[1]);
  mpz_set (s[0][0], a[0]);
  mpz_set (s[2][2], a[2]);
  mpz_set (s[0][2], b[2]);
  mpz_set (s[2][0], b[0]);
  mpz_add (sum, s[0][0], s[1][1]);
  mpz_add (sum, sum, s[2][2]);
  mpz_sub (s[0][1], sum, s[0][0]);
  mpz_sub (s[0][1], s[0][1], s[0][2]);
  mpz_sub (s[2][1], sum, s[1][1]);
  mpz_sub (s[2][1], s[2][1], s[0][1]);
  mpz_sub (s[1][0], sum, s[0][0]);
  mpz_sub (s[1][0], s[1][0], s[2][0]);
  mpz_sub (s[1][2], sum, s[1][1]);
  mpz_sub (s[1][2], s[1][2], s[1][0]);
  mpz_clear (sum);
}

static void
create_square (struct fv_app_3sq_pair_search_t *app, mpz_t *a, mpz_t *b, FILE *out)
{
  if (mpz_cmp (a[0], b[0]) == 0 &&
      mpz_cmp (a[1], b[1]) == 0 &&
      mpz_cmp (a[2], b[2]) == 0)
    return;
  mpz_t s[3][3];
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_init (s[i][j]);
  if (mpz_cmp (a[1], b[1]) == 0)
    {
      create_morgenstern_type_1 (a, b, s);
      app->display_square (s, out);
      create_morgenstern_type_2 (a, b, s);
      app->display_square (s, out);
      create_morgenstern_type_3 (a, b, s);
      app->display_square (s, out);
    }
  else if (mpz_cmp (a[0], b[0]) == 0)
    {
      create_morgenstern_type_4 (a, b, s);
      app->display_square (s, out);
    }
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_clear (s[i][j]);
}

static void
converge (struct fv_app_3sq_pair_search_t *app, mpz_t *a, mpz_t *b, int j, FILE *out)
{
  if (mpz_cmp (a[j], b[j]) == 0)
    {
      create_square (app, a, b, out);
      return;
    }
  /*
     i thought this flip-flopping approach would help, but it doesn't
     seem to, even when i vary advsq.
     maybe if i vary advsq on the inside of the loop?
   */
  for (int q = 0; q < app->flipflops; q++)
    {
      if (mpz_cmp (a[j], b[j]) < 0)
        {
          while (mpz_cmp (a[j], b[j]) < 0)
            {
              for (int i = 0; i < 3; i++)
                mpz_mul (a[i], a[i], app->advsq);
            }
        }
      else
        {
          while (mpz_cmp (b[j], a[j]) <= 0)
            {
              for (int i = 0; i < 3; i++)
                mpz_mul (b[i], b[i], app->advsq);
            }
        }
      if (mpz_cmp (a[j], b[j]) == 0)
        {
          create_square (app, a, b, out);
          break;
        }
    }
}

static void
converge_and_create (struct fv_app_3sq_pair_search_t *app, mpz_t *a, mpz_t *b, FILE *out)
{
  mpz_t c[3], d[3], root;
  for (int i = 0; i < 3; i++)
    mpz_inits (c[i], d[i], NULL);
  mpz_init (root);
  mpz_sqrt (root, app->advsq);
  for (int j = 0; j < app->tries; j++)
    {
      for (int i = 0; i < 3; i++)
        {
          mpz_set (c[i], a[i]);
          mpz_set (d[i], b[i]);
        }
      if (mpz_cmp (c[1], d[1]) < 0)
        converge (app, c, d, 1, out);
      else
        converge (app, d, c, 1, out);

      for (int i = 0; i < 3; i++)
        {
          mpz_set (c[i], a[i]);
          mpz_set (d[i], b[i]);
        }
      if (mpz_cmp (c[0], d[0]) < 0)
        converge (app, c, d, 0, out);
      else
        converge (app, d, c, 0, out);

      mpz_add (app->advsq, app->advsq, root);
      mpz_add (app->advsq, app->advsq, root);
      mpz_add_ui (app->advsq, app->advsq, 1);
      mpz_add_ui (root, root, 1);
    }

  for (int i = 0; i < 3; i++)
    mpz_clears (c[i], d[i], NULL);
  mpz_clear (root);
}

static int
pair_search (struct fv_app_3sq_pair_search_t *app, FILE *in, FILE *out)
{
  mpz_t *target = app->match;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t b[3];

  for (int i = 0; i < 3; i++)
    mpz_init (b[i]);

  while (1)
    {
      read = app->read_tuple (in, b, &line, &len);
      if (read == -1)
        break;
      converge_and_create (app, target, b, out);
    }

  for (int i = 0; i < 3; i++)
    mpz_clear (b[i]);

  if (line)
    free (line);
  return 0;
}

static void
_pair_search_file (struct fv_app_3sq_pair_search_t *app, mpz_t *a, FILE *out)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  rewind (app->infile);
  mpz_t b[3];
  for (int i = 0; i < 3; i++)
    mpz_init (b[i]);

  while (1)
    {
      read = app->read_tuple (app->infile, b, &line, &len);
      if (read == -1)
        break;
      converge_and_create (app, a, b, out);
    }

  for (int i = 0; i < 3; i++)
    mpz_clear (b[i]);
  if (line)
    free (line);
  return;
}

static int
pair_search_file (struct fv_app_3sq_pair_search_t *app, FILE *in, FILE *out)
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
  struct fv_app_3sq_pair_search_t *app = (struct fv_app_3sq_pair_search_t *) state->input;
  char *end = NULL;
  switch (key)
    {
    case 'f':
      app->flipflops = strtoull (arg, &end, 10);
      break;
    case 't':
      app->tries = strtoull (arg, &end, 10);
      break;
    case 'a':
      mpz_set_str (app->advsq, arg, 10);
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
      mpz_init (app->advsq);
      mpz_set_str (app->advsq, "4", 10);
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

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "tries", 't', "NUM", OPTION_HIDDEN, "Try this many squares after 1"},
  { "advance", 'a', "NUM", OPTION_HIDDEN, "Advance the values by this square"},
  { "flip-flops", 'f', "NUM", OPTION_HIDDEN, "Try this many flip-flops"},
  { 0 }
};

static struct argp argp ={options, parse_opt, "N1, N2, N3,\nFILE", "Try to create a 3x3 of magic square from two progressions of three squares.  The first progression is given as the arguments N1, N2, N3.  The second progression is passed in on the standard input.\vThe three values must be perfect squares, separated by a comma and terminated by a newline, and must be in ascending order.  The programs \"find-3sq-progressions\", \"find-3sq-progressions-mn\", and \"mine-3sq-progressions\" produce suitable input for this program.\n", 0};

int
fituvalu_3sq_pair_search (struct fv_app_3sq_pair_search_t *app, FILE *in, FILE *out)
{
  if (!app->infile)
    return pair_search (app, in, out);
  else
    return pair_search_file (app, in, out);
}

int
main (int argc, char **argv)
{
  struct fv_app_3sq_pair_search_t app;
  memset (&app, 0, sizeof (app));

  app.flipflops = 1;
  app.tries = 1;
  app.display_square = display_square_record;
  app.read_tuple = read_three_numbers_from_stream;

  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_3sq_pair_search (&app, stdin, stdout);
}
