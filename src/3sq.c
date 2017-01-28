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

static void
create_three_square_progression (mpz_t m, mpz_t n, mpz_t *vec, int size, mpz_t *finalroot)
{
  if (mpz_cmp_ui (m, 86) == 0 &&
      mpz_cmp_ui (n, 29) == 0)
    {
      printf("yay\n");
    }
      //mpz_set_str (m, "86", 10);
      //mpz_set_str (n, "29", 10);
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
  mpz_inits (m, n, vec[0], vec[1], vec[3], finalroot, NULL);
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
      display_three_record_with_root (vec, &finalroot, out);
    }
  mpz_clears (m, n, vec[0], vec[1], vec[2], finalroot, NULL);
  mpz_clears (x1, _y1, z1, m12, n12, yx1dif, yx1sum, NULL);
  if (line)
    free (line);
  return 0;
}

struct argp argp ={NULL, NULL, "", "Generate arithmetic progressions of three squares, with a fourth number being the square root of the 3rd square.\vThe input of this program comes from \"seq-morgenstern-mn\".", 0 };

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  return gen_3sq (stdin, stdout);
}
