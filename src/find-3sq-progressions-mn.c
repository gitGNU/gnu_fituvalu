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

struct fv_app_find_3sq_progressions_mn_t
{
  mpz_t x1, _y1, z1, m12, n12, yx1dif, yx1sum;
  int has_divisors;
  int show_diff;
  int showroot;
  int in_binary;
  void (*display_record) (mpz_t *, mpz_t*, FILE *out);
};

static void
create_three_square_progression (struct fv_app_find_3sq_progressions_mn_t *app, mpz_t m, mpz_t n, mpz_t *vec, int size, mpz_t *finalroot)
{
  //where X1 = 2*m1*n1,  Y1 = m1^2-n1^2,  Z1 = m1^2+n1^2,
  mpz_set (app->x1, m);
  mpz_mul (app->x1, app->x1, n);
  mpz_mul_ui (app->x1, app->x1, 2);

  mpz_set (app->m12, m);
  mpz_mul (app->m12, app->m12, m);

  mpz_set (app->n12, n);
  mpz_mul (app->n12, app->n12, n);

  mpz_sub (app->_y1, app->m12, app->n12);

  mpz_add (app->z1, app->m12, app->n12);

  mpz_sub (app->yx1dif, app->_y1, app->x1);

  mpz_add (app->yx1sum, app->_y1, app->x1);

  mpz_mul (vec[0], app->yx1dif, app->yx1sum);
  mpz_mul (vec[1], app->z1, app->yx1sum);
  mpz_mul (vec[2], app->yx1sum, app->yx1sum);

  mpz_mul (vec[0], vec[0], vec[0]);
  mpz_mul (vec[1], vec[1], vec[1]);
  if (app->showroot)
    mpz_set (*finalroot, vec[2]);
  else
    mpz_set_ui (*finalroot, 0);
  mpz_mul (vec[2], vec[2], vec[2]);
}

static int
divisor_filter (mpz_t *v)
{
  int divisors[] = {
    29,
    37,
    41,
    48,
    53,
    61,
    72,
    120,
    168,
    264,
    312,
    408,
    456,
    552,
    744,
    1032,
    1128,
    1608,
    0};
  int *i;
  i = divisors;
  while (*i != 0)
    {
      for (int j = 0; j < 3; j++)
        {
          if (!mpz_divisible_ui_p (v[j], *i))
            return 0;
        }
      i++;
    }
  return 1;
}

static int
gen_3sq (struct fv_app_find_3sq_progressions_mn_t *app, FILE *in, FILE *out)
{
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  mpz_t m, n, vec[3], finalroot;
  mpz_inits (m, n, vec[0], vec[1], vec[2], finalroot, NULL);
  mpz_inits (app->x1, app->_y1, app->z1, app->m12, app->n12, app->yx1dif, app->yx1sum, NULL);
  while (1)
    {
      read = fv_getdelim (&line, &len, ',', in);
      if (read == -1)
        break;
      char *comma = strchr (line, ',');
      if (comma)
        *comma = '\0';
      mpz_set_str (m, line, 10);
      read = fv_getline (&line, &len, in);
      if (read == -1)
        break;
      mpz_set_str (n, line, 10);
      create_three_square_progression (app, m, n, vec, 3, &finalroot);

      if (app->has_divisors && !divisor_filter (vec))
        continue;
      if (app->show_diff)
        {
          mpz_t diff;
          mpz_init (diff);
          mpz_sub (diff, vec[1], vec[0]);
          if (app->display_record == display_binary_three_record_with_root)
            mpz_out_raw (out, diff);
          else
            {
              char buf[mpz_sizeinbase (diff, 10) + 2];
              mpz_get_str (buf, 10, diff);
              fprintf (out, "%s, ", buf);
            }
          mpz_clear (diff);
        }
      app->display_record (vec, &finalroot, out);
    }
  mpz_clears (m, n, vec[0], vec[1], vec[2], finalroot, NULL);
  mpz_clears (app->x1, app->_y1, app->z1, app->m12, app->n12, app->yx1dif, app->yx1sum, NULL);
  if (line)
    free (line);
  return 0;
}

static int
gen_binary_3sq (struct fv_app_find_3sq_progressions_mn_t *app, FILE *in, FILE *out)
{
  ssize_t read;
  mpz_t m, n, vec[3], finalroot;
  mpz_inits (m, n, vec[0], vec[1], vec[2], finalroot, NULL);
  mpz_inits (app->x1, app->_y1, app->z1, app->m12, app->n12, app->yx1dif, app->yx1sum, NULL);
  while (1)
    {
      read = mpz_inp_raw (m, in);
      if (!read)
        break;
      read = mpz_inp_raw (n, in);
      if (!read)
        break;
      create_three_square_progression (app, m, n, vec, 3, &finalroot);
      app->display_record (vec, &finalroot, out);
    }
  mpz_clears (m, n, vec[0], vec[1], vec[2], finalroot, NULL);
  mpz_clears (app->x1, app->_y1, app->z1, app->m12, app->n12, app->yx1dif, app->yx1sum, NULL);
  return 0;
}


static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_find_3sq_progressions_mn_t *app = (struct fv_app_find_3sq_progressions_mn_t *) state->input;
  switch (key)
    {
    case 'h':
      app->has_divisors = 1;
      break;
    case 'd':
      app->show_diff = 1;
      break;
    case 'n':
      app->showroot = 0;
      break;
    case 'i':
      app->in_binary = 1;
      break;
    case 'o':
      app->display_record = display_binary_three_record_with_root;
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
  { "no-root", 'n', 0, 0, "Don't show the root of the fourth number"},
  { "show-diff", 'd', 0, 0, "Also show the diff"},
  { "has-divisors", 'h', 0, OPTION_HIDDEN, ""},
  { 0 }
};

static struct argp argp ={options, parse_opt, "", "Generate arithmetic progressions of three squares, with a fourth number being the square root of the 3rd square.\vThe input of this program comes from \"seq-morgenstern-mn\".", 0 };

int
fituvalu_find_3sq_progressions_mn (struct fv_app_find_3sq_progressions_mn_t *app, FILE *in, FILE *out)
{
  if (app->in_binary)
    return gen_binary_3sq (app, in, out);
  else
    return gen_3sq (app, in, out);
}

int
main (int argc, char **argv)
{
  struct fv_app_find_3sq_progressions_mn_t app;
  memset (&app, 0, sizeof (app));
  app.showroot = 1;
  app.display_record = display_three_record_with_root;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_find_3sq_progressions_mn (&app, stdin, stdout);
}
