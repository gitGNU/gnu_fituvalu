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
#include <argz.h>
#include <math.h>
#include "magicsquareutil.h"

char *
generate_list_of_four_square_progression_types ()
{
  char *s = NULL;
  int i = 0;
  while (four_square_progressions[i].func)
    {
      char *t = four_square_progressions[i].name;
      if (!s)
        asprintf (&s, "\"%s\"", t);
      else
        {
          if (!four_square_progressions[i+1].func)
            asprintf (&s, "%s, or \"%s\"", s, t);
          else
            asprintf (&s, "%s, \"%s\"", s, t);
        }
      i++;
    }
  return s;
}

static int
get_longest_width_of_four_square_progression_type_name ()
{
  int i = 0;
  size_t max = 0;
  while (four_square_progressions[i].func)
    {
      char *t = four_square_progressions[i].name;
      if (strlen (t) > max)
        max = strlen (t);
      i++;
    }
  return max;
}

char *
generate_list_of_four_square_progression_timelines ()
{
  int i = 0;
  int w = get_longest_width_of_four_square_progression_type_name ();
  char sp[32];
  snprintf (sp, sizeof (sp), "%%s%%-%ds%%s\n", w);
  char *s = malloc (1);
  s[0]='\0';
  while (four_square_progressions[i].func)
    {
      char *argz = NULL;
      size_t len = 0;
      char *l;
      argz_create_sep (four_square_progressions[i].timeline, '\n', &argz, &len);
      int first = 1;
      while ((l = argz_next (argz, len, l)))
        {
          if (first)
            {
              asprintf (&s, sp, s, four_square_progressions[i].name, l);
              first = 0;
            }
          else
              asprintf (&s, sp, s, "", l);
        }
      free (argz);
      i++;
    }
  return s;
}

four_square_progression_t *
lookup_four_square_progression_by_name (char *name)
{
  int i = 0;
  while (four_square_progressions[i].func)
    {
      char *t = four_square_progressions[i].name;
      if (strcasecmp (t, name) == 0)
        return &four_square_progressions[i];
      i++;
    }
  return NULL;
}

four_square_progression_t *
lookup_four_square_progression_by_kind (four_square_progression_enum_t kind)
{
  int i = 0;
  while (four_square_progressions[i].func)
    {
      if (four_square_progressions[i].kind == kind)
        return &four_square_progressions[i];
      i++;
    }
  return NULL;
}

static void
inc (mpz_t *j, mpz_t *jroot, unsigned long long incr)
{
  for (int i = 0; i < incr; i++)
    {
      mpz_add (*j, *j, *jroot);
      mpz_add (*j, *j, *jroot);
      mpz_add_ui (*j, *j, 1);
      mpz_add_ui (*jroot, *jroot, 1);
    }
}

static void
dec (mpz_t *j, mpz_t *jroot, unsigned long long incr)
{
  for (int i = 0; i < incr; i++)
    {
      mpz_sub_ui (*jroot, *jroot, 1);
      mpz_sub (*j, *j, *jroot);
      mpz_sub (*j, *j, *jroot);
      mpz_sub_ui (*j, *j, 1);
    }
}

void
fwd_4sq_progression1 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, void *), void *data)
{
/* |-----+--+--+-------+-----|
         ^  ^  ^       ^
         1  2  3       4       */
  mpz_t lastroot, j, k;
  mpz_inits (lastroot, j, k, NULL);
  mpz_sqrt (lastroot, finish);
  mpz_t progression[9];
  mpz_inits (progression[0], progression[1], progression[2],
             progression[3], progression[4], progression[5],
             progression[6], progression[7], progression[8], NULL);

  mpz_t jroot;
  mpz_init (jroot);
  mpz_sqrt (jroot, i);
  for (mpz_set (j, i); mpz_cmp (jroot, lastroot) < 0;)
    {
      inc (&j, &jroot, incr);
      mpz_t diff;
      mpz_init (diff);
      mpz_sub (diff, j, i);
      if (mpz_cmp_ui (diff, 0) > 0)
        {
          mpz_t next;
          mpz_init (next);
          mpz_add (next, j, diff);
          if (!mpz_perfect_square_p (next))
            {
              mpz_clear (next);
              mpz_clear (diff);
              continue;
            }
          mpz_t kroot;
          mpz_init (kroot);
          mpz_sqrt (kroot, next);
          for (mpz_set (k, next); mpz_cmp (kroot, lastroot) < 0;)
            {
              inc (&k, &kroot, incr);
              mpz_t amt;
              mpz_init (amt);
              mpz_sub (amt, k, j);
              mpz_add (amt, amt, diff);
              if (mpz_cmp_ui (amt, 0) > 0 && mpz_cmp (next, k) != 0)
                func (progression, i, j, next, k, data);
              mpz_clear (amt);
            }
          mpz_clear (next);
          mpz_clear (kroot);
        }
      mpz_clear (diff);
    }
  mpz_clears (progression[0], progression[1], progression[2],
              progression[3], progression[4], progression[5],
              progression[6], progression[7], progression[8], NULL);
  mpz_clears (lastroot, j, k, NULL);
}

void
fwd_4sq_progression2 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, void *), void *data)
{
/* |---+--+-------+--+---|
       ^  ^       ^  ^                        
       1  2       3  4    */
  mpz_t j, k, lastroot;
  mpz_inits (j, k, lastroot, NULL);
  mpz_t progression[9];
  mpz_inits (progression[0], progression[1], progression[2],
             progression[3], progression[4], progression[5],
             progression[6], progression[7], progression[8], NULL);

  mpz_sqrt (lastroot, finish);

  mpz_t jroot;
  mpz_init (jroot);
  mpz_sqrt (jroot, i);
  for (mpz_set (j, i); mpz_cmp (jroot, lastroot) < 0;)
    {
      inc (&j, &jroot, incr);
      mpz_t diff;
      mpz_init (diff);
      mpz_sub (diff, j, i);
      if (mpz_cmp_ui (diff, 0) > 0)
        {
          mpz_t next;
          mpz_init (next);
          mpz_set (next, j);
          mpz_t kroot;
          mpz_init (kroot);
          mpz_set (kroot, jroot);
          inc (&next, &kroot, incr);
          for (mpz_set (k, next); mpz_cmp (kroot, lastroot) < 0;)
            {
              inc (&k, &kroot, incr);
              mpz_t amt, fourth;  // amt is E1
              mpz_inits (amt, fourth, NULL);
              mpz_add (fourth, k, diff);
              if (!mpz_perfect_square_p (fourth))
                {
                  mpz_clears (amt, fourth, NULL);
                  continue;
                }
              mpz_sub (amt, k, j);
              if (mpz_cmp_ui (amt, 0) > 0)
                func (progression, i, j, k, fourth, data);
              mpz_clears (amt, fourth, NULL);
            }
          mpz_clear (next);
          mpz_clear (kroot);
        }
      mpz_clear (diff);
    }
  mpz_clears (progression[0], progression[1], progression[2],
              progression[3], progression[4], progression[5],
              progression[6], progression[7], progression[8], NULL);
  mpz_clears (j, k, lastroot, NULL);
}

void
fwd_4sq_progression3 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, void *), void *data)
{
/* |---+--------+--+-----+---|
       ^        ^  ^     ^
       1        2  3     4     */
  mpz_t j, k, lastroot;
  mpz_inits (j, k, lastroot, NULL);
  mpz_t progression[9];
  mpz_inits (progression[0], progression[1], progression[2],
             progression[3], progression[4], progression[5],
             progression[6], progression[7], progression[8], NULL);
  mpz_sqrt (lastroot, finish);
  mpz_t jroot;
  mpz_init (jroot);
  mpz_sqrt (jroot, i);
  for (mpz_set (j, i); mpz_cmp (jroot, lastroot) < 0;)
    {
      inc (&j, &jroot, incr);
      mpz_t diff;
      mpz_init (diff);
      mpz_sub (diff, j, i);
      if (mpz_cmp_ui (diff, 0) > 0)
        {
          mpz_t next;
          mpz_init (next);
          mpz_add (next, j, diff);
          if (!mpz_perfect_square_p (next))
            {
              mpz_clear (next);
              mpz_clear (diff);
              continue;
            }
          mpz_t kroot, lastkroot;
          mpz_inits (kroot, lastkroot, NULL);
          mpz_sqrt (lastkroot, next);
          mpz_sqrt (kroot, j);
          for (mpz_set (k, j); mpz_cmp (kroot, lastkroot) < 0;)
            {
              inc (&k, &kroot, incr);
              if (mpz_cmp (k, j) != 0)
                func (progression, i, j, k, next, data);
            }
          mpz_clear (next);
          mpz_clears (kroot, lastkroot, NULL);
        }
      mpz_clear (diff);
    }
  mpz_clears (progression[0], progression[1], progression[2],
              progression[3], progression[4], progression[5],
              progression[6], progression[7], progression[8], NULL);
  mpz_clears (j, k, lastroot, NULL);
}

void
fwd_4sq_progression4 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, void *), void *data)
{
/* |-----+--+--+-------+--+--+--|
         ^     ^       ^     ^
         1     2       3     4 */
  mpz_t lastroot, j, k;
  mpz_inits (lastroot, j, k, NULL);
  mpz_sqrt (lastroot, finish);
  mpz_t progression[9];
  mpz_inits (progression[0], progression[1], progression[2],
             progression[3], progression[4], progression[5],
             progression[6], progression[7], progression[8], NULL);

  mpz_t jroot;
  mpz_init (jroot);
  mpz_t nexti;
  mpz_init (nexti);
  mpz_t iroot;
  mpz_init (iroot);
  mpz_sqrt (iroot, i);
  mpz_set (nexti, i);
  inc (&nexti, &iroot, incr);
  mpz_sqrt (jroot, nexti);
  mpz_clear (iroot);
  for (mpz_set (j, nexti); mpz_cmp (jroot, lastroot) < 0;)
    {
      inc (&j, &jroot, incr);
      mpz_t diff;
      mpz_init (diff);
      mpz_sub (diff, j, i);
      if (!mpz_divisible_ui_p (diff, 24))
        continue;
      if (mpz_cmp_ui (diff, 0) > 0)
        {
          mpz_t kroot;
          mpz_init (kroot);
          mpz_sqrt (kroot, j);
          for (mpz_set (k, j); mpz_cmp (kroot, lastroot) < 0;)
            {
              inc (&k, &kroot, incr);
              mpz_t next;
              mpz_init (next);
              mpz_add (next, k, diff);
              if (!mpz_perfect_square_p (next))
                {
                  mpz_clear (next);
                  continue;
                }
              func (progression, i, j, k, next, data);
              mpz_clear (next);
            }
          mpz_clear (kroot);
        }
      mpz_clear (diff);
    }
  mpz_clear (nexti);
  mpz_clears (progression[0], progression[1], progression[2],
              progression[3], progression[4], progression[5],
              progression[6], progression[7], progression[8], NULL);
  mpz_clears (lastroot, j, k, NULL);
}

void
fwd_4sq_progression5 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, void *), void *data)
{
/* |---+--------+--+-----+--+--+------|
       ^        ^        ^     ^
       1        2        3     4 */
  mpz_t lastroot, j, k;
  mpz_inits (lastroot, j, k, NULL);
  mpz_sqrt (lastroot, finish);
  mpz_t progression[9];
  mpz_inits (progression[0], progression[1], progression[2],
             progression[3], progression[4], progression[5],
             progression[6], progression[7], progression[8], NULL);

  mpz_t jroot;
  mpz_init (jroot);
  mpz_sqrt (jroot, i);
  for (mpz_set (j, i); mpz_cmp (jroot, lastroot) < 0;)
    {
      inc (&j, &jroot, incr);
      mpz_t diff;
      mpz_init (diff);
      mpz_sub (diff, j, i);
      if (mpz_cmp_ui (diff, 0) > 0)
        {
          mpz_t next;
          mpz_init (next);
          mpz_add (next, j, diff);
          if (!mpz_perfect_square_p (next))
            {
              mpz_clear (next);
              continue;
            }
          mpz_t kroot, lastkroot, nextdiffsum;
          mpz_inits (kroot, lastkroot, nextdiffsum, NULL);
          mpz_add (nextdiffsum, next, diff);
          mpz_sqrt (lastkroot, nextdiffsum);
          mpz_clear (nextdiffsum);
          mpz_sqrt (kroot, next);
          for (mpz_set (k, next); mpz_cmp (kroot, lastkroot) < 0;)
            {
              inc (&k, &kroot, incr);
              mpz_t nextkdiff;
              mpz_init (nextkdiff);
              mpz_sub (nextkdiff, k, next);
              if (mpz_odd_p (nextkdiff))
                {
                  mpz_clear (nextkdiff);
                  continue;
                }
              mpz_clear (nextkdiff);
              if (mpz_cmp (j, k) != 0)
                func (progression, i, j, next, k, data);
            }
          mpz_clear (next);
          mpz_clears (kroot, lastkroot, NULL);
        }
      mpz_clear (diff);
    }
  mpz_clears (progression[0], progression[1], progression[2],
              progression[3], progression[4], progression[5],
              progression[6], progression[7], progression[8], NULL);
  mpz_clears (lastroot, j, k, NULL);
}

void
fwd_4sq_progression6 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, void *), void *data)
{
  //|----+--*--*--------*--+--*--------*--*--+----|
  //        ^  ^        ^     ^
  //        1  2        3     4
  mpz_t lastroot, j, k;
  mpz_inits (lastroot, j, k, NULL);
  mpz_sqrt (lastroot, finish);
  mpz_t progression[9];
  mpz_inits (progression[0], progression[1], progression[2],
             progression[3], progression[4], progression[5],
             progression[6], progression[7], progression[8], NULL);

  mpz_t jroot, root, nexti;
  mpz_inits (jroot, root, nexti, NULL);
  mpz_set (nexti, i);
  mpz_sqrt (root, i);
  inc (&nexti, &root, incr);
  mpz_clear (root);
  mpz_sqrt (jroot, nexti);
  for (mpz_set (j, nexti); mpz_cmp (jroot, lastroot) < 0;)
    {
      inc (&j, &jroot, incr);
      mpz_t diff;
      mpz_init (diff);
      mpz_sub (diff, j, i);
      if (mpz_cmp_ui (diff, 0) > 0)
        {
          mpz_t kroot;
          mpz_init (kroot);
          mpz_sqrt (kroot, j);
          for (mpz_set (k, j); mpz_cmp (kroot, lastroot) < 0;)
            {
              inc (&k, &kroot, incr);
              mpz_sub (diff, k, j);
              if (mpz_odd_p (diff))
                continue;
              mpz_t d1, next;
              mpz_inits (d1, next, NULL);
              mpz_cdiv_q_ui (d1, diff, 2);
              mpz_add (next, i, d1);
              mpz_clear (d1);
              if (!mpz_perfect_square_p (next))
                {
                  mpz_clear (next);
                  continue;
                }
              if (mpz_cmp (next, j) < 0)
                func (progression, i, next, j, k, data);
              mpz_clear (next);
            }
          mpz_clear (kroot);
        }
      mpz_clear (diff);
    }
  mpz_clears (progression[0], progression[1], progression[2],
              progression[3], progression[4], progression[5],
              progression[6], progression[7], progression[8], NULL);
  mpz_clears (lastroot, j, k, nexti, NULL);
}


void
rev_4sq_progression1 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, void *), void *data)
{
/* |------+-------+--+--+------|
          ^       ^  ^  ^       
          4       3  2  1        */
  mpz_t j, k, lastroot;
  mpz_inits (j, k, lastroot, NULL);
  mpz_t progression[9];
  mpz_inits (progression[0], progression[1], progression[2],
             progression[3], progression[4], progression[5],
             progression[6], progression[7], progression[8], NULL);
  mpz_sqrt (lastroot, start);

  mpz_t jroot;
  mpz_init (jroot);
  mpz_sqrt (jroot, i);
  for (mpz_set (j, i); mpz_cmp (jroot, lastroot) > 0;)
    {
      dec (&j, &jroot, incr);
      mpz_t diff;
      mpz_init (diff);
      mpz_sub (diff, j, i);
      if (mpz_cmp_ui (diff, 0) < 0)
        {
          mpz_t next;
          mpz_init (next);
          mpz_add (next, j, diff);
          if (!mpz_perfect_square_p (next))
            {
              mpz_clear (next);
              mpz_clear (diff);
              continue;
            }
          mpz_t kroot;
          mpz_init (kroot);
          mpz_sqrt (kroot, next);
          for (mpz_set (k, next); mpz_cmp (kroot, lastroot) > 0;)
            {
              dec (&k, &kroot, incr);
              mpz_t amt;
              mpz_init (amt);
              mpz_sub (amt, k, j);
              mpz_add (amt, amt, diff);
              if (mpz_cmp (k, next) < 0)
                func (progression, i, j, next, k, data);
              mpz_clear (amt);
            }
          mpz_clear (next);
          mpz_clear (kroot);
        }
      mpz_clear (diff);
    }

  mpz_clears (j, k, lastroot, NULL);
  mpz_clears (progression[0], progression[1], progression[2],
              progression[3], progression[4], progression[5],
              progression[6], progression[7], progression[8], NULL);
}

void
small_fwd_4sq_progression1 (unsigned long long i, unsigned long long start, unsigned long long finish, void (*func)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, void *), void *data)
{
/* |-----+--+--+-------+-----|
         ^  ^  ^       ^
         1  2  3       4       */
  unsigned long long progression[9];
  memset (progression, 0, sizeof (progression));
  unsigned long long root = sqrtl (i);
  unsigned long long jroot = sqrtl (i + root + root + 1);
  unsigned long long lastroot = sqrtl (finish);
  for (unsigned long long j = i + root + root + 1; jroot < lastroot;)
    {
      j += jroot + jroot + 1;
      unsigned long long diff = j - i;
      if (diff > 0)
        {
          unsigned long long next = j + diff;
          if (!small_is_square (next))
            {
              jroot++;
              continue;
            }
          unsigned long long kroot = sqrtl (j+diff);
          for (unsigned long long k = j + diff; kroot < lastroot; )
            {
              k += kroot + kroot + 1;
              if (k - j+diff > 0 && k - j > 0)
                func (progression, i, j, j + diff, k, data);
              kroot++;
            }
        }
      jroot++;
    }
}

void
small_fwd_4sq_progression2 (unsigned long long i, unsigned long long start, unsigned long long finish, void (*func)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, void *), void *data)
{
/* |--------+--+-------+--+--------|
            ^  ^       ^  ^                        
            1  2       3  4         */
  unsigned long long lastroot = sqrtl (finish);
  unsigned long long progression[9];
  memset (progression, 0, sizeof (progression));
  unsigned long long jroot = sqrtl (i);
  for (unsigned long long j = i; jroot < lastroot;)
    {
      j += jroot + jroot + 1;
      unsigned long long diff = j - i;
      if (diff > 0)
        {
          unsigned long long next = j;
          unsigned long long kroot = jroot + 1;
          next += kroot + kroot + 1;
          kroot++;
          for (unsigned long long k = next; kroot < lastroot; )
            {
              k += kroot + kroot + 1;
              unsigned long long fourth = k + diff;
              if (!small_is_square (fourth))
                {
                  kroot++;
                  continue;
                }
              unsigned long long amt = k - j;
              if (amt > 0)
                func (progression, i, j, k, fourth, data);
              kroot++;
            }
        }
      jroot++;
    }
}

void
small_fwd_4sq_progression3 (unsigned long long i, unsigned long long start, unsigned long long finish, void (*func)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, void *), void *data)
{
/* |---+--------+--+-----+---|
       ^        ^  ^     ^
       1        2  3     4    */
  unsigned long long lastroot = sqrtl (finish);
  unsigned long long progression[9];
  memset (progression, 0, sizeof (progression));
  unsigned long long jroot = sqrtl (i);
  for (unsigned long long j = i; jroot < lastroot;)
    {
      j += jroot + jroot + 1;
      unsigned long long diff = j - i;
      if (diff > 0)
        {
          unsigned long long next = j + diff;
          if (!small_is_square (next))
            {
              jroot++;
              continue;
            }
          unsigned long long lastkroot = sqrtl (j+diff);
          unsigned long long kroot = sqrtl (j);
          for (unsigned long long k = j; kroot < lastkroot; )
            {
              k += kroot + kroot + 1;
              if (j != k)
                func (progression, i, j, k, next, data);
              kroot++;
            }
        }
      jroot++;
    }
}

void
small_fwd_4sq_progression4 (unsigned long long i, unsigned long long start, unsigned long long finish, void (*func)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, void *), void *data)
{
/* |-----+-----+-------+-----+--|
         ^     ^       ^     ^
         1     2       3     4 */
  unsigned long long progression[9];
  memset (progression, 0, sizeof (progression));
  unsigned long long root = sqrtl (i);
  unsigned long long jroot = sqrtl (i + root + root + 1);
  unsigned long long lastroot = sqrtl (finish);
  for (unsigned long long j = i + root + root + 1; jroot < lastroot;)
    {
      j += jroot + jroot + 1;
      unsigned long long diff = j - i;
      if (diff % 24 != 0)
        {
          jroot++;
          continue;
        }
      if (diff > 0)
        {
          unsigned long long kroot = sqrtl (j);
          for (unsigned long long k = j; kroot < lastroot; )
            {
              k += kroot + kroot + 1;
              if (!small_is_square (k + diff))
                {
                  kroot++;
                continue;
                }
              func (progression, i, j, k, k+diff, data);
              kroot++;
            }
        }
      jroot++;
    }
}

void
small_fwd_4sq_progression5 (unsigned long long i, unsigned long long start, unsigned long long finish, void (*func)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, void*), void *data)
{
/* |---+--------+--+-----+--+--+------|
       ^        ^        ^     ^
       1        2        3     4 */
  unsigned long long lastroot = sqrtl (finish);
  unsigned long long progression[9];
  memset (progression, 0, sizeof (progression));
  unsigned long long jroot = sqrtl (i);
  for (unsigned long long j = i; jroot < lastroot;)
    {
      j += jroot + jroot + 1;
      unsigned long long diff = j - i;
      if (diff > 0)
        {
          unsigned long long next = j + diff;
          if (!small_is_square (next))
            {
              jroot++;
              continue;
            }
          unsigned long long kroot = sqrtl (next);
          unsigned long long lastkroot  = sqrtl (next + diff);
          for (unsigned long long k = next; kroot < lastkroot; )
            {
              k += kroot + kroot + 1;
              unsigned long long nextkdiff = k - next;
              if (nextkdiff % 2 == 1)
                {
                  kroot++;
                  continue;
                }
              if (j != k)
                func (progression, i, j, next, k, data);
              kroot++;
            }
        }
      jroot++;
    }
}

void
small_fwd_4sq_progression6 (unsigned long long i, unsigned long long start, unsigned long long finish, void (*func)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, void *), void *data)
{
  //|----+--*--*--------*--+--*--------*--*--+----|
  //        ^  ^        ^     ^
  //        1  2        3     4
  unsigned long long progression[9];
  memset (progression, 0, sizeof (progression));
  unsigned long long root = sqrtl (i);
  unsigned long long jroot = sqrtl (i + root + root + 1);
  unsigned long long lastroot = sqrtl (finish);
  for (unsigned long long j = i + root + root + 1; jroot < lastroot;)
    {
      j += jroot + jroot + 1;
      unsigned long long diff = j - i;
      if (diff > 0)
        {
          unsigned long long kroot = sqrtl (j);
          for (unsigned long long k = j; kroot < lastroot; )
            {
              k += kroot + kroot + 1;
              diff = k - j;
              if (diff % 2 == 1)
                {
                  kroot++;
                  continue;
                }
              unsigned long long d1 = diff / 2;
              unsigned long long next = i + d1;
              if (!small_is_square (next))
                {
                  kroot++;
                  continue;
                }
              if (next < j)
                func (progression, i, next, j, k, data);
              kroot++;
            }
        }
      jroot++;
    }
}

void
small_rev_4sq_progression1 (unsigned long long i, unsigned long long start, unsigned long long finish, void (*func)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, void *), void *data)
{
/* |------+-------+--+--+------|
          ^       ^  ^  ^       
          4       3  2  1        */
  unsigned long long progression[9];
  memset (progression, 0, sizeof (progression));
  unsigned long long lastroot = sqrtl (start);
  unsigned long long jroot = sqrtl (i);
  for (unsigned long long j = i; jroot > lastroot;)
    {
      jroot--;
      j -= (jroot + jroot + 1);
      long long diff = j - i;
      if (diff < 0)
        {
          unsigned long long next = j + diff;
          if (!small_is_square (next))
            continue;
          unsigned long long kroot = sqrtl (next);
          for (unsigned long long k = next; kroot > lastroot; )
            {
              kroot--;
              k -= (kroot + kroot + 1);
              if (k < next)
                func (progression, i, j, next, k, data);
            }
        }
    }
}

void
optimized_fwd_4sq_progression1 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, void *), void *data)
{
/* |-----+--+--+-------+-----|
         ^  ^  ^       ^
         1  2  3       4       */
  mpz_t lastroot, j, k, jroot, diff, next, kroot, amt, nextrem;
  mpz_inits (lastroot, j, k, jroot, diff, next, kroot, amt, nextrem, NULL);
  mpz_sqrt (lastroot, finish);
  mpz_t progression[9];
  mpz_inits (progression[0], progression[1], progression[2],
             progression[3], progression[4], progression[5],
             progression[6], progression[7], progression[8], NULL);

  mpz_sqrt (jroot, i);
  for (mpz_set (j, i); mpz_cmp (jroot, lastroot) < 0;)
    {
      inc (&j, &jroot, incr);
      mpz_sub (diff, j, i);
      if (!mpz_divisible_ui_p (diff, 24))
        continue;
      if (mpz_cmp_ui (diff, 0) > 0)
        {
          mpz_add (next, j, diff);
          mpz_sqrtrem (kroot, nextrem, next);
          if (mpz_cmp_ui (nextrem, 0) != 0)
            continue;

          for (mpz_set (k, next); mpz_cmp (kroot, lastroot) < 0;)
            {
              inc (&k, &kroot, incr);
              mpz_sub (amt, k, j);
              mpz_add (amt, amt, diff);
              if (mpz_cmp_ui (amt, 0) > 0 && mpz_cmp (next, k) != 0)
                func (progression, i, j, next, k, data);
            }
        }
    }
  mpz_clears (progression[0], progression[1], progression[2],
              progression[3], progression[4], progression[5],
              progression[6], progression[7], progression[8], NULL);
  mpz_clears (lastroot, j, k, jroot, diff, next, kroot, amt, nextrem, NULL);
}

void
optimized_rev_4sq_progression1 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, void *), void *data)
{
/* |------+-------+--+--+------|
          ^       ^  ^  ^       
          4       3  2  1        */
  mpz_t j, k, lastroot;
  mpz_inits (j, k, lastroot, NULL);
  mpz_t progression[9];
  mpz_inits (progression[0], progression[1], progression[2],
             progression[3], progression[4], progression[5],
             progression[6], progression[7], progression[8], NULL);
  mpz_sqrt (lastroot, start);

  mpz_t jroot;
  mpz_init (jroot);
  mpz_sqrt (jroot, i);
  for (mpz_set (j, i); mpz_cmp (jroot, lastroot) > 0;)
    {
      dec (&j, &jroot, incr);
      mpz_t diff;
      mpz_init (diff);
      mpz_sub (diff, j, i);
      if (!mpz_divisible_ui_p (diff, 24))
        continue;
      if (mpz_cmp_ui (diff, 0) < 0)
        {
          mpz_t next;
          mpz_init (next);
          mpz_add (next, j, diff);
          if (!mpz_perfect_square_p (next))
            {
              mpz_clear (next);
              mpz_clear (diff);
              continue;
            }
          mpz_t kroot;
          mpz_init (kroot);
          mpz_sqrt (kroot, next);
          for (mpz_set (k, next); mpz_cmp (kroot, lastroot) > 0;)
            {
              dec (&k, &kroot, incr);
              mpz_t amt;
              mpz_init (amt);
              mpz_sub (amt, k, j);
              mpz_add (amt, amt, diff);
              if (mpz_cmp (k, next) < 0)
                func (progression, i, j, next, k, data);
              mpz_clear (amt);
            }
          mpz_clear (next);
          mpz_clear (kroot);
        }
      mpz_clear (diff);
    }

  mpz_clears (j, k, lastroot, NULL);
  mpz_clears (progression[0], progression[1], progression[2],
              progression[3], progression[4], progression[5],
              progression[6], progression[7], progression[8], NULL);
}

void
optimized_fwd_4sq_progression3 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, void *), void *data)
{
/* |---+--------+--+-----+---|
       ^        ^  ^     ^
       1        2  3     4     */
  mpz_t j, k, lastroot;
  mpz_inits (j, k, lastroot, NULL);
  mpz_t progression[9];
  mpz_inits (progression[0], progression[1], progression[2],
             progression[3], progression[4], progression[5],
             progression[6], progression[7], progression[8], NULL);
  mpz_sqrt (lastroot, finish);
  mpz_t jroot;
  mpz_init (jroot);
  mpz_sqrt (jroot, i);
  for (mpz_set (j, i); mpz_cmp (jroot, lastroot) < 0;)
    {
      inc (&j, &jroot, incr);
      mpz_t diff;
      mpz_init (diff);
      mpz_sub (diff, j, i);
      if (!mpz_divisible_ui_p (diff, 24))
        continue;
      if (mpz_cmp_ui (diff, 0) > 0)
        {
          mpz_t next;
          mpz_init (next);
          mpz_add (next, j, diff);
          if (!mpz_perfect_square_p (next))
            {
              mpz_clear (next);
              mpz_clear (diff);
              continue;
            }
          mpz_t kroot, lastkroot;
          mpz_inits (kroot, lastkroot, NULL);
          mpz_sqrt (lastkroot, next);
          mpz_sqrt (kroot, j);
          for (mpz_set (k, j); mpz_cmp (kroot, lastkroot) < 0;)
            {
              inc (&k, &kroot, incr);
              if (mpz_cmp (k, j) != 0)
                func (progression, i, j, k, next, data);
            }
          mpz_clear (next);
          mpz_clears (kroot, lastkroot, NULL);
        }
      mpz_clear (diff);
    }
  mpz_clears (progression[0], progression[1], progression[2],
              progression[3], progression[4], progression[5],
              progression[6], progression[7], progression[8], NULL);
  mpz_clears (j, k, lastroot, NULL);
}

four_square_progression_t
four_square_progressions[] = 
{
    { FWD_PROGRESSION_1, "fwd-progression-1", fwd_4sq_progression1,
      "|--------+--+--+-------+--------|\n"
      "         ^  ^  ^       ^\n"
      "         1  2  3       4\n"
    },
    { FWD_PROGRESSION_2, "fwd-progression-2", fwd_4sq_progression2,
      "|--------+--+-------+--+--------|\n"
      "         ^  ^       ^  ^\n"
      "         1  2       3  4\n"
    },
    { FWD_PROGRESSION_3, "fwd-progression-3", fwd_4sq_progression3,
      "|------+--------+--+-----+------|\n"
      "       ^        ^  ^     ^\n"
      "       1        2  3     4\n"
    },
    { FWD_PROGRESSION_4, "fwd-progression-4", fwd_4sq_progression4,
      "|-----+--------+--+-----+-----|\n"
      "      ^        ^  ^     ^\n"
      "      1        2  3     4\n"
    },
    { FWD_PROGRESSION_5, "fwd-progression-5", fwd_4sq_progression5,
      "|--+--------+--+-----+--+--+----|\n"
      "   ^        ^        ^     ^\n"
      "   1        2        3     4\n"
    },
    { FWD_PROGRESSION_6, "fwd-progression-6", fwd_4sq_progression6,
      "|-------+-+---+-+-+-------------|\n"
      "        ^ ^   ^   ^\n"
      "        1 2   3   4\n"
    },
    { REV_PROGRESSION_1, "rev-progression-1", rev_4sq_progression1,
      "|--------+-------+--+--+--------|\n"
      "         ^       ^  ^  ^\n"
      "         4       3  2  1\n"
    },
    { FWD_PROGRESSION_1, "opti-fwd-prog-1", optimized_fwd_4sq_progression1,
      "|--------+--+--+-------+--------|\n"
      "         ^  ^  ^       ^\n"
      "         1  2  3       4\n"
    },
    { FWD_PROGRESSION_3, "opti-fwd-prog-3", optimized_fwd_4sq_progression3,
      "|------+--------+--+-----+------|\n"
      "       ^        ^  ^     ^\n"
      "       1        2  3     4\n"
    },
    { REV_PROGRESSION_1, "opti-rev-prog-1", optimized_rev_4sq_progression1,
      "|--------+-------+--+--+--------|\n"
      "         ^       ^  ^  ^\n"
      "         4       3  2  1\n"
    },
    { 0, 0, 0, 0},
};

