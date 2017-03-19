/* Copyright (C) 2016, 2017 Ben Asselstine
   
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

struct fv_app_mod_magic_square_t
{
  int num_args;
  mpz_t num;
  int num_filters;
  int *filters;
  void (*display_square) (mpz_t s[3][3], FILE *out);
  int (*read_square) (FILE *, mpz_t (*)[3][3], char **, size_t *);
};

int
fituvalu_mod_magic_square (struct fv_app_mod_magic_square_t *app, FILE *stream)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t a[3][3];

  int i, j;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      mpz_init (a[i][j]);

  while (1)
    {
      read = app->read_square (stream, &a, &line, &len);
      if (read == -1)
        break;

      if (app->num_filters)
        {
          mpz_t num;
          mpz_init (num);
          int count = 0;
          for (int k = 0; k < app->num_filters; k++)
            {
              for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                  {
                    mpz_mod (num, a[i][j], app->num);
                    if (mpz_cmp_ui (num, app->filters[k]) == 0)
                      count++;
                  }
            }
          mpz_clear (num);
          if (count == 9)
            app->display_square (a, stdout);
        }
      else
        {
          for (i = 0; i < 3; i++)
            for (j = 0; j < 3; j++)
              mpz_mod (a[i][j], a[i][j], app->num);
          app->display_square (a, stdout);
        }
    }

  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      mpz_clear (a[i][j]);

  if (line)
    free (line);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_mod_magic_square_t *app = (struct fv_app_mod_magic_square_t *) state->input;
  switch (key)
    {
    case 'i':
      app->read_square = binary_read_square_from_stream;
      break;
    case 'o':
      app->display_square = display_binary_square_record;
      break;
    case 'f':
        {
          char *end = NULL;
          int num = strtoul (arg, &end, 10);
          if (num >= 0)
            {
              app->filters =
                realloc (app->filters , (app->num_filters + 1) * sizeof (int));
              app->filters[app->num_filters] = num;
              app->num_filters++;
            }
          else
            argp_error (state, "%s is an invalid value for -f", arg);
        }
      break;
    case ARGP_KEY_ARG:
      if (app->num_args == 1)
        argp_error (state, "too many arguments");
      else if (app->num_args == 0)
        mpz_set_str (app->num, arg, 10);
      app->num_args++;
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
  { "filter", 'f', "REM", 0, "Show squares that have at least one value that is REM mod NUM"},
  { 0 }
};

static struct argp
argp =
{
  options, parse_opt, "NUM",
  "Accept 3x3 magic squares from the standard input, and mod them by NUM.\vThe nine values must be separated by a comma and terminated by a newline.",
  0
};

int
main (int argc, char **argv)
{
  struct fv_app_mod_magic_square_t app;
  memset (&app, 0, sizeof (app));
  app.display_square = display_square_record;
  app.read_square = read_square_from_stream;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  is_magic_square_init ();
  return fituvalu_mod_magic_square (&app, stdin);
}
