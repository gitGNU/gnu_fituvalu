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

int invert;
int show_abc;
void (*display_square) (mpz_t s[3][3], FILE *out) = display_square_record;
int (*read_square) (FILE *, mpz_t (*)[3][3], char **, size_t *) = read_square_from_stream;
static int
check_one_of_four (mpz_t num, mpz_t *others)
{
  int found = 0;
  for (int i = 0; i < 4; i++)
    {
      if (mpz_cmp (others[i], num) == 0)
        {
          found = 1;
          break;
        }
    }
  return found;
}

static int
is_lucas (mpz_t s[3][3], mpz_t *abc)
{
  int found = 0;
  mpz_t diff1, diff2;
  mpz_inits (diff1, diff2, NULL);
  mpz_set (abc[2], s[1][1]);
  mpz_sub (diff1, s[0][0], abc[2]);
  mpz_sub (diff2, s[2][2], abc[2]);
  mpz_abs (diff1, diff1);
  mpz_abs (diff2, diff2);
  if (mpz_cmp (diff1, diff2) == 0)
    {
      mpz_set (abc[0], diff1);
      mpz_sub (diff1, s[0][2], abc[2]);
      mpz_sub (diff2, s[2][0], abc[2]);
      mpz_abs (diff1, diff1);
      mpz_abs (diff2, diff2);
      if (mpz_cmp (diff1, diff2) == 0)
        {
          if (mpz_cmp (diff1, abc[0]) > 0)
            mpz_set (abc[1], diff1);
          else
            {
              mpz_set (abc[1], abc[0]);
              mpz_set (abc[0], diff1);
            }
          mpz_t others[4], absum, abdif;
          mpz_inits (others[0], others[1], others[2], others[3], absum, abdif, NULL);
          mpz_add (absum, abc[0], abc[1]);
          mpz_sub (abdif, abc[0], abc[1]);
          mpz_add (others[0], abc[2], absum);
          mpz_sub (others[1], abc[2], absum);
          mpz_add (others[2], abc[2], abdif);
          mpz_sub (others[3], abc[2], abdif);
          if (check_one_of_four (s[0][1], others) &&
              check_one_of_four (s[2][1], others) &&
              check_one_of_four (s[1][0], others) &&
              check_one_of_four (s[1][2], others))
            found = 1;
          mpz_clears (others[0], others[1], others[2], others[3], absum, abdif, NULL);
        }
    }
  mpz_clears (diff1, diff2, NULL);
  return found;
}

static int
lucas_square (FILE *in, FILE *out)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t a[3][3];
  mpz_t abc[3];

  int i, j;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      mpz_init (a[i][j]);

  for (i = 0; i < 3; i++)
    mpz_init (abc[i]);
  while (1)
    {
      read = read_square (in, &a, &line, &len);
      if (read == -1)
        break;
      int magic = is_magic_square (a, 1);
      int ret = is_lucas (a, abc);
      if ((magic && invert && ret == 0) || (magic && !invert && ret) || (!magic && invert))
        {
          if (show_abc)
            display_three_record (abc, out);
          else
            display_square (a, out);
        }
    }

  for (i = 0; i < 3; i++)
      mpz_clear (abc[i]);

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
    case 'a':
      show_abc = 1;
      break;
    case 'v':
      invert = 1;
      break;
    }
  return 0;
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "inverse", 'v', 0, 0, "Show the squares that are not in the lucas family"},
  { "show-abc", 'a', 0, 0, "Instead of the showing the square, show the A, B, and C values"},
  { 0 }
};

struct argp argp ={options, parse_opt, 0, "Accept 3x3 magic squares from the standard input, and only show the ones that are in the Lucas family.\vThe nine values must be separated by a comma and terminated by a newline.  Lucas family squares have the following structure:\n"
"+-------------+-------------+-------------+\n"
"|    c - b    | c + (a + b) |    c - a    |\n"
"+-------------+-------------+-------------+\n"
"| c - (a - b) |      c      | c + (a - b) |\n"
"+-------------+-------------+-------------+\n"
"|    c + a    | c - (a + b) |    c + b    |\n"
"+-------------+-------------+-------------+\n"
  , 0};

int
main (int argc, char **argv)
{
  argp_parse (&argp, argc, argv, 0, 0, 0);
  is_magic_square_init ();
  return lucas_square (stdin, stdout);
}
