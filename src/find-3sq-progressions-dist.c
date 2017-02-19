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

int no_second;
int no_third;
int break_after;
int in_binary;
mpz_t distance, start, max_tries, iter;
int showsum;
int showroot = 1;
int showdiff;
void (*display_record) (mpz_t *, mpz_t*, FILE *out) = display_three_record_with_root;

static int
gen_3sq (FILE *out)
{
  int count = 0;
  if (mpz_odd_p (distance) || mpz_cmp_ui (distance, 0) < 0)
    return 0;
  mpz_t m, vec[3], finalroot, root, i, j, k;
  mpz_inits (m, vec[0], vec[1], vec[2], finalroot, j, i, root, k, NULL);
  mpz_set (i, start);
  mpz_sqrt (root, i);
  mpz_mul (i, root, root);
  for (mpz_set_ui (j, 1);
       mpz_cmp_ui (iter, 0) != 0 || break_after || mpz_cmp (j, max_tries) < 0;
       mpz_add_ui (j, j, 1))
    {
      mpz_set (vec[0], i);
      mpz_add (vec[1], vec[0], distance);
      if (mpz_perfect_square_p (vec[1]) || no_second)
        {
          mpz_add (vec[2], vec[1], distance);
          if (mpz_perfect_square_p (vec[2]) || no_third)
            {
              count++;
              if (showsum)
                {
                  mpz_t sum;
                  mpz_init (sum);
                  mpz_add (sum, vec[0], vec[1]);
                  mpz_add (sum, sum, vec[2]);
                  if (display_record == display_binary_three_record_with_root)
                    mpz_out_raw (out, sum);
                  else
                    {
                      char buf[mpz_sizeinbase (sum, 10) + 2];
                      mpz_get_str (buf, 10, sum);
                      fprintf (out, "%s, ", buf);
                    }
                  mpz_clear (sum);
                }
              if (showdiff)
                {
                  if (display_record == display_binary_three_record_with_root)
                    mpz_out_raw (out, distance);
                  else
                    {
                      char buf[mpz_sizeinbase (distance, 10) + 2];
                      mpz_get_str (buf, 10, distance);
                      fprintf (out, "%s, ", buf);
                    }
                }
              if (showroot)
                mpz_sqrt (finalroot, vec[2]);
              display_record (vec, &finalroot, out);
              if (break_after && count >= break_after)
                break;
            }
        }
      mpz_add (i, i, root);
      mpz_add (i, i, root);
      mpz_add_ui (i, i, 1);
      mpz_add_ui (root, root, 1);
      if (mpz_cmp_ui (iter, 0) != 0)
        {
          mpz_sub (k, i, start);
          if (mpz_cmp (k, iter) > 0)
            break;
        }
    }
  mpz_clears (m, vec[0], vec[1], vec[2], finalroot, root, i, j, k, NULL);
  return 0;
}

static int
gen_3sq_binary_in (FILE *in, FILE *out)
{
  ssize_t read;
  while (1)
    {
      read = binary_read_numbers_from_stream (in, &distance, 1, NULL, NULL);
      if (read == -1)
        break;
      gen_3sq (out);
    }
  return 0;
}

static int
gen_3sq_in (FILE *in, FILE *out)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  while (1)
    {
      read = read_numbers_from_stream (in, &distance, 1, &line, &len);
      if (read == -1)
        break;
      gen_3sq (out);
    }
  if (line)
    free (line);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'r':
      mpz_set_str (iter, arg, 10);
      break;
    case '2':
      no_second = 1;
      break;
    case '3':
      no_third = 1;
      break;
    case 'b':
      break_after = atoi (arg);
      break;
    case 's':
      showsum = 1;
      break;
    case 'i':
      in_binary = 1;
      break;
    case 't':
      mpz_set_str (max_tries, arg, 10);
      break;
    case 'S':
      mpz_set_str (start, arg, 10);
      break;
    case 'd':
      showdiff = 1;
      break;
    case 'n':
      showroot = 0;
      break;
    case 'o':
      display_record = display_binary_three_record_with_root;
      break;
    case ARGP_KEY_ARG:
      if (mpz_cmp_ui (distance, 0) != 0)
        argp_error (state, "too many arguments");
      else
        {
          mpz_set_str (distance, arg, 10);
          if (mpz_odd_p (distance))
            argp_error (state, "DISTANCE must be even.");
          else if (mpz_cmp_ui (distance, 0) <= 0)
            argp_error (state, "DISTANCE must be a positive number.");
        }
      break;
    case ARGP_KEY_INIT:
      mpz_inits (distance, start, max_tries, iter, NULL);
      mpz_set_ui (start, 1);
      mpz_set_ui (max_tries, 5000000);
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
  { "start", 'S', "NUM", 0, "Start at NUM instead of 1"},
  { "show-sum", 's', 0, 0, "Also show the sum"},
  { "tries", 't', "NUM", 0, "Loop forward to a total of NUM squares"},
  { "break-after", 'b', "NUM", OPTION_HIDDEN, "Break after NUM found"},
  { "no-second", '2', 0, 0, "The second number doesn't have to be square"},
  { "no-third", '3', 0, 0, "The third number doesn't have to be square"},
  { "range", 'r', "NUM", 0, "Iterate until the value has NUM more than start"},
  { 0 }
};

struct argp argp ={options, parse_opt, "[DISTANCE]", "Generate arithmetic progressions of three squares, that are DISTANCE apart.  If DISTANCE is not provided as an argument, it is read from the standard input.\vBy default this program checks 5 million squares for a progression with the given distance.", 0 };

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  if (mpz_cmp_ui (distance, 0) == 0)
    {
      if (in_binary)
        return gen_3sq_binary_in (stdin, stdout);
      else
        return gen_3sq_in (stdin, stdout);
    }
  else
    return gen_3sq (stdout);
}
