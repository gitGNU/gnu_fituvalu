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

unsigned long long max_tries;
void (*display_square) (mpz_t s[3][3], FILE *out) = display_square_record;
int (*read_tuple) (FILE *, mpz_t (*)[3], char **, size_t *) = read_three_numbers_from_stream;

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
create_square (mpz_t *a, mpz_t *b, FILE *out)
{
  mpz_t s[3][3];
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_init (s[i][j]);
  if (mpz_cmp (a[1], b[1]) == 0)
    {
      create_morgenstern_type_1 (a, b, s);
      display_square (s, out);
      create_morgenstern_type_2 (a, b, s);
      display_square (s, out);
      create_morgenstern_type_3 (a, b, s);
      display_square (s, out);
    }
  else if (mpz_cmp (a[0], b[0]) == 0)
    {
      create_morgenstern_type_4 (a, b, s);
      display_square (s, out);
    }
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_clear (s[i][j]);
}

static void
converge (mpz_t *a, mpz_t *b, int j, FILE *out)
{
  int count = 0;
  while (count < max_tries)
    {
      if (mpz_cmp (a[j], b[j]) < 0)
        {
          while (mpz_cmp (a[j], b[j]) < 0)
            {
              for (int i = 0; i < 3; i++)
                mpz_mul_ui (a[i], a[i], 4);
            }
        }
      else
        {
          while (mpz_cmp (b[j], a[j]) <= 0)
            {
              for (int i = 0; i < 3; i++)
                mpz_mul_ui (b[i], b[i], 4);
            }
        }
      if (mpz_cmp (a[j], b[j]) == 0)
        {
          create_square (a, b, out);
          break;
        }
      count++;
    }
}

static void
converge_and_create (mpz_t *a, mpz_t *b, FILE *out)
{
  for (int j = 0; j < 2; j++)
    {
      if (mpz_cmp (a[j], b[j]) < 0)
        converge (a, b, j, out);
      else
        converge (b, a, j, out);
    }
}

static int
pair_search (FILE *stream, FILE *out)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t a[3], b[3];

  for (int i = 0; i < 3; i++)
    mpz_inits (a[i], b[i], NULL);

  while (1)
    {
      read = read_tuple (stream, &a, &line, &len);
      if (read == -1)
        break;
      read = read_tuple (stream, &b, &line, &len);
      if (read == -1)
        break;
      converge_and_create (a, b, out);
    }

  for (int i = 0; i < 3; i++)
    mpz_clears (a[i], b[i], NULL);

  if (line)
    free (line);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  char *end = NULL;
  switch (key)
    {
    case 'i':
      read_tuple = binary_read_three_numbers_from_stream;
      break;
    case 'o':
      display_square = display_binary_square_record;
      break;
    case ARGP_KEY_ARG:
      if (max_tries)
        argp_error (state, "too many arguments");
      else
        max_tries = strtoull (arg, &end, 10);
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

struct argp argp ={options, parse_opt, "NUM", "Try to create a magic square from two progressions of three squares.\vThe two progressions must be separated by a newline. For example:\n"
"1, 25, 49, \n"
"289, 625, 961, \n", 0};

int
main (int argc, char **argv)
{
  argp_parse (&argp, argc, argv, 0, 0, 0);
  return pair_search (stdin, stdout);
}
