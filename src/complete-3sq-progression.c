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

struct fv_app_complete_3sq_progressions_t
{
  unsigned long long incr;
  int in_binary;
  void (*display_record) (mpz_t *, FILE *out);
  unsigned long long max_tries;
};

static void
advance (struct fv_app_complete_3sq_progressions_t *app, mpz_t *vec, mpz_t root, FILE *out)
{
  for (unsigned long long i = 0; i < app->max_tries; i++)
    {
      for (int i = 0; i < app->incr; i++)
        {
          mpz_add (vec[3], vec[3], root);
          mpz_add (vec[3], vec[3], root);
          mpz_add_ui (vec[3], vec[3], 1);
          mpz_add_ui (root, root, 1);
        }
      app->display_record (vec, out);
    }
}

static int
complete_3sq (struct fv_app_complete_3sq_progressions_t *app, FILE *in, FILE *out)
{
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  mpz_t vec[4], root;
  mpz_inits (vec[0], vec[1], vec[2], vec[3], root, NULL);
  while (1)
    {
      read = read_four_numbers_from_stream (in, vec, &line, &len);
      if (read == -1)
        break;
      mpz_set (root, vec[3]); //4th number is root
      mpz_set (vec[3], vec[2]);
      advance (app, vec, root, out);
    }
  mpz_clears (vec[0], vec[1], vec[2], vec[3], root, NULL);
  if (line)
    free (line);
  return 0;
}

static int
complete_binary_3sq (struct fv_app_complete_3sq_progressions_t *app, FILE *in, FILE *out)
{
  ssize_t read;
  mpz_t vec[4], root;
  mpz_inits (vec[0], vec[1], vec[2], vec[3], root, NULL);
  while (1)
    {
      read = binary_read_four_numbers_from_stream (in, vec, NULL, NULL);
      if (read == -1)
        break;
      mpz_set (root, vec[3]); //4th number is root
      mpz_set (vec[3], vec[2]);
      advance (app, vec, root, out);
    }
  mpz_clears (vec[0], vec[1], vec[2], vec[3], root, NULL);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_complete_3sq_progressions_t *app = (struct fv_app_complete_3sq_progressions_t *) state->input;
  char *end = NULL;
  switch (key)
    {
    case 'I':
      app->incr = strtoull (arg, &end, 10);
      break;
    case 'i':
      app->in_binary = 1;
      break;
    case 'o':
      app->display_record = display_binary_four_record;
      break;
    case ARGP_KEY_ARG:
      if (app->max_tries)
        argp_error (state, "too many arguments");
      else
        app->max_tries = strtoull (arg, &end, 10);
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


int
fituvalu_complete_3sq_progressions (struct fv_app_complete_3sq_progressions_t *app, FILE *in, FILE *out)
{
  if (app->in_binary)
    return complete_binary_3sq (app, in, out);
  else
    return complete_3sq (app, in, out);
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "increment", 'I', "NUM", 0, "Advance by NUM squares instead of 1"},
  { 0 }
};

static struct argp
argp =
{
  options, parse_opt, "NUM",
  "Generate progressions of four squares given an arithmetic progression of three squares.\vThe input of this program comes from \"3sq\" -- three squares that are equidistant, seperated by commas, and followed by the square root of the third square.  NUM is the number of squares to iterate after the third square.",
  0
};

int
main (int argc, char **argv)
{
  struct fv_app_complete_3sq_progressions_t app;
  memset (&app, 0, sizeof (app));
  app.incr = 1;
  app.display_record = display_four_record;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_complete_3sq_progressions (&app, stdin, stdout);
}
