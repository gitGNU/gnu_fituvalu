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

int *filter_types;
int num_filters;
void (*display_square) (mpz_t s[3][3], FILE *out) = display_square_record;
int (*read_square) (FILE *, mpz_t (*)[3][3], char **, size_t *) = read_square_from_stream;

static int
is_morgenstern_five_square_type_one (mpz_t a[3][3], int s[3][3])
{
/*
    -----------
    A^2  -  C^2
     -  E^2  - 
    G^2  -  I^2
    -----------
         1     
      type 23
*/
  if (!s[0][0] ||
      !s[0][2] ||
      !s[1][1] ||
      !s[2][0] ||
      !s[2][2])
    return 0;
  //1:  A^2 + I^2  =  C^2 + G^2  =  2*E^2
  mpz_t ai, cg, ee;
  mpz_inits (ai, cg, ee, NULL);
  mpz_add (ai, a[0][0], a[2][2]);
  mpz_add (cg, a[0][2], a[2][0]);
  mpz_mul_ui (ee, a[1][1], 2);
  int ret = mpz_cmp (ai, cg) == 0 && mpz_cmp (ai, ee) == 0;
  mpz_clears (ai, cg, ee, NULL);
  return ret;
}

static int
is_morgenstern_five_square_type_two (mpz_t a[3][3], int s[3][3])
{
/*
    -----------
     -  B^2  - 
    D^2 E^2 F^2
     -  H^2  - 
    -----------
         2
      type 20
*/
  if (!s[0][1] ||
      !s[1][0] ||
      !s[1][1] ||
      !s[1][2] ||
      !s[2][1])
    return 0;
  //2:  H^2 + B^2  =  F^2 + D^2  =  2*E^2
  mpz_t hb, fd, ee;
  mpz_inits (hb, fd, ee, NULL);
  mpz_add (hb, a[2][1], a[0][1]);
  mpz_add (fd, a[1][0], a[1][2]);
  mpz_mul_ui (ee, a[1][1], 2);
  int ret = mpz_cmp (hb, fd) == 0 && mpz_cmp (hb, ee) == 0;
  mpz_clears (hb, fd, ee, NULL);
  return ret;
}

static int
is_morgenstern_five_square_type_three_one (mpz_t a[3][3], int s[3][3])
{
/*
    -----------
    A^2 B^2  -
     -  E^2  -
     -  H^2 I^2
    -----------
         3
      type 17
*/
  int ret = 0;
  if (s[0][0] && s[0][1] && s[1][1] && s[2][1] && s[2][2])
    {
      //3:  A^2 + I^2  =  H^2 + B^2  =  2*E^2
      mpz_t ai, hb, ee;
      mpz_inits (ai, hb, ee, NULL);
      mpz_add (ai, a[0][0], a[2][2]);
      mpz_add (hb, a[2][1], a[0][1]);
      mpz_mul_ui (ee, a[1][1], 2);
      ret = mpz_cmp (ai, hb) == 0 && mpz_cmp (ai, ee) == 0;
      mpz_clears (ai, hb, ee, NULL);
    }
  return ret;
}

static int
is_morgenstern_five_square_type_three_two (mpz_t a[3][3], int s[3][3])
{
/*
    -----------
     -  B^2 A^2
     -  E^2  - 
    I^2 H^2  - 
     -----------
         3
      type 17
*/
  int ret = 0;
  if (s[0][1] && s[0][2] && s[1][1] && s[2][0] && s[2][1])
    {
      //3:  A^2 + I^2  =  H^2 + B^2  =  2*E^2
      mpz_t ai, hb, ee;
      mpz_inits (ai, hb, ee, NULL);
      mpz_add (ai, a[0][2], a[2][0]);
      mpz_add (hb, a[2][1], a[0][1]);
      mpz_mul_ui (ee, a[1][1], 2);
      ret = mpz_cmp (ai, hb) == 0 && mpz_cmp (ai, ee) == 0;
      mpz_clears (ai, hb, ee, NULL);
    }
  return ret;
}

static int
is_morgenstern_five_square_type_three_three (mpz_t a[3][3], int s[3][3])
{
/*
    -----------
     -   -  I^2
    B^2 E^2 H^2
    A^2  -   - 
    -----------
         3     
      type 17  
*/
  int ret = 0;
  if (s[0][2] && s[1][0] && s[1][1] && s[1][2] && s[2][0])
    {
      //3:  A^2 + I^2  =  H^2 + B^2  =  2*E^2
      mpz_t ai, hb, ee;
      mpz_inits (ai, hb, ee, NULL);
      mpz_add (ai, a[2][0], a[0][2]);
      mpz_add (hb, a[1][2], a[1][0]);
      mpz_mul_ui (ee, a[1][1], 2);
      ret = mpz_cmp (ai, hb) == 0 && mpz_cmp (ai, ee) == 0;
      mpz_clears (ai, hb, ee, NULL);
    }
  return ret;
}

static int
is_morgenstern_five_square_type_three_four (mpz_t a[3][3], int s[3][3])
{
/*
    -----------
    I^2  -   - 
    H^2 E^2 B^2
     -   -  A^2
    -----------
         3     
      type 17  
*/
  int ret = 0;
  if (s[0][0] && s[1][0] && s[1][1] && s[1][2] && s[2][2])
    {
      //3:  A^2 + I^2  =  H^2 + B^2  =  2*E^2
      mpz_t ai, hb, ee;
      mpz_inits (ai, hb, ee, NULL);
      mpz_add (ai, a[2][2], a[0][0]);
      mpz_add (hb, a[1][0], a[1][2]);
      mpz_mul_ui (ee, a[1][1], 2);
      ret = mpz_cmp (ai, hb) == 0 && mpz_cmp (ai, ee) == 0;
      mpz_clears (ai, hb, ee, NULL);
    }
  return ret;
}

static int
is_morgenstern_five_square_type_three (mpz_t a[3][3], int s[3][3])
{
/*
    -----------   -----------
    A^2 B^2  -     -  B^2 A^2
     -  E^2  -     -  E^2  - 
     -  H^2 I^2   I^2 H^2  - 
    -----------   -----------
         3             3
      type 17       type 17
    -----------   -----------
     -   -  I^2   I^2  -   - 
    B^2 E^2 H^2   H^2 E^2 B^2
    A^2  -   -     -   -  A^2
    -----------   -----------
         3             3
      type 17       type 17
*/
  return
    is_morgenstern_five_square_type_three_one (a, s) ||
    is_morgenstern_five_square_type_three_two (a, s) ||
    is_morgenstern_five_square_type_three_three (a, s) ||
    is_morgenstern_five_square_type_three_four (a, s);
}

static int
is_morgenstern_five_square_type_four_one (mpz_t a[3][3], int s[3][3])
{
/*
    -----------
     -  B^2  - 
    D^2  -  F^2
    G^2  -  I^2
    -----------
         4     
      type 19  
*/
  int ret = 0;
  if (s[0][1] && s[1][0] && s[1][2] && s[2][0] && s[2][2])
    {
      //4:  2*G^2 - F^2  =  2*I^2 - D^2  =  B^2
      mpz_t gg, ii, ggf, iid;
      mpz_inits (gg, ii, ggf, iid, NULL);
      mpz_mul_ui (gg, a[2][0], 2);
      mpz_mul_ui (ii, a[2][2], 2);
      mpz_sub (ggf, gg, a[1][2]);
      mpz_sub (iid, ii, a[1][0]);
      ret = mpz_cmp (ggf, iid) == 0 && mpz_cmp (ggf, a[0][1]) == 0;
      mpz_clears (gg, ii, ggf, iid, NULL);
    }
  return ret;
}

static int
is_morgenstern_five_square_type_four_two (mpz_t a[3][3], int s[3][3])
{
/*
    -----------
    A^2  -  C^2
    D^2  -  F^2
     -  H^2  - 
    -----------
         5     
      type 19  
*/
  int ret = 0;
  if (s[0][0] && s[0][2] && s[1][0] && s[1][2] && s[2][1])
    {
      //5:  2*A^2 - F^2  =  2*C^2 - D^2  =  H^2
      mpz_t aa, cc, aaf, ccd;
      mpz_inits (aa, cc, aaf, ccd, NULL);
      mpz_mul_ui (aa, a[0][0], 2);
      mpz_mul_ui (cc, a[0][2], 2);
      mpz_sub (aaf, aa, a[1][2]);
      mpz_sub (ccd, cc, a[1][0]);
      ret = mpz_cmp (aaf, ccd) == 0 && mpz_cmp (aaf, a[2][1]) == 0;
      mpz_clears (aa, cc, aaf, ccd, NULL);
    }
  return ret;
}

static int
is_morgenstern_five_square_type_four_three (mpz_t a[3][3], int s[3][3])
{
/*
    -----------
    A^2 B^2  - 
     -  -   F^2
    G^2 H^2  - 
    -----------
         6     
      type 19  
*/
  int ret = 0;
  if (s[0][0] && s[0][1] && s[1][2] && s[2][0] && s[2][1])
    {
      //6:  2*G^2 - B^2  =  2*A^2 - H^2  =  F^2
      mpz_t gg, aa, ggb, aah;
      mpz_inits (gg, aa, ggb, aah, NULL);
      mpz_mul_ui (gg, a[2][0], 2);
      mpz_mul_ui (aa, a[0][0], 2);
      mpz_sub (ggb, gg, a[0][1]);
      mpz_sub (aah, aa, a[2][1]);
      ret = mpz_cmp (ggb, aah) == 0 && mpz_cmp (ggb, a[1][2]) == 0;
      mpz_clears (gg, aa, ggb, aah, NULL);
    }
  return ret;
}

static int
is_morgenstern_five_square_type_four_four (mpz_t a[3][3], int s[3][3])
{
/*
    -----------
     -  B^2 A^2
    F^2  -   - 
     -  H^2 G^2
    -----------
         7
      type 19
*/
  int ret = 0;
  if (s[0][1] && s[0][2] && s[1][0] && s[2][1] && s[2][2])
    {
      //7:  2*G^2 - B^2  =  2*A^2 - H^2  =  F^2
      mpz_t gg, aa, ggb, aah;
      mpz_inits (gg, aa, ggb, aah, NULL);
      mpz_mul_ui (gg, a[2][2], 2);
      mpz_mul_ui (aa, a[0][2], 2);
      mpz_sub (ggb, gg, a[0][1]);
      mpz_sub (aah, aa, a[2][1]);
      ret = mpz_cmp (ggb, aah) == 0 && mpz_cmp (ggb, a[1][0]) == 0;
      mpz_clears (gg, aa, ggb, aah, NULL);
      return 1;
    }
  return ret;
}

static int
is_morgenstern_five_square_type_four (mpz_t a[3][3], int s[3][3])
{
/*
    -----------   -----------   -----------   -----------
     -  B^2  -    A^2  -  C^2   A^2 B^2  -     -  B^2 A^2
    D^2  -  F^2   D^2  -  F^2    -  -   F^2   F^2 -    - 
    G^2  -  I^2    -  H^2  -    G^2 H^2  -     -  H^2 G^2
    -----------   -----------   -----------   -----------
         4             5             6             7
      type 19       type 19       type 19       type 19
*/
  return
    is_morgenstern_five_square_type_four_one (a, s) ||
    is_morgenstern_five_square_type_four_two (a, s) ||
    is_morgenstern_five_square_type_four_three (a, s) ||
    is_morgenstern_five_square_type_four_four (a, s);
}

static int
get_morgenstern_type (mpz_t a[3][3], int s[3][3])
{
  if (is_morgenstern_five_square_type_one (a, s))
    return 1;
  if (is_morgenstern_five_square_type_two (a, s))
    return 2;
  if (is_morgenstern_five_square_type_three (a, s))
    return 3;
  if (is_morgenstern_five_square_type_four (a, s))
    return 4;
  return 0;
}

static int
morgenstern_five_type (FILE *stream)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t a[3][3];
  int s[3][3];

  int i, j;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      mpz_init (a[i][j]);

  while (1)
    {
      read = read_square (stream, &a, &line, &len);
      if (read == -1)
        break;
      int count = 0;
      for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
          {
            s[i][j] = mpz_perfect_square_p (a[i][j]);
            if (s[i][j])
              count++;
          }
      if (is_magic_square (a, 1) && count >= 5)
        {
          int type = get_morgenstern_type (a, s);
          if (num_filters)
            {
              for (int i = 0; i < num_filters; i++)
                {
                  if (filter_types[i] == type)
                    {
                      display_square (a, stdout);
                      break;
                    }
                }
            }
          else
            fprintf (stdout, "%d\n", type);
        }
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
  int type;
  switch (key)
    {
    case 'i':
      read_square = binary_read_square_from_stream;
      break;
    case 'o':
      display_square = display_binary_square_record;
      break;
    case 'f':
      type = atoi (arg);
      if (type < 1 || type > 4)
        argp_error (state, "type must be between 1 and 4.");
      else
        {
          filter_types =
            realloc (filter_types, sizeof (int) * (num_filters + 1));
          filter_types[num_filters] = type;
          num_filters++;
        }
      break;
    }
  return 0;
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "filter", 'f', "TYPE", 0, "Show the magic squares that have this morgenstern type."},
  { 0 }
};

struct argp argp ={options, parse_opt, 0, "Accept 3x3 magic squares from the standard input, and get the type of five-square morgenstern type of magic square.  These magic squares have five perfect squares in a configuration identified by Lee Morgenstern as being candidates for seven perfect squares.\vThe nine values must be separated by a comma and terminated by a newline.", 0};

int
main (int argc, char **argv)
{
  argp_parse (&argp, argc, argv, 0, 0, 0);
  is_magic_square_init ();
  return morgenstern_five_type (stdin);
}
