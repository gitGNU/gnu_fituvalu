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
#include <stdio.h>
#include <gmp.h>
#include <argp.h>
#include <string.h>
#include <stdlib.h>
#include "magicsquareutil.h"

struct fv_app_seq_mul_t
{
  int num_args;
  mpz_t args[3];
  void (*dump_func) (mpz_t *, FILE *);
};

static int
seq_mul (struct fv_app_seq_mul_t *app, mpz_t start, mpz_t multiplier, mpz_t finish, FILE *out)
{
  mpz_t i;
  mpz_init (i);
  for (mpz_set (i, start); mpz_cmp_ui (finish, 0) == 0 || 
       mpz_cmp (i, finish) <= 0; mpz_mul (i, i, multiplier))
    app->dump_func (&i, out);
  mpz_clear (i);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_seq_mul_t *app = (struct fv_app_seq_mul_t *) state->input;
  switch (key)
    {
    case 'o':
      app->dump_func = display_binary_number;
      break;
    case ARGP_KEY_ARG:
      if (app->num_args == 3)
        argp_error (state, "too many arguments");
      else
        {
          mpz_set_str (app->args[app->num_args], arg, 10);
          app->num_args++;
        }
      break;
    case ARGP_KEY_INIT:
      app->dump_func = display_textual_number;
      for (int i = 0; i < 3; i++)
        mpz_init (app->args[i]);
      break;
    case ARGP_KEY_FINI:
      if (app->num_args <= 1)
        argp_error (state, "missing argument.");
      break;
    case ARGP_KEY_NO_ARGS:
      argp_error (state, "missing argument.");
      break;
    }
  return 0;
}

int
fituvalu_seq_mul (struct fv_app_seq_mul_t *app, FILE *out)
{
  mpz_t zero;
  mpz_init (zero);
  mpz_set_ui (zero, 0);

  int ret = 0;

  switch (app->num_args)
    {
    case 2:
      ret = seq_mul (app, app->args[0], app->args[1], zero, out);
      break;
    case 3:
      ret = seq_mul (app, app->args[0], app->args[1], app->args[2], out);
      break;
    case ARGP_KEY_INIT:
      setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
      break;
    }

  mpz_clear (zero);
  return ret;
}

static struct argp_option
options[] =
{
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { 0 }
};

static struct argp
argp =
{
  options, parse_opt, "FIRST MULTIPLE\nFIRST MULTIPLE LAST",
  "Compute a sequence of numbers in steps of MULTIPLE.",
  0
};

int
main (int argc, char **argv)
{
  struct fv_app_seq_mul_t app;
  memset (&app, 0, sizeof (app));
  argp_parse (&argp, argc, argv, 0, 0, &app);

  return fituvalu_seq_mul (&app, stdout);
}
