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
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <argp.h>
#include <string.h>
#include <gmp.h>
#include "magicsquareutil.h"

void (*display_square) (mpz_t s[3][3], FILE *out) = display_square_record;

int filter_num_squares;
int num_args;
unsigned long long max;
mpz_t a[3][3];
mpz_t x1, _y1, z1, m12, n12, x2, y2, z2, m22, n22,
      yx1dif, yx1sum, yx2dif, yx2sum;

static int
no_filter (int num)
{
  return 1;
}

int (*filter_square) (int) = no_filter;

static int
filter (int num_squares)
{
  int count = 5;
  if (mpz_perfect_square_p (a[0][1]))
    {
      count++;
      if (count >= num_squares)
        return 1;
    }

  if (mpz_perfect_square_p (a[1][0]))
    {
      count++;
      if (count >= num_squares)
        return 1;
    }
  if (mpz_perfect_square_p (a[1][2]))
    {
      count++;
      if (count >= num_squares)
        return 1;
    }
  if (mpz_perfect_square_p (a[2][1]))
    {
      count++;
      if (count >= num_squares)
        return 1;
    }
  return 0;
}

static void
search_type_1 (unsigned long long m1, unsigned long long n1, unsigned long long m2, unsigned long long n2, FILE *out)
{
  //where X1 = 2*m1*n1,  Y1 = m1^2-n1^2,  Z1 = m1^2+n1^2,
  mpz_set_ui (x1, m1);
  mpz_mul_ui (x1, x1, n1);
  mpz_mul_ui (x1, x1, 2);

  mpz_set_ui (m12, m1);
  mpz_mul_ui (m12, m12, m1);

  mpz_set_ui (n12, n1);
  mpz_mul_ui (n12, n12, n1);

  mpz_sub (_y1, m12, n12);

  mpz_add (z1, m12, n12);

  mpz_sub (yx1dif, _y1, x1);

  mpz_add (yx1sum, _y1, x1);

  // where X2 = 2*m2*n2,  Y2 = m2^2-n2^2,  Z2 = m2^2+n2^2,
  mpz_set_ui (x2, m2);
  mpz_mul_ui (x2, x2, n2);
  mpz_mul_ui (x2, x2, 2);

  mpz_set_ui (m22, m2);
  mpz_mul_ui (m22, m22, m2);

  mpz_set_ui (n22, n2);
  mpz_mul_ui (n22, n22, n2);

  mpz_sub (y2, m22, n22);

  mpz_add (z2, m22, n22);

  mpz_sub (yx2dif, y2, x2);

  mpz_add (yx2sum, y2, x2);

/*
    -----------
    A^2  -  C^2
     -  E^2  - 
    G^2  -  I^2
    -----------
         1     
*/
  //I = Z2*(Y1-X1), E = Z2*Z1, A = Z2*(Y1+X1)
  mpz_mul (a[2][2], z2, yx1dif);
  mpz_mul (a[2][2], a[2][2], a[2][2]);

  mpz_mul (a[1][1], z2, z1);
  mpz_mul (a[1][1], a[1][1], a[1][1]);

  mpz_mul (a[0][0], z2, yx1sum);
  mpz_mul (a[0][0], a[0][0], a[0][0]);


  //G = (Y2-X2)*Z1, E = Z2*Z1, C = (Y2+X2)*Z1.
  mpz_mul (a[2][0], z1, yx2dif);
  mpz_mul (a[2][0], a[2][0], a[2][0]);

  mpz_mul (a[0][2], z1, yx2sum);
  mpz_mul (a[0][2], a[0][2], a[0][2]);

  //B^2 = G^2 + I^2 - E^2;
  //D^2 = C^2 + I^2 - E^2;
  //F^2 = G^2 + A^2 - E^2;
  //H^2 = C^2 + A^2 - E^2;
  mpz_add (a[0][1], a[2][0], a[2][2]);
  mpz_sub (a[0][1], a[0][1], a[1][1]);

  mpz_add (a[1][0], a[0][2], a[2][2]);
  mpz_sub (a[1][0], a[1][0], a[1][1]);

  mpz_add (a[1][2], a[2][0], a[0][0]);
  mpz_sub (a[1][2], a[1][2], a[1][1]);

  mpz_add (a[2][1], a[0][2], a[0][0]);
  mpz_sub (a[2][1], a[2][1], a[1][1]);

  if (filter_square (filter_num_squares))
    display_square (a, out);
}

static int
morgenstern_search_type_1 (FILE *in, FILE *out)
{
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_init (a[i][j]);
  mpz_inits (x1, _y1, z1, m12, n12, x2, y2, z2, m22, n22,
             yx1dif, yx1sum, yx2dif, yx2sum,
             NULL);
  morgenstern_search (max, in, search_type_1, out);
  mpz_clears (x1, _y1, z1, m12, n12, x2, y2, z2, m22, n22,
              yx1dif, yx1sum, yx2dif, yx2sum, NULL);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_clear (a[i][j]);
  return 0;
}

static struct argp_option
options[] =
{
  { "filter", 'f', "NUM", 0, "Only show magic squares that have at least NUM perfect squares" },
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { 0 }
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'f':
      filter_num_squares = atoi (arg);
      filter_square = filter;
      break;
    case 'o':
      display_square = display_binary_square_record;
      break;
    case ARGP_KEY_ARG:
      if (num_args == 1)
        argp_error (state, "too many arguments");
      else
        {
          char *end = NULL;
          max = strtoull (arg, &end, 10);
          num_args++;
        }
      break;
    case ARGP_KEY_NO_ARGS:
      argp_error (state, "missing argument");
      break;
    case ARGP_KEY_INIT:
      break;
    }
  return 0;
}

struct argp argp ={options, parse_opt, "MAX", "Generate 3x3 magic squares with 5 perfect squares or more by creating two arithmetic progressions of three perfect squares with the center square in common.\vThe standard input provides the parametric \"MN\" values -- two values per record to assist in the transformation.  Use the \"seq-morgenstern-mn\" program to provide this data on the standard input.  Morgenstern type 1 squares have 5 perfect squares in this configuration:\n\
+-------+-------+-------+\n\
|  A^2  |       |  C^2  |\n\
+-------+-------+-------+\n\
|       |  E^2  |       |\n\
+-------+-------+-------+\n\
|  G^2  |       |  I^2  |\n\
+-------+-------+-------+", 0};


int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  return morgenstern_search_type_1 (stdin, stdout);
}
