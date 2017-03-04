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
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <argp.h>
#include <string.h>
#include <gmp.h>
#include "magicsquareutil.h"

struct fv_app_find_3sq_progressions_t
{
  int showroot;
  unsigned long long incr;
  unsigned long long max_tries;
  int show_diff;
  int in_binary;
  void (*display_record) (mpz_t *, mpz_t*, FILE *out);
};

static void
create_three_square_progression (struct fv_app_find_3sq_progressions_t *app, mpz_t m, mpz_t *vec, int size, mpz_t *finalroot, FILE *out)
{
  mpz_t root, diff;
  mpz_inits (root, diff, NULL);
  mpz_sqrt (root, m);
  mpz_set (vec[0], m);
  mpz_set (vec[1], vec[0]);
  for (unsigned long long count = 0; count < app->max_tries; count++)
    {
      for (int i = 0; i < app->incr; i++)
        {
          mpz_add (vec[1], vec[1], root);
          mpz_add (vec[1], vec[1], root);
          mpz_add_ui (vec[1], vec[1], 1);
          mpz_add_ui (root, root, 1);
        }

      mpz_sub (diff, vec[1], vec[0]);
      mpz_add (vec[2], vec[1], diff);
      if (mpz_perfect_square_p (vec[2]))
        {
          if (app->showroot)
            mpz_sqrt (*finalroot, vec[2]);
          else
            mpz_set_ui (*finalroot, 0);
          app->display_record (vec, finalroot, out);
        }
      mpz_sub (vec[2], vec[0], diff);
      if (mpz_cmp_ui (vec[2], 0) <= 0)
        break;
      if (mpz_perfect_square_p (vec[2]))
        {
          if (app->showroot)
            mpz_set (*finalroot, root);
          else
            mpz_set_ui (*finalroot, 0);
          mpz_set (diff, vec[0]);
          mpz_set (vec[0], vec[2]);
          mpz_set (vec[2], vec[1]);
          mpz_set (vec[1], diff);
          if (app->show_diff)
            {
              mpz_sub (diff, vec[1], vec[0]);
              if (app->display_record == display_binary_three_record_with_root)
                mpz_out_raw (out, diff);
              else
                {
                  char buf[mpz_sizeinbase (diff, 10) + 2];
                  mpz_get_str (buf, 10, diff);
                  fprintf (out, "%s, ", buf);
                }
            }
          app->display_record (vec, finalroot, out);
        }
    }
  mpz_clears (root, diff, NULL);
}

static int
gen_3sq (struct fv_app_find_3sq_progressions_t *app, FILE *in, FILE *out)
{
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  mpz_t m, vec[3], finalroot;
  mpz_inits (m, vec[0], vec[1], vec[2], finalroot, NULL);
  while (1)
    {
      read = fv_getline (&line, &len, in);
      if (read == -1)
        break;
      mpz_set_str (m, line, 10);
      create_three_square_progression (app, m, vec, 3, &finalroot, out);
    }
  mpz_clears (m, vec[0], vec[1], vec[2], finalroot, NULL);
  if (line)
    free (line);
  return 0;
}

static int
gen_binary_3sq (struct fv_app_find_3sq_progressions_t *app, FILE *in, FILE *out)
{
  ssize_t read;
  mpz_t m, vec[3], finalroot;
  mpz_inits (m, vec[0], vec[1], vec[2], finalroot, NULL);
  while (1)
    {
      read = mpz_inp_raw (m, in);
      if (!read)
        break;
      create_three_square_progression (app, m, vec, 3, &finalroot, out);
    }
  mpz_clears (m, vec[0], vec[1], vec[2], finalroot, NULL);
  return 0;
}


static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_find_3sq_progressions_t *app = (struct fv_app_find_3sq_progressions_t *) state->input;
  char *end = NULL;
  switch (key)
    {
    case 'd':
      app->show_diff = 1;
      break;
    case 'n':
      app->showroot = 0;
      break;
    case 'i':
      app->in_binary = 1;
      break;
    case 'o':
      app->display_record = display_binary_three_record_with_root;
      break;
    case 'I':
      app->incr = strtoull (arg, &end, 10);
      break;
    case ARGP_KEY_ARG:
      if (app->max_tries)
        argp_error (state, "too many arguments");
      else
        app->max_tries = strtoull (arg, &end, 10);
      break;
    case ARGP_KEY_INIT:
      setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
      break;
    case ARGP_KEY_NO_ARGS:
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
  { "increment", 'I', "NUM", 0, "Advance by NUM squares instead of 1"},
  { "no-root", 'n', 0, 0, "Don't show the root of the fourth number"},
  { "show-diff", 'd', 0, 0, "Also show the diff"},
  { 0 }
};

static struct argp argp ={options, parse_opt, "NUM", "Generate arithmetic progressions of three squares, with a fourth number being the square root of the 3rd square.\vThe input of this program comes from \"sq-seq\".", 0 };

int
fituvalu_find_3sq_progressions (struct fv_app_find_3sq_progressions_t *app, FILE *in, FILE *out)
{
  if (app->in_binary)
    return gen_binary_3sq (app, in, out);
  else
    return gen_3sq (app, in, out);
}

int
main (int argc, char **argv)
{
  struct fv_app_find_3sq_progressions_t app;
  memset (&app, 0, sizeof (app));
  app.showroot = 1;
  app.incr = 1;
  app.display_record = display_three_record_with_root;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_find_3sq_progressions (&app, stdin, stdout);
}
