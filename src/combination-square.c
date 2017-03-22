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

struct fv_app_combination_square_t
{
  int (*read_numbers)(FILE *, mpz_t *, int, char **, size_t *);
  void (*display_square) (mpz_t s[3][3], FILE *out);
};

static void
dump_squares (struct fv_app_combination_square_t *app, mpz_t *squares, int num_squares, mpz_t s[3][3], FILE *out)
{
  mpz_t sum1, sum2;
  mpz_inits (sum1, sum2, NULL);
  for (int a = 0; a < num_squares; a++)
    {
      mpz_set (s[0][0], squares[a]);
      for (int b = 0; b < num_squares; b++)
        {
          mpz_set (s[0][1], squares[b]);
          for (int c = 0; c < num_squares; c++)
            {
              mpz_set (s[0][2], squares[c]);
              for (int d = 0; d < num_squares; d++)
                {
                  mpz_set (s[1][0], squares[d]);
                  for (int e = 0; e < num_squares; e++)
                    {
                      mpz_set (s[1][1], squares[e]);
                      for (int f = 0; f < num_squares; f++)
                        {
                          mpz_set (s[1][2], squares[f]);
                          mpz_add (sum1, s[0][0], s[0][1]);
                          mpz_add (sum1, sum1, s[0][2]);
                          mpz_add (sum2, s[1][0], s[1][1]);
                          mpz_add (sum2, sum2, s[1][2]);
                          if (mpz_cmp (sum1, sum2) != 0)
                            continue;
                          for (int g = 0; g < num_squares; g++)
                            {
                              mpz_set (s[2][0], squares[g]);
                              mpz_add (sum2, s[0][0], s[1][0]);
                              mpz_add (sum2, sum2, s[2][0]);
                              if (mpz_cmp (sum1, sum2) != 0)
                                continue;
                              for (int h = 0; h < num_squares; h++)
                                {
                                  mpz_set (s[2][1], squares[h]);
                                  for (int i = 0; i < num_squares; i++)
                                    {
                                      mpz_set (s[2][2], squares[i]);
                                      if (is_magic_square (s, 0))
                                        app->display_square (s, out);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
  mpz_clears (sum1, sum2, NULL);
}

int
fituvalu_combination_square_count (struct fv_app_combination_square_t *app, FILE *stream)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t num;
  mpz_t s[3][3];
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_init (s[i][j]);

  mpz_init (num);
  int num_squares = 0;
  mpz_t *squares = NULL;

  while (1)
    {
      read = app->read_numbers (stream, &num, 1, &line, &len);
      if (read == -1)
        break;
      squares = realloc (squares, sizeof (mpz_t) * (num_squares + 1));
      mpz_init_set (squares[num_squares], num);
      num_squares++;
    }

  mpz_clear (num);

  if (line)
    free (line);

  dump_squares (app, squares, num_squares, s, stdout);
  for (int i = 0; i < num_squares; i++)
    mpz_clear (squares[i]);
  free (squares);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_clear (s[i][j]);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_combination_square_t *app = (struct fv_app_combination_square_t *) state->input;
  switch (key)
    {
    case 'i':
      app->read_numbers = binary_read_numbers_from_stream;
      break;
    case 'o':
      app->display_square = display_binary_square_record;
      break;
    case ARGP_KEY_INIT:
      setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
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
  options, parse_opt, 0,
  "Accept perfect squares from the standard input, and display them in every combination that is a 3x3 magic square with non-distinct values.",
  0
};

int
main (int argc, char **argv)
{
  struct fv_app_combination_square_t app;
  memset (&app, 0, sizeof (app));
  app.read_numbers = read_numbers_from_stream;
  app.display_square = display_square_record;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  is_magic_square_init ();
  return fituvalu_combination_square_count (&app, stdin);
}
