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
            read = getline (line, len, stream);
          else
            read = getdelim (line, len, ',', stream);
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
read_numbers_from_stream (FILE *stream, mpz_t (*a)[SIZE], char **line, size_t *len)
{
  int i;
  ssize_t read;
  for (i = 0; i < SIZE; i++)
    {
      if (i == SIZE - 1)
        read = getline (line, len, stream);
      else
        read = getdelim (line, len, ',', stream);
      if (read == -1)
        break;
      char *end = strpbrk (*line, ",\n");
      if (end)
        *end = '\0';
      mpz_set_str ((*a)[i], *line, 10);
    }
  return read;
}

int
binary_read_numbers_from_stream (FILE *stream, mpz_t (*a)[SIZE], char **line, size_t *len)
{
  int i;
  ssize_t read;
  for (i = 0; i < SIZE; i++)
    {
      read = mpz_inp_raw ((*a)[i], stream);
      if (!read)
        break;
    }
  if (!read)
    read = -1;
  return read;
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
      read = getline (&line, &len, stream);
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
read_square_and_run (FILE *stream, void (*iterfunc)(mpz_t, mpz_t, mpz_t, void (*)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *), void (*checkfunc)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE*), mpz_t start, mpz_t finish, FILE *out)
{
  mpz_t i;
  mpz_init (i);
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  while (1)
    {
      read = getline (&line, &len, stream);
      if (read == -1)
        break;
      char *end = strchr (line, '\n');
      if (end)
        *end = '\0';
      mpz_set_str (i, line, 10);
      iterfunc (i, start, finish, checkfunc, out);
    }
  mpz_clear (i);
  free (line);
  return;
}

void
binary_read_square_and_run (FILE *stream, void (*iterfunc)(mpz_t, mpz_t, mpz_t, void (*)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *), void (*checkfunc)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE*), mpz_t start, mpz_t finish, FILE *out)
{
  mpz_t i;
  mpz_init (i);
  ssize_t read;
  while (1)
    {
      read = mpz_inp_raw (i, stream);
      if (!read)
        break;
      iterfunc (i, start, finish, checkfunc, out);
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
loop_and_run (void (*iterfunc)(mpz_t, mpz_t, mpz_t, void (*)(mpz_t*, mpz_t, mpz_t, mpz_t, mpz_t, FILE *), FILE*), void (*checkfunc)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE*), mpz_t start, mpz_t finish, FILE *out)
{
  mpz_t i, j, k, root, lastroot;
  mpz_inits (i, j, k, root, lastroot, NULL);
  for (mpz_set (i, start); mpz_cmp (i, finish) < 0; mpz_add_ui (i, i, 1))
    {
      if (mpz_perfect_square_p (i))
        {
          iterfunc (i, start, finish, checkfunc, out);
          mpz_sqrt (root, i);
          mpz_sqrt (lastroot, finish);
          do
            {
              mpz_add (i, i, root);
              mpz_add (i, i, root);
              mpz_add_ui (i, i, 1);
              iterfunc (i, start, finish, checkfunc, out);
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
display_three_record (mpz_t *progression, FILE *out)
{
  for (int i = 0; i < 3; i++)
    {
      dump_num (&progression[i], out);
      fprintf (out, ", ");
    }
  fprintf (out, "\n");
}

void
display_four_record (mpz_t *progression, FILE *out)
{
  for (int i = 0; i < 4; i++)
    {
      dump_num (&progression[i], out);
      fprintf (out, ", ");
    }
  fprintf (out, "\n");
}
void
display_three_record_with_root (mpz_t *progression, mpz_t *root, FILE *out)
{
  for (int i = 0; i < 3; i++)
    {
      dump_num (&progression[i], out);
      fprintf (out, ", ");
    }
  dump_num (root, out);
  fprintf (out, ", ");
  fprintf (out, "\n");
}

void
display_nine_record (mpz_t *progression, FILE *out)
{
  for (int i = 0; i < 9; i++)
    {
      dump_num (&progression[i], out);
      fprintf (out, ", ");
    }
  fprintf (out, "\n");
}

void
display_binary_nine_record (mpz_t *progression, FILE *out)
{
  for (int i = 0; i < 9; i++)
    mpz_out_raw (out, progression[i]);
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
}

void
display_binary_square_record (mpz_t s[3][3], FILE *out)
{
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mpz_out_raw (out, s[i][j]);
}

static void
seq (unsigned long long int m, unsigned long long int n, int finish, FILE *out, void (*search)(unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), unsigned long long _m2, unsigned long long _n2)
{
  search (m, n, _m2, _n2, out);
  unsigned long long int s = m + n;
  if (s < finish)
    {
      unsigned long long int m2 = s;
      unsigned long long int n2 = m;
      if ((n2 & 1) == 0)
        seq (m2, n2, finish, out, search, _m2, _n2);
      else
        {
          s = m2 + n2;
          if (s < finish)
            {
              seq (s, m2, finish, out, search, _m2, _n2);
              seq (s, n2, finish, out, search, _m2, _n2);
            }
        }
      n2 = n;
      if ((n2 & 1) == 0)
        seq (m2, n2, finish, out, search, _m2, _n2);
      else
        {
          s = m2 + n2;
          if (s < finish)
            {
              seq (s, m2, finish, out, search, _m2, _n2);
              seq (s, n2, finish, out, search, _m2, _n2);
            }
        }
    }
}

static void
symmetric_seq (unsigned long long int m, unsigned long long int n, int finish, FILE *out, void (*search)(unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), unsigned long long _m2, unsigned long long _n2)
{
  if (m >= _m2)
    return;
  search (m, n, _m2, _n2, out);
  unsigned long long int s = m + n;
  if (s < finish)
    {
      unsigned long long int m2 = s;
      unsigned long long int n2 = m;
      if ((n2 & 1) == 0)
        seq (m2, n2, finish, out, search, _m2, _n2);
      else
        {
          s = m2 + n2;
          if (s < finish)
            {
              seq (s, m2, finish, out, search, _m2, _n2);
              seq (s, n2, finish, out, search, _m2, _n2);
            }
        }
      n2 = n;
      if ((n2 & 1) == 0)
        seq (m2, n2, finish, out, search, _m2, _n2);
      else
        {
          s = m2 + n2;
          if (s < finish)
            {
              seq (s, m2, finish, out, search, _m2, _n2);
              seq (s, n2, finish, out, search, _m2, _n2);
            }
        }
    }
}

int
morgenstern_search (unsigned long long max, FILE *in, void (*search) (unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE*), FILE *out)
{
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  unsigned long long m, n;
  char *end = NULL;
  while (1)
    {
      read = getdelim (&line, &len, ',', in);
      if (read == -1)
        break;
      end = NULL;
      m = strtoull (line, &end, 10);
      read = getline (&line, &len, in);
      if (read == -1)
        break;
      end = NULL;
      n = strtoull (line, &end, 10);
      seq (1, 2, max, out, search, m, n);
    }
  if (line)
    free (line);
  return 0;
}

int
morgenstern_symmetric_search (unsigned long long max, FILE *in, void (*search) (unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE*), FILE *out)
{
  ssize_t read;
  char *line = NULL;
  size_t len = 0;
  unsigned long long m, n;
  char *end = NULL;
  while (1)
    {
      read = getdelim (&line, &len, ',', in);
      if (read == -1)
        break;
      end = NULL;
      m = strtoull (line, &end, 10);
      read = getline (&line, &len, in);
      if (read == -1)
        break;
      end = NULL;
      n = strtoull (line, &end, 10);
      symmetric_seq (1, 2, max, out, search, m, n);
    }
  if (line)
    free (line);
  return 0;
}
