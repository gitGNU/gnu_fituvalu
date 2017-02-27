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
#include <argp.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "magicsquareutil.h"

int num_args;
int showroot = 1;
int show_diff;
void (*display_record) (mpz_t *, mpz_t*, FILE *out) = display_three_record_with_root;
mpz_t max, start;

//from http://www.math.uconn.edu/~kconrad/blurbs/ugradnumthy/3squarearithprog.pdf
//
//  m^2 - 2m - 1   -m^2 - 2m + 1
//  ------------ + ------------- = 2
//     m^2 + 1        m^2 + 1 
//
//   a   c
//   - + - = 2
//   b   b
//
//  and the progression is:  a^2, b^2, c^2
//
mpz_t m2, twom;

static void
generate_progression (mpz_t m, mpz_t *progression)
{
  mpz_mul (m2, m, m);
  mpz_add (twom, m, m);
  mpz_add_ui (progression[1], m2, 1);
  mpz_sub (progression[0], m2, twom);
  mpz_sub_ui (progression[0], progression[0], 1);
  mpz_mul_si (progression[2], m2, -1);
  mpz_sub (progression[2], progression[2], twom);
  mpz_add_ui (progression[2], progression[2], 1);

  for (int i = 0; i < 3; i++)
    mpz_mul (progression[i], progression[i], progression[i]);
}

static int
conrad (FILE *out)
{
  mpz_t i, n;
  mpz_t progression[3], root, diff;
  mpz_inits (m2, twom, NULL);
  mpz_inits (i, progression[0], progression[1], progression[2], root, diff, NULL);
  mpz_set (n, start);
  for (mpz_set_ui (i, 1); mpz_cmp (i, max) < 0; mpz_add_ui (i, i, 1))
    {
      generate_progression (n, progression);
      if (show_diff)
        {
          mpz_sub (diff, progression[1], progression[0]);
          if (display_record == display_binary_three_record_with_root)
            mpz_out_raw (out, diff);
          else
            {
              char buf[mpz_sizeinbase (diff, 10) + 2];
              mpz_get_str (buf, 10, diff);
              fprintf (out, "%s, ", buf);
            }
        }
      if (showroot)
        mpz_sqrt (root, progression[2]);
      display_record (progression, &root, out);
      mpz_add_ui (n, n, 1);
    }
  mpz_clears (i, progression[0], progression[1], progression[2], root, diff, NULL);
  mpz_clears (m2, twom, NULL);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 's':
      mpz_set_str (start, arg, 10);
      break;
    case 'd':
      show_diff = 1;
      break;
    case 'n':
      showroot = 0;
      break;
    case 'o':
      display_record = display_binary_three_record_with_root;
      break;
    case ARGP_KEY_ARG:
      if (num_args == 1)
        argp_error (state, "too many arguments");
      else
        {
          switch (num_args)
            {
            case 0:
              mpz_set_str (max, arg, 10);
              break;
            }
          num_args++;
        }
      break;
    case ARGP_KEY_INIT:
      mpz_init (max);
      mpz_init_set_ui (start, 1);
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
  { "no-root", 'n', 0, 0, "Don't show the root of the fourth number"},
  { "show-diff", 'd', 0, 0, "Also show the diff"},
  { "start", 's', "NUM", 0, "Start at NUM instead of 1"},
  { 0 }
};

struct argp argp ={options, parse_opt, "MAX", "Find an arithmetic progression consisting of three squares.\vMAX is the number of iterations. \"conrad\" refers to Keith Conrad, and his paper 'Arithmetic Progressions of Three Squares'." , 0};

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  int ret = conrad (stdout);
  return ret;
}
