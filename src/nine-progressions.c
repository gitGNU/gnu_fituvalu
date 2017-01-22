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
#include <string.h>
#include <math.h>
#include "magicsquareutil.h"

void
four_square_to_nine_number_step_progression1 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four)
{
  mpz_set (progression[0], one);
  mpz_set (progression[1], two);
  mpz_set (progression[2], three);
  mpz_set (progression[3], four);
  mpz_t amt1, amt2;
  mpz_inits (amt1, amt2, NULL);
  mpz_sub (amt1, progression[1], progression[0]);
  mpz_sub (amt2, progression[3], progression[2]);
  mpz_add (progression[4], progression[3], amt1);
  mpz_add (progression[5], progression[4], amt1);
  mpz_add (progression[6], progression[5], amt2);
  mpz_add (progression[7], progression[6], amt1);
  mpz_add (progression[8], progression[7], amt1);
  mpz_clears (amt1, amt2, NULL);
}

void
four_square_to_nine_number_step_progression2 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four)
{
  mpz_set (progression[1], one);
  mpz_set (progression[2], two);
  mpz_set (progression[3], three);
  mpz_set (progression[4], four);
  mpz_t amt1, amt2;
  mpz_inits (amt1, amt2, NULL);
  mpz_sub (amt1, progression[2], progression[1]);
  mpz_sub (amt2, progression[3], progression[2]);
  mpz_sub (progression[0], progression[1], amt1);
  mpz_add (progression[5], progression[4], amt1);
  mpz_add (progression[6], progression[5], amt2);
  mpz_add (progression[7], progression[6], amt1);
  mpz_add (progression[8], progression[7], amt1);
  mpz_clears (amt1, amt2, NULL);
}

void
four_square_to_nine_number_step_progression3 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four)
{
  mpz_t d1, e1, twozerodif;
  mpz_inits (d1, e1, twozerodif, NULL);
  mpz_set (progression[0], one);
  mpz_set (progression[2], two);
  mpz_set (progression[3], three);
  mpz_set (progression[5], four);
  mpz_sub (twozerodif, progression[2], progression[0]);
  mpz_cdiv_q_ui (d1, twozerodif, 2);
  mpz_sub (e1, progression[3], progression[2]);
  mpz_add (progression[1], progression[0], d1);
  mpz_add (progression[4], progression[3], d1);
  mpz_add (progression[6], progression[5], e1);
  mpz_add (progression[7], progression[6], d1);
  mpz_add (progression[8], progression[7], d1);
  mpz_clears (d1, e1, twozerodif, NULL);
}

void
four_square_to_nine_number_step_progression4 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four)
{
  mpz_t d1, e1;
  mpz_inits (d1, e1, NULL);
  mpz_set (progression[1], one);
  mpz_set (progression[2], two);
  mpz_set (progression[3], three);
  mpz_set (progression[5], four);
  mpz_sub (d1, progression[2], progression[1]);
  mpz_sub (e1, progression[3], progression[2]);
  mpz_sub (progression[0], progression[1], d1);
  mpz_add (progression[4], progression[3], d1);
  mpz_add (progression[6], progression[5], e1);
  mpz_add (progression[7], progression[6], d1);
  mpz_add (progression[8], progression[7], d1);
  mpz_clears (d1, e1, NULL);
}

void
four_square_to_nine_number_reverse_step_progression1 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four)
{
  mpz_set (progression[0], one);
  mpz_set (progression[1], two);
  mpz_set (progression[2], three);
  mpz_set (progression[3], four);
  mpz_t amt1, amt2;
  mpz_inits (amt1, amt2, NULL);
  mpz_sub (amt1, progression[1], progression[0]);
  mpz_sub (amt2, progression[3], progression[2]);
  mpz_add (progression[4], progression[3], amt1);
  mpz_add (progression[5], progression[4], amt1);
  mpz_add (progression[6], progression[5], amt2);
  mpz_add (progression[7], progression[6], amt1);
  mpz_add (progression[8], progression[7], amt1);
  mpz_clears (amt1, amt2, NULL);
}

void
four_square_to_nine_number_fulcrum_progression1 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four)
{
  mpz_set (progression[0], one);
  mpz_set (progression[1], two);
  mpz_set (progression[2], three);
  mpz_set (progression[3], four);
  mpz_t d1, e1, f1;
  mpz_inits (d1, e1, f1, NULL);
  mpz_sub (d1, progression[1], progression[0]);
  mpz_sub (e1, progression[2], progression[1]);
  mpz_sub (f1, progression[3], progression[2]);
  mpz_add (progression[4], progression[3], e1);
  mpz_add (progression[5], progression[4], e1);
  mpz_add (progression[6], progression[5], f1);
  mpz_add (progression[7], progression[6], e1);
  mpz_add (progression[8], progression[7], d1);
  mpz_clears (d1, e1, f1, NULL);
}

void
four_square_to_nine_number_fulcrum_progression2 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four)
{
  mpz_set (progression[1], one);
  mpz_set (progression[2], two);
  mpz_set (progression[3], three);
  mpz_set (progression[4], four);
  mpz_t d1, e1, f1;
  mpz_inits (d1, e1, f1, NULL);
  mpz_sub (e1, progression[2], progression[1]);
  mpz_sub (f1, progression[3], progression[2]);
  mpz_add (d1, e1, f1);
  mpz_sub (progression[0], progression[1], d1);
  mpz_add (progression[5], progression[4], e1);
  mpz_add (progression[6], progression[5], f1);
  mpz_add (progression[7], progression[6], e1);
  mpz_add (progression[8], progression[7], d1);
  mpz_clears (d1, e1, f1, NULL);
}

void
four_square_to_nine_number_fulcrum_progression3 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four)
{
  mpz_set (progression[3], one);
  mpz_set (progression[4], two);
  mpz_set (progression[5], three);
  mpz_set (progression[6], four);
  mpz_t d1, e1, f1;
  mpz_inits (d1, e1, f1, NULL);
  mpz_sub (e1, progression[4], progression[3]);
  mpz_sub (f1, progression[6], progression[5]);
  mpz_add (d1, e1, f1);
  mpz_add (progression[7], progression[6], e1);
  mpz_add (progression[8], progression[7], d1);
  mpz_sub (progression[2], progression[3], f1);
  mpz_sub (progression[1], progression[2], e1);
  mpz_sub (progression[0], progression[1], d1);
  mpz_clears (d1, e1, f1, NULL);
}

void
four_square_to_nine_number_fulcrum_progression4 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four)
{
  mpz_set (progression[0], one);
  mpz_set (progression[1], two);
  mpz_set (progression[3], three);
  mpz_set (progression[5], four);
  mpz_t d1, e1, f1;
  mpz_inits (d1, e1, f1, NULL);
  mpz_sub (d1, progression[1], progression[0]);
  mpz_sub (e1, progression[5], progression[3]);
  mpz_cdiv_q_ui (e1, e1, 2);
  mpz_sub (f1, d1, e1);
  mpz_add (progression[2], progression[1], e1);
  mpz_add (progression[4], progression[3], e1);
  mpz_add (progression[6], progression[5], f1);
  mpz_add (progression[7], progression[6], e1);
  mpz_add (progression[8], progression[7], d1);
  mpz_clears (d1, e1, f1, NULL);
}

void
four_square_to_nine_number_fulcrum_progression5 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four)
{
  mpz_set (progression[1], one);
  mpz_set (progression[2], two);
  mpz_set (progression[3], three);
  mpz_set (progression[5], four);
  mpz_t d1, e1, f1;
  mpz_inits (d1, e1, f1, NULL);
  mpz_sub (e1, progression[2], progression[1]);
  mpz_sub (f1, progression[3], progression[2]);
  mpz_add (d1, e1, f1);
  mpz_sub (progression[0], progression[1], d1);
  mpz_add (progression[4], progression[3], e1);
  mpz_add (progression[6], progression[5], f1);
  mpz_add (progression[7], progression[6], e1);
  mpz_add (progression[8], progression[7], d1);
  mpz_clears (d1, e1, f1, NULL);
}

void
four_square_to_nine_number_reverse_fulcrum_progression1 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four)
{
  mpz_set (progression[5], one);
  mpz_set (progression[4], two);
  mpz_set (progression[3], three);
  mpz_set (progression[2], four);
  mpz_t d1, e1, f1;
  mpz_inits (d1, e1, f1, NULL);
  mpz_sub (e1, progression[5], progression[4]);
  mpz_sub (f1, progression[3], progression[2]);
  mpz_add (d1, e1, f1);
  mpz_add (progression[6], progression[5], f1);
  mpz_add (progression[7], progression[6], e1);
  mpz_add (progression[8], progression[7], d1);

  mpz_sub (progression[1], progression[2], e1);
  mpz_sub (progression[0], progression[1], d1);
  mpz_clears (d1, e1, f1, NULL);
}

int
count_squares_in_step_progression1 (mpz_t *progression)
{
  int count = 4;
  for (int i = 4; i <= 8; i++)
    {
      if (mpz_perfect_square_p (progression[i]))
        count++;
    }
  return count;
}

int
count_squares_in_step_progression2 (mpz_t *progression)
{
  int count = 4;
  if (mpz_perfect_square_p (progression[0]))
    count++;
  for (int i = 5; i <= 8; i++)
    {
      if (mpz_perfect_square_p (progression[i]))
        count++;
    }
  return count;
}

int
count_squares_in_step_progression3 (mpz_t *progression)
{
  int count = 4;
  if (mpz_perfect_square_p (progression[1]))
    count++;
  if (mpz_perfect_square_p (progression[4]))
    count++;
  for (int i = 6; i <= 8; i++)
    {
      if (mpz_perfect_square_p (progression[i]))
        count++;
    }
  return count;
}

int
count_squares_in_step_progression4 (mpz_t *progression)
{
  int count = 4;
  if (mpz_perfect_square_p (progression[0]))
    count++;
  if (mpz_perfect_square_p (progression[4]))
    count++;
  for (int i = 6; i <= 8; i++)
    {
      if (mpz_perfect_square_p (progression[i]))
        count++;
    }
  return count;
}

int
count_squares_in_reverse_step_progression1 (mpz_t *progression)
{
  int count = 4;
  for (int i = 4; i <= 8; i++)
    {
      if (mpz_perfect_square_p (progression[i]))
        count++;
    }
  return count;
}

int
count_squares_in_fulcrum_progression1 (mpz_t *progression)
{
  int count = 4;
  for (int i = 4; i <= 8; i++)
    {
      if (mpz_perfect_square_p (progression[i]))
        count++;
    }
  return count;
}

int
count_squares_in_fulcrum_progression2 (mpz_t *progression)
{
  int count = 4;
  if (mpz_perfect_square_p (progression[0]))
    count++;
  for (int i = 5; i <= 8; i++)
    {
      if (mpz_perfect_square_p (progression[i]))
        count++;
    }
  return count;
}

int
count_squares_in_fulcrum_progression3 (mpz_t *progression)
{
  int count = 4;
  for (int i = 0; i <= 2; i++)
    {
      if (mpz_perfect_square_p (progression[i]))
        count++;
    }
  for (int i = 7; i <= 8; i++)
    {
      if (mpz_perfect_square_p (progression[i]))
        count++;
    }
  return count;
}

int
count_squares_in_fulcrum_progression4 (mpz_t *progression)
{
  int count = 4;
  if (mpz_perfect_square_p (progression[2]))
    count++;
  if (mpz_perfect_square_p (progression[4]))
    count++;
  for (int i = 6; i <= 8; i++)
    {
      if (mpz_perfect_square_p (progression[i]))
        count++;
    }
  return count;
}

int
count_squares_in_fulcrum_progression5 (mpz_t *progression)
{
  int count = 4;
  if (mpz_perfect_square_p (progression[0]))
    count++;
  if (mpz_perfect_square_p (progression[4]))
    count++;
  for (int i = 6; i <= 8; i++)
    {
      if (mpz_perfect_square_p (progression[i]))
        count++;
    }
  return count;
}

int
count_squares_in_reverse_fulcrum_progression1 (mpz_t *progression)
{
  int count = 4;
  for (int i = 0; i <= 1; i++)
    {
      if (mpz_perfect_square_p (progression[i]))
        count++;
    }
  for (int i = 6; i <= 8; i++)
    {
      if (mpz_perfect_square_p (progression[i]))
        count++;
    }
  return count;
}

char *
generate_list_of_nine_progression_types ()
{
  char *s = NULL;
  int i = 0;
  while (nine_progressions[i].progfunc)
    {
      char *t = nine_progressions[i].name;
      if (!s)
        asprintf (&s, "\"%s\"", t);
      else
        {
          if (!nine_progressions[i+1].progfunc)
            asprintf (&s, "%s, or \"%s\"", s, t);
          else
            asprintf (&s, "%s, \"%s\"", s, t);
        }
      i++;
    }
  return s;
}

static int
get_longest_width_of_nine_progression_type_name ()
{
  int i = 0;
  size_t max = 0;
  while (nine_progressions[i].progfunc)
    {
      char *t = nine_progressions[i].name;
      if (strlen (t) > max)
        max = strlen (t);
      i++;
    }
  return max;
}

char *
generate_list_of_nine_progression_timelines ()
{
  int w = get_longest_width_of_nine_progression_type_name ();
  char *spaces = malloc (w + 2 * sizeof (char));
  memset (spaces, ' ', w + 1);
  spaces[w + 1] = '\0';

  char *s = malloc (1);
  s[0] = '\0';
  int i = 0;
  /*FIXME: there's some horrible off-by-one stuff going on with count */
  while (nine_progressions[i].progfunc)
    {
      char *t = strdup (nine_progressions[i].timeline);
      char *n = nine_progressions[i].name;
      char *l, *sav;
      int count = 0;
      for (l = strtok_r (t, "\n", &sav); l; l = strtok_r (NULL, "\n", &sav))
        {
          if (count == 0)
            {
              strncpy (spaces, n, strlen (n));
              spaces[strlen (n)] = ':';
            }
          else
            memset (spaces, ' ', w + 1);
          if (!s)
            asprintf (&s, "%s %s\n", spaces, l);
          else
            {
              if (count == 0)
                asprintf (&s, "%s%s %s\n", s, spaces, l);
              else if (count == 2)
                asprintf (&s, "%s%s   %s", s, spaces, l);
              else
                asprintf (&s, "%s%s %s", s, spaces, l);
            }
          count++;
        }
      free (t);
      i++;
    }
  return s;
}

nine_progression_t *
lookup_nine_progression_by_name (char *name)
{
  int i = 0;
  while (nine_progressions[i].progfunc)
    {
      char *t = nine_progressions[i].name;
      if (strcasecmp (t, name) == 0)
        return &nine_progressions[i];
      i++;
    }
  return NULL;
}

nine_progression_t
nine_progressions[] = 
{
    {
      "step-progression1", FWD_PROGRESSION_1,
      four_square_to_nine_number_step_progression1,
      count_squares_in_step_progression1,
      "|-----+--+--+-------+--+--+-------+--+--+------|\n"
      "      ^  ^  ^       ^                           \n"
      "      1  2  3       4                           \n"
    },
    {
      "step-progression2", FWD_PROGRESSION_2,
      four_square_to_nine_number_step_progression2,
      count_squares_in_step_progression2,
      "|-----+--+--+-------+--+--+-------+--+--+------|\n"
      "         ^  ^       ^  ^                        \n"
      "         1  2       3  4                        \n"
    },
    {
      "step-progression3", FWD_PROGRESSION_4,
      four_square_to_nine_number_step_progression3,
      count_squares_in_step_progression3,
      "|------+--+--+-------+--+--+------+--+--+-----|\n"
      "       ^     ^       ^     ^                   \n"
      "       1     2       3     4                   \n"
    },
    {
      "step-progression4", FWD_PROGRESSION_6,
      four_square_to_nine_number_step_progression4,
      count_squares_in_step_progression4,
      "|----+--*--*--------*--+--*--------*--*--+----|\n"
      "        ^  ^        ^     ^\n"
      "        1  2        3     4\n"
    },
    {
      "reverse-step-progression1", REV_PROGRESSION_1,
      four_square_to_nine_number_reverse_step_progression1,
      count_squares_in_reverse_step_progression1,
      "|-----+--+--+-------+--+--+-------+--+--+------|\n"
      "                          ^       ^  ^  ^       \n"
      "                          4       3  2  1       \n"
    },
    {
      "fulcrum-progression1", FWD_PROGRESSION_3,
      four_square_to_nine_number_fulcrum_progression1,
      count_squares_in_fulcrum_progression1,
      "|-+--------+--+-----+--+--+-----+--+--------+--|\n"
      "  ^        ^  ^     ^                           \n"
      "  1        2  3     4                           \n"
    },
    {
      "fulcrum-progression2", FWD_PROGRESSION_2,
      four_square_to_nine_number_fulcrum_progression2,
      count_squares_in_fulcrum_progression2,
      "|-+--------+--+-----+--+--+-----+--+--------+--|\n"
      "           ^  ^     ^  ^                        \n"
      "           1  2     3  4                        \n"
    },
    {
      "fulcrum-progression3", FWD_PROGRESSION_1,
      four_square_to_nine_number_fulcrum_progression3,
      count_squares_in_fulcrum_progression3,
      "|-+--------+--+-----+--+--+-----+--+--------+--|\n"
      "                    ^  ^  ^     ^               \n"
      "                    1  2  3     4               \n"
    },
    {
      "fulcrum-progression4", FWD_PROGRESSION_5,
      four_square_to_nine_number_fulcrum_progression4,
      count_squares_in_fulcrum_progression4,
      "|--+--------+--+-----+--+--+-----+--+--------+-|\n"
      "    ^        ^        ^     ^\n"
      "    1        2        3     4\n"
    },
    {
      "fulcrum-progression5", FWD_PROGRESSION_6,
      four_square_to_nine_number_fulcrum_progression5,
      count_squares_in_fulcrum_progression5,
      "|--+--------*-*-------*-+-*-------*-*--------+-|\n"
      "            ^ ^       ^   ^\n"
      "            1 2       3   4\n"
    },
    {
      "reverse-fulcrum-progression1", REV_PROGRESSION_1,
      four_square_to_nine_number_reverse_fulcrum_progression1,
      count_squares_in_reverse_fulcrum_progression1,
      "|-+--------+--+-----+--+--+-----+--+--------+--|\n"
      "              ^     ^  ^  ^                     \n"
      "              4     3  2  1                     \n"
    },
    { 0, 0, 0, 0, 0 },
};
