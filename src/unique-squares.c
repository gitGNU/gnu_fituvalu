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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <argp.h>
#include <string.h>
#include <gmp.h>
#include "magicsquareutil.h"

void (*display_square) (mpz_t s[3][3], FILE *out) = display_square_record;
int (*read_numbers)(FILE *, mpz_t *, int, char **, size_t *) = read_numbers_from_stream;


struct rec
{
  mpz_t sq[9];
};
struct rec *recs;
int numrecs;

struct irec
{
  mpz_t sq[9];
  unsigned long long idx;
};

int
compar (const void *left, const void *right)
{
  int ret;
  const struct rec *l = left;
  const struct rec *r = right;
  for (int i = 0; i < 9; i++)
    {
      ret = mpz_cmp (l->sq[i], r->sq[i]);
      if (ret)
        return ret;
    }
  return 0;
}

int
compar_irec (const void *left, const void *right)
{
  int ret;
  const struct irec *l = left;
  const struct irec *r = right;
  for (int i = 0; i < 9; i++)
    {
      ret = mpz_cmp (l->sq[i], r->sq[i]);
      if (ret)
        return ret;
    }
  return 0;
}

static void
get_squares (FILE *in)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  while (1)
    {
      recs = realloc (recs, (numrecs + 1) * sizeof (struct rec));
      for (int i = 0; i < 9; i++)
        mpz_init (recs[numrecs].sq[i]);
      read = read_numbers (in, recs[numrecs].sq, 9, &line, &len);
      if (read == -1)
        break;
      numrecs++;
    }

  if (line)
    free (line);
}

static void
remove_dups ()
{
  qsort (recs, numrecs, sizeof (struct rec), compar);
  struct rec *newrec = NULL;
  int newnumrecs = 0;
  for (int i = 0; i < numrecs - 1; i++)
    {
      if (compar (&recs[i + 1], &recs[i]) == 0)
        continue;
      newrec = realloc (newrec, (newnumrecs + 1) * sizeof (struct rec));
      for (int j = 0; j < 9; j++)
        mpz_init_set (newrec[newnumrecs].sq[j], recs[i].sq[j]);
      newnumrecs++;
    }

  if (numrecs > 1 && compar (&recs[numrecs-1], &recs[numrecs-2]) != 0)
    {
      newrec = realloc (newrec, (newnumrecs + 1) * sizeof (struct rec));
      for (int j = 0; j < 9; j++)
        mpz_init_set (newrec[newnumrecs].sq[j], recs[numrecs-1].sq[j]);
      newnumrecs++;
    }

  //free old list
  for (int i = 0; i < numrecs; i++)
    {
      for (int j = 0; j < 9; j++)
        mpz_clear (recs[i].sq[j]);
    }
  free (recs);
  recs = newrec;
  numrecs = newnumrecs;
}

static void
reduce ()
{
  mpz_t sq[3][3];
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_init (sq[i][j]);
  for (int i = 0; i < numrecs; i++)
    {
      mpz_set (sq[0][0], recs[i].sq[0]);
      mpz_set (sq[0][1], recs[i].sq[1]);
      mpz_set (sq[0][2], recs[i].sq[2]);
      mpz_set (sq[1][0], recs[i].sq[3]);
      mpz_set (sq[1][1], recs[i].sq[4]);
      mpz_set (sq[1][2], recs[i].sq[5]);
      mpz_set (sq[2][0], recs[i].sq[6]);
      mpz_set (sq[2][1], recs[i].sq[7]);
      mpz_set (sq[2][2], recs[i].sq[8]);
      reduce_square (sq);
      mpz_set (recs[i].sq[0], sq[0][0]);
      mpz_set (recs[i].sq[1], sq[0][1]);
      mpz_set (recs[i].sq[2], sq[0][2]);
      mpz_set (recs[i].sq[3], sq[1][0]);
      mpz_set (recs[i].sq[4], sq[1][1]);
      mpz_set (recs[i].sq[5], sq[1][2]);
      mpz_set (recs[i].sq[6], sq[2][0]);
      mpz_set (recs[i].sq[7], sq[2][1]);
      mpz_set (recs[i].sq[8], sq[2][2]);
    }
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_clear (sq[i][j]);
}

int
compar_elem (const void *left, const void *right)
{
  const mpz_t *l = left;
  const mpz_t *r = right;
  return mpz_cmp (*l, *r);
}

static int
unique_squares (FILE *in, FILE *out)
{
  get_squares (in);
  qsort (recs, numrecs, sizeof (struct rec), compar);

  remove_dups ();

  reduce ();
  remove_dups ();

  //for (int i = 0; i < numrecs; i++)
    //display_nine_record (recs[i].sq, out);
      //return 0;

  struct irec *irec = NULL;
  int numirecs = 0;
  for (int i = 0; i < numrecs; i++)
    {
      irec = realloc (irec, sizeof (struct irec) * (numirecs + 1));
      irec[numirecs].idx = i;
      for (int j = 0; j < 9; j++)
        {
          mpz_init_set (irec[numirecs].sq[j], recs[i].sq[j]);
        }
      qsort (irec[numirecs].sq, 9, sizeof (mpz_t), compar_elem);
      numirecs++;
    }
  qsort (irec, numirecs, sizeof (struct irec), compar_irec);

  for (int i = 0; i < numirecs - 1; i++)
    {
      if (compar_irec (&irec[i + 1], &irec[i]) != 0)
        display_nine_record (recs[irec[i].idx].sq, out);
    }
  if (compar_irec (&irec[numirecs-1], &irec[numirecs-2]) != 0)
    display_nine_record (recs[irec[numirecs-1].idx].sq, out);

  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'i':
      read_numbers = binary_read_numbers_from_stream;
      break;
    case 'o':
      display_square = display_binary_square_record;
      break;
    }
  return 0;
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { 0 }
};

struct argp argp ={options, parse_opt, 0, "Accept 3x3 magic squares from the standard input, and filter out the repeated, rotated and reflected ones.\vThe nine values must be separated by a comma and terminated by a newline.  This program uses more memory than uniq-squares, but is much faster and is intended for smaller datasets." , 0};

int
main (int argc, char **argv)
{
  argp_parse (&argp, argc, argv, 0, 0, 0);
  is_magic_square_init ();
  return unique_squares (stdin, stdout);
}
