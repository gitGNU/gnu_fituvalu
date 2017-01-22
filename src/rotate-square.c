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
#include <math.h>
#include <stdlib.h>
#include <argp.h>
#include <gmp.h>
#include "magicsquareutil.h"

void (*display_square) (mpz_t s[3][3], FILE *out) = display_square_record;
int (*read_square) (FILE *, mpz_t (*)[3][3], char **, size_t *) = read_square_from_stream;

static void
rotate (mpz_t (*result)[3][3])
{
  int i, j;
  mpz_t a[3][3];
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      mpz_init (a[i][j]);

  mpz_set (a[0][0],(*result)[0][0]);
  mpz_set (a[0][1],(*result)[0][1]);
  mpz_set (a[0][2],(*result)[0][2]);
  mpz_set (a[1][0],(*result)[1][0]);
  mpz_set (a[1][1],(*result)[1][1]);
  mpz_set (a[1][2],(*result)[1][2]);
  mpz_set (a[2][0],(*result)[2][0]);
  mpz_set (a[2][1],(*result)[2][1]);
  mpz_set (a[2][2],(*result)[2][2]);
  mpz_set ((*result)[0][0], a[0][2]);
  mpz_set ((*result)[0][1], a[1][2]);
  mpz_set ((*result)[0][2], a[2][2]);
  mpz_set ((*result)[1][0], a[0][1]);
  mpz_set ((*result)[1][1], a[1][1]);
  mpz_set ((*result)[1][2], a[2][1]);
  mpz_set ((*result)[2][0], a[0][0]);
  mpz_set ((*result)[2][1], a[1][0]);
  mpz_set ((*result)[2][2], a[2][0]);
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      mpz_clear (a[i][j]);
}

static void
flip_horizontally (mpz_t (*result)[3][3])
{
  int i, j;
  mpz_t a[3][3];
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      mpz_init (a[i][j]);
  mpz_set (a[0][0], (*result)[0][2]);
  mpz_set (a[0][1], (*result)[0][1]);
  mpz_set (a[0][2], (*result)[0][0]);
  mpz_set (a[1][0], (*result)[1][2]);
  mpz_set (a[1][1], (*result)[1][1]);
  mpz_set (a[1][2], (*result)[1][0]);
  mpz_set (a[2][0], (*result)[2][2]);
  mpz_set (a[2][1], (*result)[2][1]);
  mpz_set (a[2][2], (*result)[2][0]);
  mpz_set ((*result)[0][0], a[0][0]);
  mpz_set ((*result)[0][1], a[0][1]);
  mpz_set ((*result)[0][2], a[0][2]);
  mpz_set ((*result)[1][0], a[1][0]);
  mpz_set ((*result)[1][1], a[1][1]);
  mpz_set ((*result)[1][2], a[1][2]);
  mpz_set ((*result)[2][0], a[2][0]);
  mpz_set ((*result)[2][1], a[2][1]);
  mpz_set ((*result)[2][2], a[2][2]);
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      mpz_clear (a[i][j]);
}

static void
flip_vertically (mpz_t (*result)[3][3])
{
  int i, j;
  mpz_t a[3][3];
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      mpz_init (a[i][j]);
  mpz_set (a[0][0], (*result)[2][0]);
  mpz_set (a[0][1], (*result)[2][1]);
  mpz_set (a[0][2], (*result)[2][2]);
  mpz_set (a[1][0], (*result)[1][0]);
  mpz_set (a[1][1], (*result)[1][1]);
  mpz_set (a[1][2], (*result)[1][2]);
  mpz_set (a[2][0], (*result)[0][0]);
  mpz_set (a[2][1], (*result)[0][1]);
  mpz_set (a[2][2], (*result)[0][2]);
  mpz_set ((*result)[0][0], a[0][0]);
  mpz_set ((*result)[0][1], a[0][1]);
  mpz_set ((*result)[0][2], a[0][2]);
  mpz_set ((*result)[1][0], a[1][0]);
  mpz_set ((*result)[1][1], a[1][1]);
  mpz_set ((*result)[1][2], a[1][2]);
  mpz_set ((*result)[2][0], a[2][0]);
  mpz_set ((*result)[2][1], a[2][1]);
  mpz_set ((*result)[2][2], a[2][2]);
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      mpz_clear (a[i][j]);
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

struct argp argp ={options, parse_opt, 0, "Rotate a 3x3 magic square given on the standard input, and show all the ways the square can be turned and flipped, and still be the same square.\vThe nine values must be separated by a comma and terminated by a newline.", 0};

int
main (int argc, char **argv)
{
  argp_parse (&argp, argc, argv, 0, 0, 0);
  int i, j, k;
  mpz_t a[3][3];
  mpz_t result[3][3];
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      {
        mpz_init (a[i][j]);
        mpz_init (result[i][j]);
      }

  char *line = NULL;
  size_t len = 0;
  ssize_t read = read_square (stdin, &a, &line, &len);
  free (line);
  if (read == -1)
    return 0;

  for (j = 0; j < 3; j++)
    for (k = 0; k < 3; k++)
      mpz_set (result[j][k], a[j][k]);
  for (j = 0; j < 4; j++)
    {
      rotate (&result);
      display_square (result, stdout);
    }
  flip_horizontally (&result);
  for (j = 0; j < 4; j++)
    {
      rotate (&result);
      display_square (result, stdout);
    }
  flip_vertically (&result);
  for (j = 0; j < 4; j++)
    {
      rotate (&result);
      display_square (result, stdout);
    }
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      mpz_clears (a[i][j], result[i][j], NULL);
  return 0;
}
