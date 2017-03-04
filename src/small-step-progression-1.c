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
struct fv_app_small_step_progession1_t
{
  FILE *instream;
  unsigned long long start, finish;
  int num_args;
  FILE *out;
};

static inline void
check_progression (unsigned long long *progression, unsigned long long one, unsigned long long two, unsigned long long three, unsigned long long four, void *data)
{
  struct fv_app_small_step_progession1_t *app = (struct fv_app_small_step_progession1_t *) data;
  progression[0] = one;
  progression[1] = two;
  progression[2] = three;
  progression[3] = four;
  int count = 4;
  progression[4] = progression[3] + progression[1] - progression[0];
  progression[5] = progression[4] + progression[1] - progression[0];
  progression[6] = progression[5] + progression[3] - progression[2];
  progression[7] = progression[6] + progression[1] - progression[0];
  progression[8] = progression[7] + progression[1] - progression[0];
  for (int i = 4; i <= 8; i++)
    {
      if (small_is_square (progression[i]))
        count++;
    }
  if (count > 4)
    {
      fprintf (app->out, 
               "%llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, \n",
               progression[0], progression[1], progression[2],
               progression[3], progression[4], progression[5],
               progression[6], progression[7], progression[8]);
    }
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_small_step_progession1_t *app = (struct fv_app_small_step_progession1_t *) state->input;
  switch (key)
    {
    case 'i':
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
          char *end = NULL;
          switch (app->num_args)
            {
            case 0:
              app->start = strtoull (arg, &end, 10);
              break;
            case 1:
              app->finish = strtoull (arg, &end, 10);
              break;
            }
          app->num_args++;
        }
      break;
    case ARGP_KEY_FINI:
      if (app->num_args != 2)
        argp_error (state, "not enough arguments");
      break;
    case ARGP_KEY_INIT:
      setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
      break;
    }
  return 0;
}

int
fituvalu_small_step_progression1 (struct fv_app_small_step_progession1_t *app)
{
  if (app->instream)
    small_read_square_and_run (app->instream,
                               small_fwd_4sq_progression1,
                               check_progression,
                               app->start, app->finish, app);
  else
    small_loop_and_run (small_fwd_4sq_progression1,
                        check_progression,
                        app->start, app->finish, app);

  return 0;
}

static char *
help_filter (int key, const char *text, void *input)
{
  if (key == ARGP_KEY_HELP_POST_DOC)
    {
      char *s =
        "|-----+--+--+-------+--+--+-------+--+--+------|\n"
        "      ^  ^  ^       ^                           \n"
        "      1  2  3       4                           \n";
      char *new_text = NULL;
      if (asprintf (&new_text, text, s) != -1)
        return new_text;
    }
  return (char *) text;
}

static struct argp_option
options[] =
{
    { "squares", 'i', "FILE", 0, "Use perfect squares in FILE for the main loop"},
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
  struct fv_app_small_step_progession1_t app;
  memset (&app, 0, sizeof (app));
  app.out = stdout;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_small_step_progression1 (&app);
}
