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

void (*display_square) (mpz_t s[3][3], FILE *out) = display_square_record;
int (*read_square) (FILE *, mpz_t (*)[3][3], char **, size_t *) = read_square_from_stream;

static int
acount_squares (mpz_t a[3][3])
{
  int i, j;
  int count = 0;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      if (mpz_perfect_square_p (a[i][j]))
        count++;
  return count;
}

static void
get_lowest (mpz_t a[3][3], mpz_t *low)
{
  mpz_t i;
  mpz_set_si (*low, -1);
  mpz_init (i);

  int j, k;
  for (j = 0; j < 3; j++)
    for (k = 0; k < 3; k++)
      {
        if (mpz_sgn (a[j][k]) > 0 &&
            (mpz_cmp (a[j][k], *low) < 0 || mpz_cmp_si (*low, -1) == 0))
          mpz_set (*low, a[j][k]);
      }
  if (mpz_sgn (*low) < 0)
    mpz_abs (*low, *low);
  mpz_clear (i);
}

static void
dec (mpz_t *j, mpz_t *jroot)
{
  mpz_sub_ui (*jroot, *jroot, 1);
  mpz_sub (*j, *j, *jroot);
  mpz_sub (*j, *j, *jroot);
  mpz_sub_ui (*j, *j, 1);
}

void
old_reduce_square (mpz_t a[3][3])
{
  int found = 0;
  int j, k;
  mpz_t low, i, lowroot;
  mpz_init (low);
  get_lowest (a, &low);
  mpz_init (i);

  int orig_count = acount_squares (a);
  mpz_init (lowroot);
  mpz_sqrt (lowroot, low);
  int initially_magic = is_magic_square (a, 1);
  for (mpz_set (i, low); mpz_cmp_ui (lowroot, 1) > 0; dec (&i, &lowroot))
    {
      int all_whole = 1;
      for (j = 0; j < 3; j++)
        for (k = 0; k < 3; k++)
          if (!mpz_divisible_p (a[j][k], i))
            {
              all_whole = 0;
              break;
            }
      if (all_whole)
        {
          mpz_t b[3][3];
          for (j = 0; j < 3; j++)
            for (k = 0; k < 3; k++)
              {
                mpz_init (b[j][k]);
                mpz_cdiv_q (b[j][k], a[j][k], i);
              }
          int count = acount_squares (b);
          if ((initially_magic && is_magic_square (b, 1)) || !initially_magic)
            {
              if (count >= orig_count)
                {
                  display_square (b, stdout);
                  found = 1;
                }
            }
          for (j = 0; j < 3; j++)
            for (k = 0; k < 3; k++)
              mpz_clear (b[j][k]);
          if (found)
            break;
        }
    }
  if (!found)
    display_square (a, stdout);
  mpz_clears (low, i, lowroot, NULL);
}

void
reduce_square (mpz_t a[3][3])
{
  int j, k;
  mpz_t low, gcd;
  mpz_inits (low, gcd, NULL);
  get_lowest (a, &low);
  mpz_set (gcd, low);
  for (j = 0; j < 3; j++)
    for (k = 0; k < 3; k++)
      {
        if (mpz_cmp (a[j][k], low) == 0)
          continue;
        mpz_gcd (gcd, gcd, a[j][k]);
      }
  if (mpz_cmp_ui (gcd, 1) > 0)
    {
      mpz_t d;
      mpz_init (d);
      int squares_retained = 1;
      for (j = 0; j < 3; j++)
        for (k = 0; k < 3; k++)
          {
            mpz_cdiv_q (d, a[j][k], gcd);
            if (mpz_perfect_square_p (a[j][k]))
              {
                if (!mpz_perfect_square_p (d))
                  {
                    squares_retained = 0;
                    break;
                  }
              }
          }
      mpz_clear (d);

      if (squares_retained)
        {
          for (j = 0; j < 3; j++)
            for (k = 0; k < 3; k++)
              mpz_cdiv_q (a[j][k], a[j][k], gcd);
        }
    }
  display_square (a, stdout);
  mpz_clears (low, gcd, NULL);
}

static int
reduce (FILE *stream)
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
      read = read_square (stream, &a, &line, &len);
      if (read == -1)
        break;
      reduce_square (a);
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
  switch (key)
    {
    case 'i':
      read_square = binary_read_square_from_stream;
      break;
    case 'o':
      display_square = display_binary_square_record;
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

struct argp argp ={options, parse_opt, 0, "Accept 3x3 magic squares from the standard input, and reduce it to have its smallest values.\vThe nine values must be separated by a comma and terminated by a newline." , 0};

int
main (int argc, char **argv)
{
  argp_parse (&argp, argc, argv, 0, 0, 0);
  is_magic_square_init ();
  return reduce (stdin);
}
