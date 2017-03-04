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

//from:
//http://www.multimagie.com/English/Morgenstern06.htm
struct fv_app_hillyer_square_t
{
  int in_binary;
  int show_ratio;
  void (*display_square) (mpz_t s[3][3], FILE *out);
};

static void
generate_hillyer_square (mpz_t sq[3][3], mpz_t k, mpz_t l)
{
  mpz_t k2, kl, l2, twokl, p, q, r, s, v, u;
  mpz_t u2, p2, q2, r2, s2, v2, pq, twopq, rs, twors, uv, twouv;
  mpz_inits (k2, kl, l2, twokl, p, q, r, s, v, u, NULL);
  mpz_inits (u2, p2, q2, r2, s2, v2, pq, twopq, rs, twors, uv, twouv, NULL);

  mpz_mul (k2, k, k);
  mpz_mul (l2, l, l);
  mpz_mul (kl, k, l);
  mpz_mul_ui (twokl, kl, 2);

  mpz_add (p, k2, kl);
  mpz_add (p, p, l2);

  mpz_sub (q, k2, l2);

  mpz_add (r, k2, kl);
  mpz_add (r, r, l2);

  mpz_add (s, twokl, l2);

  mpz_add (v, k2, kl);
  mpz_add (v, v, l2);

  mpz_add (u, twokl, k2);

  mpz_mul (p2, p, p);
  mpz_mul (q2, q, q);
  mpz_mul (r2, r, r);
  mpz_mul (s2, s, s);
  mpz_mul (u2, u, u);
  mpz_mul (v2, v, v);

  mpz_mul (pq, p, q);
  mpz_mul_ui (twopq, pq, 2);
  mpz_mul (rs, r, s);
  mpz_mul_ui (twors, rs, 2);
  mpz_mul (uv, u, v);
  mpz_mul_ui (twouv, uv, 2);

  mpz_add (sq[0][0], u2, v2);
  mpz_sub (sq[0][1], p2, q2);
  mpz_sub (sq[0][1], sq[0][1], twopq);
  mpz_abs (sq[0][1], sq[0][1]);

  mpz_sub (sq[0][2], r2, s2);
  mpz_add (sq[0][2], sq[0][2], twors);

  mpz_sub (sq[1][0], p2, q2);
  mpz_add (sq[1][0], sq[1][0], twopq);

  mpz_add (sq[1][1], r2, s2);

  mpz_sub (sq[1][2], u2, v2);
  mpz_sub (sq[1][2], sq[1][2], twouv);
  mpz_abs (sq[1][2], sq[1][2]);

  mpz_sub (sq[2][0], r2, s2);
  mpz_sub (sq[2][0], sq[2][0], twors);
  mpz_abs (sq[2][0], sq[2][0]);

  mpz_sub (sq[2][1], u2, v2);
  mpz_add (sq[2][1], sq[2][1], twouv);

  mpz_add (sq[2][2], p2, q2);

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_mul (sq[i][j], sq[i][j], sq[i][j]);
  mpz_clears (u2, p2, q2, r2, s2, v2, pq, twopq, rs, twors, uv, twouv, NULL);
  mpz_clears (k2, kl, l2, twokl, p, q, r, s, v, u, NULL);
}

int
hillyer (struct fv_app_hillyer_square_t *app, FILE *in, FILE *out)
{
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  mpz_t k, l, sq[3][3];
  mpz_inits (k, l, NULL);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_init (sq[i][j]);
  while (1)
    {
      read = fv_getdelim (&line, &len, ',', in);
      if (read == -1)
        break;
      char *comma = strchr (line, ',');
      if (comma)
        *comma = '\0';
      mpz_set_str (k, line, 10);
      read = fv_getline (&line, &len, in);
      if (read == -1)
        break;
      mpz_set_str (l, line, 10);
      generate_hillyer_square (sq, k, l);
      if (app->show_ratio)
        {
          mpf_t sum1f, sum2f, ratio;
          mpz_t sum1, sum2;
          mpz_inits (sum1, sum2, NULL);
          mpz_add (sum1, sq[0][0], sq[0][1]);
          mpz_add (sum1, sum1, sq[0][2]);
          mpz_add (sum2, sq[0][2], sq[1][1]);
          mpz_add (sum2, sum2, sq[2][0]);
          mpf_inits (sum1f, sum2f, ratio, NULL);
          mpf_set_z (sum1f, sum1);
          mpf_set_z (sum2f, sum2);
          if (mpz_cmp (sum1, sum2) > 0)
            mpf_div (ratio, sum1f, sum2f);
          else
            mpf_div (ratio, sum2f, sum1f);
            {
              mpf_out_str (out, 10, 0, ratio);
              fprintf (out, ", ");
            }
          mpf_clears (sum1f, sum2f, ratio, NULL);

            {
              char buf[mpz_sizeinbase (k, 10) + 2];
              mpz_get_str (buf, 10, k);
              fprintf (out, "%s, ", buf);
            }
            {
              char buf[mpz_sizeinbase (l, 10) + 2];
              mpz_get_str (buf, 10, l);
              fprintf (out, "%s\n", buf);
              fflush (out);
            }
          mpz_clears (sum1, sum2, NULL);
        }
      else
        app->display_square (sq, out);
    }
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_clear (sq[i][j]);
  mpz_clears (k, l, NULL);
  if (line)
    free (line);
  return 0;
}

static int
binary_hillyer (struct fv_app_hillyer_square_t *app, FILE *in, FILE *out)
{
  ssize_t read;
  mpz_t k, l, sq[3][3];
  mpz_inits (k, l, NULL);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_init (sq[i][j]);
  while (1)
    {
      read = mpz_inp_raw (k, in);
      if (!read)
        break;
      read = mpz_inp_raw (l, in);
      if (!read)
        break;
      generate_hillyer_square (sq, k, l);
      app->display_square (sq, out);
    }
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_clear (sq[i][j]);
  mpz_clears (k, l, NULL);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_hillyer_square_t *app = (struct fv_app_hillyer_square_t *) state->input;
  switch (key)
    {
    case 'i':
      app->in_binary = 1;
      break;
    case 'o':
      app->display_square = display_binary_square_record;
      break;
    case 's':
      app->show_ratio = 1;
      break;
    case ARGP_KEY_INIT:
      setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
      break;
    }
  return 0;
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "show-ratio", 's', 0, 0, "Instead of showing squares, show how close they are to magic"},
  { 0 }
};

static struct argp argp ={options, parse_opt, 0, "Generate a nearly-magic 3x3 square according to Lee Morgenstern's algorithm that employes hillyer's formula on multimagie.com.  These squares are generated from two input values on the standard input, K and L, separated by a comma." , 0};

int
fituvalu_hillyer_square (struct fv_app_hillyer_square_t *app, FILE *in, FILE *out)
{
  int ret;
  if (app->in_binary)
    ret = binary_hillyer (app, in, out);
  else
    ret = hillyer (app, in, out);
  return ret;
}

int
main (int argc, char **argv)
{
  struct fv_app_hillyer_square_t app;
  memset (&app, 0, sizeof (app));
  app.display_square = display_square_record;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_hillyer_square (&app, stdin, stdout);
}
