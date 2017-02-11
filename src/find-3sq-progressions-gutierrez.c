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
int in_binary;
void (*display_record) (mpz_t *, mpz_t*, FILE *out) = display_three_record_with_root;
mpz_t max, startc, starte;

//from:
//http://www.oddwheel.com/square_sequence%20tableGV.html
//an algorithm described by Eddie N Gutierrez. 
//
//it looks like c must be a prime number
//but getting an e value is more difficult
//any even number over 3 and under c?
static int
gutierrez (FILE *out)
{
  mpz_t a, b, c, d, e, s, f, g, twob, n, n2, fourc, fourb, e2, en, twob2, c2;
  mpz_t na, nb, nc, delta1, delta2, na2, nb2, nc2;
  mpz_t ob, oc, fourcfourbdiff, fourcfourbdiffen, onetwob2dif, onetwob2difc2sum;
  mpz_inits (a, b, c, d, e, s, f, g, twob, n, n2, fourc, fourb, e2, en, twob2, c2, na, nb, nc, delta1, delta2, na2, nb2, nc2, ob, oc, fourcfourbdiff, fourcfourbdiffen, onetwob2dif, onetwob2difc2sum, NULL);

  mpz_set_ui (a, 1);
  mpz_set_ui (b, 1);
  mpz_set (c, startc);

  if (mpz_cmp_ui (starte, 0) == 0)
    mpz_sub (e, c, b);
  else
    mpz_set (e, starte);
  mpz_mul_ui (g, e, 2);
  mpz_mul (e2, e, e);
  mpz_set (ob, b);
  mpz_set (oc, c);

  mpz_mul (c2, oc, oc);
  mpz_mul_ui (fourc, oc, 4);
  mpz_mul_ui (fourb, ob, 4);
  mpz_mul_ui (twob, ob, 2);
  mpz_add (twob2, twob, twob);
  for (mpz_set_ui (n, 0); mpz_cmp (n, max) < 0; mpz_add_ui (n, n, 1))
    {
      mpz_mul (n2, n, n);
      mpz_mul (en, e, n);

      mpz_mul (s, e2, n2);
      mpz_mul_ui (s, s, 2);

      mpz_sub (fourcfourbdiff, fourc, fourb);
      mpz_mul (fourcfourbdiffen, fourcfourbdiff, en);
      mpz_add (s, s, fourcfourbdiffen);

      mpz_set_ui (onetwob2dif, 1);
      mpz_sub (onetwob2dif, onetwob2dif, twob2);
      mpz_add (onetwob2difc2sum, onetwob2dif, c2);
      mpz_add (s, s, onetwob2difc2sum);

      mpz_sub (d, twob, oc);
      mpz_sub_ui (d, d, 1);
      mpz_mul_ui (d, d, 2);
      if (mpz_cmp_ui (d, 0) < 0)
        mpz_fdiv_q (f, s, d);
      else
        mpz_cdiv_q (f, s, d);
      //printf("s is %d, d is %d\n", mpz_get_si (s), mpz_get_si (d));
      mpz_add (na, a, f);
      mpz_add (nb, b, f);
      mpz_add (nc, c, f);
      mpz_mul (nc2, nc, nc);
      mpz_mul (nb2, nb, nb);
      mpz_mul (na2, na, na);
      mpz_sub (delta1, nb2, na2);
      mpz_sub (delta2, nc2, nb2);
      //printf ("%d, %d, %d, %d, %d, %d, %d, %d, %d\n", mpz_get_si(a), mpz_get_si(b), mpz_get_si(c), mpz_get_si(f), mpz_get_si(na), mpz_get_si(nb), mpz_get_si(nc), mpz_get_si(delta1), mpz_get_si(delta2));
      if (mpz_cmp (delta1, delta2) == 0)
        {
          mpz_t root;
          mpz_init (root);
          if (showroot)
            mpz_sqrt (root, na2);
          mpz_t progression[3];
          mpz_init_set (progression[0], nc2);
          mpz_init_set (progression[1], nb2);
          mpz_init_set (progression[2], na2);
          if (show_diff)
            {
              mpz_abs (delta1, delta1);
              char buf[mpz_sizeinbase (delta1, 10) + 2];
              mpz_get_str (buf, 10, delta1);
              fprintf (out, "%s, ", buf);
            }
          display_record (progression, &root, out);
          for (int i = 0; i < 3; i++)
            mpz_clear (progression[i]);
          mpz_clear (root);
        }
      mpz_add (b, b, e);
      mpz_add (c, c, g);
    }
  mpz_clears (a, b, c, d, e, s, f, g, twob, n, n2, fourc, fourb, e2, en, twob2, c2, na, nb, nc, delta1, delta2, na2, nb2, nc2, ob, oc, fourcfourbdiff, fourcfourbdiffen, onetwob2dif, onetwob2difc2sum, NULL);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'd':
      show_diff = 1;
      break;
    case 'n':
      showroot = 0;
      break;
    case 'i':
      in_binary = 1;
      break;
    case 'o':
      display_record = display_binary_three_record_with_root;
      break;
    case ARGP_KEY_ARG:
      if (num_args == 3)
        argp_error (state, "too many arguments");
      else
        {
          switch (num_args)
            {
            case 0:
              mpz_set_str (max, arg, 10);
              break;
            case 1:
              mpz_set_str (startc, arg, 10);
              break;
            case 2:
              mpz_set_str (starte, arg, 10);
              break;
            }
          num_args++;
        }
      break;
    case ARGP_KEY_INIT:
      mpz_init_set_ui (startc, 3);
      mpz_init (starte);
      mpz_init (max);
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
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "no-root", 'n', 0, 0, "Don't show the root of the fourth number"},
  { "show-diff", 'd', 0, 0, "Also show the diff"},
  { 0 }
};

struct argp argp ={options, parse_opt, "MAX [C [E]]", "Find an arithmetic progression consisting of three squares, and that is suitable to be the right diagonal of a 3x3 magic square.\vMAX is how many times we're going to try to make a progression in the sequence.  C is a prime number, and E is an even number over 3 and under C." , 0};

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  int ret = gutierrez (stdout);
  return ret;
}
