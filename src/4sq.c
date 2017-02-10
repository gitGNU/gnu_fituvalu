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
#include <string.h>
#include <math.h>
#include <argp.h>
#include <gmp.h>
#include "magicsquareutil.h"

unsigned long long incr = 1;
int in_binary;
void (*func)(mpz_t, mpz_t, mpz_t, unsigned long long, void (*)(mpz_t*, mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *) = fwd_4sq_progression1;
mpz_t start, finish, oneshot;
int num_args;

static void (*display_four_progression)(mpz_t *, FILE *);

static void
dump_num (mpz_t *i, FILE *out)
{
  char buf[mpz_sizeinbase (*i, 10) + 2];
  mpz_get_str (buf, 10, *i);
  fprintf (out, "%s", buf);
}

static void
display_four_progression_as_text (mpz_t *progression, FILE *out)
{
  for (int i = 0; i < 4; i++)
    {
      dump_num (&progression[i], out);
      fprintf (out, ", ");
    }
  fprintf (out, "\n");
}

static void
display_four_progression_as_binary (mpz_t *progression, FILE *out)
{
  mpz_out_raw (out, progression[0]);
  mpz_out_raw (out, progression[1]);
  mpz_out_raw (out, progression[2]);
  mpz_out_raw (out, progression[3]);
}

static void
display_squares (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four, FILE *out)
{
  mpz_set (progression[0], one);
  mpz_set (progression[1], two);
  mpz_set (progression[2], three);
  mpz_set (progression[3], four);
  display_four_progression (progression, out);
}

void
generate_4sq_from_binary_input (FILE *in, FILE *out)
{
  ssize_t read;
  mpz_t i;
  mpz_init (i);
  while (1)
    {
      read = mpz_inp_raw (i, in);
      if (!read)
        break;
      func (i, i, finish, incr, display_squares, out);
    }
  mpz_clear (i);
}

void
generate_4sq_from_input (FILE *in, FILE *out)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t i;
  mpz_init (i);
  while (1)
    {
      read = getline (&line, &len, in);
      if (read == -1)
        break;
      char *end = strchr (line, '\n');
      if (end)
        *end = '\0';
      mpz_set_str (i, line, 10);
      func (i, i, finish, incr, display_squares, out);
    }
  mpz_clear (i);
  free (line);
}

void
generate_4sq (FILE *out)
{
  mpz_t i;
  mpz_init (i);
  mpz_t root;
  mpz_inits (root, NULL);
  mpz_sqrt (root, start);
  mpz_mul (i, root, root);
  func (i, start, finish, incr, display_squares, stdout);
  while (1)
    {
      for (int j = 0; j < incr; j++)
        {
          mpz_add (i, i, root);
          mpz_add (i, i, root);
          mpz_add_ui (i, i, 1);
          mpz_add_ui (root, root, 1);
        }

      if (mpz_cmp (i, finish) >= 0)
        break;
      func (i, start, finish, incr, display_squares, out);
    }
  mpz_clears (root, NULL);
  mpz_clear (i);
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
    case '1':
      mpz_set_str (oneshot, arg, 10);
      break;
    case 'o':
      display_four_progression = display_four_progression_as_binary;
      break;
    case 'i':
      in_binary = 1;
      break;
    case 't':
        {
          four_square_progression_t *p =
            lookup_four_square_progression_by_name (arg);
          if (p)
            func = p->func;
          else
            {
              char *types = generate_list_of_four_square_progression_types ();
              argp_error (state, "--type must be one of %s.", types);
              free (types);
            }
        }
      break;
    case ARGP_KEY_ARG:
      if (num_args == 2)
        argp_error (state, "too many arguments.");
      else
        {
          if (num_args == 0)
            mpz_init_set_str (finish, arg, 10);
          else
            {
              mpz_init_set (start, finish);
              mpz_init_set_str (finish, arg, 10);
            }
          num_args++;
        }
      break;
    case ARGP_KEY_INIT:
      mpz_init (start);
      mpz_init (finish);
      display_four_progression = display_four_progression_as_text;
      break;
    case ARGP_KEY_FINI:
      if (num_args == 0)
        argp_error (state, "too few arguments.");
      break;
    }
  return 0;
}

static char *
help_filter (int key, const char *text, void *input)
{
  if (key == ARGP_KEY_HELP_POST_DOC)
    {
      char *s = generate_list_of_four_square_progression_timelines ();
      char *q = NULL;
      asprintf (&q, "\n%s", s);
      free (s);
      char *new_text = NULL;
      if (asprintf (&new_text, text, q) != -1)
        return new_text;
    }
  return (char *) text;
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "type", 't', "NAME", 0, "Use NAME as the progression strategy"},
  { NULL, '1', "NUM", 0, "Do one iteration with NUM as first square"},
  { "increment", 'I', "NUM", 0, "Advance by NUM squares instead of 1"},
  { 0 }
};

struct argp argp ={options, parse_opt, "START FINISH\nFINISH", "Generate progressions of 4 perfect squares.  If only FINISH is specified, read the starting perfect square from the standard input.  Continue iterating until the progression reaches FINISH.\vAn arithmetic progression of 4 squares in a row is impossible (e.g. the distance between the squares is the same), so we iterate over 4 squares with a different gap between one or more of the squares.  --type must be one of:%s", 0, help_filter};

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  if (num_args == 1)
    {
      if (in_binary)
        generate_4sq_from_binary_input (stdin, stdout);
      else
        generate_4sq_from_input (stdin, stdout);
    }
  else
    {
      if (mpz_cmp_ui (oneshot, 0) != 0)
        func (oneshot, start, finish, incr, display_squares, stdout);
      else
        generate_4sq (stdout);
    }
  return 0;
}

