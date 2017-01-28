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

unsigned long long max_tries;

static void
inc (mpz_t *j, mpz_t *jroot)
{
  mpz_add (*j, *j, *jroot);
  mpz_add (*j, *j, *jroot);
  mpz_add_ui (*j, *j, 1);
}

static int
complete_3sq (FILE *in, FILE *out)
{
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  mpz_t vec[4], root;
  mpz_inits (vec[0], vec[1], vec[3], vec[4], root, NULL);
  while (1)
    {
      read = getdelim (&line, &len, ',', in);
      if (read == -1)
        break;
      char *comma = strchr (line, ',');
      if (comma)
        *comma = '\0';
      mpz_set_str (vec[0], line, 10);
      read = getdelim (&line, &len, ',', in);
      if (read == -1)
        break;
      comma = strchr (line, ',');
      if (comma)
        *comma = '\0';
      mpz_set_str (vec[1], line, 10);
      read = getdelim (&line, &len, ',', in);
      if (read == -1)
        break;
      comma = strchr (line, ',');
      if (comma)
        *comma = '\0';
      mpz_set_str (vec[2], line, 10);
      read = getline (&line, &len, in);
      if (read == -1)
        break;
      comma = strchr (line, ',');
      if (comma)
        *comma = '\0';
      mpz_set_str (root, line, 10);
      mpz_set (vec[3], vec[2]);
      for (unsigned long long i = 0; i < max_tries; i++)
        {
          inc (&vec[3], &root);
          display_four_record (vec, out);
          mpz_add_ui (root, root, 1);
        }
    }
  mpz_clears (vec[0], vec[1], vec[2], vec[3], root, NULL);
  if (line)
    free (line);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  char *end = NULL;
  switch (key)
    {
    case ARGP_KEY_ARG:
      if (max_tries)
        argp_error (state, "too many arguments");
      else
        max_tries = strtoull (arg, &end, 10);
      break;
    case ARGP_KEY_NO_ARGS:
      argp_error (state, "missing argument");
      break;
    }
  return 0;
}


static struct argp_option
options[] =
{
  { 0 }
};

struct argp argp ={options, parse_opt, "NUM", "Generate progressions of four squares.\vThe input of this program comes from \"3sq\".", 0 };

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  return complete_3sq (stdin, stdout);
}
