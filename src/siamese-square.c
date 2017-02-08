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

int in_binary;
int fulcrum;
void (*display_square) (mpz_t s[3][3], FILE *out) = display_square_record;
int (*read_record) (FILE *, mpz_t*, char **, size_t *) = read_three_numbers_from_stream;

static void
siamese_step_square (mpz_t a[3][3], mpz_t s, mpz_t d1, mpz_t e1)
{
  mpz_t next;
  mpz_init (next);
  mpz_set (next, s);
  mpz_set (a[0][1], next);
  mpz_add (next, next, d1);
  mpz_set (a[2][2], next);
  mpz_add (next, next, d1);
  mpz_set (a[1][0], next);
  mpz_add (next, next, e1);
  mpz_set (a[2][0], next);
  mpz_add (next, next, d1);
  mpz_set (a[1][1], next);
  mpz_add (next, next, d1);
  mpz_set (a[0][2], next);
  mpz_add (next, next, e1);
  mpz_set (a[1][2], next);
  mpz_add (next, next, d1);
  mpz_set (a[0][0], next);
  mpz_add (next, next, d1);
  mpz_set (a[2][1], next);
  mpz_clear (next);
}

static void
siamese_fulcrum_square (mpz_t a[3][3], mpz_t s, mpz_t d1, mpz_t e1, mpz_t f1)
{
  mpz_t next;
  mpz_init (next);
  mpz_set (next, s);
  mpz_set (a[0][1], next);
  mpz_add (next, next, d1);
  mpz_set (a[2][2], next);
  mpz_add (next, next, e1);
  mpz_set (a[1][0], next);
  mpz_add (next, next, f1);
  mpz_set (a[2][0], next);
  mpz_add (next, next, e1);
  mpz_set (a[1][1], next);
  mpz_add (next, next, e1);
  mpz_set (a[0][2], next);
  mpz_add (next, next, f1);
  mpz_set (a[1][2], next);
  mpz_add (next, next, e1);
  mpz_set (a[0][0], next);
  mpz_add (next, next, d1);
  mpz_set (a[2][1], next);
  mpz_clear (next);
}

static int
siamese (FILE *in, FILE *out)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t a[3][3];
  mpz_t v[4];

  int i, j;
  for (i = 0; i < 3; i++)
    {
      for (j = 0; j < 3; j++)
        mpz_init (a[i][j]);

      mpz_init (v[i]);
    }
  mpz_init (v[3]);

  if (fulcrum)
    {
      while (1)
        {
          read = read_record (in, v, &line, &len);
          if (read == -1)
            break;
          siamese_fulcrum_square (a, v[0], v[1], v[2], v[3]);
          if (is_magic_square (a, 1))
            display_square (a, out);
        }
    }
  else
    {
      while (1)
        {
          read = read_record (in, v, &line, &len);
          if (read == -1)
            break;
          siamese_step_square (a, v[0], v[1], v[2]);
          if (is_magic_square (a, 1))
            display_square (a, out);
        }
    }

      for (i = 0; i < 3; i++)
        {
          for (j = 0; j < 3; j++)
            mpz_clear (a[i][j]);
          mpz_clear (v[i]);
        }
      mpz_clear (v[3]);

      if (line)
        free (line);
      return 0;
    }

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'i':
      in_binary = 1;
      break;
    case 'f':
      fulcrum = 1;
      break;
    case 'o':
      display_square = display_binary_square_record;
      break;
    case ARGP_KEY_FINI:
      if (in_binary)
        {
          if (fulcrum)
            read_record = binary_read_four_numbers_from_stream;
          else
            read_record = binary_read_three_numbers_from_stream;
        }
      else
        {
          if (fulcrum)
            read_record = read_four_numbers_from_stream;
          else
            read_record = read_three_numbers_from_stream;
        }
      break;
    }
  return 0;
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "fulcrum", 'f', 0, 0, "Generate squares with a fulcrum progression"},
  { 0 }
};

struct argp argp ={options, parse_opt, 0, "Generate magic squares using a modified Siamese step method.\vThe input to this program is a starting number, followed by two or three distances separated by commas.  The default is to pass in a start, D1, and E1, which generates a square with the the step progression:\n"
"  |-----+--+--+-------+--+--+-------+--+--+------|\n"
"         D1       E1\n"
"When --fulcrum is passed, the standard input must have start, D1, E1, F1 which generates a square with the fulcrum progression:\n"
"  |-+--------+--+-----+--+--+-----+--+--------+--|\n"
"        D1    E1   F1\n"
"D1 must be equal to the sum of E1 and F1."
               ,0};

int
main (int argc, char **argv)
{
  argp_parse (&argp, argc, argv, 0, 0, 0);
  is_magic_square_init ();
  return siamese (stdin, stdout);
}
