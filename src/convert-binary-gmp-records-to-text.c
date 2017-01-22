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
int num_columns = 9;

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'n':
      num_columns = atoi (arg);
      break;
    }
  return 0;
}

static struct argp_option
options[] =
{
  { "num-columns", 'n', "NUM", 0, "a record has NUM columns (default 9)"},
  { 0 }
};

struct argp argp ={options, parse_opt, NULL, "Convert raw GMP numbers on the standard input to their textual representation on the standard output.", 0 };

static void
dump_num (mpz_t *i, FILE *out)
{
  char buf[mpz_sizeinbase (*i, 10) + 2];
  mpz_get_str (buf, 10, *i);
  fprintf (out, "%s", buf);
}

static void
convert_binary_records (FILE *in, FILE *out)
{
  mpz_t i;
  mpz_init (i);
  ssize_t read;
  int count = 0;
  while (1)
    {
      count++;
      read = mpz_inp_raw (i, in);
      if (!read)
        break;
      dump_num (&i, out);
      fprintf (out, ", ");
      if (count == num_columns)
        {
          fprintf (out, "\n");
          count = 0;
        }
    }
  mpz_clear (i);
}

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  convert_binary_records (stdin, stdout);
  return 0;
}

