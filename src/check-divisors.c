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
#include <string.h>
#include <math.h>
#include <argp.h>
#include <gmp.h>
#include "magicsquareutil.h"
int in_binary;
int num_columns;

int default_divisors[] = 
{
  1608,
  1128,
  1032,
  744,
  552,
  456,
  408,
  312,
  264,
  168,
  120,
  72,
  61,
  53,
  48,
  41,
  37,
  29,
  0
};
int *divisors;
int num_divisors;

static int
check (mpz_t *num)
{
  for (int i = 0; i < num_divisors; i++)
    {
      if (!mpz_divisible_ui_p (*num, divisors[i]))
        return 0;
    }
  return 1;
}

static void
binary_check_divisors (FILE *in, FILE *out)
{
  ssize_t read;
  mpz_t vec[num_columns];
  for (int j = 0; j < num_columns; j++)
    mpz_init (vec[j]);
  while (1)
    {
      for (int j = 0; j < num_columns; j++)
        {
          read = mpz_inp_raw (vec[j], in);
          if (!read)
            break;
        }
      if (!read)
        break;
      for (int j = 0; j < num_columns; j++)
        {
          if (check (&vec[j]))
            {
              disp_record (vec, num_columns, out);
              break;
            }
        }
    }
  for (int j = 0; j < num_columns; j++)
    mpz_clear (vec[j]);
}

static void
check_divisors (FILE *in, FILE *out)
{
  ssize_t read;
  char *line = NULL;
  char *orig_line;
  size_t len = 0;
  char *sav;
  mpz_t i;
  mpz_init (i);
  while (1)
    {
      read = fv_getline (&line, &len, in);
      if (read == -1)
        break;
      orig_line = strdup (line);
      for (char *l = strtok_r (line, ",\n", &sav); l;
           l = strtok_r (NULL, ",\n", &sav))
        {
          mpz_set_str (i, l, 10);
          if (check (&i))
            fprintf (out, "%s", orig_line);
        }
      free (orig_line);
    }
  mpz_clear (i);
  if (line)
    free (line);
  return;
}


static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  char *end = NULL;
  switch (key)
    {
    case 'i':
      in_binary = 1;
      break;
    case 'n':
      num_columns = strtoull (arg, &end, 10);
      break;
    case ARGP_KEY_ARG:
        {
          char *line = NULL, *end = NULL;
          size_t len = 0;
          ssize_t read;
          FILE *fp = fopen (arg, "r");
          while (1)
            {
              read = fv_getline (&line, &len, fp);
              if (read != -1)
                break;
              divisors = realloc (divisors,
                                  (num_divisors + 1) * sizeof (int));
              divisors[num_divisors] = strtoul (line, &end, 10);
              num_divisors++;
            }
          fclose (fp);
          free (line);
          if (num_divisors == 0)
            argp_error (state, "no divisors found in %s", arg);
        }
      break;
    case ARGP_KEY_FINI:
      if (!divisors)
        {
          divisors = default_divisors;
          num_divisors = 18;
        }
      if (!num_columns && in_binary)
        argp_error (state, "when using -i, you must also use -n.");
      break;
    }
  return 0;
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "num-columns", 'n', "NUM", 0, "How many columns there are in a record (with -i)"},
  { 0 }
};

struct argp argp ={options, parse_opt, "[FILE]", "Display the records from the standard input that have a number that is divisible by a set of a divisors.\vThe default divisors are: 29, 37, 41, 48, 53, 61, 72, 120, 168, 264, 312, 408, 456, 552, 744, 1032, 1128, and 1608.  Alternative divisors can be specified in FILE.", 0 };

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  if (in_binary)
    binary_check_divisors (stdin, stdout);
  else
    check_divisors (stdin, stdout);
  return 0;
}

