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
#include <math.h>
#include <argp.h>
#include <string.h>
#include <gmp.h>
#include "magicsquareutil.h"
struct fv_app_display_square_t
{
  int simple;
  int show_sums;
  int csvthree;
  int magic_number;
  int number_line;
  int cr_no_lf;
  int (*read_square) (FILE *, mpz_t (*)[3][3], char **, size_t *);
  int nohat;
};

static int
get_width (int nohat, mpz_t i)
{
  int ret = 0;
  if (mpz_perfect_square_p (i) && !nohat)
    {
      mpz_t root;
      mpz_init (root);
      mpz_sqrt (root, i);
      ret = mpz_sizeinbase (i, 10) + 2 + 1;
      mpz_clear (root);
    }
  else
    ret = mpz_sizeinbase (i, 10) + 2;
  return ret;
}

static int
get_width_of_widest_cell (struct fv_app_display_square_t *app, mpz_t a[3][3])
{
  int widest = 0;
  for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
        {
          int width = get_width (app->nohat, a[i][i]);
          if (width > widest)
            widest = width;
        }
    }
  return widest;
}

static void
display_row_border (int width, FILE *out)
{
  for (int i = 0; i < width * 3 + 1; i++)
    if (i % width == 0)
      fprintf (out, "+");
    else
      fprintf (out, "-");
  fprintf (out, "\n");
}

static char *
get_magic_number_string (mpz_t a[3][3])
{
  char *s = NULL;
  mpz_t sum;
  mpz_init (sum);
  mpz_set (sum, a[0][0]);
  mpz_add (sum, sum, a[0][1]);
  mpz_add (sum, sum, a[0][2]);
  char buf[mpz_sizeinbase (sum, 10) + 2];
  mpz_get_str (buf, 10, sum);
  asprintf (&s, "\tmagic number = %s", buf);
  mpz_clear (sum);
  return s;
}

static char *
get_number_string (int nohat, mpz_t i, int width)
{
  char s[32768];
  /* FIXME, it's an unfortunate limit */
  memset (s, 0, sizeof (s));
  memset (s, ' ', width);
  if (mpz_perfect_square_p (i) && !nohat)
    {
      mpz_t root;
      mpz_init (root);
      mpz_sqrt (root, i);
      char buf[mpz_sizeinbase (root, 10) + 2];
      mpz_get_str (buf, 10, root);
      int length = mpz_sizeinbase (root, 10) + 1;
      int place = (width - length) / 2;
      strncpy (&s[place], buf, strlen (buf));
      strncpy (&s[place + strlen (buf)], "^2", strlen ("^2"));
      mpz_clear (root);
    }
  else
    {
      if (mpz_cmp_ui (i, 0) < 0)
        {
          char buf[mpz_sizeinbase (i, 10) + 2];
          mpz_get_str (buf, 10, i);
          int length = mpz_sizeinbase (i, 10) + 1;
          int place = (width - length) / 2;
          strncpy (&s[place], buf, strlen (buf));
        }
      else
        {
          char buf[mpz_sizeinbase (i, 10) + 2];
          mpz_get_str (buf, 10, i);
          int length = mpz_sizeinbase (i, 10) + 1;
          int place = (width - length) / 2;
          strncpy (&s[place + 1], buf, strlen (buf));
        }
    }
  return strdup (s);
}

static void
display_row (struct fv_app_display_square_t *app, int width, mpz_t a[3][3], int row, FILE *out, char *extra)
{
  int count = 0;
  for (int i = 0; i < width * 3; i++)
    if (i % width == 0)
      {
        char *num = get_number_string (app->nohat, a[row][count], width-1);
        fprintf (out, "|%s", num);
        free (num);
        count++;
      }
  if (extra)
    fprintf (out, "|%s\n", extra);
  else
    fprintf (out, "|\n");
}

static void
display (struct fv_app_display_square_t *app, mpz_t a[3][3], FILE *out)
{
  int width = get_width_of_widest_cell (app, a);

  char *extra = NULL;
  if (app->magic_number)
    extra = get_magic_number_string (a);
  display_row_border (width, out);
  display_row (app, width, a, 0, out, NULL);
  display_row_border (width, out);
  display_row (app, width, a, 1, out, extra);
  if (extra)
    free (extra);
  display_row_border (width, out);
  display_row (app, width, a, 2, out, NULL);
  display_row_border (width, out);
}

static void
display_simple (mpz_t a[3][3], FILE *out)
{
  int num_squares = count_squares (a);
  fprintf (out, "%d squares: ", num_squares);
  display_square_record (a, out);
}

static void
fill_sums (mpz_t a[3][3], mpz_t row1, mpz_t row2, mpz_t row3, mpz_t col1, mpz_t col2, mpz_t col3, mpz_t diag1, mpz_t diag2)
{
  mpz_add (row1, a[0][0], a[0][1]);
  mpz_add (row1, row1, a[0][2]);
  mpz_add (row2, a[1][0], a[1][1]);
  mpz_add (row2, row2, a[1][2]);
  mpz_add (row3, a[2][0], a[2][1]);
  mpz_add (row3, row3, a[2][2]);
  mpz_add (col1, a[0][0], a[1][0]);
  mpz_add (col1, col1, a[2][0]);
  mpz_add (col2, a[0][1], a[1][1]);
  mpz_add (col2, col2, a[2][1]);
  mpz_add (col3, a[0][2], a[1][2]);
  mpz_add (col3, col3, a[2][2]);
  mpz_add (diag1, a[0][0], a[1][1]);
  mpz_add (diag1, diag1, a[2][2]);
  mpz_add (diag2, a[0][2], a[1][1]);
  mpz_add (diag2, diag2, a[2][0]);
}

static int
get_widest_sum (struct fv_app_display_square_t *app, mpz_t row1, mpz_t row2, mpz_t row3, mpz_t col1, mpz_t col2, mpz_t col3, mpz_t diag1, mpz_t diag2)
{
  int width = 0;
  int w = get_width (app->nohat, row1);
  if (w > width)
    width = w;
  w = get_width (app->nohat, row2);
  if (w > width)
    width = w;
  w = get_width (app->nohat, row3);
  if (w > width)
    width = w;
  w = get_width (app->nohat, col1);
  if (w > width)
    width = w;
  w = get_width (app->nohat, col2);
  if (w > width)
    width = w;
  w = get_width (app->nohat, col3);
  if (w > width)
    width = w;
  w = get_width (app->nohat, diag1);
  if (w > width)
    width = w;
  w = get_width (app->nohat, diag2);
  if (w > width)
    width = w;
  return width;
}

static char *
get_num_space_str (int num_spaces, mpz_t num)
{
  char *s = NULL;
  char buf[mpz_sizeinbase (num, 10) + 2];
  mpz_get_str (buf, 10, num);
  char *spaces = malloc (num_spaces + 1);
  memset (spaces, ' ', num_spaces);
  spaces[num_spaces] = '\0';
  asprintf (&s, "%s%s", spaces, buf);
  free (spaces);
  return s;
}

static void
display_broken_square (struct fv_app_display_square_t *app, mpz_t a[3][3], FILE *out)
{
  mpz_t row1, row2, row3, col1, col2, col3, diag1, diag2;
  mpz_inits (row1, row2, row3, col1, col2, col3, diag1, diag2, NULL);
  fill_sums (a, row1, row2, row3, col1, col2, col3, diag1, diag2);
  int sumwidth =
    get_widest_sum (app, row1, row2, row3, col1, col2, col3, diag1, diag2);
  int cellwidth = get_width_of_widest_cell (app, a);
  int width = sumwidth;
  if (cellwidth > width)
    width = cellwidth;

  char *buf = get_num_space_str (2 + (width * 3), diag2);
  printf ("%s\n", buf);
  free (buf);
  buf = get_num_space_str (1, row1);
  display_row_border (width, out);
  display_row (app, width, a, 0, out, buf);
  free (buf);
  display_row_border (width, out);
  buf = get_num_space_str (1, row2);
  display_row (app, width, a, 1, out, buf);
  free (buf);
  display_row_border (width, out);
  buf = get_num_space_str (1, row3);
  display_row (app, width, a, 2, out, buf);
  free (buf);
  display_row_border (width, out);
  buf = get_number_string (app->nohat, col1, width-1);
  fprintf (out, " %s", buf);
  free (buf);
  buf = get_number_string (app->nohat, col2, width-1);
  fprintf (out, " %s", buf);
  free (buf);
  buf = get_number_string (app->nohat, col3, width-1);
  fprintf (out, " %s", buf);
  free (buf);
  buf = get_num_space_str (2, diag1);
  fprintf (out, "%s\n", buf);
  free (buf);

  mpz_clears (row1, row2, row3, col1, col2, col3, diag1, diag2, NULL);
}

static void
get_max_and_min (mpz_t a[3][3], mpz_t max, mpz_t min)
{
  mpz_set_ui (max, 0);
  mpz_set_ui (min, -1);
  int min_set = 0;
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      {
        if (mpz_cmp (a[i][j], max) > 0)
          mpz_set (max, a[i][j]);
        if (mpz_cmp (a[i][j], min) < 0 ||
            (mpz_cmp_ui (a[i][j], -1) && !min_set))
          {
            mpz_set (min, a[i][j]);
            min_set = 1;
          }
      }
}

static void
display_number_line (struct fv_app_display_square_t *app, mpz_t a[3][3], FILE *out)
{
  mpf_t num, maxnum, p, result, difff;
  mpz_t min, max, diff;
  mpf_inits (num, maxnum, p, result, difff, NULL);
  mpz_inits (max, min, diff, NULL);
  int maxlen = 50;
  char line[maxlen+1];
  memset (line, '-', sizeof (line));
  line[maxlen] = '\0';

  get_max_and_min (a, max, min);
  mpz_set_ui (diff, 0);
  if (mpz_cmp_ui (min, 0) < 0)
    {
      /* translate all numbers so they're above zero */
      mpz_mul_si (diff, min, -1);
      mpz_add (min, min, diff);
      mpz_add (max, max, diff);
    }
  mpf_set_z (difff, diff);
  mpf_set_z (maxnum, max);

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      {
        mpf_set_z (num, a[i][j]);
        mpf_add (num, num, difff); /* translate */
        mpf_div (p, num, maxnum);
        mpf_mul_ui (result, p, maxlen);
        int n = mpf_get_si (result);
        char marker = '+';
        if (mpz_perfect_square_p (a[i][j]))
          marker = '*';

        if (line[n] == '-')
          line[n] = marker;
        else
          {
            /* find a nearby home for this marker */
            for (int k = 1; k < 5; k++)
              {
                int newk = n + k;
                if (newk < maxlen && line[newk] == '-')
                  {
                    line[newk] = marker;
                    break;
                  }
                else
                  {
                    newk = n - k;
                    if (newk >= 0 && line[newk] == '-')
                      {
                        line[newk] = marker;
                        break;
                      }
                  }
              }
          }
      }
  if (app->cr_no_lf)
    fprintf (out, "|--%s--|\r", line);
  else
    fprintf (out, "|--%s--|\n", line);
  mpf_clears (num, maxnum, p, result, difff, NULL);
  mpz_clears (max, min, diff, NULL);
  fflush (out);
}

static void
display_csv_three_rows (mpz_t a[3][3], FILE *out)
{
  display_three_record (a[0], out);
  display_three_record (a[1], out);
  display_three_record (a[2], out);
}

int
fituvalu_display_square (struct fv_app_display_square_t *app, FILE *stream)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t a[3][3];

  int i, j;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      mpz_init (a[i][j]);

  while (1)
    {
      read = app->read_square (stream, &a, &line, &len);
      if (read == -1)
        break;
      if (app->number_line)
        display_number_line (app, a, stdout);
      else if (app->csvthree)
        display_csv_three_rows (a, stdout);
      else if (is_magic_square (a, 1) && !app->show_sums)
        {
          if (app->simple)
            display_simple (a, stdout);
          else
            display (app, a, stdout);
        }
      else
        display_broken_square (app, a, stdout);
    }

  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      mpz_clear (a[i][j]);

  if (line)
    free (line);
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_display_square_t *app = (struct fv_app_display_square_t *) state->input;
  switch (key)
    {
    case 'h':
      app->nohat = 1;
      break;
    case '3':
      app->csvthree = 1;
      break;
    case 'i':
      app->read_square = binary_read_square_from_stream;
      break;
    case 'c':
      app->cr_no_lf = 1;
      break;
    case 'S':
      app->simple = 1;
      break;
    case 's':
      app->show_sums = 1;
      break;
    case 'N':
      app->magic_number = 0;
      break;
    case 'n':
      app->number_line = 1;
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
  { "simple", 'S', 0, 0, "Use simple formatting"},
  { "no-magic-number", 'N', 0, 0, "Don't show the magic number"},
  { "show-sums", 's', 0, 0, "Show row and column totals"},
  { "number-line", 'n', 0, 0, "Chart the numbers"},
  { "cr-no-lf", 'c', 0, 0, "With -n, redraw on the same line"},
  { "csv-three", '3', 0, 0, "Dump the square in csv format, 3 numbers per row"},
  { "no-hat-2", 'h', 0, 0, "Don't show squares as ^2"},
  { 0 }
};

static struct argp
argp =
{
  options, parse_opt, 0,
  "Accept 3x3 magic squares from the standard input, and display it.\vThe nine values must be separated by a comma and terminated by a newline.",
  0
};

int
main (int argc, char **argv)
{
  struct fv_app_display_square_t app;
  memset (&app, 0, sizeof (app));
  app.magic_number = 1;
  app.read_square = read_square_from_stream;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  is_magic_square_init ();
  return fituvalu_display_square (&app, stdin);
}
