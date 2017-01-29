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

int num_args;
mpz_t max, min;

static void
seq (mpz_t m, mpz_t n, mpz_t finish, FILE *out)
{
    {
      char buf[mpz_sizeinbase (m, 10) + 2];
      mpz_get_str (buf, 10, m);
      fprintf (out, "%s, ", buf);
    }
    {
      char buf[mpz_sizeinbase (n, 10) + 2];
      mpz_get_str (buf, 10, n);
      fprintf (out, "%s,\n", buf);
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
    case ARGP_KEY_NO_ARGS:
      argp_error (state, "missing argument.");
      break;
    }
  return 0;
}

struct argp argp ={NULL, parse_opt, "MAX\nMIN MAX", "Compute an MN list.\vThe output of this program is suitable for input into the \"morgenstern-search-type-1\" program.  This sequence of numbers has the form:\nM > N > 0, where M and N are coprime, and with one being even and the other one odd." , 0};

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  int ret = morgenstern_seq (stdout);
  return ret;
}
