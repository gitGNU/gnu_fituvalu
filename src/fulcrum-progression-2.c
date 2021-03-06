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
#include <math.h>
#include <argp.h>
#include <string.h>
#include <gmp.h>
#include "magicsquareutil.h"

struct fv_app_fulcrum_progression2_t
{
  unsigned long long incr ;
  int in_binary;
  FILE *instream;
  mpz_t start, finish, oneshot;
  int num_args;
  void (*display_record) (mpz_t *progression, FILE *out);
  FILE *out;
};
/*
some magic squares can be found fairly directly by exploiting the following:

if you sort the values of a magic square and then diff the numbers,
sometimes you get a repeating sequence that looks like this:

D1 E1 F1 E1 E1 F1 E1 D1

so the distance between the values is somewhat regular.
E1 + F1 = D1.

so the sequence is:
number1 (go fwd by D1) number2 (go fwd by E1) number 3 (go fwd by F1) number 4,
and so on.

the name "fulcrum" comes from the 3rd number in the series which acts as a sort of balance between the 2nd and 4th numbers. (because  E1 + F1 = D1)

for example, here is a progression that this program creates:
25, 289, 361, 553, 625, 697, 889, 961, 1225
D1 = 264 = 289 - 25
E1 = 72 = 361 - 289
F1 = 192 = 553 - 361

E1 + F1 = D1 (72 + 192 = 264)

 289 -  25 = 264
 361 - 289 =  72
 553 - 361 = 192
 625 - 553 =  72
 697 - 625 =  72
 889 - 697 = 192
 961 - 889 =  72
1225 - 961 = 264

|-+--------+--+-----+--+--+-----+--+--------+--|
           ^  ^     ^  ^
           1  2     3  4

the general strategy is this:
first of all we always start on a square,
and then find the next square by iterating forward,
and then we skip finding the 3rd number to find the 4th sqaure
and then we iterate through different values to find the 3rd square
if all four of these values are squares, then we check the progression.
we display the progression if there's 5 or more squares in it.

when we display the progression, it is important to note that this is not a square.
to turn it into a square, use the square-combinations program to shuffle it
around in every possible permutation.

 */

static inline void
check_progression (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four, void *data)
{
  struct fv_app_fulcrum_progression2_t *app = (struct fv_app_fulcrum_progression2_t *) data;
  four_square_to_nine_number_fulcrum_progression2 (progression, one, two, three, four);
  int count = count_squares_in_fulcrum_progression2 (progression);
  if (count > 4)
    app->display_record (progression, app->out);
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_fulcrum_progression2_t *app = (struct fv_app_fulcrum_progression2_t *) state->input;
  char *end = NULL;
  switch (key)
    {
    case 'I':
      app->incr = strtoull (arg, &end, 10);
      break;
    case '1':
      mpz_set_str (app->oneshot, arg, 10);
      break;
    case 'o':
      app->display_record = display_binary_nine_record;
      break;
    case 'i':
      app->in_binary = 1;
      break;
    case 's':
      if (strcmp (arg, "-") == 0)
        app->instream = stdin;
      else
        app->instream = fopen (arg, "r");
      break;
    case ARGP_KEY_ARG:
      if (app->num_args == 2)
        argp_error (state, "too many arguments");
      else
        {
          switch (app->num_args)
            {
            case 0:
              mpz_init_set_str (app->start, arg, 10);
              break;
            case 1:
              mpz_init_set_str (app->finish, arg, 10);
              break;
            }
          app->num_args++;
        }
      break;
    case ARGP_KEY_INIT:
      setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
      break;
    case ARGP_KEY_FINI:
      if (app->num_args != 2)
        argp_error (state, "not enough arguments");
      break;
    }
  return 0;
}

int
fituvalu_fulcrum_progression2 (struct fv_app_fulcrum_progression2_t *app)
{
  if (app->instream && !app->in_binary)
    read_square_and_run (app->instream,
                         fwd_4sq_progression2,
                         check_progression,
                         app->start, app->finish, app->incr, app);
  else if (app->instream && app->in_binary)
    binary_read_square_and_run (app->instream,
                                fwd_4sq_progression2,
                                check_progression,
                                app->start, app->finish, app->incr, app);
  else if (mpz_cmp_ui (app->oneshot, 0) != 0)
    fwd_4sq_progression2 (app->oneshot, app->start, app->finish, app->incr,
                          check_progression, app);
  else
    loop_and_run (fwd_4sq_progression2,
                  check_progression,
                  app->start, app->finish, app->incr, app);

  return 0;
}

static char *
help_filter (int key, const char *text, void *input)
{
  if (key == ARGP_KEY_HELP_POST_DOC)
    {
      char *s =
        "|-+--------+--+-----+--+--+-----+--+--------+--|\n"
        "           ^  ^     ^  ^                        \n"
        "           1  2     3  4                        \n";
      char *new_text = NULL;
      if (asprintf (&new_text, text, s) != -1)
        return new_text;
    }
  return (char *) text;
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "squares", 's', "FILE", 0, "Use perfect squares in FILE for the main loop"},
  { NULL, '1', "NUM", 0, "Do one iteration with NUM as first square"},
  { "increment", 'I', "NUM", 0, "Advance by NUM squares instead of 1"},
  { 0 }
};

static struct argp
argp =
{
  options, parse_opt, "MIN MAX",
  "Find arithmetic progressions of 9 numbers that have perfect squares.\vThis program searches for perfect squares in this progression:\n%s",
  0,
  help_filter
};

int
main (int argc, char **argv)
{
  struct fv_app_fulcrum_progression2_t app;
  memset (&app, 0, sizeof (app));
  app.incr = 1;
  app.display_record = display_nine_record;
  app.out = stdout;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_fulcrum_progression2 (&app);
}
