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

int num_args;
unsigned long long max, min;

static void
seq (unsigned long long int m, unsigned long long int n, int finish, FILE *out)
{
  fprintf (out, "%llu, %llu\n", m, n);
  unsigned long long int s = m + n;
  if (s < finish)
    {
      unsigned long long int m2 = s;
      unsigned long long int n2 = m;
      if ((n2 & 1) == 0)
        seq (m2, n2, finish, out);
      else
        {
          s = m2 + n2;
          if (s < finish)
            {
              seq (s, m2, finish, out);
              seq (s, n2, finish, out);
            }
        }
      n2 = n;
      if ((n2 & 1) == 0)
        seq (m2, n2, finish, out);
      else
        {
          s = m2 + n2;
          if (s < finish)
            {
              seq (s, m2, finish, out);
              seq (s, n2, finish, out);
            }
        }
    }
}

static int
morgenstern_seq (FILE *out)
{
  if (min)
    seq (min-1, min-2, max, out);
  else
    seq (2, 1, max, out);
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
          char *end = NULL;
          switch (num_args)
            {
            case 0:
              max = strtoull (arg, &end, 10);
              break;
            case 1:
              min = max;
              max = strtoull (arg, &end, 10);
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
