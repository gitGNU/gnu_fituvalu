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

void (*display_tuple) (mpz_t s[3], FILE *out) = display_three_record;
int (*read_tuple) (FILE *, mpz_t *, char **, size_t *) = read_three_numbers_from_stream;

static int
reduce (FILE *stream, FILE *out)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t a[3];

  for (int i = 0; i < 3; i++)
    mpz_init (a[i]);

  while (1)
    {
      read = read_tuple (stream, a, &line, &len);
      if (read == -1)
        break;
      reduce_three_square_progression (a);
      display_tuple (a, out);
    }

  for (int i = 0; i < 3; i++)
    mpz_clear (a[i]);

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
      read_tuple = binary_read_three_numbers_from_stream;
      break;
    case 'o':
      display_tuple = display_binary_three_record;
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

struct argp argp ={options, parse_opt, 0, "Accept three-square arithmetic progressions from the standard input, and reduce them to their smallest values.\vThe three values must be separated by a comma and terminated by a newline, and must be in ascending order." , 0};

int
main (int argc, char **argv)
{
  argp_parse (&argp, argc, argv, 0, 0, 0);
  return reduce (stdin, stdout);
}
