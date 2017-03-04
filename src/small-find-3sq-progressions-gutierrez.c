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

struct fv_app_small_gutierrez_progressions_t
{
int num_args;
long long max, startc, starte, startb;
};

//from:
//http://www.oddwheel.com/square_sequence%20tableGV.html
//an algorithm described by Eddie N Gutierrez. 
//
//it looks like c must be a prime number
//but getting an e value is more difficult
//any even number over 3 and under c?
int
gutierrez (struct fv_app_small_gutierrez_progressions_t *app, FILE *out)
{
  long long a, b, c, d, e, s, f, g, twob, n, n2, fourc, fourb, e2, en, twob2, c2;
  long long na, nb, nc, delta1, delta2, na2, nb2, nc2;
  long long ob, oc;
  a = 1;
  b = app->startb;
  c = app->startc;

  if (!app->starte)
    e = abs (c - b);
  else
    e = app->starte;
  g = 2 * e;
  e2 = e * e;
  ob = b;
  oc = c;
  for (n = 0; n < app->max; n++)
    {
      n2 = n * n;
      en = e * n;
      c2 = oc * oc;
      fourc = 4 * oc;
      fourb = 4 * ob;
      twob = ob + ob;
      twob2 =  2 * ob * ob;
      s = (2 * e2 * n2) + (fourc - fourb) * en + (1 - twob2 + c2);
      //printf("last part is %d\n", (1 - twob2 + c2));
      d = 2 * (twob - oc - 1);
      f = rintl ((long double)s / (long double)d);
      //printf("s is %d, d is %d\n", s, d);
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

int
fituvalu_gutierrez (struct fv_app_small_gutierrez_progressions_t *app, FILE *out)
{
  long long a, b, c, k, m, d, s, f, na, nb, nc, delta1, delta2, j, e, g;

  a = 1;
  b = app->startb;
  c = app->startc;

  if (app->startc != 1 && app->startb != 1)
    return 0;
  else if (app->startc != 1)
    {
      k = app->startc;
      // FIXME: there is a bug with startc being < 0 here
      // every other row gets skipped/misaligned wrt delta1/delta2
      m = (k - 1) / 2;
      d = -4 * m;
      j = (k-1) % 64;
    }
  else if (app->startb != 1)
    {
      k = app->startb;
      m = k - 1;
      d = 4 * m;
      j = k % 64;
    }
  else
    return 0;

  switch (j)
    {
    case 1:
    case 33:
      e = d/16;
      break;
    case 9:
    case 17:
    case 25:
    case 41:
    case 49:
    case 57:
      e = d/8;
      break;
    default:
      e = d/4;
      break;
    }
  if (app->starte)
    e = app->starte;
  g = 2 * e;
  for (long long i = 0; i < app->max; i++)
    {
      s = (a*a) + (b*b) + (c*c) - (3*b*b);
      f = rintl ((long double)s / (long double)d);
      na = a + f;
      nb = b + f;
      nc = c + f;
      delta1 = (nb*nb) - (na*na);
      delta2 = (nc*nc) - (nb*nb);

      //printf ("%d, %d, %d, %d, %d, %d, %d, %d, %d\n", a, b, c, f, na, nb, nc, delta1, delta2);
      if (delta1 == delta2)
        fprintf (out, "%lld, %lld, %lld, \n", nc*nc, nb*nb, na*na);
      b += e;
      c += g;
    }
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_small_gutierrez_progressions_t *app = (struct fv_app_small_gutierrez_progressions_t *) state->input;
  switch (key)
    {
    case ARGP_KEY_ARG:
      if (app->num_args == 4)
        argp_error (state, "too many arguments");
      else
        {
          char *end = NULL;
          switch (app->num_args)
            {
            case 0:
              app->max = strtoull (arg, &end, 10);
              break;
            case 1:
              app->startb = strtoull (arg, &end, 10);
              break;
            case 2:
              app->startc = strtoull (arg, &end, 10);
              break;
            case 3:
              app->starte = strtoull (arg, &end, 10);
              break;
            }
          app->num_args++;
        }
      break;
    case ARGP_KEY_NO_ARGS:
      argp_error (state, "missing argument.");
      break;
    case ARGP_KEY_INIT:
      setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
      break;
    }
  return 0;
}

static struct argp argp ={NULL, parse_opt, "MAX [B [C [E]]]", "Find an arithmetic progression consisting of three squares, and that is suitable to be the right diagonal of a 3x3 magic square.\vMAX is how many times we're going to try to make a progression in the sequence.  C is a prime number, and E is an even number over 3 and under C." , 0};

int
main (int argc, char **argv)
{
  struct fv_app_small_gutierrez_progressions_t app;
  memset (&app, 0, sizeof (app));
  app.startc = 3;
  app.startb = 1;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_gutierrez (&app, stdout);
}
