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
#include <string.h>
#include <stdlib.h>
#include <gmp.h>
#include "magicsquareutil.h"

int num_args;
int brute;
mpz_t max, min;

void (*display_record)(mpz_t*, mpz_t*, FILE *) = display_two_record;
static void
seq (mpz_t m, mpz_t n, mpz_t finish, FILE *out)
{
    {
      //XXX FIXME: we put m and n into one and two to avoid a warning
      mpz_t one, two;
      mpz_init_set (one, m);
      mpz_init_set (two, n);
      display_record (&one, &two, out);
      mpz_clears (one, two, NULL);
    }
  mpz_t s;
  mpz_init (s);
  mpz_add (s, m, n);
  if (mpz_cmp (s, finish) < 0)
    {
      mpz_t m2, n2;
      mpz_inits (m2, n2, NULL);
      mpz_set (m2, s);
      mpz_set (n2, m);
      if (mpz_even_p (n2))
        seq (m2, n2, finish, out);
      else
        {
          mpz_add (s, m2, n2);
          if (mpz_cmp (s, finish) < 0)
            {
              seq (s, m2, finish, out);
              seq (s, n2, finish, out);
            }
        }
      mpz_set (n2, n);
      if (mpz_even_p (n2))
        seq (m2, n2, finish, out);
      else
        {
          mpz_add (s, m2, n2);
          if (mpz_cmp (s, finish) < 0)
            {
              seq (s, m2, finish, out);
              seq (s, n2, finish, out);
            }
        }
      mpz_clears (m2, n2, NULL);
    }
  mpz_clear (s);
}

static int
morgenstern_seq (FILE *out)
{
  mpz_t m, n;
  mpz_inits (m, n, NULL);
  if (mpz_cmp_ui (min, 0) > 0)
    {
      mpz_sub_ui (m, min, 1);
      mpz_sub_ui (n, min, 2);
      seq (m, n, max, out);
    }
  else
    {
      mpz_set_ui (m, 2);
      mpz_set_ui (n, 1);
      seq (m, n, max, out);
    }
  mpz_clears (m, n, NULL);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'b':
      brute = 1;
      break;
    case 'o':
      display_record = display_binary_two_record;
      break;
    case ARGP_KEY_ARG:
      if (num_args == 2)
        argp_error (state, "too many arguments");
      else
        {
          switch (num_args)
            {
            case 0:
              mpz_set_str (max, arg, 10);
              break;
            case 1:
              mpz_set (min, max);
              mpz_set_str (max, arg, 10);
              break;
            }
          num_args++;
        }
      break;
     case ARGP_KEY_INIT:
      mpz_init_set_ui (min, 0);
      mpz_init_set_ui (max, 0);
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
  { "brute", 'b', 0, OPTION_HIDDEN, ""},
  { 0 }
};
struct argp argp ={options, parse_opt, "MAX\nMIN MAX", "Compute an MN list.\vThe output of this program is suitable for input into the \"morgenstern-search-type-*\" programs, as well as \"3sq\".  This sequence of numbers has the form:\nM > N > 0, where M and N are coprime, and with one number being even, and the other number being odd." , 0};

static int
morgenstern_brute (FILE *out, char *start)
{
  mpz_t i;
  mpz_init_set_str (i, start, 10);
  for (; ; mpz_add_ui (i, i, 2))
    {
      char buf[mpz_sizeinbase (i, 10) + 2];
      mpz_get_str (buf, 10, i);
      fprintf (out, "%s, %d\n", buf, 1);
    }
  mpz_clear (i);
  return 0;
}

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  int ret = 0;
  if (brute)
    ret = morgenstern_brute (stdout, "54342291404080490827096080");
  else
    ret = morgenstern_seq (stdout);
  return ret;
}
