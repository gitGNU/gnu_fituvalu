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
#include <stdlib.h>
#include <math.h>
#include <argp.h>
#include <signal.h>
#include <gmp.h>
#include <stdbool.h>
#include <string.h>
#include "magicsquareutil.h"

struct fv_app_nine_search_t
{
  mpz_t sq;
  mpz_t lastsq;
};

struct fv_app_nine_search_t *g;

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_nine_search_t *app = (struct fv_app_nine_search_t *) state->input;
  switch (key)
    {
    case 'c':
      mpz_set_str (app->sq, arg, 10);
      break;
    case ARGP_KEY_INIT:
      mpz_init (app->sq);
      mpz_init (app->lastsq);
      break;
    }
  return 0;
}

static void
calculate_middle_square (mpz_t c)
{
  //the idea here is to produce a center square value that is divisible by all
  //numbers in divisors, and is a perfect square
  int multiples[] = {
    29,
    37,
    41,
    //48,
    53,
    61,
    //72,
    120,
    168,
    264,
    312,
    408,
    456,
    552,
    744,
    1032,
    1128,
    1608,
    0};
  int divisors[] = {
    29,
    37,
    41,
    48,
    53,
    61,
    72,
    120,
    168,
    264,
    312,
    408,
    456,
    552,
    744,
    1032,
    1128,
    1608,
    0};
  int *i;
  mpz_set_ui (c, 1);
  i = multiples;
  while (*i != 0)
    {
      mpz_mul_ui (c, c, *i);
      i++;
    }
  mpz_mul (c, c, c);
  mpz_cdiv_q_ui (c, c, 10);
  i--;
  while (*i != 120)
    {
      mpz_cdiv_q_ui (c, c, *i);
      i--;
    }
  mpz_cdiv_q_ui (c, c, 72);
  mpz_cdiv_q_ui (c, c, 61);
  mpz_cdiv_q_ui (c, c, 53);
  mpz_cdiv_q_ui (c, c, 48);
  mpz_cdiv_q_ui (c, c, 41);
  mpz_cdiv_q_ui (c, c, 37);
  mpz_cdiv_q_ui (c, c, 29);

  mpz_cdiv_q_ui (c, c, 2);
  mpz_cdiv_q_ui (c, c, 3);

  mpz_mul (c, c, c);
  mpz_cdiv_q_ui (c, c, 1608);
  mpz_cdiv_q_ui (c, c, 1128);
  mpz_cdiv_q_ui (c, c, 1032);
  mpz_cdiv_q_ui (c, c, 744);
  mpz_cdiv_q_ui (c, c, 552);
  mpz_cdiv_q_ui (c, c, 456);
  mpz_cdiv_q_ui (c, c, 312);
  mpz_cdiv_q_ui (c, c, 264);
  mpz_cdiv_q_ui (c, c, 168);
  mpz_cdiv_q_ui (c, c, 120);
  mpz_cdiv_q_ui (c, c, 72);
  mpz_cdiv_q_ui (c, c, 61);
  mpz_cdiv_q_ui (c, c, 53);
  mpz_cdiv_q_ui (c, c, 48);
  mpz_cdiv_q_ui (c, c, 41);
  mpz_cdiv_q_ui (c, c, 37);
  mpz_cdiv_q_ui (c, c, 29);
  mpz_cdiv_q_ui (c, c, 2);
  mpz_cdiv_q_ui (c, c, 3);
  mpz_cdiv_q_ui (c, c, 4);
  mpz_cdiv_q_ui (c, c, 8);
  mpz_cdiv_q_ui (c, c, 2);
  mpz_cdiv_q_ui (c, c, 4);
  mpz_cdiv_q_ui (c, c, 2);
  mpz_cdiv_q_ui (c, c, 4);
  mpz_cdiv_q_ui (c, c, 2);
  mpz_mul (c, c, c); //here is  10^53
  mpz_cdiv_q_ui (c, c, 9);
  mpz_cdiv_q_ui (c, c, 4); //here is 10^51
  mpz_cdiv_q_ui (c, c, 4);
  mpz_cdiv_q_ui (c, c, 289); //here is 10^48

  i = divisors;
  while (*i != 0)
    {
      if (!mpz_divisible_ui_p (c, *i))
        {
          printf("not divisible by %d\n", *i);
        exit (0);
        }
      i++;
    }
  if (!mpz_perfect_square_p (c))
    {
      printf ("not a square\n");
      exit (0);
    }
}

static int
fill_in (mpz_t s[3][3], mpz_t a, mpz_t b, mpz_t c)
{
  mpz_t absum, abdiff;
  mpz_inits (absum, abdiff, NULL);
  mpz_add (absum, a, b);
  mpz_sub (abdiff, a, b);
  mpz_sub (s[0][0], c, b);
  mpz_add (s[2][2], c, b);
  mpz_add (s[0][1], c, absum);
  mpz_sub (s[1][0], c, abdiff);
  mpz_add (s[1][2], c, abdiff);
  mpz_sub (s[2][1], c, absum);
  mpz_clears (absum, abdiff, NULL);
  return 1;
}

static void
generate_square (mpz_t c, mpz_t a, mpz_t s[3][3], FILE *out)
{
  mpz_t b;
  mpz_inits (b, NULL);
  mpz_sqrt (a, a);
  mpz_set (s[1][1], c);
  mpz_add (s[2][0], c, a);
  for (unsigned long long j = 1; j < 100; j+=1)
    {
      mpz_t h;
      mpz_init (h);
      mpz_sqrt (h, s[2][0]);
      mpz_add_ui (h, h, j);
      mpz_mul (s[2][0], h, h);
      mpz_clear (h);
      mpz_sub (a, s[2][0], c);
      mpz_sub (s[0][2], c, a);

      for (unsigned long long i = 1; i < 100; i+=1)
        {
          mpz_init (h);
          mpz_add (h, c, a);
          mpz_sqrt (h, h);
          mpz_add_ui (h, h, i);
          mpz_mul (h, h, h);
          mpz_sub (b, h, c);
          mpz_sub (b, b, a);
          mpz_clear (h);
          if (fill_in (s, a, b, c))
            display_square_record (s, out);
        }
    }
  mpz_clears (b, NULL);
}

static void generate_progression (struct fv_app_nine_search_t *app, mpz_t num, FILE *out)
{
  mpz_t root, j, r, s, prevr, diff;
  mpz_inits (root, j, r, s, prevr, diff, NULL);
  if (mpz_cmp_ui (app->sq, 0) == 0)
    mpz_set (app->sq, num);
  mpz_sqrt (root, app->sq);
  mpz_mul (app->sq, root, root);
  int incr = 1;
  /*
   * go forward by 1 square at a time until we find one that is 1 mod 24
   * then switch to every 4, and then then every 2
   * if the one after us is zero, the next one is 2 away
   */
  mpz_set_si (r, -1);
  int i = 1;
  while (1)
    {
      for (int k = 0; k < i; k++)
        {
          mpz_add (app->sq, app->sq, root);
          mpz_add (app->sq, app->sq, root);
          mpz_add_ui (app->sq, app->sq, incr);
          mpz_add_ui (root, root, incr);
        }

      if (incr == 1 && i == 1)
        {
          mpz_set (prevr, r);
          mpz_mod_ui (r, app->sq, 24);
          if (mpz_cmp_ui (r, 1) != 0)
            continue;
          else
            {
              char buf[mpz_sizeinbase (app->sq, 10) + 2];
              mpz_get_str (buf, 10, app->sq);
              if (mpz_cmp_si (prevr, -1) == 0)
                {
                  printf("crappers. we have some work to do here\n");
                  break;
                }
              if (mpz_cmp_ui (prevr, 0) == 0 ||
                  mpz_cmp_ui (prevr, 12) == 0)
                {
                  fprintf (out, "got %s as 1 mod 24, continuing by 4s and 2s\n", buf);
                  i = 4;
                }
              else
                {
                  fprintf (out, "got %s as 1 mod 24, continuing by 2s and 4s\n", buf);
                  i = 2;
                }
            }
        }
      else if (i == 2)
        i = 4;
      else if (i == 4)
        i = 2;

      mpz_sub (diff, app->sq, num);
      mpz_sub (j, num, diff);
      mpz_set (app->lastsq, app->sq);
      mpz_mod_ui (s, j, 24);
      if (mpz_cmp_ui (s, 1) != 0)
        continue;
      if (mpz_perfect_square_p (j))
        {
            {
              char buf[mpz_sizeinbase (j, 10) + 2];
              mpz_get_str (buf, 10, j);
              fprintf (out, "%s, ", buf);
            }
            {
              char buf[mpz_sizeinbase (num, 10) + 2];
              mpz_get_str (buf, 10, num);
              fprintf (out, "%s, ", buf);
            }
            {
              char buf[mpz_sizeinbase (app->sq, 10) + 2];
              mpz_get_str (buf, 10, app->sq);
              fprintf (out, "%s\n", buf);
            }
        }
    }
  mpz_clears (root, j, r, s, prevr, diff, NULL);
}

#define MAX 10
void
fituvalu_nine_search (struct fv_app_nine_search_t *app, FILE *out)
{
  mpz_t sqs[MAX], a, c;
  mpz_t s[3][3];
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_init (s[i][j]);
  mpz_inits (a, c, NULL);
  calculate_middle_square (c);
    {
      char buf[mpz_sizeinbase (c, 10) + 2];
      mpz_get_str (buf, 10, c);
      fprintf (out, "Using a center square value of:\n%s\n", buf);
    }
  for (int i = 0; i < MAX; i++)
    mpz_init (sqs[i]);

  mpz_set_str (sqs[0], "1141038727000515012471639230946556542452519793198975538605757840922089881600", 10);

  mpz_set_str (sqs[1], "382131560986433622039381065319357552636758868499956978538986759782400", 10);
  mpz_set_str (sqs[2], "2953084635046000403178814795705439251046325551366400", 10);
  mpz_set_str (sqs[3], "328120515005111155908757199522826583449591727929600", 10);
  mpz_set_str (sqs[4], "82030128751277788977189299880706645862397931982400", 10);
  mpz_set_str (sqs[5], "20507532187819447244297324970176661465599482995600", 10);
  mpz_set_str (sqs[6], "283841275955978508571589272943621611980615681600", 10);
  mpz_set_str (sqs[7], "1301969376514955827350056968602881708416749345672067025360683326640113771587666155193469956405902674008804368055378079990024552377993237749102018560000", 10);
  mpz_set_str (sqs[8], "80565869936317943405011564868041307925877071120461853796097043311951210848117230199832578560000", 10);

  mpz_set (sqs[MAX-1], c);

  mpz_set_str (a, "54342291404080490827096080", 10);

  //for (int i = 0; i < MAX; i++)
  generate_progression (app, sqs[MAX-1], out);

  //for (int i = 0; i < MAX; i++)
    //generate_square (sqs[i], a, s, out);

  for (int i = 0; i < MAX; i++)
    mpz_clear (sqs[i]);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_clear (s[i][j]);
  mpz_clears (a, c, NULL);

  //according to someone on multimagie, one of (a, b, c, b+c, b-c)
  //must be divisible by a the set of divisors seen herein.
  //
  //so we find the lowest number that has those divisors and is a perfect
  //square.
  //
  //in this program we're gambling that it's the center square c.
  //
  //but it might not be the center square.
  // it can be one of: (a, b, c, b+c, b-c)
  // if it doesn't have to be a perfect square (c, b+c, or b-c),
  // we can go with the much smaller value of 54342291404080490827096080
  //
  //we take the closest a value that is a square
  //and then we iterate over b values

}

void intHandler(int dummy)
{
  char buf[mpz_sizeinbase (g->lastsq, 10) + 2];
  mpz_get_str (buf, 10, g->lastsq);
  fprintf (stderr, "\nStopped searching at square: %s\n", buf);
  exit (1);
}


static struct argp_option
options[] =
{
  { "continue", 'c', "NUM", OPTION_HIDDEN, "Continue at this square"},
  { 0 }
};

static struct argp
argp =
{
  options, parse_opt, 0,
  "Search for a 3x3 magic square of 9 perfect squares.",
  0
};
int
main (int argc, char **argv)
{
  struct fv_app_nine_search_t app;
  memset (&app, 0, sizeof (app));
  g = &app;
  signal(SIGINT, intHandler);
  argp_parse (&argp, argc, argv, 0, 0, &app);
  fituvalu_nine_search (&app, stdout);

  return 0;
}

//left off at 70960318989168414918317505017485401453316883664
