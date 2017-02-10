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

unsigned long long incr = 1;
int in_binary;
void (*display_record) (mpz_t *, FILE *out) = display_four_record;
unsigned long long max_tries;

static void
advance (mpz_t *vec, mpz_t root, FILE *out)
{
  for (unsigned long long i = 0; i < max_tries; i++)
    {
      for (int i = 0; i < incr; i++)
        {
          mpz_add (vec[3], vec[3], root);
          mpz_add (vec[3], vec[3], root);
          mpz_add_ui (vec[3], vec[3], 1);
          mpz_add_ui (root, root, 1);
        }
      display_record (vec, out);
    }
}

static int
complete_3sq (FILE *in, FILE *out)
{
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  mpz_t vec[4], root;
  mpz_inits (vec[0], vec[1], vec[2], vec[3], root, NULL);
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
      advance (vec, root, out);
    }
  mpz_clears (vec[0], vec[1], vec[2], vec[3], root, NULL);
  if (line)
    free (line);
  return 0;
}

static int
complete_binary_3sq (FILE *in, FILE *out)
{
  ssize_t read;
  mpz_t vec[4], root;
  mpz_inits (vec[0], vec[1], vec[2], vec[3], root, NULL);
  while (1)
    {
      read = mpz_inp_raw (vec[0], in);
      if (!read)
        break;
      read = mpz_inp_raw (vec[1], in);
      if (!read)
        break;
      read = mpz_inp_raw (vec[2], in);
      if (!read)
        break;
      read = mpz_inp_raw (root, in);
      if (!read)
        break;
      mpz_set (vec[3], vec[2]);
      advance (vec, root, out);
    }
  mpz_clears (vec[0], vec[1], vec[2], vec[3], root, NULL);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  char *end = NULL;
  switch (key)
    {
    case 'I':
      incr = strtoull (arg, &end, 10);
      break;
    case 'i':
      in_binary = 1;
      break;
    case 'o':
      display_record = display_binary_four_record;
      break;
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
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "increment", 'I', "NUM", 0, "Advance by NUM squares instead of 1"},
  { 0 }
};

struct argp argp ={options, parse_opt, "NUM", "Generate progressions of four squares given an arithmetic progression of three squares.\vThe input of this program comes from \"3sq\" -- three squares that are equidistant, seperated by commas, and followed by the square root of the third square.  NUM is the number of squares to iterate after the third square.", 0 };

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  if (in_binary)
    return complete_binary_3sq (stdin, stdout);
  else
    return complete_3sq (stdin, stdout);
}
