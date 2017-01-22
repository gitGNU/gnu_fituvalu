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
FILE *instream;
unsigned long long start, finish;
int num_args;

static inline void
check_progression (unsigned long long *progression, unsigned long long one, unsigned long long two, unsigned long long three, unsigned long long four, FILE *out)
{
/* |-----+--+--+-------+--+--+--|
         ^     ^       ^     ^
         1     2       3     4 */
  progression[0] = one;
  progression[2] = two;
  progression[3] = three;
  progression[5] = four;
  int count = 4;
  unsigned long long d1 = (progression[2] - progression[0]) / 2;
  unsigned long long e1 = progression[3] - progression[2];
  progression[1] = progression[0] + d1;
  progression[4] = progression[3] + d1;
  progression[6] = progression[5] + e1;
  progression[7] = progression[6] + d1;
  progression[8] = progression[7] + d1;

  if (small_is_square (progression[1]))
    count++;
  if (small_is_square (progression[4]))
    count++;
  for (int i = 6; i <= 8; i++)
    {
      if (small_is_square (progression[i]))
        count++;
    }
  if (count > 4)
    {
      fprintf (out, 
               "%llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, \n",
               progression[0], progression[1], progression[2],
               progression[3], progression[4], progression[5],
               progression[6], progression[7], progression[8]);
    }
}

static struct argp_option
options[] =
{
    { "squares", 'i', "FILE", 0, "Use perfect squares in FILE for the main loop"},
    { 0 }
};

static char *
help_filter (int key, const char *text, void *input)
{
  if (key == ARGP_KEY_HELP_POST_DOC)
    {
      char *s =
        "|------+--+--+-------+--+--+------+--+--+-----|\n"
        "       ^     ^       ^     ^                   \n"
        "       1     2       3     4                   \n";
      char *new_text = NULL;
      if (asprintf (&new_text, text, s) != -1)
        return new_text;
    }
  return (char *) text;
}


static error_t parse_opt (int key, char *arg, struct argp_state *state);
struct argp argp ={options, parse_opt, "MIN MAX", "Find arithmetic progressions of 9 numbers that have perfect squares.\vThis program searches for perfect squares in this progression:\n%s" , 0, help_filter};
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'i':
      if (strcmp (arg, "-") == 0)
        instream = stdin;
      else
        instream = fopen (arg, "r");
      break;
    case ARGP_KEY_ARG:
      if (num_args == 2)
        argp_error (state, "too many arguments");
      else
        {
          char *end = NULL;
          switch (num_args)
            {
            case 0:
              start = strtoull (arg, &end, 10);
              break;
            case 1:
              finish = strtoull (arg, &end, 10);
              break;
            }
          num_args++;
        }
      break;
    case ARGP_KEY_FINI:
      if (num_args != 2)
        argp_error (state, "not enough arguments");
      break;
    }
  return 0;
}

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);

  if (instream)
    small_read_square_and_run (instream,
                               small_fwd_4sq_progression4,
                               check_progression,
                               start, finish, stdout);
  else
    small_loop_and_run (small_fwd_4sq_progression4,
                        check_progression,
                        start, finish, stdout);

  return 0;
}
