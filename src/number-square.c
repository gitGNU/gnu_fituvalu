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

int (*read_square) (FILE *, mpz_t (*)[3][3], char **, size_t *) = read_square_from_stream;

static int
display_magic_number (FILE *stream)
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
      mpz_t sum;
      mpz_init (sum);
      mpz_set (sum, a[0][0]);
      mpz_add (sum, sum, a[0][1]);
      mpz_add (sum, sum, a[0][2]);
      char buf[mpz_sizeinbase (sum, 10) + 2];
      mpz_get_str (buf, 10, sum);
      fprintf (stdout, "%s\n", buf);
      mpz_clear (sum);
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
    }
  return 0;
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { 0 },
};

struct argp argp ={options, parse_opt, 0, "Accept 3x3 magic squares from the standard input, and display the magic number.\vThe nine values must be separated by a comma and terminated by a newline." , 0};

int
main (int argc, char **argv)
{
  argp_parse (&argp, argc, argv, 0, 0, 0);
  return display_magic_number (stdin);
}
