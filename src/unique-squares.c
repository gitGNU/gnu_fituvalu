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

struct rec
{
  mpz_t sq[9];
};
struct fv_app_unique_squares_t
{
  void (*display_record) (mpz_t *progression, FILE *out);
  int (*read_numbers)(FILE *, mpz_t *, int, char **, size_t *);
  struct rec *recs;
  int numrecs;
};


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
get_squares (struct fv_app_unique_squares_t *app, FILE *in)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t sq[3][3];

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_init (sq[i][j]);

  while (1)
    {
      app->recs = realloc (app->recs, (app->numrecs + 1) * sizeof (struct rec));
      for (int i = 0; i < 9; i++)
        mpz_init (app->recs[app->numrecs].sq[i]);
      read = app->read_numbers (in, app->recs[app->numrecs].sq, 9, &line, &len);
      if (read == -1)
        break;
      mpz_set (sq[0][0], app->recs[app->numrecs].sq[0]);
      mpz_set (sq[0][1], app->recs[app->numrecs].sq[1]);
      mpz_set (sq[0][2], app->recs[app->numrecs].sq[2]);
      mpz_set (sq[1][0], app->recs[app->numrecs].sq[3]);
      mpz_set (sq[1][1], app->recs[app->numrecs].sq[4]);
      mpz_set (sq[1][2], app->recs[app->numrecs].sq[5]);
      mpz_set (sq[2][0], app->recs[app->numrecs].sq[6]);
      mpz_set (sq[2][1], app->recs[app->numrecs].sq[7]);
      mpz_set (sq[2][2], app->recs[app->numrecs].sq[8]);
      int is_square = is_magic_square (sq, 1);
      if (!is_square)
        continue;
      app->numrecs++;
    }

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_clear (sq[i][j]);

  if (line)
    free (line);
}

static void
remove_dups (struct fv_app_unique_squares_t *app)
{
  qsort (app->recs, app->numrecs, sizeof (struct rec), compar);
  struct rec *newrec = NULL;
  int newnumrecs = 0;
  for (int i = 0; i < app->numrecs - 1; i++)
    {
      if (compar (&app->recs[i + 1], &app->recs[i]) == 0)
        continue;
      newrec = realloc (newrec, (newnumrecs + 1) * sizeof (struct rec));
      for (int j = 0; j < 9; j++)
        mpz_init_set (newrec[newnumrecs].sq[j], app->recs[i].sq[j]);
      newnumrecs++;
    }

  if (app->numrecs > 1 &&
      compar (&app->recs[app->numrecs-1], &app->recs[app->numrecs-2]) != 0)
    {
      newrec = realloc (newrec, (newnumrecs + 1) * sizeof (struct rec));
      for (int j = 0; j < 9; j++)
        mpz_init_set (newrec[newnumrecs].sq[j],
                      app->recs[app->numrecs-1].sq[j]);
      newnumrecs++;
    }

  //free old list
  for (int i = 0; i < app->numrecs; i++)
    {
      for (int j = 0; j < 9; j++)
        mpz_clear (app->recs[i].sq[j]);
    }
  free (app->recs);
  app->recs = newrec;
  app->numrecs = newnumrecs;
}

static void
reduce (struct fv_app_unique_squares_t *app)
{
  mpz_t sq[3][3];
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_init (sq[i][j]);
  for (int i = 0; i < app->numrecs; i++)
    {
      mpz_set (sq[0][0], app->recs[i].sq[0]);
      mpz_set (sq[0][1], app->recs[i].sq[1]);
      mpz_set (sq[0][2], app->recs[i].sq[2]);
      mpz_set (sq[1][0], app->recs[i].sq[3]);
      mpz_set (sq[1][1], app->recs[i].sq[4]);
      mpz_set (sq[1][2], app->recs[i].sq[5]);
      mpz_set (sq[2][0], app->recs[i].sq[6]);
      mpz_set (sq[2][1], app->recs[i].sq[7]);
      mpz_set (sq[2][2], app->recs[i].sq[8]);
      reduce_square (sq);
      mpz_set (app->recs[i].sq[0], sq[0][0]);
      mpz_set (app->recs[i].sq[1], sq[0][1]);
      mpz_set (app->recs[i].sq[2], sq[0][2]);
      mpz_set (app->recs[i].sq[3], sq[1][0]);
      mpz_set (app->recs[i].sq[4], sq[1][1]);
      mpz_set (app->recs[i].sq[5], sq[1][2]);
      mpz_set (app->recs[i].sq[6], sq[2][0]);
      mpz_set (app->recs[i].sq[7], sq[2][1]);
      mpz_set (app->recs[i].sq[8], sq[2][2]);
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

int
fituvalu_unique_squares (struct fv_app_unique_squares_t *app, FILE *in, FILE *out)
{
  get_squares (app, in);
  qsort (app->recs, app->numrecs, sizeof (struct rec), compar);

  remove_dups (app);

  reduce (app);
  remove_dups (app);

  struct irec *irec = NULL;
  int numirecs = 0;
  for (int i = 0; i < app->numrecs; i++)
    {
      irec = realloc (irec, sizeof (struct irec) * (numirecs + 1));
      irec[numirecs].idx = i;
      for (int j = 0; j < 9; j++)
        {
          mpz_init_set (irec[numirecs].sq[j], app->recs[i].sq[j]);
        }
      qsort (irec[numirecs].sq, 9, sizeof (mpz_t), compar_elem);
      numirecs++;
    }
  qsort (irec, numirecs, sizeof (struct irec), compar_irec);

  int first_found = 0;
  for (int i = 0; i < numirecs - 1; i++)
    {
      if (compar_irec (&irec[i + 1], &irec[i]) != 0)
        {
          app->display_record (app->recs[irec[i].idx].sq, out);
          first_found = 1;
        }
    }
  if (compar_irec (&irec[numirecs-1], &irec[numirecs-2]) != 0)
    {
      app->display_record (app->recs[irec[numirecs-1].idx].sq, out);
      first_found = 1;
    }
  if (!first_found)
    app->display_record (app->recs[0].sq, out);

  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_unique_squares_t *app = (struct fv_app_unique_squares_t *) state->input;
  switch (key)
    {
    case 'i':
      app->read_numbers = binary_read_numbers_from_stream;
      break;
    case 'o':
      app->display_record = display_binary_nine_record;
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

static struct argp argp ={options, parse_opt, 0, "Accept 3x3 magic squares from the standard input, and filter out the repeated, rotated and reflected ones.\vThe nine values must be separated by a comma and terminated by a newline.  This program uses more memory than uniq-squares, but is much faster and is intended for smaller datasets." , 0};

int
main (int argc, char **argv)
{
  struct fv_app_unique_squares_t app;
  memset (&app, 0, sizeof (app));
  app.display_record = display_nine_record;
  app.read_numbers = read_numbers_from_stream;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  is_magic_square_init ();
  return fituvalu_unique_squares (&app, stdin, stdout);
}
