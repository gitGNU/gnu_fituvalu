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
#include <math.h>
#include <string.h>
#include <stdlib.h>

int num_args;
long long max, startc = 3, starte;

//from:
//http://www.oddwheel.com/square_sequence%20tableGV.html
//an algorithm described by Eddie N Gutierrez. 
//
//it looks like c must be a prime number
//but getting an e value is more difficult
//any even number over 3 and under c?
static int
gutierrez (FILE *out)
{
  long long a, b, c, d, e, s, f, g, twob, n, n2, fourc, fourb, e2, en, twob2, c2;
  long long na, nb, nc, delta1, delta2, na2, nb2, nc2;
  long long ob, oc;
  a = 1;
  b = 1;
  c = startc;

  if (!starte)
    e = c - b;
  else
    e = starte;
  g = 2 * e;
  e2 = e * e;
  ob = b;
  oc = c;
  for (n = 0; n < max; n++)
    {
      n2 = n * n;
      en = e * n;
      c2 = oc * oc;
      fourc = 4 * oc;
      fourb = 4 * ob;
      twob = 2 * ob;
      twob2 = twob + twob;
      s = (2 * e2 * n2) + (fourc - fourb) * en + (1 - twob2 + c2);
      d = 2 * (twob - oc - 1);
      f = rintl ((long double)s / (long double)d);
      na = a + f;
      nb = b + f;
      nc = c + f;
      nc2 = nc * nc;
      nb2 = nb * nb;
      na2 = na * na;
      delta1 = nb2 - na2;
      delta2 = nc2 - nb2;
      //printf ("%d, %d, %d, %d, %d, %d, %d, %d, %d\n", a, b, c, f, na, nb, nc, delta1, delta2);
      if (delta1 == delta2)
        printf ("%lld, %lld, %lld, \n", nc2, nb2, na2);
      b += e;
      c += g;
    }
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case ARGP_KEY_ARG:
      if (num_args == 3)
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
              startc = strtoull (arg, &end, 10);
              break;
            case 2:
              starte = strtoull (arg, &end, 10);
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

struct argp argp ={NULL, parse_opt, "MAX [C [E]]", "Find an arithmetic progression consisting of three squares, and that is suitable to be the right diagonal of a 3x3 magic square.\vMAX is how many times we're going to try to make a progression in the sequence.  C is a prime number, and E is an even number over 3 and under C." , 0};

int
main (int argc, char **argv)
{
  setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
  argp_parse (&argp, argc, argv, 0, 0, 0);
  int ret = gutierrez (stdout);
  return ret;
}
