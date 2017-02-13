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
int in_binary;
void (*display_square) (mpz_t s[3][3], FILE *out) = display_square_record;
mpz_t max, startb, startc, starte;

//from:
//http://www.oddwheel.com/square_sequence%20tableGV.html
//an algorithm described by Eddie N Gutierrez. 
//
//it looks like c must be a prime number
//but getting an e value is more difficult
//any even number over 3 and under c?
static void
generate_gutierrez_squares (mpz_t sq[3][3], mpz_t d, mpz_t e, mpz_t f, mpz_t delta, FILE *out)
{
  mpz_t a, b, c, absum, abdif;
  mpz_inits (a, b, c, absum, abdif, NULL);
  mpz_set (sq[2][0], d);
  mpz_set (sq[1][1], e);
  mpz_set (sq[0][2], f);
  mpz_sub (a, sq[1][1], sq[0][2]);
  mpz_set (c, e);
  mpz_set (b, a);
  mpz_add_ui (b, b, 1);
  for (;mpz_cmp (b, c) < 0; mpz_add_ui (b, b, 1))
    {
      int num_squares = 3;
      mpz_add (absum, a, b);
      mpz_sub (abdif, a, b);
      mpz_sub (sq[0][0], c, b);
      if (mpz_perfect_square_p (sq[0][0]))
        num_squares++;
      mpz_add (sq[0][1], c, absum);
      if (mpz_perfect_square_p (sq[0][1]))
        num_squares++;
      mpz_sub (sq[1][0], c, abdif);
      if (mpz_perfect_square_p (sq[1][0]))
        num_squares++;
      mpz_add (sq[1][2], c, abdif);
      if (mpz_perfect_square_p (sq[1][2]))
        num_squares++;
      mpz_sub (sq[2][1], c, absum);
      if (mpz_perfect_square_p (sq[2][1]))
        num_squares++;
      mpz_add (sq[2][2], c, b);
      if (mpz_perfect_square_p (sq[2][2]))
        num_squares++;
      if (num_squares > 5)
        display_square (sq, out);
    }

  mpz_clears (a, b, c, absum, abdif, NULL);
}

static int
gutierrez (FILE *out)
{
  mpz_t a, b, c, k, m, d, s, f, na, nb, nc, delta1, delta2, j, e, g, a2, b2,
        c2, threeb2, na2, nb2, nc2, n, sq[3][3];
  mpz_inits (a, b, c, k, m, d, s, f, na, nb, nc, delta1, delta2, j, e, g, a2,
             b2, c2, threeb2, na2, nb2, nc2, n, NULL);
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      mpz_init (sq[x][y]);
  mpz_set_ui (a, 1);
  mpz_set (b, startb);
  mpz_set (c, startc);

  if (mpz_cmp_ui (startc, 1) != 0 &&
      mpz_cmp_ui (startb, 1) != 0)
    return 0;
  else if (mpz_cmp_ui (startc, 1) != 0)
    {
      mpz_set (k, startc);
      // FIXME: there is a bug with startc being < 0 here
      // every other row gets skipped/misaligned wrt delta1/delta2
      mpz_sub_ui (m, k, 1);
      mpz_cdiv_q_ui (m, m, 2);
      mpz_mul_si (d, m, -4);
      mpz_sub_ui (j, k, 1);
      mpz_mod_ui (j, j, 64);
    }
  else if (mpz_cmp_ui (startb, 1) != 0)
    {
      mpz_set (k, startb);
      mpz_sub_ui (m, k, 1);
      mpz_mul_ui (d, m, 4);
      mpz_mod_ui (j, k, 64);
    }
  else
    return 0;

  switch (mpz_get_ui (j))
    {
    case 1:
    case 33:
      mpz_cdiv_q_ui (e, d, 16);
      break;
    case 9:
    case 17:
    case 25:
    case 41:
    case 49:
    case 57:
      mpz_cdiv_q_ui (e, d, 8);
      break;
    default:
      mpz_cdiv_q_ui (e, d, 4);
      break;
    }
  if (mpz_cmp_ui (starte, 0) != 0)
    mpz_set (e, starte);
  mpz_mul_ui (g, e, 2);
  for (mpz_set_ui (n, 0); mpz_cmp (n, max) < 0; mpz_add_ui (n, n, 1))
    {
      mpz_mul (a2, a, a);
      mpz_mul (b2, b, b);
      mpz_mul (c2, c, c);
      mpz_add (s, a2, b2);
      mpz_add (s, s, c2);
      mpz_mul_ui (threeb2, b2, 3);
      mpz_sub (s, s, threeb2);
      //s = (a*a) + (b*b) + (c*c) - (3*b*b);
      if (mpz_cmp_ui (startb, 0) < 0)
        {
          if (mpz_cmp_ui (d, 0) < 0)
            mpz_cdiv_q (f, s, d);
          else
            mpz_fdiv_q (f, s, d);
        }
      else
        {
          if (mpz_cmp_ui (d, 0) < 0)
            mpz_fdiv_q (f, s, d);
          else
            mpz_cdiv_q (f, s, d);
        }
      mpz_add (na, a, f);
      mpz_add (nb, b, f);
      mpz_add (nc, c, f);
      mpz_mul (na2, na, na);
      mpz_mul (nb2, nb, nb);
      mpz_mul (nc2, nc, nc);
      mpz_sub (delta1, nb2, na2);
      mpz_sub (delta2, nc2, nb2);

      //printf ("%d, %d, %d, %d, %d, %d, %d, %d, %d\n", mpz_get_si(a), mpz_get_si(b), mpz_get_si(c), mpz_get_si(f), mpz_get_si(na), mpz_get_si(nb), mpz_get_si(nc), mpz_get_si(delta1), mpz_get_si(delta2));
      if (mpz_cmp (delta1, delta2) == 0)
        generate_gutierrez_squares (sq, nc2, nb2, na2, delta1, out);
      mpz_add (b, b, e);
      mpz_add (c, c, g);
    }
  mpz_clears (a, b, c, k, m, d, s, f, na, nb, nc, delta1, delta2, j, e, g, a2,
              b2, c2, threeb2, na2, nb2, nc2, n, NULL);
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      mpz_clear (sq[x][y]);
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
      display_square = display_binary_square_record;
      break;
    case ARGP_KEY_ARG:
      if (num_args == 4)
        argp_error (state, "too many arguments");
      else
        {
          switch (num_args)
            {
            case 0:
              mpz_set_str (max, arg, 10);
              break;
            case 1:
              mpz_set_str (startb, arg, 10);
              break;
            case 2:
              mpz_set_str (startc, arg, 10);
              break;
            case 3:
              mpz_set_str (starte, arg, 10);
              break;
            }
          num_args++;
        }
      break;
    case ARGP_KEY_INIT:
      mpz_init_set_ui (startc, 3);
      mpz_init_set_ui (startb, 1);
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
  { 0 }
};

struct argp argp ={options, parse_opt, "MAX [B [C [E]]]", "Generate a 3x3 magic square according to Eddie N. Gutierrez' algorithm outlined on www.oddwheel.com.\vMAX is how many times we're going to try to make a progression in the sequence.  Either B or C must be 1, the other must be prime.  E is another step value." , 0};

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  int ret = gutierrez (stdout);
  return ret;
}
