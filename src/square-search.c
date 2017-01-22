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
#include <gmp.h>
#include <stdbool.h>
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  return 0;
}
struct argp argp ={0, parse_opt, "MIN", "Find a perfect square that is divisible by a bunch of numbers.", 0};

int
main (int argc, char **argv)
{
  int divisors[]={
   /*2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 26, 28,*/ 29, /*30, 31, 33, 34, 36,*/ 37, /*38, 39, 40,*/ 41, /*42, 43, 44, 46, 47,*/ 48, /*51, 52,*/ 53, /*56, 57, 60,*/ 61, /*62, 66, 67, 68, 69,*/ 72, /*76, 78, 84, 86, 88, 92, 93, 94, 102, 104, 114,*/ 120, /*124, 129, 132, 134, 136, 138, 141, 152, 156,*/ 168, /*172, 184, 186, 188, 201, 204, 228, 248, 258,*/ 264, /*268, 276, 282,*/ 312, /*344, 372, 376, 402,*/ 408, 456, /*516, 536,*/ 552, /*564,*/ 744, /*804,*/ 1032, 1128, 1608};
  int max = sizeof (divisors) / sizeof (int) - 1;
  argp_parse (&argp, argc, argv, 0, 0, 0);
  if (argc != 2)
    {
      char *myargv[] = {"square-search", "--help", 0};
      argp_parse (&argp, 2, myargv, 0, 0, 0);
    }
  mpz_t i, start;
  mpz_init_set_str (start, argv[1], 10);
  mpz_init (i);
  mpz_set (i, start);
  mpz_t root;
  mpz_init (root);
  mpz_sqrt (root, i);
  mpz_mul (i, root, root);
  while (1)
    {
      mpz_add (i, i, root);
      mpz_add (i, i, root);
      mpz_add_ui (i, i, 1);
      bool divisible = true;
      for (int j = max; j >= 0; j--)
        {
          if (!mpz_divisible_ui_p (i, divisors[j]))
            {
              divisible = false;
              break;
            }
        }
      if (divisible)
        {
          char buf[mpz_sizeinbase (i, 10) + 2];
          mpz_get_str (buf, 10, i);
          printf("%s\n", buf);
        }
      mpz_add_ui (root, root, 1);
    }
  mpz_clears (i, start, root, NULL);
  return 0;
}

