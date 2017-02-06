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
int invert;
int mixed;

static int
check_func (mpz_t a)
{
  if (invert)
    return mpz_even_p (a);
  else
    return mpz_odd_p (a);
}

static int
filter (mpz_t a[3][3])
{
  if (mixed)
    {
      int found = 0;
      int result = check_func (a[0][0]);
      for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
          {
            if (check_func (a[i][j]) != result)
              {
                found = 1;
                break;
              }
          }
      return found;
    }
  else
    {
      int found = 0;
      for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
          {
            if (!check_func (a[i][j]))
              {
                found = 1;
                break;
              }
          }
      return !found;
    }
}

static int
odd_square (FILE *in, FILE *out)
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
      read = read_square (in, &a, &line, &len);
      if (read == -1)
        break;
      if (filter (a))
        display_square (a, stdout);
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
    case 'm':
      mixed = 1;
      break;
    case 'e':
      invert = 1;
      break;
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
  { "even", 'e', 0, 0, "Show the squares that are all even"},
  { "mixed", 'm', 0, 0, "Show the squares comprised of even and odd"},
  { 0 }
};

struct argp argp ={options, parse_opt, 0, "Accept 3x3 magic squares from the standard input, and displays them if they are comprised of odd numbers.\vThe nine values must be separated by a comma and terminated by a newline.", 0};

int
main (int argc, char **argv)
{
  argp_parse (&argp, argc, argv, 0, 0, 0);
  return odd_square (stdin, stdout);
}
