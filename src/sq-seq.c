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

int num_args;
mpz_t args[3];
static void (*dump_func) (mpz_t *, FILE *);
char *buf;

static void
dump_binary_num (mpz_t *i, FILE *out)
{
  mpz_out_raw (out, *i);
}

static void
dump_num (mpz_t *i, FILE *out)
{
  mpz_get_str (buf, 10, *i);
  fprintf (out, "%s", buf);
  fprintf (out, "\n");
}

static int
sq_seq (mpz_t start, mpz_t finish, mpz_t incr, FILE *out)
{
  buf = malloc (mpz_sizeinbase (finish, 10) + 2);
  mpz_t i, root, nroot, lastroot;
  mpz_inits (i, root, nroot, lastroot, NULL);
  mpz_set (i, start);
  mpz_sqrt (root, i);
  if (mpz_cmp_ui (root, 0) == 0)
    mpz_add_ui (root, root, 1);
  mpz_mul (i, root, root);
  mpz_sqrt (lastroot, finish);

  dump_func (&i, out);

  while (mpz_cmp (root, lastroot) < 0)
    {
      mpz_mul (nroot, root, incr);
      mpz_add (i, i, nroot);
      mpz_add (i, i, nroot);
      mpz_add (i, i, incr);

      dump_func (&i, out);

      mpz_add (root, root, incr);
    }
  mpz_clears (i, root, nroot, lastroot, NULL);
  free (buf);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'o':
      dump_func = dump_binary_num;
      break;
    case ARGP_KEY_ARG:
      if (num_args == 3)
        argp_error (state, "too many arguments");
      else
        {
          mpz_set_str (args[num_args], arg, 10);
          num_args++;
        }
      break;
    case ARGP_KEY_INIT:
      dump_func = dump_num;
      for (int i = 0; i < 3; i++)
        mpz_init (args[i]);
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
  { 0 }
};

struct argp argp ={options, parse_opt, "FIRST\nFIRST LAST\nFIRST INCREMENT LAST", "Compute a sequence of perfect squares.\vIf FIRST or INCREMENT is omitted, it defaults to 1." , 0};

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);

  mpz_t one;
  mpz_init (one);
  mpz_set_ui (one, 1);

  FILE *out = stdout;
  int ret = 0;

  switch (num_args)
    {
    case 1:
      ret = sq_seq (one, args[0], one, out);
      break;
    case 2:
      ret = sq_seq (args[0], args[1], one, out);
      break;
    case 3:
      ret = sq_seq (args[0], args[2], args[1], out);
      break;
    }

  mpz_clear (one);
  return ret;
}
