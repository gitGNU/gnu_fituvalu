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
#include <gmp.h>
#include <argp.h>
#include <string.h>
#include <stdlib.h>
#include "magicsquareutil.h"

struct fv_app_mine_3sq_progressions_t
{
  int squares;
  int show_sum;
  int show_diff;
  int show_root;
  int (*read_numbers)(FILE *, mpz_t *, int size, char **, size_t *);
  FILE *out;
};

int
compar (const void *left, const void *right)
{
  const mpz_t *l = left;
  const mpz_t *r = right;
  return mpz_cmp (*l, *r);
}

//http://www.geeksforgeeks.org/print-all-possible-combinations-of-r-elements-in-a-given-array-of-size-n/
void combinationUtil (mpz_t arr[], mpz_t data[], int start, int end,
                      int index, int r, struct fv_app_mine_3sq_progressions_t *app);

void printCombination (mpz_t arr[], int n, int r, struct fv_app_mine_3sq_progressions_t *app)
{
  mpz_t data[r];
  for (int i = 0; i < r; i++)
    mpz_init (data[i]);
  combinationUtil (arr, data, 0, n-1, 0, r, app);
  for (int i = 0; i < r; i++)
    mpz_clear (data[i]);
}
 
void combinationUtil (mpz_t arr[], mpz_t data[], int start, int end,
                      int index, int r, struct fv_app_mine_3sq_progressions_t *app)
{
  if (index == r)
    {
      mpz_t diff1, diff2;
      mpz_inits (diff1, diff2, NULL);
      mpz_sub (diff1, data[1], data[0]);
      mpz_sub (diff2, data[2], data[1]);
      int match = mpz_cmp (diff1, diff2) == 0;
      if (app->show_diff && match)
        {
          char buf[mpz_sizeinbase (diff1, 10) + 2];
          mpz_get_str (buf, 10, diff1);
          fprintf (app->out, "%s, ", buf);
        }
      mpz_clears (diff1, diff2, NULL);
      if (!match)
        return;
      if (app->show_sum)
        {
          mpz_t sum;
          mpz_init (sum);
          for (int j = 0; j < r; j++)
            mpz_add (sum, sum, data[j]);
          char buf[mpz_sizeinbase (sum, 10) + 2];
          mpz_get_str (buf, 10, sum);
          fprintf (app->out, "%s, ", buf);
          mpz_clear (sum);
        }
      for (int j = 0; j < r; j++)
        {
          char buf[mpz_sizeinbase (data[j], 10) + 2];
          mpz_get_str (buf, 10, data[j]);
          fprintf (app->out, "%s, ", buf);
        }
      if (app->show_root)
        {
          mpz_t root;
          mpz_init (root);
          mpz_sqrt (root, data[r-1]);
          char buf[mpz_sizeinbase (root, 10) + 2];
          mpz_get_str (buf, 10, root);
          fprintf (app->out, "%s, ", buf);
          mpz_clear (root);
        }
      fprintf (app->out, "\n");
      return;
    }

  for (int i = start; i <= end && end - i + 1 >= r - index; i++)
    {
      mpz_set (data[index], arr[i]);
      combinationUtil(arr, data, i + 1, end, index + 1, r, app);
    }
}

static void
check (mpz_t one, mpz_t two, mpz_t three, mpz_t diff, struct fv_app_mine_3sq_progressions_t *app)
{
  if (app->show_diff)
    {
      char buf[mpz_sizeinbase (diff, 10) + 2];
      mpz_get_str (buf, 10, diff);
      fprintf (app->out, "%s, ", buf);
    }
  if (app->show_sum)
    {
      mpz_t sum;
      mpz_init (sum);
      mpz_add (sum, sum, one);
      mpz_add (sum, sum, two);
      mpz_add (sum, sum, three);
      char buf[mpz_sizeinbase (sum, 10) + 2];
      mpz_get_str (buf, 10, sum);
      fprintf (app->out, "%s, ", buf);
      mpz_clear (sum);
    }
    {
      char buf[mpz_sizeinbase (one, 10) + 2];
      mpz_get_str (buf, 10, one);
      fprintf (app->out, "%s, ", buf);
    }
    {
      char buf[mpz_sizeinbase (two, 10) + 2];
      mpz_get_str (buf, 10, two);
      fprintf (app->out, "%s, ", buf);
    }
    {
      char buf[mpz_sizeinbase (three, 10) + 2];
      mpz_get_str (buf, 10, three);
      fprintf (app->out, "%s, ", buf);
    }
  if (app->show_root)
    {
      mpz_t root;
      mpz_init (root);
      mpz_sqrt (root, three);
      char buf[mpz_sizeinbase (root, 10) + 2];
      mpz_get_str (buf, 10, root);
      fprintf (app->out, "%s, ", buf);
      mpz_clear (root);
    }
  fprintf (app->out, "\n");
}

void combination2Util (mpz_t arr[], mpz_t data[], int start, int end,
                      int index, int r, struct fv_app_mine_3sq_progressions_t *app);

void print2Combination (mpz_t arr[], int n, int r, struct fv_app_mine_3sq_progressions_t *app)
{
  mpz_t data[r];
  for (int i = 0; i < r; i++)
    mpz_init (data[i]);
  combination2Util (arr, data, 0, n-1, 0, r, app);
  for (int i = 0; i < r; i++)
    mpz_clear (data[i]);
}

void combination2Util (mpz_t arr[], mpz_t data[], int start, int end,
                      int index, int r, struct fv_app_mine_3sq_progressions_t *app)
{
  if (index == r)
    {
      //we have exactly 2 in data.
      //so we check to see if another square exists on either side.
      mpz_t diff, next;
      mpz_inits (diff, next, NULL);
      mpz_sub (diff, data[1], data[0]);
      mpz_abs (diff, diff);
      int match = mpz_cmp_ui (diff, 0) == 0;
      if (match)
        {
          mpz_clears (diff, next, NULL);
          return;
        }
      if (mpz_cmp (data[1], data[0]) > 0)
        {
          mpz_add (next, data[1], diff);
          if (mpz_perfect_square_p (next))
            check (data[0], data[1], next, diff, app);
          mpz_sub (next, data[0], diff);
          if (mpz_perfect_square_p (next))
            check (next, data[0], data[1], diff, app);
        }
      else
        {
          mpz_add (next, data[0], diff);
          if (mpz_perfect_square_p (next))
            check (data[1], data[0], next, diff, app);
          mpz_sub (next, data[1], diff);
          if (mpz_perfect_square_p (next))
            check (next, data[1], data[0], diff, app);
        }

      mpz_clears (diff, next, NULL);

      return;
    }

  for (int i = start; i <= end && end - i + 1 >= r - index; i++)
    {
      mpz_set (data[index], arr[i]);
      combination2Util(arr, data, i + 1, end, index + 1, r, app);
    }
}

static void
mine_progression (mpz_t vec[], int size, struct fv_app_mine_3sq_progressions_t *app)
{
  int count = 0;
  mpz_t progression[size];

  for (int i = 0; i < size; i++)
    {
      if (mpz_perfect_square_p (vec[i]))
        {
          mpz_init_set (progression[count], vec[i]);
          count++;
        }
    }
  if (count < 2)
    {
      for (int i = 0; i < count; i++)
        mpz_clear (progression[i]);
      return;
    }

  qsort (progression, count, sizeof (mpz_t), compar);

  printCombination (progression, count, 3, app);

  for (int i = 0; i < count; i++)
    mpz_clear (progression[i]);
  return;
}

static int
mine_3sq_progressions (struct fv_app_mine_3sq_progressions_t *app, FILE *in)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  mpz_t vec[SIZE];
  mpz_t orig[SIZE];
  for (int i = 0; i < SIZE; i++)
    mpz_inits (vec[i], orig[i], NULL);
  while (1)
    {
      read = app->read_numbers (in, vec, SIZE, &line, &len);
      if (read == -1)
        break;
      mine_progression (vec, SIZE, app);
    }

  for (int i = 0; i < SIZE; i++)
    mpz_clears (vec[i], orig[i], NULL);

  if (line)
    free (line);
  return 0;
}

static int
mine_3sq_progressions_sq (struct fv_app_mine_3sq_progressions_t *app, FILE *in)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  mpz_t sq;
  mpz_t *sqs = NULL;
  int num_sqs = 0;
  mpz_init (sq);
  while (1)
    {
      if (app->read_numbers == binary_read_numbers_from_stream)
        {
          read = mpz_inp_raw (sq, in);
          if (!read)
            break;
        }
      else
        {
          read = fv_getline (&line, &len, in);
          if (read == -1)
            break;
          mpz_set_str (sq, line, 10);
        }
      if (mpz_perfect_square_p (sq))
        {
          sqs = realloc (sqs, sizeof (mpz_t) * (num_sqs + 1));
          mpz_init_set (sqs[num_sqs], sq);
          num_sqs++;
        }
    }
  print2Combination (sqs, num_sqs, 2, app);
  mpz_clear (sq);
  for (int i = 0; i < num_sqs; i++)
    mpz_clear (sqs[i]);
  free (sqs);

  if (line)
    free (line);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_mine_3sq_progressions_t *app = (struct fv_app_mine_3sq_progressions_t *) state->input;
  switch (key)
    {
    case 'S':
      app->squares = 1;
      break;
    case 'd':
      app->show_diff = 1;
      break;
    case 's':
      app->show_sum = 1;
      break;
    case 'r':
      app->show_root = 1;
      break;
    case 'i':
      app->read_numbers = binary_read_numbers_from_stream;
      break;
    }
  return 0;
}

int
fituvalu_mine_3sq_progressions (struct fv_app_mine_3sq_progressions_t *app, FILE *in)
{
  if (app->squares)
    return mine_3sq_progressions_sq (app, in);
  else
    return mine_3sq_progressions (app, in);
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "show-sum", 's', 0, 0, "Also show the sum"},
  { "show-diff", 'd', 0, 0, "Also show the diff"},
  { "show-root", 'r', 0, 0, "Also show the root"},
  { "squares", 'S', 0, 0, "Instead of reading in 3x3 squares, read in perfect squares." },
  { 0 }
};

static struct argp
argp =
{
  options, parse_opt, 0,
  "Accept 3x3 magic squares and output any arithmetic progressions that are formed by three perfect squares.\vThe 3x3 magic squares must be separated by a comma and terminated by a newline.",
  0
};

int
main (int argc, char **argv)
{
  struct fv_app_mine_3sq_progressions_t app;
  memset (&app, 0, sizeof (app));
  app.read_numbers = read_numbers_from_stream;
  app.out = stdout;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_mine_3sq_progressions (&app, stdin);
}
