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
#include <gmp.h>
#include <argp.h>
#include <string.h>
#include <stdlib.h>
#include "magicsquareutil.h"

int show_def;
int dump_progression;
int num_filters;
int *filter_types;
int (*read_numbers)(FILE *, mpz_t (*)[SIZE], char **, size_t *) = read_numbers_from_stream;
void (*display_record) (mpz_t *, FILE *) = display_nine_record;

int
compar (const void *left, const void *right)
{
  const mpz_t *l = left;
  const mpz_t *r = right;
  return mpz_cmp (*l, *r);
}

static int
is_step_progression (mpz_t diffs[], int size)
{
  if (mpz_cmp (diffs[0], diffs[1]) == 0 &&
      mpz_cmp (diffs[2], diffs[5]) == 0 &&
      mpz_cmp (diffs[3], diffs[4]) == 0 &&
      mpz_cmp (diffs[6], diffs[7]) == 0)
    return 1;
  return 0;
}

static int
is_fulcrum_progression (mpz_t diffs[], int size)
{
  int ret = 0;
  mpz_t amt;
  mpz_init (amt);
  mpz_add (amt, diffs[1], diffs[2]);
  if (mpz_cmp (amt, diffs[0]) == 0 &&
      mpz_cmp (amt, diffs[7]) == 0 && 
      mpz_cmp (diffs[1], diffs[3]) == 0 &&
      mpz_cmp (diffs[3], diffs[4]) == 0 &&
      mpz_cmp (diffs[4], diffs[6]) == 0 &&
      mpz_cmp (diffs[2], diffs[5]) == 0)
    ret = 1;
  mpz_clear (amt);
  return ret;
}

static int
get_progression_type (mpz_t vec[], int size)
{
  int ret = 0;
  mpz_t progression[size];
  mpz_t diffs [size];

  for (int i = 0; i < size; i++)
    {
      mpz_inits (progression[i], diffs[i], NULL);
      mpz_set (progression[i], vec[i]);
    }

  qsort (progression, size, sizeof (mpz_t), compar);

  if (dump_progression)
    {
      display_record (progression, stdout);
      for (int i = 0; i < size; i++)
        mpz_clears (progression[i], diffs[i], NULL);
      return -1;
    }

  for (int i = 1; i < size; i++)
    mpz_sub (diffs[i-1], progression[i], progression[i-1]);

  if (show_def)
    {
      mpz_t v[3];
      for (int i = 0; i < 3; i++)
        mpz_init (v[i]);
      if (is_step_progression (diffs, size - 1))
        {
          mpz_sub (v[0], progression[1], progression[0]);
          mpz_sub (v[1], progression[3], progression[2]);
          display_two_record (&v[0], &v[1], stdout);
        }
      else if (is_fulcrum_progression (diffs, size - 1))
        {
          mpz_sub (v[0], progression[1], progression[0]);
          mpz_sub (v[1], progression[2], progression[1]);
          mpz_sub (v[2], progression[3], progression[2]);
          display_three_record (v, stdout);
        }
      for (int i = 0; i < 3; i++)
        mpz_clear (v[i]);
      for (int i = 0; i < size; i++)
        mpz_clears (progression[i], diffs[i], NULL);
      return -1;
    }

  if (is_step_progression (diffs, size - 1))
    ret = 1;
  else if (is_fulcrum_progression (diffs, size - 1))
    ret = 2;

  for (int i = 0; i < size; i++)
    mpz_clears (progression[i], diffs[i], NULL);
  return ret;
}

static int
find_progression_type (FILE *in, FILE *out)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  mpz_t vec[SIZE];
  mpz_t orig[SIZE];
  for (int i = 0; i < SIZE; i++)
    mpz_inits (vec[i], orig[i], NULL);
  while (1)
    {
      read = read_numbers (in, &vec, &line, &len);
      if (read == -1)
        break;
      int progression_type = get_progression_type (vec, SIZE);
      if (dump_progression || show_def)
        continue;
      if (num_filters)
        {
          for (int i = 0; i < num_filters; i++)
            if (filter_types[i] == progression_type)
              {
                display_record (vec, out);
                break;
              }
        }
      else
        fprintf (out, "%d\n", progression_type);
    }

  for (int i = 0; i < SIZE; i++)
    mpz_clears (vec[i], orig[i], NULL);

  if (line)
    free (line);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 's':
      show_def = 1;
      break;
    case 'd':
      dump_progression = 1;
      break;
    case 'o':
      display_record = display_binary_nine_record;
      break;
    case 'i':
      read_numbers = binary_read_numbers_from_stream;
      break;
    case 'f':
      filter_types = realloc (filter_types, sizeof (int) * (num_filters + 1));
      filter_types[num_filters] = atoi (arg);
      num_filters++;
      break;
    }
  return 0;
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "filter", 'f', "TYPE", 0, "Only show magic squares that have this progression type"},
  { "dump-progression", 'd', 0, OPTION_HIDDEN, "Just display the progression"},
  { "show-def", 's', 0, 0, "show the D1, E1 (and F1) values"},
  { 0 }
};

struct argp argp ={options, parse_opt, 0, "Accept 3x3 magic squares from the standard input, and determine which progression type it is.\vThe nine values must be separated by a comma and terminated by a newline.  1 is the \"step\" progression, and 2 is the \"fulcrum\" progression, and 0 is unknown. --out-binary is only used with --filter." , 0};

int
main (int argc, char **argv)
{
  argp_parse (&argp, argc, argv, 0, 0, 0);
  return find_progression_type (stdin, stdout);
}
