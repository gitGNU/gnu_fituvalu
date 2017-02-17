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

int num_columns = 9;
int *only_show;
int num_only_show;
int (*read_numbers)(FILE *, mpz_t *, int, char **, size_t *) = read_numbers_from_stream;
void (*display_square) (mpz_t s[3][3], FILE *out) = display_square_record;

static int
display_square_count (FILE *stream)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t a[num_columns];

  int i;
  for (i = 0; i < num_columns; i++)
    mpz_init (a[i]);

  while (1)
    {
      read = read_numbers (stream, a, num_columns, &line, &len);
      if (read == -1)
        break;
      int count = 0;
      for (i = 0; i < num_columns; i++)
        {
          if (mpz_perfect_square_p (a[i]))
            count++;
        }
      if (num_only_show)
        {
          for (int i = 0; i < num_only_show; i++)
            {
              if (only_show[i] == count)
                {
                  disp_record (a, num_columns, stdout);
                  break;
                }
            }
        }
      else
        {
          fprintf (stdout, "%d\n", count);
          fflush (stdout);
        }
    }

  for (i = 0; i < num_columns; i++)
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
    case 'n':
      num_columns = atoi (arg);
      break;
    case 'i':
      read_numbers = binary_read_numbers_from_stream;
      break;
    case 'o':
      display_square = display_binary_square_record;
      break;
    case 'f':
        {
          int num = atoi (arg);
          if (num >= 0 && num <= 9)
            {
              only_show =
                realloc (only_show, (num_only_show + 1) * sizeof (int));
              only_show[num_only_show] = atoi (arg);
              num_only_show++;
            }
          else
            argp_error (state, "%s is an invalid count for -S", arg);
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
  { "filter", 'f', "NUM-SQUARES", 0, "Instead of showing the count, show the magic square if it has NUM-SQUARES perfect squares"},
  { "num-columns", 'n', "COLS", 0, "Instead of reading in 9 numbers read in this many"},
  { 0 }
};

struct argp argp ={options, parse_opt, 0, "Accept 9 numbers from the standard input, and determine if it is a magic square.  If it is, display the number of perfect squares.\vThe nine values must be separated by a comma and terminated by a newline.  Whereas other programs with the \"-squares\" suffix refers to magic squares, in this program it refers to perfect squares.  We are counting the perfect squares.  --out-binary is only used with --filter." , 0};

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  return display_square_count (stdin);
}
