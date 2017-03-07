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
#include "magicsquareutil.h"

struct fv_app_multiply_progression_t
{
  int num_args;
  mpq_t multiplier;
  mpz_t max_tries;
  void (*display_tuple) (mpz_t s[3], FILE *out);
  int (*read_tuple) (FILE *, mpz_t *, char **, size_t *);
};

static void
multiply_three_square_progression (struct fv_app_multiply_progression_t *app, mpq_t *a)
{
  for (int i = 0; i < 3; i++)
    mpq_mul (a[i], a[i], app->multiplier);
}

int
fituvalu_multiply_progression (struct fv_app_multiply_progression_t *app, FILE *stream, FILE *out)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t ar[3], j;
  mpq_t a[3];

  for (int i = 0; i < 3; i++)
    {
      mpq_init (a[i]);
      mpz_init (ar[i]);
    }
  mpz_init (j);

  read = app->read_tuple (stream, ar, &line, &len);
  if (read != -1)
    {
      app->display_tuple (ar, out);
      while (1)
        {
          for (int i = 0; i < 3; i++)
            mpq_set_z (a[i], ar[i]);
          for (mpz_set_ui (j, 0); mpz_cmp (j, app->max_tries) < 0;
               mpz_add_ui (j, j, 1))
            {
              multiply_three_square_progression (app, a);
              for (int i = 0; i < 3; i++)
                mpz_set_q (ar[i], a[i]);
              app->display_tuple (ar, out);
            }
          read = app->read_tuple (stream, ar, &line, &len);
          if (read == -1)
            break;
        }
    }

  for (int i = 0; i < 3; i++)
    {
      mpq_clear (a[i]);
      mpz_clear (ar[i]);
    }
  mpz_clear (j);

  if (line)
    free (line);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_multiply_progression_t *app = (struct fv_app_multiply_progression_t *) state->input;
  switch (key)
    {
    case 'i':
      app->read_tuple = binary_read_three_numbers_from_stream;
      break;
    case 'o':
      app->display_tuple = display_binary_three_record;
      break;
    case ARGP_KEY_ARG:
      if (app->num_args == 2)
        argp_error (state, "too many arguments");
      else if (app->num_args == 1)
        mpz_set_str (app->max_tries, arg, 10);
      else if (app->num_args == 0)
        mpq_set_str (app->multiplier, arg, 10);
      app->num_args++;
      break;
    case ARGP_KEY_INIT:
      mpq_init (app->multiplier);
      mpz_init (app->max_tries);
      mpz_set_ui (app->max_tries, 1);
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
  { 0 }
};

static struct argp
argp =
{
  options, parse_opt, "MUL [TRIES]",
  "Accept three-square arithmetic progressions from the standard input, and multiply them by MUL which can be a rational number.\vThe three values must be separated by a comma and terminated by a newline, and must be in ascending order.",
  0
};

int
main (int argc, char **argv)
{
  struct fv_app_multiply_progression_t app;
  memset (&app, 0, sizeof (app));
  app.display_tuple = display_three_record;
  app.read_tuple = read_three_numbers_from_stream;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_multiply_progression (&app, stdin, stdout);
}
