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

struct fv_app_display_magic_square_t
{
  mpz_t *filters;
  int num_filters;
  int showmax;
  int showmin;
  int showdiff;
  int (*read_square) (FILE *, mpz_t (*)[3][3], char **, size_t *);
  void (*display_square) (mpz_t s[3][3], FILE *out);
};

static void
get_lowest_number (mpz_t a[3][3], mpz_t *low)
{
  mpz_set_si (*low, -1);

  int j, k;
  for (j = 0; j < 3; j++)
    for (k = 0; k < 3; k++)
      {
        if (mpz_cmp (a[j][k], *low) < 0 || mpz_cmp_si (*low, -1) == 0)
          mpz_set (*low, a[j][k]);
      }
}

static void
get_largest_number (mpz_t sq[3][3], mpz_t m)
{
  mpz_set_ui (m, 0);
  for (int i = 0; i < 3; i++)
  for (int j = 0; j < 3; j++)
    if (mpz_cmp (sq[i][j], m) > 0)
      mpz_set (m, sq[i][j]);
}

int
fituvalu_display_magic_number (struct fv_app_display_magic_square_t *app, FILE *stream)
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
          mpz_t sum;
          mpz_init (sum);
          mpz_set (sum, a[0][0]);
          mpz_add (sum, sum, a[0][1]);
          mpz_add (sum, sum, a[0][2]);
          for (i = 0; i < app->num_filters; i++)
            {
              if (mpz_cmp (app->filters[i], sum) == 0)
                app->display_square (a, stdout);
            }
          mpz_clear (sum);
        }
      else if (app->showmax)
        {
          mpz_t m;
          mpz_init (m);
          get_largest_number (a, m);
          display_textual_number (&m, stdout);
          mpz_clear (m);
        }
      else if (app->showmin)
        {
          mpz_t m;
          mpz_init (m);
          get_lowest_number (a, &m);
          display_textual_number (&m, stdout);
          mpz_clear (m);
        }
      else if (app->showdiff)
        {
          mpz_t mi, ma, diff;
          mpz_inits (mi, ma, diff, NULL);
          get_lowest_number (a, &mi);
          get_largest_number (a, ma);
          if (mpz_cmp_ui (mi, 0) < 0)
            {
              mpz_abs (mi, mi);
              mpz_add (diff, ma, mi);
            }
          else
            mpz_sub (diff, ma, mi);
          display_textual_number (&diff, stdout);
          mpz_clears (mi, ma, diff, NULL);
        }
      else
        {
          mpz_t sum;
          mpz_init (sum);
          mpz_set (sum, a[0][0]);
          mpz_add (sum, sum, a[0][1]);
          mpz_add (sum, sum, a[0][2]);
          display_textual_number (&sum, stdout);
          mpz_clear (sum);
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
  struct fv_app_display_magic_square_t *app = (struct fv_app_display_magic_square_t *) state->input;
  switch (key)
    {
    case 'f':
      app->filters = realloc (app->filters,
                              sizeof (mpz_t) * (app->num_filters + 1));
      mpz_init_set_str (app->filters[app->num_filters], arg, 10);
      app->num_filters++;
      break;
    case 'd':
      app->showdiff = 1;
      break;
    case 'm':
      app->showmax = 1;
      break;
    case 'M':
      app->showmin = 1;
      break;
    case 'i':
      app->read_square = binary_read_square_from_stream;
      break;
    }
  return 0;
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "max", 'm', 0, 0, "Show the largest value instead of the magic number"},
  { "min", 'M', 0, 0, "Show the smallest value instead of the magic number"},
  { "diff", 'd', 0, OPTION_HIDDEN, "Show the difference between the largest value and the smallest value"},
  { "filter", 'f', "NUM", 0, "Only show squares with magic number NUM"},
  { 0 },
};

static struct argp
argp =
{
  options, parse_opt, 0,
  "Accept 3x3 magic squares from the standard input, and display the magic number.\vThe nine values must be separated by a comma and terminated by a newline.",
  0
};

int
main (int argc, char **argv)
{
  struct fv_app_display_magic_square_t app;
  memset (&app, 0, sizeof (app));
  app.read_square = read_square_from_stream;
  app.display_square = display_square_record;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_display_magic_number (&app, stdin);
}
