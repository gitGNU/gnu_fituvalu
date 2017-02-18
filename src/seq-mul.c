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

int num_args;
mpz_t args[3];
static void (*dump_func) (mpz_t *, FILE *);

static int
seq_mul (mpz_t start, mpz_t multiplier, mpz_t finish, FILE *out)
{
  mpz_t i;
  mpz_init (i);
  for (mpz_set (i, start); mpz_cmp_ui (finish, 0) == 0 || 
       mpz_cmp (i, finish) <= 0; mpz_mul (i, i, multiplier))
    dump_func (&i, out);
  mpz_clear (i);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'o':
      dump_func = display_binary_number;
      break;
    case ARGP_KEY_ARG:
      if (num_args == 3)
        argp_error (state, "too many arguments");
      else
        {
          mpz_set_str (args[num_args], arg, 10);
          num_args++;
        }
      break;
    case ARGP_KEY_INIT:
      dump_func = display_textual_number;
      for (int i = 0; i < 3; i++)
        mpz_init (args[i]);
      break;
    case ARGP_KEY_FINI:
      if (num_args <= 1)
        argp_error (state, "missing argument.");
      break;
    case ARGP_KEY_NO_ARGS:
      argp_error (state, "missing argument.");
      break;
    }
  return 0;
}

static struct argp_option
options[] =
{
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { 0 }
};

struct argp argp ={options, parse_opt, "FIRST MULTIPLE\nFIRST MULTIPLE LAST", "Compute a sequence of numbers in steps of MULTIPLE." , 0};

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);

  mpz_t zero;
  mpz_init (zero);
  mpz_set_ui (zero, 0);

  FILE *out = stdout;
  int ret = 0;

  switch (num_args)
    {
    case 2:
      ret = seq_mul (args[0], args[1], zero, out);
      break;
    case 3:
      ret = seq_mul (args[0], args[1], args[2], out);
      break;
    }

  mpz_clear (zero);
  return ret;
}
