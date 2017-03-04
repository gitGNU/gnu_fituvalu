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

struct fv_app_find_3sq_progressions_conrad_t
{
  int num_args;
  int showroot;
  int show_diff;
  void (*display_record) (mpz_t *, mpz_t*, FILE *out);
  mpz_t max, start;
  mpz_t m2, twom;
};

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

static void
generate_progression (struct fv_app_find_3sq_progressions_conrad_t *app, mpz_t m, mpz_t *progression)
{
  mpz_mul (app->m2, m, m);
  mpz_add (app->twom, m, m);
  mpz_add_ui (progression[1], app->m2, 1);
  mpz_sub (progression[0], app->m2, app->twom);
  mpz_sub_ui (progression[0], progression[0], 1);
  mpz_mul_si (progression[2], app->m2, -1);
  mpz_sub (progression[2], progression[2], app->twom);
  mpz_add_ui (progression[2], progression[2], 1);

  for (int i = 0; i < 3; i++)
    mpz_mul (progression[i], progression[i], progression[i]);
}

int
fituvalu_find_3sq_progression_conrad (struct fv_app_find_3sq_progressions_conrad_t *app, FILE *out)
{
  mpz_t i, n;
  mpz_t progression[3], root, diff;
  mpz_inits (app->m2, app->twom, n, NULL);
  mpz_inits (i, progression[0], progression[1], progression[2], root, diff, NULL);
  mpz_set (n, app->start);
  for (mpz_set_ui (i, 1); mpz_cmp (i, app->max) < 0; mpz_add_ui (i, i, 1))
    {
      generate_progression (app, n, progression);
      if (app->show_diff)
        {
          mpz_sub (diff, progression[1], progression[0]);
          if (app->display_record == display_binary_three_record_with_root)
            mpz_out_raw (out, diff);
          else
            {
              char buf[mpz_sizeinbase (diff, 10) + 2];
              mpz_get_str (buf, 10, diff);
              fprintf (out, "%s, ", buf);
            }
        }
      if (app->showroot)
        mpz_sqrt (root, progression[2]);
      app->display_record (progression, &root, out);
      mpz_add_ui (n, n, 1);
    }
  mpz_clears (i, progression[0], progression[1], progression[2], root, diff, NULL);
  mpz_clears (app->m2, app->twom, NULL);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_find_3sq_progressions_conrad_t *app = (struct fv_app_find_3sq_progressions_conrad_t *) state->input;
  switch (key)
    {
    case 's':
      mpz_set_str (app->start, arg, 10);
      break;
    case 'd':
      app->show_diff = 1;
      break;
    case 'n':
      app->showroot = 0;
      break;
    case 'o':
      app->display_record = display_binary_three_record_with_root;
      break;
    case ARGP_KEY_ARG:
      if (app->num_args == 1)
        argp_error (state, "too many arguments");
      else
        {
          switch (app->num_args)
            {
            case 0:
              mpz_set_str (app->max, arg, 10);
              break;
            }
          app->num_args++;
        }
      break;
    case ARGP_KEY_INIT:
      mpz_init (app->max);
      mpz_init_set_ui (app->start, 1);
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
  { "no-root", 'n', 0, 0, "Don't show the root of the fourth number"},
  { "show-diff", 'd', 0, 0, "Also show the diff"},
  { "start", 's', "NUM", 0, "Start at NUM instead of 1"},
  { 0 }
};

static struct argp argp ={options, parse_opt, "MAX", "Find an arithmetic progression consisting of three squares.\vMAX is the number of iterations. \"conrad\" refers to Keith Conrad, and his paper 'Arithmetic Progressions of Three Squares'." , 0};

int
main (int argc, char **argv)
{
  struct fv_app_find_3sq_progressions_conrad_t app;
  memset (&app, 0, sizeof (app));
  app.showroot = 1;
  app.display_record = display_three_record_with_root;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_find_3sq_progression_conrad (&app, stdout);
}
