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

struct fv_app_transform_magic_square_t
{
  int display_errors;
  int (*read_numbers)(FILE *, mpz_t *, int, char **, size_t *);
  void (*display_square) (mpz_t s[3][3], FILE *out);

  int **transformations;
  int num_transformations;
};
/*
So you've generated some progressions and now you want to turn them into
magic squares.  You've tried permute-square but it's just too darned slow
-- it wants to run for 24 hours or more.

transform-square will make things easier.

Typically when processing large progression files (like generated from the
step-progression program and friends), the records are same permutation every
time.  We can take advantage of that to save some time.

Run permute-square -d on a subset of the progression file to determine
which permutations are popular.  Put those permutations in a file and pass
it to transform-square.

transform-square will try each of the transformations in turn, and test the
square to see if it is magic.  If it is, it displays the square and moves on
to the next record.  If none of the transformations work, the square is
displayed on the standard error unless -i is passed.

for example:

$ step-progression 1 10000000 > data
$ head -n100 data | permute-square -d | sort | uniq > my-permutations
$ cat data | transform-square my-permutations > squares 2> rejected


for best results you can see which permutations are the most popular and
put those first in your file.
e.g. order your transformation file by popularity.
$ head -n100 data | permute-square -d | sort | uniq -c
*/

static int
try_transformation (struct fv_app_transform_magic_square_t *app, mpz_t a[], int size, int t, mpz_t s[3][3])
{

  int *transform = app->transformations[t];
  //we were doing this but it was wrong, and working some of the time?
  //for (int i = 0; i < size; i++)
    //mpz_set (s[i/3][i%3], a[transform[i] + i]);
  for (int i = 0; i < size; i++)
    {
      int j = transform[i] + i;
      mpz_set (s[j/3][j%3], a[i]);
    }
  if (is_magic_square (s, 1))
    return 1;
  return 0;
}

static void
transform_square (struct fv_app_transform_magic_square_t *app, mpz_t a[], int size, FILE *out)
{
  int found = 0;
  mpz_t s[3][3];
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_init (s[i][j]);
  for (int i = 0; i < app->num_transformations; i++)
    {
      if (try_transformation (app, a, size, i, s))
        {
          app->display_square (s, out);
          found = 1;
          break;
        }
    }
  if (!found && app->display_errors)
    {
      int count = 0;
      for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
          {
            mpz_set (s[i][j], a[count]);
            count++;
          }
      app->display_square (s, stderr);
    }
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_clear (s[i][j]);
}

int
fituvalu_transform_magic_square (struct fv_app_transform_magic_square_t *app, FILE *in, FILE *out)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t a[SIZE];

  int i;
  for (i = 0; i < SIZE; i++)
    mpz_init (a[i]);

  while (1)
    {
      read = app->read_numbers (in, a, SIZE, &line, &len);
      if (read == -1)
        break;
      transform_square (app, a, SIZE, out);
    }

  for (i = 0; i < SIZE; i++)
    mpz_clear (a[i]);

  if (line)
    free (line);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_transform_magic_square_t *app = (struct fv_app_transform_magic_square_t *) state->input;
  switch (key)
    {
    case 'i':
      app->read_numbers = binary_read_numbers_from_stream;
      break;
    case 'o':
      app->display_square = display_binary_square_record;
      break;
    case 'g':
      app->display_errors = 0;
      break;
    case ARGP_KEY_ARG:
        {
          char *line = NULL;
          size_t len = 0;
          FILE *fp = fopen (arg, "r");
          mpz_t a[SIZE];
          for (int i = 0; i < SIZE; i++)
            mpz_init (a[i]);
          while (1)
            {
              ssize_t read = read_numbers_from_stream (fp, a, SIZE, &line, &len);
              if (read == -1)
                break;
              app->transformations =
                realloc (app->transformations, 
                         (app->num_transformations+1) * sizeof (int*));
              app->transformations[app->num_transformations] =
                malloc (sizeof (int) * SIZE);
              for (int i = 0; i < SIZE; i++)
                app->transformations[app->num_transformations][i] = 
                  mpz_get_si (a[i]);
              app->num_transformations++;
            }
          fclose (fp);
          free (line);
          for (int i = 0; i < SIZE; i++)
            mpz_clear (a[i]);
          if (app->num_transformations == 0)
            argp_error (state, "no transformations found in %s", arg);
        }
      break;
    case ARGP_KEY_INIT:
      setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
      break;
    }
  return 0;
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "ignore", 'g', 0, 0, "Ignore records we can't transform into magic squares" },
    { 0 }
};

static struct argp
argp =
{
  options, parse_opt, "TRANSFORM-FILE",
  "Accept 9 numbers on the standard input, and transform them into 3x3 magic squares according to the records in TRANSFORM-FILE.\vThe nine values must be separated by a comma and terminated by a newline.  TRANSFORM-FILE has comma separated values of -8 to 8, and nine of them per line.  Records that cannot be transformed into valid squares are outputted to the standard error.  Use `permute-square -d' to generate suitable records for TRANSFORM-FILE.",
  0
};

int
main (int argc, char **argv)
{
  struct fv_app_transform_magic_square_t app;
  memset (&app, 0, sizeof (app));
  app.display_errors = 1;
  app.read_numbers = read_numbers_from_stream;
  app.display_square = display_square_record;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  is_magic_square_init ();
  return fituvalu_transform_magic_square (&app, stdin, stdout);
}
