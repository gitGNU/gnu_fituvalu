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
#include <gmp.h>
#include <argp.h>
#include <string.h>
#include <stdlib.h>
#include "magicsquareutil.h"
struct fv_app_sort_square_t
{
  char *prefix;
  void (*display_square) (mpz_t s[3][3], FILE *out);
  int (*read_square) (FILE *, mpz_t (*)[3][3], char **, size_t *);
};

static FILE *
open_file (struct fv_app_sort_square_t *app, int num_squares)
{
  char *filename = NULL;
  if (num_squares > 9 || num_squares < 0)
    return NULL;
  char *names[] = 
    {
      "zeroes",
      "ones",
      "twos",
      "threes",
      "fours",
      "fives",
      "sixes",
      "sevens",
      "eights",
      "nines",
    };
  if (app->prefix)
    asprintf (&filename, "%s-%s", app->prefix, names[num_squares]);
  else
    asprintf (&filename, "%s", names[num_squares]);

  FILE *fp = fopen (filename, "a");
  free (filename);
  return fp;
}

int
fituvalu_sort_square (struct fv_app_sort_square_t *app, FILE *stream)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  mpz_t a[3][3];
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_init (a[i][j]);
  FILE *fp[10];
  memset (fp, 0, sizeof (fp));
  while (1)
    {
      read = app->read_square (stream, &a, &line, &len);
      if (read == -1)
        break;
      int squares = count_squares (a);

      if (!fp[squares])
        fp[squares] = open_file (app, squares);
      app->display_square (a, fp[squares]);
    }
  for (int i = 0; i < 10; i++)
    if (fp[i])
      fclose (fp[i]);

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_clear (a[i][j]);
  if (line)
    free (line);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_sort_square_t *app = (struct fv_app_sort_square_t *) state->input;
  switch (key)
    {
    case 'i':
      app->read_square = binary_read_square_from_stream;
      break;
    case 'o':
      app->display_square = display_binary_square_record;
      break;
    case 'p':
      app->prefix = arg;
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
  { "prefix", 'p', "NAME", 0, "Prefix filenames with NAME"},
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { 0 }
};

static struct argp argp ={options, parse_opt, 0, "Accept 3x3 magic squares from the standard input, determine the number of squares and put it in a file named ones, twos, threes and so on.\vThe nine values must be separated by a comma and terminated by a newline." , 0};

int
main (int argc, char **argv)
{
  struct fv_app_sort_square_t app;
  memset (&app, 0, sizeof (app));
  app.display_square = display_square_record;
  app.read_square = read_square_from_stream;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_sort_square (&app, stdin);
}
