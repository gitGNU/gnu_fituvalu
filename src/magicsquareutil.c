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

mpz_t row1;
mpz_t row2;
mpz_t row3;
mpz_t col1;
mpz_t col2;
mpz_t col3;
mpz_t diag1;
mpz_t diag2;

void
is_magic_square_init ()
{
  mpz_inits (row1, row2, row3, col1, col2, col3, diag1, diag2, NULL);
}

void
is_magic_square_fini ()
{
  mpz_clears (row1, row2, row3, col1, col2, col3, diag1, diag2, NULL);
}

int 
is_magic_square (mpz_t a[3][3], int diag)
{
  mpz_add (row1, a[0][0], a[0][1]);
  mpz_add (row1, row1, a[0][2]);

  mpz_add (row2, a[1][0], a[1][1]);
  mpz_add (row2, row2, a[1][2]);
  int res = mpz_cmp (row1, row2);
  if (res)
    return 0;

  mpz_add (row3, a[2][0], a[2][1]);
  mpz_add (row3, row3, a[2][2]);
  res = mpz_cmp (row3, row2);
  if (res)
    return 0;

  mpz_add (col1, a[0][0], a[1][0]);
  mpz_add (col1, col1, a[2][0]);
  res = mpz_cmp (col1, row1);
  if (res)
    return 0;

  mpz_add (col2, a[0][1], a[1][1]);
  mpz_add (col2, col2, a[2][1]);
  res = mpz_cmp (col2, col1);
  if (res)
    return 0;

  mpz_add (col3, a[0][2], a[1][2]);
  mpz_add (col3, col3, a[2][2]);
  res = mpz_cmp (col3, col2);
  if (res)
    return 0;

  mpz_add (diag1, a[0][0], a[1][1]);
  mpz_add (diag1, diag1, a[2][2]);
  res = mpz_cmp (diag1, col3);
  if (res)
    return 0;

  mpz_add (diag2, a[0][2], a[1][1]);
  mpz_add (diag2, diag2, a[2][0]);
  res = mpz_cmp (diag2, diag1);
  if (res)
    return 0;

  int i, j, k, l;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      for (k = 0; k < 3; k++)
        for (l = 0; l < 3; l++)
          if (mpz_cmp (a[i][j], a[k][l]) == 0 && (i != k || j != l))
            return 0;
  return 1;
}

int
read_square_from_stream (FILE *stream, mpz_t (*a)[3][3], char **line, size_t *len)
{
  int i, j;
  ssize_t read;
  for (i = 0; i < 3; i++)
    {
      for (j = 0; j < 3; j++)
        {
          if (i == 2 && j == 2)
            read = fv_getline (line, len, stream);
          else
            read = fv_getdelim (line, len, ',', stream);
          if (read == -1)
            break;
          char *end = strpbrk (*line, ",\n");
          if (end)
            *end = '\0';
          mpz_set_str ((*a)[i][j], *line, 10);
        }
      if (read == -1)
        break;
    }
  return read;
}

int
binary_read_square_from_stream (FILE *stream, mpz_t (*a)[3][3], char **s, size_t *len)
{
  int i, j;
  ssize_t read;
  for (i = 0; i < 3; i++)
    {
      for (j = 0; j < 3; j++)
        {
          read = mpz_inp_raw ((*a)[i][j], stream);
          if (!read)
            break;
        }
      if (!read)
        break;
    }
  if (read == 0)
    read = -1;
  return read;
}

int
read_numbers_from_stream (FILE *stream, mpz_t *a, int size, char **line, size_t *len)
{
  int i;
  ssize_t read = 0;
  for (i = 0; i < size; i++)
    {
      if (i == size - 1)
        read = fv_getline (line, len, stream);
      else
        read = fv_getdelim (line, len, ',', stream);
      if (read == -1)
        break;
      char *end = strpbrk (*line, ",\n");
      if (end)
        *end = '\0';
      mpz_set_str (a[i], *line, 10);
    }
  return read;
}

int
binary_read_numbers_from_stream (FILE *stream, mpz_t *a, int size, char **line, size_t *len)
{
  int i;
  ssize_t read = 0;
  for (i = 0; i < size; i++)
    {
      read = mpz_inp_raw (a[i], stream);
      if (!read)
        break;
    }
  if (!read)
    read = -1;
  return read;
}

int
read_three_numbers_from_stream (FILE *stream, mpz_t *a, char **line, size_t *len)
{
  return read_numbers_from_stream (stream, a, 3, line, len);
}

int
binary_read_three_numbers_from_stream (FILE *stream, mpz_t *a, char **line, size_t *len)
{
  return binary_read_numbers_from_stream (stream, a, 3, line, len);
}

int
read_four_numbers_from_stream (FILE *stream, mpz_t *a, char **line, size_t *len)
{
  return read_numbers_from_stream (stream, a, 4, line, len);
}

int
binary_read_four_numbers_from_stream (FILE *stream, mpz_t *a, char **line, size_t *len)
{
  return binary_read_numbers_from_stream (stream, a, 4, line, len);
}

int
count_squares (mpz_t a[3][3])
{
  int i, j;
  int count = 0;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      if (mpz_perfect_square_p (a[i][j]))
        count++;
  return count;
}

void
small_read_square_and_run (FILE *stream, void (*iterfunc)(unsigned long long, unsigned long long, unsigned long long, void (*)(unsigned long long*, unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE*), FILE *), void (*checkfunc)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), unsigned long long start, unsigned long long finish, FILE *out)
{
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  unsigned long long i;
  char *end = NULL;
  while (1)
    {
      read = fv_getline (&line, &len, stream);
      if (read == -1)
        break;
      end = NULL;
      i = strtoull (line, &end, 10);
      iterfunc (i, start, finish, checkfunc, out);
    }
  free (line);
  return;
}

void
read_square_and_run (FILE *stream, void (*iterfunc)(mpz_t, mpz_t, mpz_t, unsigned long long, void (*)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *), void (*checkfunc)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE*), mpz_t start, mpz_t finish, unsigned long long incr, FILE *out)
{
  mpz_t i;
  mpz_init (i);
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  while (1)
    {
      read = fv_getline (&line, &len, stream);
      if (read == -1)
        break;
      char *end = strchr (line, '\n');
      if (end)
        *end = '\0';
      mpz_set_str (i, line, 10);
      iterfunc (i, start, finish, incr, checkfunc, out);
    }
  mpz_clear (i);
  free (line);
  return;
}

void
binary_read_square_and_run (FILE *stream, void (*iterfunc)(mpz_t, mpz_t, mpz_t, unsigned long long, void (*)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *), void (*checkfunc)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE*), mpz_t start, mpz_t finish, unsigned long long incr, FILE *out)
{
  mpz_t i;
  mpz_init (i);
  ssize_t read;
  while (1)
    {
      read = mpz_inp_raw (i, stream);
      if (!read)
        break;
      iterfunc (i, start, finish, incr, checkfunc, out);
    }
  mpz_clear (i);
  return;
}

int
small_is_square (long long num)
{
  if (num < 0)
    return 0;
   long double root = sqrtl (num);
   if ((long long)root * (long long)root == num)
     return 1;
   return 0;
}

void
small_loop_and_run (void (*iterfunc)(unsigned long long, unsigned long long, unsigned long long, void (*)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), FILE *), void (*checkfunc)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), unsigned long long start, unsigned long long finish, FILE *out)
{
  unsigned long long root, lastroot, i;
  for (i = start; i < finish; i++)
    {
      if (small_is_square (i))
        {
          iterfunc (i, start, finish, checkfunc, out);
          root = sqrtl (i);
          lastroot = sqrtl (finish);
          do
            {
              i += root;
              i += root;
              i++;
              iterfunc (i, start, finish, checkfunc, out);
              root++;
            }
          while (root < lastroot);
          break;
        }
    }
}

void
loop_and_run (void (*iterfunc)(mpz_t, mpz_t, mpz_t, unsigned long long, void (*)(mpz_t*, mpz_t, mpz_t, mpz_t, mpz_t, FILE *), FILE*), void (*checkfunc)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE*), mpz_t start, mpz_t finish, unsigned long long incr, FILE *out)
{
  mpz_t i, j, k, root, lastroot;
  mpz_inits (i, j, k, root, lastroot, NULL);
  for (mpz_set (i, start); mpz_cmp (i, finish) < 0; mpz_add_ui (i, i, 1))
    {
      if (mpz_perfect_square_p (i))
        {
          iterfunc (i, start, finish, incr, checkfunc, out);
          mpz_sqrt (root, i);
          mpz_sqrt (lastroot, finish);
          do
            {
              mpz_add (i, i, root);
              mpz_add (i, i, root);
              mpz_add_ui (i, i, 1);
              iterfunc (i, start, finish, incr, checkfunc, out);
              mpz_add_ui (root, root, 1);
            }
          while (mpz_cmp (root, lastroot) < 0);
          break;
        }
    }
  mpz_clears (i, j, k, root, lastroot, NULL);
}

static void
dump_num (mpz_t *i, FILE *out)
{
  char buf[mpz_sizeinbase (*i, 10) + 2];
  mpz_get_str (buf, 10, *i);
  fprintf (out, "%s", buf);
}


void
disp_record (mpz_t *vec, int size, FILE *out)
{
  for (int i = 0; i < size; i++)
    {
      dump_num (&vec[i], out);
      fprintf (out, ", ");
    }
  fprintf (out, "\n");
  fflush (out);
}

void
disp_binary_record (mpz_t *vec, int size, FILE *out)
{
  for (int i = 0; i < size; i++)
    mpz_out_raw (out, vec[i]);
}
void
display_three_record (mpz_t *progression, FILE *out)
{
  disp_record (progression, 3, out);
}

void
display_four_record (mpz_t *progression, FILE *out)
{
  disp_record (progression, 4, out);
  fflush (out);
}

void
display_binary_four_record (mpz_t *progression, FILE *out)
{
  disp_binary_record (progression, 4, out);
}

void
display_three_record_with_root (mpz_t *progression, mpz_t *root, FILE *out)
{
  for (int i = 0; i < 3; i++)
    {
      dump_num (&progression[i], out);
      fprintf (out, ", ");
    }
  if (mpz_cmp_ui (*root, 0) != 0)
    {
      dump_num (root, out);
      fprintf (out, ", ");
    }
  fprintf (out, "\n");
  fflush (out);
}

void
display_binary_three_record (mpz_t *progression, FILE *out)
{
  disp_binary_record (progression, 3, out);
}

void
display_binary_three_record_with_root (mpz_t *progression, mpz_t *root, FILE *out)
{
  for (int i = 0; i < 3; i++)
      mpz_out_raw (out, progression[i]);
  if (mpz_cmp_ui (*root, 0) != 0)
    mpz_out_raw (out, *root);
  fflush (out);
}

void
display_nine_record (mpz_t *progression, FILE *out)
{
  disp_record (progression, 9, out);
}

void
display_binary_two_record (mpz_t *one, mpz_t *two, FILE *out)
{
  mpz_out_raw (out, *one);
  mpz_out_raw (out, *two);
  fflush (out);
}

void
display_two_record (mpz_t *one, mpz_t *two, FILE *out)
{
  dump_num (one, out);
  fprintf (out, ", ");
  dump_num (two, out);
  fprintf (out, "\n");
  fflush (out);
}

void
display_binary_nine_record (mpz_t *progression, FILE *out)
{
  disp_binary_record (progression, 9, out);
}

void
display_square_record (mpz_t s[3][3], FILE *out)
{
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      {
        dump_num (&s[i][j], out);
        fprintf (out, ", ");
      }
  fprintf (out, "\n");
  fflush (out);
}

void
display_binary_square_record (mpz_t s[3][3], FILE *out)
{
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_out_raw (out, s[i][j]);
  fflush (out);
}

static void
seq (mpz_t m, mpz_t n, mpz_t finish, FILE *out, void (*search)(mpz_t, mpz_t, mpz_t, mpz_t, FILE *), mpz_t _m2, mpz_t _n2)
{
  search (m, n, _m2, _n2, out);
  mpz_t s;
  mpz_init (s);
  mpz_add (s, m, n);
  if (mpz_cmp (s, finish) < 0)
    {
      mpz_t m2, n2;
      mpz_inits (m2, n2, NULL);
      mpz_set (m2, s);
      mpz_set (n2, m);
      if (mpz_even_p (n2))
        seq (m2, n2, finish, out, search, _m2, _n2);
      else
        {
          mpz_add (s, m2, n2);
          if (mpz_cmp (s, finish) < 0)
            {
              seq (s, m2, finish, out, search, _m2, _n2);
              seq (s, n2, finish, out, search, _m2, _n2);
            }
        }
      mpz_set (n2, n);
      if (mpz_even_p (n2))
        seq (m2, n2, finish, out, search, _m2, _n2);
      else
        {
          mpz_add (s, m2, n2);
          if (mpz_cmp (s, finish) < 0)
            {
              seq (s, m2, finish, out, search, _m2, _n2);
              seq (s, n2, finish, out, search, _m2, _n2);
            }
        }
      mpz_clears (m2, n2, NULL);
    }
  mpz_clear (s);
}

static void
small_seq (unsigned long long int m, unsigned long long int n, int finish, FILE *out, void (*search)(unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), unsigned long long _m2, unsigned long long _n2)
{
  search (m, n, _m2, _n2, out);
  unsigned long long int s = m + n;
  if (s < finish)
    {
      unsigned long long int m2 = s;
      unsigned long long int n2 = m;
      if ((n2 & 1) == 0)
        small_seq (m2, n2, finish, out, search, _m2, _n2);
      else
        {
          s = m2 + n2;
          if (s < finish)
            {
              small_seq (s, m2, finish, out, search, _m2, _n2);
              small_seq (s, n2, finish, out, search, _m2, _n2);
            }
        }
      n2 = n;
      if ((n2 & 1) == 0)
        small_seq (m2, n2, finish, out, search, _m2, _n2);
      else
        {
          s = m2 + n2;
          if (s < finish)
            {
              small_seq (s, m2, finish, out, search, _m2, _n2);
              small_seq (s, n2, finish, out, search, _m2, _n2);
            }
        }
    }
}

int
small_morgenstern_search (unsigned long long max, FILE *in, void (*search) (unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE*), FILE *out)
{
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  unsigned long long m, n;
  char *end = NULL;
  while (1)
    {
      read = fv_getdelim (&line, &len, ',', in);
      if (read == -1)
        break;
      end = NULL;
      m = strtoull (line, &end, 10);
      read = fv_getline (&line, &len, in);
      if (read == -1)
        break;
      end = NULL;
      n = strtoull (line, &end, 10);
      small_seq (1, 2, max, out, search, m, n);
    }
  if (line)
    free (line);
  return 0;
}

int
morgenstern_search (mpz_t max, FILE *in, void (*search) (mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *out)
{
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  mpz_t m, n, startm, startn;
  mpz_inits (m, n, startm, startn, NULL);
  mpz_set_ui (startm, 1);
  mpz_set_ui (startn, 2);
  while (1)
    {
      read = fv_getdelim (&line, &len, ',', in);
      if (read == -1)
        break;
      char *comma = strchr (line, ',');
      if (comma)
        *comma = '\0';
      mpz_set_str (m, line, 10);
      read = fv_getline (&line, &len, in);
      if (read == -1)
        break;
      mpz_set_str (n, line, 10);
      seq (startm, startn, max, out, search, m, n);
    }
  mpz_clears (m, n, startm, startn, NULL);
  if (line)
    free (line);
  return 0;
}

int
morgenstern_search_from_binary (mpz_t max, FILE *in, void (*search) (mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *out)
{
  ssize_t read;
  mpz_t m, n, startm, startn;
  mpz_inits (m, n, startm, startn, NULL);
  mpz_set_ui (startm, 1);
  mpz_set_ui (startn, 2);
  while (1)
    {
      read = mpz_inp_raw (m, in);
      if (!read)
        break;
      read = mpz_inp_raw (n, in);
      if (!read)
        break;
      seq (startm, startn, max, out, search, m, n);
    }
  mpz_clears (m, n, startm, startn, NULL);
  return 0;
}

void
reduce_three_square_progression (mpz_t *progression)
{
  //progression must already be sorted
  //and all 3 numbers must be perfect squares
  mpz_t gcd;
  mpz_init (gcd);
  mpz_set (gcd, progression[0]);
  mpz_gcd (gcd, gcd, progression[1]);
  mpz_gcd (gcd, gcd, progression[2]);
  if (mpz_cmp_ui (gcd, 1) > 0)
    {
      mpz_t d;
      mpz_init (d);
      int squares_retained = 1;
      for (int i = 0; i < 3; i++)
        {
          mpz_cdiv_q (d, progression[i], gcd);
          if (!mpz_perfect_square_p (d))
            {
              squares_retained = 0;
              break;
            }
        }
      mpz_clear (d);
      if (squares_retained)
        {
          for (int i = 0; i < 3; i++)
            mpz_cdiv_q (progression[i], progression[i], gcd);
        }
    }
  mpz_clear (gcd);
}

static void
_dual_inner (FILE *in, mpz_t m, mpz_t n, void (*search)(mpz_t, mpz_t, mpz_t, mpz_t, FILE *), FILE *out)
{
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  mpz_t r, s;
  mpz_inits (r, s, NULL);
  while (1)
    {
      read = fv_getdelim (&line, &len, ',', in);
      if (read == -1)
        break;
      char *comma = strchr (line, ',');
      if (comma)
        *comma = '\0';
      mpz_set_str (r, line, 10);
      read = fv_getline (&line, &len, in);
      if (read == -1)
        break;
      mpz_set_str (s, line, 10);
      search (m, n, r, s, out);
    }
  mpz_clears (r, s, NULL);
  if (line)
    free (line);
}

void
morgenstern_search_dual (FILE *in1, FILE *in2, void (*search) (mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *out)
{
  //in2 is rewindable, in1 is not.
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  mpz_t m, n;
  mpz_inits (m, n, NULL);
  while (1)
    {
      read = fv_getdelim (&line, &len, ',', in1);
      if (read == -1)
        break;
      char *comma = strchr (line, ',');
      if (comma)
        *comma = '\0';
      mpz_set_str (m, line, 10);
      read = fv_getline (&line, &len, in1);
      if (read == -1)
        break;
      mpz_set_str (n, line, 10);
      rewind (in2);
      _dual_inner (in2, m, n, search, out);
    }
  mpz_clears (m, n, NULL);
  if (line)
    free (line);
  return;
}

static void
_dual_binary_inner (FILE *in, mpz_t m, mpz_t n, void (*search)(mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *out)
{
  ssize_t read;
  mpz_t r, s;
  mpz_inits (r, s, NULL);
  while (1)
    {
      read = mpz_inp_raw (r, in);
      if (!read)
        break;
      read = mpz_inp_raw (s, in);
      if (!read)
        break;
      search (m, n, r, s, out);
    }
  mpz_clears (r, s, NULL);
  return;
}

void
morgenstern_search_dual_binary (FILE *in1, FILE *in2, void (*search) (mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *out)
{
  ssize_t read;
  mpz_t m, n;
  mpz_inits (m, n, NULL);
  while (1)
    {
      read = mpz_inp_raw (m, in1);
      if (!read)
        break;
      read = mpz_inp_raw (n, in1);
      if (!read)
        break;
      rewind (in2);
      _dual_binary_inner (in2, m, n, search, out);
    }
  mpz_clears (m, n, NULL);
  return;
}

void
display_textual_number (mpz_t *i, FILE *out)
{
  dump_num (i, out);
  fprintf (out, "\n");
  fflush (out);
}

void
display_binary_number (mpz_t *i, FILE *out)
{
  mpz_out_raw (out, *i);
  fflush (out);
}

int
fv_getline (char **line, size_t *len, FILE *stream)
{
  ssize_t read;
  while (1)
    {
      read = getline (line, len, stream);
      if (read == -1)
        break;
      if ((*line)[0] == '#')
        continue;
      break;
    }
  return read;
}

int
fv_getdelim (char **line, size_t *len, int delim, FILE *stream)
{
  ssize_t read;
  while (1)
    {
      read = getdelim (line, len, delim, stream);
      if (read == -1)
        break;
      if ((*line)[0] == '#')
        {
          if (strchr (*line, '\n'))
            {
              /* crapola, we're in a special case here.
               * we've sucked in a comment and gone clear over
               * a number to the next comma. */
              char *nl = strrchr (*line, '\n');
              nl++;
              if (nl[0] == '#')
                {
                  read = getline (line, len, stream);
                  continue;
                }
              memcpy (*line, nl, strlen (nl) + 1);
              break;
            }
          else
            {
              read = getline (line, len, stream);
              if (read == -1)
                break;
            }
          continue;
        }
      break;
    }
  return read;
}

static void
get_lowest (mpz_t a[3][3], mpz_t *low)
{
  mpz_t i;
  mpz_set_si (*low, -1);
  mpz_init (i);

  int j, k;
  for (j = 0; j < 3; j++)
    for (k = 0; k < 3; k++)
      {
        if (mpz_sgn (a[j][k]) > 0 &&
            (mpz_cmp (a[j][k], *low) < 0 || mpz_cmp_si (*low, -1) == 0))
          mpz_set (*low, a[j][k]);
      }
  if (mpz_sgn (*low) < 0)
    mpz_abs (*low, *low);
  mpz_clear (i);
}

void
reduce_square (mpz_t a[3][3])
{
  int j, k;
  mpz_t low, gcd;
  mpz_inits (low, gcd, NULL);
  get_lowest (a, &low);
  mpz_set (gcd, low);
  for (j = 0; j < 3; j++)
    for (k = 0; k < 3; k++)
      {
        if (mpz_cmp (a[j][k], low) == 0)
          continue;
        mpz_gcd (gcd, gcd, a[j][k]);
      }
  if (mpz_cmp_ui (gcd, 1) > 0)
    {
      mpz_t d;
      mpz_init (d);
      int squares_retained = 1;
      for (j = 0; j < 3; j++)
        for (k = 0; k < 3; k++)
          {
            mpz_cdiv_q (d, a[j][k], gcd);
            if (mpz_perfect_square_p (a[j][k]))
              {
                if (!mpz_perfect_square_p (d))
                  {
                    squares_retained = 0;
                    break;
                  }
              }
          }
      mpz_clear (d);

      if (squares_retained)
        {
          for (j = 0; j < 3; j++)
            for (k = 0; k < 3; k++)
              mpz_cdiv_q (a[j][k], a[j][k], gcd);
        }
    }
  mpz_clears (low, gcd, NULL);
}

struct rec
{
  mpz_t mn[2];
};

static void
load_mn_binary (FILE *in, struct rec **recs, int *numrecs)
{
  ssize_t read;
  mpz_t m, n;
  mpz_inits (m, n, NULL);
  while (1)
    {
      read = mpz_inp_raw (m, in);
      if (!read)
        break;
      read = mpz_inp_raw (n, in);
      if (!read)
        break;
      *recs = realloc (*recs, (*numrecs + 1) * sizeof (struct rec));
      mpz_init_set ((*recs)[*numrecs].mn[0], m);
      mpz_init_set ((*recs)[*numrecs].mn[1], n);
      (*numrecs)++;
    }
  mpz_clears (m, n, NULL);
}

static void
load_mn (FILE *in, struct rec **recs, int *numrecs)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t m, n;
  mpz_inits (m, n, NULL);
  while (1)
    {
      read = fv_getdelim (&line, &len, ',', in);
      if (read == -1)
        break;
      char *comma = strchr (line, ',');
      if (comma)
        *comma = '\0';
      mpz_set_str (m, line, 10);
      read = fv_getline (&line, &len, in);
      if (read == -1)
        break;
      mpz_set_str (n, line, 10);
      *recs = realloc (*recs, (*numrecs + 1) * sizeof (struct rec));
      mpz_init_set ((*recs)[*numrecs].mn[0], m);
      mpz_init_set ((*recs)[*numrecs].mn[1], n);
      (*numrecs)++;
    }
  if (line)
    free (line);
  mpz_clears (m, n, NULL);
}

static int
same_mn_files (struct rec *list1, struct rec *list2, int num)
{
  int identical = 1;
  for (int i = 0; i < num; i++)
    {
      if (mpz_cmp (list1[i].mn[0], list2[i].mn[0]) != 0)
        return 0;
      if (mpz_cmp (list1[i].mn[1], list2[i].mn[1]) != 0)
        return 0;
    }
  return identical;
}

static void
_morgenstern_search_dual_mem (struct rec *in1recs, int num_in1recs, struct rec *in2recs, int num_in2recs, void (*search) (mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *out)
{
  int infiles_identical = 0;
  if (num_in1recs == num_in2recs)
    infiles_identical = same_mn_files (in1recs, in2recs, num_in1recs);

  if (infiles_identical)
    {
      for (int i = 0; i < num_in1recs; i++)
        for (int j = 0; j < i; j++)
          search (in1recs[i].mn[0], in1recs[i].mn[1],
                  in2recs[j].mn[0], in2recs[j].mn[1], out);
    }
  else
    {
      for (int i = 0; i < num_in1recs; i++)
        for (int j = 0; j < num_in2recs; j++)
          search (in1recs[i].mn[0], in1recs[i].mn[1],
                  in2recs[j].mn[0], in2recs[j].mn[1], out);
    }

  //clean up
  for (int i = 0; i < num_in1recs; i++)
    {
      mpz_clear (in1recs[i].mn[0]);
      mpz_clear (in1recs[i].mn[1]);
    }
  free (in1recs);
  for (int i = 0; i < num_in2recs; i++)
    {
      mpz_clear (in2recs[i].mn[0]);
      mpz_clear (in2recs[i].mn[1]);
    }
  free (in2recs);
  return;
}

void
morgenstern_search_dual_binary_mem (FILE *in1, FILE *in2, void (*search) (mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *out)
{
  struct rec *in1recs = NULL, *in2recs = NULL;
  int num_in1recs = 0, num_in2recs = 0;
  load_mn_binary (in1, &in1recs, &num_in1recs);
  load_mn_binary (in2, &in2recs, &num_in2recs);
  return _morgenstern_search_dual_mem
    (in1recs, num_in1recs, in2recs, num_in2recs, search, out);
}

void
morgenstern_search_dual_mem (FILE *in1, FILE *in2, void (*search) (mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *out)
{
  struct rec *in1recs = NULL, *in2recs = NULL;
  int num_in1recs = 0, num_in2recs = 0;
  load_mn (in1, &in1recs, &num_in1recs);
  load_mn (in2, &in2recs, &num_in2recs);
  return _morgenstern_search_dual_mem
    (in1recs, num_in1recs, in2recs, num_in2recs, search, out);
}
