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

struct fv_app_find_3sq_progressions_dist_t
{
  int no_second;
  int no_third;
  int break_after;
  int in_binary;
  mpz_t distance, start, max_tries, iter;
  int showsum;
  int showroot;
  int showdiff;
  void (*display_record) (mpz_t *, mpz_t*, FILE *out);
};

static int
gen_3sq (struct fv_app_find_3sq_progressions_dist_t *app, FILE *out)
{
  int count = 0;
  if (mpz_odd_p (app->distance) || mpz_cmp_ui (app->distance, 0) < 0)
    return 0;
  mpz_t m, vec[3], finalroot, root, i, j, k;
  mpz_inits (m, vec[0], vec[1], vec[2], finalroot, j, i, root, k, NULL);
  mpz_set (i, app->start);
  mpz_sqrt (root, i);
  mpz_mul (i, root, root);
  for (mpz_set_ui (j, 1);
       mpz_cmp_ui (app->iter, 0) != 0 || app->break_after ||
       mpz_cmp (j, app->max_tries) < 0;
       mpz_add_ui (j, j, 1))
    {
      mpz_set (vec[0], i);
      mpz_add (vec[1], vec[0], app->distance);
      if (mpz_perfect_square_p (vec[1]) || app->no_second)
        {
          mpz_add (vec[2], vec[1], app->distance);
          if (mpz_perfect_square_p (vec[2]) || app->no_third)
            {
              count++;
              if (app->showsum)
                {
                  mpz_t sum;
                  mpz_init (sum);
                  mpz_add (sum, vec[0], vec[1]);
                  mpz_add (sum, sum, vec[2]);
                  if (app->display_record ==
                      display_binary_three_record_with_root)
                    mpz_out_raw (out, sum);
                  else
                    {
                      char buf[mpz_sizeinbase (sum, 10) + 2];
                      mpz_get_str (buf, 10, sum);
                      fprintf (out, "%s, ", buf);
                    }
                  mpz_clear (sum);
                }
              if (app->showdiff)
                {
                  if (app->display_record ==
                      display_binary_three_record_with_root)
                    mpz_out_raw (out, app->distance);
                  else
                    {
                      char buf[mpz_sizeinbase (app->distance, 10) + 2];
                      mpz_get_str (buf, 10, app->distance);
                      fprintf (out, "%s, ", buf);
                    }
                }
              if (app->showroot)
                mpz_sqrt (finalroot, vec[2]);
              app->display_record (vec, &finalroot, out);
              if (app->break_after && count >= app->break_after)
                break;
            }
        }
      mpz_add (i, i, root);
      mpz_add (i, i, root);
      mpz_add_ui (i, i, 1);
      mpz_add_ui (root, root, 1);
      if (mpz_cmp_ui (app->iter, 0) != 0)
        {
          mpz_sub (k, i, app->start);
          if (mpz_cmp (k, app->iter) > 0)
            break;
        }
    }
  mpz_clears (m, vec[0], vec[1], vec[2], finalroot, root, i, j, k, NULL);
  return 0;
}

static int
gen_3sq_binary_in (struct fv_app_find_3sq_progressions_dist_t *app, FILE *in, FILE *out)
{
  ssize_t read;
  while (1)
    {
      read =
        binary_read_numbers_from_stream (in, &app->distance, 1, NULL, NULL);
      if (read == -1)
        break;
      gen_3sq (app, out);
    }
  return 0;
}

static int
gen_3sq_in (struct fv_app_find_3sq_progressions_dist_t *app, FILE *in, FILE *out)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  while (1)
    {
      read = read_numbers_from_stream (in, &app->distance, 1, &line, &len);
      if (read == -1)
        break;
      gen_3sq (app, out);
    }
  if (line)
    free (line);
  return 0;
}

/*
static void
calculate_start_and_max_tries (struct fv_app_find_3sq_progressions_dist_t *app)
{
  //app->distance is the number
  //we want to fill out app->start
  //and app->max_tries
  //
  //using the square difference triangle to find the starting row
  //and ending row
  //
  //it doesn't work though.  the starting square for 3360 can be 4
  mpz_t finish, i, j, icount, jcount, row, col, next, root;
  mpz_inits (finish, i, j, icount, jcount, row, col, next, root, NULL);
  mpz_set (finish, app->distance);
  mpz_set_ui (icount, 3);
  mpz_set (i, icount);
  mpz_set_ui (row, 2);

  mpz_sqrt (root, finish);
  mpz_t rootplus2;
  mpz_init (rootplus2);
  mpz_add_ui (rootplus2, root, 2);
  mpz_mul (i, root, rootplus2);
  mpz_sub_ui (rootplus2, rootplus2, 1);
  mpz_sub_ui (root, root, 1);
  mpz_mul (j, root, rootplus2);
  mpz_clear (rootplus2);
  mpz_sub (icount, i, j);
  mpz_set (row, root);
  mpz_add_ui (row, row, 2);

  //mpz_set (app->start, i);

  //((num / 2) + 1) * ((num / 2) + 1)  - 1
  mpz_cdiv_q_ui (app->max_tries, app->distance, 2);
  mpz_add_ui (app->max_tries, app->max_tries, 1);
  mpz_mul (app->max_tries, app->max_tries, app->max_tries);
  mpz_sub_ui (app->max_tries, app->max_tries, 1);
  display_textual_number (&i, stdout);
  display_textual_number (&app->max_tries, stdout);
  mpz_clears (finish, i, j, icount, jcount, row, col, next, root, NULL);
}
*/

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_find_3sq_progressions_dist_t *app = (struct fv_app_find_3sq_progressions_dist_t *) state->input;
  switch (key)
    {
    case 'r':
      mpz_set_str (app->iter, arg, 10);
      break;
    case '2':
      app->no_second = 1;
      break;
    case '3':
      app->no_third = 1;
      break;
    case 'b':
      app->break_after = atoi (arg);
      break;
    case 's':
      app->showsum = 1;
      break;
    case 'i':
      app->in_binary = 1;
      break;
    case 't':
      mpz_set_str (app->max_tries, arg, 10);
      break;
    case 'S':
      mpz_set_str (app->start, arg, 10);
      break;
    case 'd':
      app->showdiff = 1;
      break;
    case 'n':
      app->showroot = 0;
      break;
    case 'o':
      app->display_record = display_binary_three_record_with_root;
      break;
    case ARGP_KEY_ARG:
      if (mpz_cmp_ui (app->distance, 0) != 0)
        argp_error (state, "too many arguments");
      else
        {
          mpz_set_str (app->distance, arg, 10);
          if (mpz_odd_p (app->distance))
            argp_error (state, "DISTANCE must be even.");
          else if (mpz_cmp_ui (app->distance, 0) <= 0)
            argp_error (state, "DISTANCE must be a positive number.");
        }
      break;
    case ARGP_KEY_INIT:
      mpz_inits (app->distance, app->start, app->max_tries, app->iter, NULL);
      mpz_set_ui (app->start, 1);
      mpz_set_ui (app->max_tries, 5000000);
      setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
      break;
    }
  return 0;
}

int
fituvalu_find_3sq_progressions_dist (struct fv_app_find_3sq_progressions_dist_t *app, FILE *in, FILE *out)
{
  if (mpz_cmp_ui (app->distance, 0) == 0)
    {
      if (app->in_binary)
        return gen_3sq_binary_in (app, in, out);
      else
        return gen_3sq_in (app, in, out);
    }
  else
    return gen_3sq (app, out);
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

static struct argp
argp =
{
  options, parse_opt, "[DISTANCE]",
  "Generate arithmetic progressions of three squares, that are DISTANCE apart.  If DISTANCE is not provided as an argument, it is read from the standard input.\vBy default this program checks 5 million squares for a progression with the given distance.",
  0
};

int
main (int argc, char **argv)
{
  struct fv_app_find_3sq_progressions_dist_t app;
  memset (&app, 0, sizeof (app));
  app.showroot = 1;
  app.display_record = display_three_record_with_root;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_find_3sq_progressions_dist (&app, stdin, stdout);
}
