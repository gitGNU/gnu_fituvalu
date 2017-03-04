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

struct fv_app_gutierrez_square_t
{
  int slow_method;
  int num_args;
  void (*display_square) (mpz_t s[3][3], FILE *out);
  mpz_t max, startb, startc, starte;
};

//from:
//http://www.oddwheel.com/square_sequence%20tableGV.html
//an algorithm described by Eddie N Gutierrez. 
//
//it looks like c must be a prime number
//but getting an e value is more difficult
//any even number over 3 and under c?
static void
generate_gutierrez_squares (struct fv_app_gutierrez_square_t *app, mpz_t sq[3][3], mpz_t d, mpz_t e, mpz_t f, mpz_t delta, FILE *out)
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
        app->display_square (sq, out);
    }

  mpz_clears (a, b, c, absum, abdif, NULL);
}

static int
slow_gutierrez (struct fv_app_gutierrez_square_t *app, FILE *out)
{
  mpz_t a, b, c, d, e, s, f, g, twob, n, n2, fourc, fourb, e2, en, twob2, c2;
  mpz_t na, nb, nc, delta1, delta2, na2, nb2, nc2, sq[3][3];
  mpz_t ob, oc, fourcfourbdiff, fourcfourbdiffen, onetwob2dif, onetwob2difc2sum;
  mpz_inits (a, b, c, d, e, s, f, g, twob, n, n2, fourc, fourb, e2, en, twob2, c2, na, nb, nc, delta1, delta2, na2, nb2, nc2, ob, oc, fourcfourbdiff, fourcfourbdiffen, onetwob2dif, onetwob2difc2sum, NULL);
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      mpz_init (sq[x][y]);

  mpz_set_ui (a, 1);
  mpz_set (b, app->startb);
  mpz_set (c, app->startc);

  if (mpz_cmp_ui (app->starte, 0) == 0)
    mpz_sub (e, c, b);
  else
    mpz_set (e, app->starte);
  mpz_mul_ui (g, e, 2);
  mpz_mul (e2, e, e);
  mpz_set (ob, b);
  mpz_set (oc, c);

  mpz_mul (c2, oc, oc);
  mpz_mul_ui (fourc, oc, 4);
  mpz_mul_ui (fourb, ob, 4);
  mpz_mul_ui (twob, ob, 2);
  mpz_mul (twob2, ob, ob);
  mpz_mul_ui (twob2, twob2, 2);
  for (mpz_set_ui (n, 0); mpz_cmp (n, app->max) < 0; mpz_add_ui (n, n, 1))
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
      //dogs breakfast starting here
      if (mpz_cmp_ui (ob, 0) < 0)
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
      //and ending here.

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
      if (mpz_cmp (delta1, delta2) == 0 &&
          mpz_cmp (na2, nb2) != 0 &&
          mpz_cmp (na2, nc2) != 0)
        {
          if (mpz_cmp (nc2, na2) > 0)
            generate_gutierrez_squares (app, sq, na2, nb2, nc2, delta1, out);
          else
            generate_gutierrez_squares (app, sq, nc2, nb2, na2, delta1, out);
        }
      mpz_add (b, b, e);
      mpz_add (c, c, g);
    }
  mpz_clears (a, b, c, d, e, s, f, g, twob, n, n2, fourc, fourb, e2, en, twob2, c2, na, nb, nc, delta1, delta2, na2, nb2, nc2, ob, oc, fourcfourbdiff, fourcfourbdiffen, onetwob2dif, onetwob2difc2sum, NULL);
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      mpz_clear (sq[x][y]);
  return 0;
}

static int
gutierrez (struct fv_app_gutierrez_square_t *app, FILE *out)
{
  mpz_t a, b, c, k, m, d, s, f, na, nb, nc, delta1, delta2, j, e, g, a2, b2,
        c2, threeb2, na2, nb2, nc2, n, sq[3][3];
  mpz_inits (a, b, c, k, m, d, s, f, na, nb, nc, delta1, delta2, j, e, g, a2,
             b2, c2, threeb2, na2, nb2, nc2, n, NULL);
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      mpz_init (sq[x][y]);
  mpz_set_ui (a, 1);
  mpz_set (b, app->startb);
  mpz_set (c, app->startc);

  if (mpz_cmp_ui (app->startc, 1) != 0 &&
      mpz_cmp_ui (app->startb, 1) != 0)
    return 0;
  else if (mpz_cmp_ui (app->startc, 1) != 0)
    {
      mpz_set (k, app->startc);
      // FIXME: there is a bug with startc being < 0 here
      // every other row gets skipped/misaligned wrt delta1/delta2
      mpz_sub_ui (m, k, 1);
      mpz_cdiv_q_ui (m, m, 2);
      mpz_mul_si (d, m, -4);
      mpz_sub_ui (j, k, 1);
      mpz_mod_ui (j, j, 64);
    }
  else if (mpz_cmp_ui (app->startb, 1) != 0)
    {
      mpz_set (k, app->startb);
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
  if (mpz_cmp_ui (app->starte, 0) != 0)
    mpz_set (e, app->starte);
  mpz_mul_ui (g, e, 2);
  for (mpz_set_ui (n, 0); mpz_cmp (n, app->max) < 0; mpz_add_ui (n, n, 1))
    {
      mpz_mul (a2, a, a);
      mpz_mul (b2, b, b);
      mpz_mul (c2, c, c);
      mpz_add (s, a2, b2);
      mpz_add (s, s, c2);
      mpz_mul_ui (threeb2, b2, 3);
      mpz_sub (s, s, threeb2);
      //s = (a*a) + (b*b) + (c*c) - (3*b*b);
      if (mpz_cmp_ui (app->startb, 0) < 0)
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
      if (mpz_cmp (delta1, delta2) == 0 &&
          mpz_cmp (na2, nb2) != 0 &&
          mpz_cmp (na2, nc2) != 0)
        {
          if (mpz_cmp (nc2, na2) > 0)
            generate_gutierrez_squares (app, sq, na2, nb2, nc2, delta1, out);
          else
            generate_gutierrez_squares (app, sq, nc2, nb2, na2, delta1, out);
        }
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
  struct fv_app_gutierrez_square_t *app = (struct fv_app_gutierrez_square_t *) state->input;
  switch (key)
    {
    case 's':
      app->slow_method = 1;
      break;
    case 'o':
      app->display_square = display_binary_square_record;
      break;
    case ARGP_KEY_ARG:
      if (app->num_args == 4)
        argp_error (state, "too many arguments");
      else
        {
          switch (app->num_args)
            {
            case 0:
              mpz_set_str (app->max, arg, 10);
              break;
            case 1:
              mpz_set_str (app->startb, arg, 10);
              break;
            case 2:
              mpz_set_str (app->startc, arg, 10);
              break;
            case 3:
              mpz_set_str (app->starte, arg, 10);
              break;
            }
          app->num_args++;
        }
      break;
    case ARGP_KEY_INIT:
      mpz_init_set_ui (app->startc, 3);
      mpz_init_set_ui (app->startb, 1);
      mpz_init (app->starte);
      mpz_init (app->max);
      setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
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
  { "slow-method", 's', 0, OPTION_HIDDEN, "Use the slow method for calculations"},
  { 0 }
};

static struct argp argp ={options, parse_opt, "MAX [B [C [E]]]", "Generate a 3x3 magic square according to Eddie N. Gutierrez' algorithm outlined on www.oddwheel.com.\vMAX is how many times we're going to try to make a progression in the sequence.  Either B or C must be 1, the other must be prime.  E is another step value." , 0};

int
fituvalu_gutierrez_square (struct fv_app_gutierrez_square_t *app, FILE *out)
{
  int ret;
  if (app->slow_method)
    ret = slow_gutierrez (app, out);
  else
    ret = gutierrez (app, out);
  return ret;
}

int
main (int argc, char **argv)
{
  struct fv_app_gutierrez_square_t app;
  memset (&app, 0, sizeof (app));
  app.display_square = display_square_record;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_gutierrez_square (&app, stdout);
}
