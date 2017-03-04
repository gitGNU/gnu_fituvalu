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
#include <math.h>
#include <argp.h>
#include <string.h>
#include <gmp.h>
#include "magicsquareutil.h"

struct fv_app_small_morgenstern_search_type_3_t;
static int
no_filter (struct fv_app_small_morgenstern_search_type_3_t *app, int num)
{
  return 1;
}

struct fv_app_small_morgenstern_search_type_3_t
{
  void (*display_square) (mpz_t s[3][3], FILE *);

  int filter_num_squares;
  int num_args;
  unsigned long long max;
  mpz_t a[3][3];
  mpz_t x1, _y1, z1, m12, n12, x2, y2, z2, m22, n22,
        yx1dif, yx1sum, yx2dif, yx2sum;

  int (*filter_square) (struct fv_app_small_morgenstern_search_type_3_t *, int);
  FILE *out;
};

static int
filter (struct fv_app_small_morgenstern_search_type_3_t *app, int num_squares)
{
  int count = 5;
  if (mpz_perfect_square_p (app->a[0][2]))
    {
      count++;
      if (count >= num_squares)
        return 1;
    }
  if (mpz_perfect_square_p (app->a[1][0]))
    {
      count++;
      if (count >= num_squares)
        return 1;
    }
  if (mpz_perfect_square_p (app->a[1][2]))
    {
      count++;
      if (count >= num_squares)
        return 1;
    }
  if (mpz_perfect_square_p (app->a[2][0]))
    {
      count++;
      if (count >= num_squares)
        return 1;
    }
  return 0;
}

static void
search_type_3 (unsigned long long m1, unsigned long long n1, unsigned long long m2, unsigned long long n2, void *data)
{
  struct fv_app_small_morgenstern_search_type_3_t *app = (struct fv_app_small_morgenstern_search_type_3_t *) data;
  //where X1 = 2*m1*n1,  Y1 = m1^2-n1^2,  Z1 = m1^2+n1^2,
  mpz_set_ui (app->x1, m1);
  mpz_mul_ui (app->x1, app->x1, n1);
  mpz_mul_ui (app->x1, app->x1, 2);

  mpz_set_ui (app->m12, m1);
  mpz_mul_ui (app->m12, app->m12, m1);

  mpz_set_ui (app->n12, n1);
  mpz_mul_ui (app->n12, app->n12, n1);

  mpz_sub (app->_y1, app->m12, app->n12);

  mpz_add (app->z1, app->m12, app->n12);

  mpz_sub (app->yx1dif, app->_y1, app->x1);

  mpz_add (app->yx1sum, app->_y1, app->x1);

  // where X2 = 2*m2*n2,  Y2 = m2^2-n2^2,  Z2 = m2^2+n2^2,
  mpz_set_ui (app->x2, m2);
  mpz_mul_ui (app->x2, app->x2, n2);
  mpz_mul_ui (app->x2, app->x2, 2);

  mpz_set_ui (app->m22, m2);
  mpz_mul_ui (app->m22, app->m22, m2);

  mpz_set_ui (app->n22, n2);
  mpz_mul_ui (app->n22, app->n22, n2);

  mpz_sub (app->y2, app->m22, app->n22);

  mpz_add (app->z2, app->m22, app->n22);

  mpz_sub (app->yx2dif, app->y2, app->x2);

  mpz_add (app->yx2sum, app->y2, app->x2);

/*
    -----------
    A^2 B^2  - 
     -  E^2  - 
     -  H^2 I^2
    -----------
         3     
*/
  // E = Z1 * Z2;
  // A = YX1sum * Z2;
  // I = YX1dif * Z2;
  // H = YX2sum * Z1;
  // B = YX2dif * Z1;
  mpz_mul (app->a[1][1], app->z1, app->z2);
  mpz_mul (app->a[1][1], app->a[1][1], app->a[1][1]);
  mpz_mul (app->a[0][0], app->yx1sum, app->z2);
  mpz_mul (app->a[0][0], app->a[0][0], app->a[0][0]);
  mpz_mul (app->a[2][2], app->yx1dif, app->z2);
  mpz_mul (app->a[2][2], app->a[2][2], app->a[2][2]);
  mpz_mul (app->a[2][1], app->yx2sum, app->z1);
  mpz_mul (app->a[2][1], app->a[2][1], app->a[2][1]);
  mpz_mul (app->a[0][1], app->yx2dif, app->z1);
  mpz_mul (app->a[0][1], app->a[0][1], app->a[0][1]);

  // C^2 = H^2 + E^2 - A^2;
  // D^2 = H^2 + I^2 - A^2;
  // F^2 = B^2 + A^2 - I^2;
  // G^2 = B^2 + E^2 - I^2;
  mpz_add (app->a[0][2], app->a[2][1], app->a[1][1]);
  mpz_sub (app->a[0][2], app->a[0][2], app->a[0][0]);
  mpz_add (app->a[1][0], app->a[2][1], app->a[2][2]);
  mpz_sub (app->a[1][0], app->a[1][0], app->a[0][0]);
  mpz_add (app->a[1][2], app->a[0][1], app->a[0][0]);
  mpz_sub (app->a[1][2], app->a[1][2], app->a[2][2]);
  mpz_add (app->a[2][0], app->a[0][1], app->a[1][1]);
  mpz_sub (app->a[2][0], app->a[2][0], app->a[2][2]);

  if (app->filter_square (app, app->filter_num_squares))
    app->display_square (app->a, app->out);
}

int
fituvalu_morgenstern_search_type_3 (struct fv_app_small_morgenstern_search_type_3_t *app, FILE *in)
{
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_init (app->a[i][j]);
  mpz_inits (app->x1, app->_y1, app->z1, app->m12, app->n12, app->x2, app->y2, app->z2, app->m22, app->n22,
             app->yx1dif, app->yx1sum, app->yx2dif, app->yx2sum,
             NULL);
  small_morgenstern_search (app->max, in, search_type_3, app);
  mpz_clears (app->x1, app->_y1, app->z1, app->m12, app->n12, app->x2, app->y2, app->z2, app->m22, app->n22,
              app->yx1dif, app->yx1sum, app->yx2dif, app->yx2sum, NULL);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_clear (app->a[i][j]);
  return 0;
}

static struct argp_option
options[] =
{
  { "filter", 'f', "NUM", 0, "Only show magic squares that have at least NUM perfect squares" },
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { 0 }
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_small_morgenstern_search_type_3_t *app = (struct fv_app_small_morgenstern_search_type_3_t *) state->input;
  switch (key)
    {
    case 'f':
      app->filter_num_squares = atoi (arg);
      app->filter_square = filter;
      break;
    case 'o':
      app->display_square = display_binary_square_record;
      break;
    case ARGP_KEY_ARG:
      if (app->num_args == 1)
        argp_error (state, "too many arguments");
      else
        {
          char *end = NULL;
          app->max = strtoull (arg, &end, 10);
          app->num_args++;
        }
      break;
    case ARGP_KEY_NO_ARGS:
      argp_error (state, "missing argument");
      break;
    case ARGP_KEY_INIT:
      setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
      break;
    }
  return 0;
}

static struct argp argp ={options, parse_opt, "MAX", "Generate 3x3 magic squares with 5 perfect squares or more by creating two arithmetic progressions of three perfect squares with the center square in common.\vThe standard input provides the parametric \"MN\" values -- two values per record to assist in the transformation.  Use the \"seq-morgenstern-mn\" program to provide this data on the standard input.  Morgenstern type 3 squares have 5 perfect squares in this configuration:\n\
+-------+-------+-------+\n\
|  A^2  |  B^2  |       |\n\
+-------+-------+-------+\n\
|       |  E^2  |       |\n\
+-------+-------+-------+\n\
|       |  H^2  |  I^2  |\n\
+-------+-------+-------+", 0};


int
main (int argc, char **argv)
{
  struct fv_app_small_morgenstern_search_type_3_t app;
  memset (&app, 0, sizeof (app));
  app.display_square = display_square_record;
  app.filter_square = no_filter;
  app.out = stdout;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_morgenstern_search_type_3 (&app, stdin);
}
