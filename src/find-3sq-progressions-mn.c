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

mpz_t x1, _y1, z1, m12, n12, yx1dif, yx1sum;

int in_binary;
void (*display_record) (mpz_t *, mpz_t*, FILE *out) = display_three_record_with_root;

static void
create_three_square_progression (mpz_t m, mpz_t n, mpz_t *vec, int size, mpz_t *finalroot)
{
  //where X1 = 2*m1*n1,  Y1 = m1^2-n1^2,  Z1 = m1^2+n1^2,
  mpz_set (x1, m);
  mpz_mul (x1, x1, n);
  mpz_mul_ui (x1, x1, 2);

  mpz_set (m12, m);
  mpz_mul (m12, m12, m);

  mpz_set (n12, n);
  mpz_mul (n12, n12, n);

  mpz_sub (_y1, m12, n12);

  mpz_add (z1, m12, n12);

  mpz_sub (yx1dif, _y1, x1);

  mpz_add (yx1sum, _y1, x1);

  mpz_mul (vec[0], yx1dif, yx1sum);
  mpz_mul (vec[1], z1, yx1sum);
  mpz_mul (vec[2], yx1sum, yx1sum);

  mpz_mul (vec[0], vec[0], vec[0]);
  mpz_mul (vec[1], vec[1], vec[1]);
  mpz_set (*finalroot, vec[2]);
  mpz_mul (vec[2], vec[2], vec[2]);
}

static int
gen_3sq (FILE *in, FILE *out)
{
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  mpz_t m, n, vec[3], finalroot;
  mpz_inits (m, n, vec[0], vec[1], vec[2], finalroot, NULL);
  mpz_inits (x1, _y1, z1, m12, n12, yx1dif, yx1sum, NULL);
  while (1)
    {
      read = getdelim (&line, &len, ',', in);
      if (read == -1)
        break;
      char *comma = strchr (line, ',');
      if (comma)
        *comma = '\0';
      mpz_set_str (m, line, 10);
      read = getline (&line, &len, in);
      if (read == -1)
        break;
      mpz_set_str (n, line, 10);
      create_three_square_progression (m, n, vec, 3, &finalroot);
      display_record (vec, &finalroot, out);
    }
  mpz_clears (m, n, vec[0], vec[1], vec[2], finalroot, NULL);
  mpz_clears (x1, _y1, z1, m12, n12, yx1dif, yx1sum, NULL);
  if (line)
    free (line);
  return 0;
}

static int
gen_binary_3sq (FILE *in, FILE *out)
{
  ssize_t read;
  mpz_t m, n, vec[3], finalroot;
  mpz_inits (m, n, vec[0], vec[1], vec[2], finalroot, NULL);
  mpz_inits (x1, _y1, z1, m12, n12, yx1dif, yx1sum, NULL);
  while (1)
    {
      read = mpz_inp_raw (m, in);
      if (!read)
        break;
      read = mpz_inp_raw (n, in);
      if (!read)
        break;
      create_three_square_progression (m, n, vec, 3, &finalroot);
      display_record (vec, &finalroot, out);
    }
  mpz_clears (m, n, vec[0], vec[1], vec[2], finalroot, NULL);
  mpz_clears (x1, _y1, z1, m12, n12, yx1dif, yx1sum, NULL);
  return 0;
}


static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'i':
      in_binary = 1;
      break;
    case 'o':
      display_record = display_binary_three_record_with_root;
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

struct argp argp ={options, parse_opt, "", "Generate arithmetic progressions of three squares, with a fourth number being the square root of the 3rd square.\vThe input of this program comes from \"seq-morgenstern-mn\".", 0 };

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