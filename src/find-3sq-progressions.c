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
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <argp.h>
#include <string.h>
#include <gmp.h>
#include "magicsquareutil.h"

unsigned long long incr = 1;
unsigned long long max_tries;
mpz_t x1, _y1, z1, m12, n12, yx1dif, yx1sum;

int in_binary;
void (*display_record) (mpz_t *, mpz_t*, FILE *out) = display_three_record_with_root;

static void
create_three_square_progression (mpz_t m, mpz_t *vec, int size, mpz_t *finalroot, FILE *out)
{
  mpz_t root, nroot, diff;
  mpz_inits (root, nroot, diff, NULL);
  mpz_sqrt (root, m);
  mpz_set (vec[0], m);
  for (unsigned long long count = 0; count < max_tries; count++)
    {
      mpz_mul_ui (nroot, root, incr);
      mpz_add (vec[1], vec[1], nroot);
      mpz_add (vec[1], vec[1], nroot);
      mpz_add_ui (vec[1], vec[1], incr);
      mpz_add_ui (root, root, incr);

      mpz_sub (diff, vec[1], vec[0]);
      mpz_add (vec[2], vec[1], diff);
      if (mpz_perfect_square_p (vec[2]))
        {
          mpz_sqrt (*finalroot, vec[2]);
          display_record (vec, finalroot, out);
        }
      mpz_sub (vec[2], vec[0], diff);
      if (mpz_cmp_ui (vec[2], 0) <= 0)
        break;
      if (mpz_perfect_square_p (vec[2]))
        {
          mpz_set (*finalroot, root);
          mpz_set (diff, vec[0]);
          mpz_set (vec[0], vec[2]);
          mpz_set (vec[2], vec[1]);
          mpz_set (vec[1], diff);
          display_record (vec, finalroot, out);
        }
    }
  mpz_clears (root, nroot, diff, NULL);
}

static int
gen_3sq (FILE *in, FILE *out)
{
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  mpz_t m, vec[3], finalroot;
  mpz_inits (m, vec[0], vec[1], vec[2], finalroot, NULL);
  while (1)
    {
      read = getline (&line, &len, in);
      if (read == -1)
        break;
      mpz_set_str (m, line, 10);
      create_three_square_progression (m, vec, 3, &finalroot, out);
    }
  mpz_clears (m, vec[0], vec[1], vec[2], finalroot, NULL);
  if (line)
    free (line);
  return 0;
}

static int
gen_binary_3sq (FILE *in, FILE *out)
{
  ssize_t read;
  mpz_t m, vec[3], finalroot;
  mpz_inits (m, vec[0], vec[1], vec[2], finalroot, NULL);
  while (1)
    {
      read = mpz_inp_raw (m, in);
      if (!read)
        break;
      create_three_square_progression (m, vec, 3, &finalroot, out);
    }
  mpz_clears (m, vec[0], vec[1], vec[2], finalroot, NULL);
  return 0;
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
    case 'o':
      display_record = display_binary_three_record_with_root;
      break;
    case 'I':
      incr = strtoull (arg, &end, 10);
      break;
    case ARGP_KEY_ARG:
      if (max_tries)
        argp_error (state, "too many arguments");
      else
        max_tries = strtoull (arg, &end, 10);
      break;
    case ARGP_KEY_NO_ARGS:
      argp_error (state, "missing argument");
      break;
    }
  return 0;
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "increment", 'I', "NUM", 0, "Advance by NUM squares instead of 1"},
  { 0 }
};

struct argp argp ={options, parse_opt, "NUM", "Generate arithmetic progressions of three squares, with a fourth number being the square root of the 3rd square.\vThe input of this program comes from \"sq-seq\".", 0 };

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  if (in_binary)
    return gen_binary_3sq (stdin, stdout);
  else
    return gen_3sq (stdin, stdout);
}
