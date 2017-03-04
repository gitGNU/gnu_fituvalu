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
#include <math.h>
#include <stdlib.h>
#include <argp.h>
#include <gmp.h>
#include <string.h>
#include "magicsquareutil.h"
struct fv_app_type_square_t
{
  int num_filters;
  int *filter_types;
  int *filter_squares;
  int (*read_square) (FILE *, mpz_t (*)[3][3], char **, size_t *);
  void (*display_square) (mpz_t s[3][3], FILE *out);
  int show_num_squares;
};
/*
 how to generate the lookup tables:
 the doit.sh script:
#!/bin/bash
while IFS='' read -r line || [[ -n "$line" ]]; do
  tmpfile=`mktemp`
  echo $line | ./rotate-square > $tmpfile
  type=""
  while IFS='' read -r rline || [[ -n "$rline" ]]; do
    type=`grep -n "^$rline$" $2 | cut -f1 -d:`
    if [ "x$type" != "x" ]; then
      break
    fi
  done < "$tmpfile"
  echo -n "[0b"
  echo -n "$line" | sed -e 's/[, ]//g'
  echo "] = $type,"
  rm $tmpfile
done < "$1"

then run:
echo "1,0,0,0,0,0,0,0,0" | ./permute-square -S | sort | uniq > 1
echo "1,0,0,0,0,0,0,0,0" | ./permute-square -S | sort | uniq | ./uniq-squares > 1.uniq
./doit.sh 1 1.uniq
echo "1,1,0,0,0,0,0,0,0" | ./permute-square -S | sort | uniq > 2
echo "1,1,0,0,0,0,0,0,0" | ./permute-square -S | sort | uniq | ./uniq-squares > 2.uniq
./doit.sh 2 2.uniq
echo "1,1,1,0,0,0,0,0,0" | ./permute-square -S | sort | uniq > 3
echo "1,1,1,0,0,0,0,0,0" | ./permute-square -S | sort | uniq | ./uniq-squares > 3.uniq
./doit.sh 3 3.uniq
and so on.

*/

static int
get_type_from_signature (int signature, int num_squares)
{
  int types[512] =  /* 3x3 means 9 bits total which means a size of 2^9 */
    {
      /* no squares */
      [0b000000000] = 0,

      /* one square */
      [0b000000001] = 1,
      [0b000000010] = 2,
      [0b000000100] = 1,
      [0b000001000] = 2,
      [0b000010000] = 3,
      [0b000100000] = 2,
      [0b001000000] = 1,
      [0b010000000] = 2,
      [0b100000000] = 1,

      /* two squares */
      [0b000000011] = 1,
      [0b000000101] = 2,
      [0b000000110] = 1,
      [0b000001001] = 1,
      [0b000001010] = 3,
      [0b000001100] = 4,
      [0b000010001] = 5,
      [0b000010010] = 6,
      [0b000010100] = 5,
      [0b000011000] = 6,
      [0b000100001] = 4,
      [0b000100010] = 3,
      [0b000100100] = 1,
      [0b000101000] = 7,
      [0b000110000] = 6,
      [0b001000001] = 2,
      [0b001000010] = 4,
      [0b001000100] = 8,
      [0b001001000] = 1,
      [0b001010000] = 5,
      [0b001100000] = 4,
      [0b010000001] = 4,
      [0b010000010] = 7,
      [0b010000100] = 4,
      [0b010001000] = 3,
      [0b010010000] = 6,
      [0b010100000] = 3,
      [0b011000000] = 1,
      [0b100000001] = 8,
      [0b100000010] = 4,
      [0b100000100] = 2,
      [0b100001000] = 4,
      [0b100010000] = 5,
      [0b100100000] = 1,
      [0b101000000] = 2,
      [0b110000000] = 1,

      /* three squares */
      [0b000000111] = 1,
      [0b000001011] = 2,
      [0b000001101] = 3,
      [0b000001110] = 4,
      [0b000010011] = 5,
      [0b000010101] = 6,
      [0b000010110] = 5,
      [0b000011001] = 5,
      [0b000011010] = 7,
      [0b000011100] = 8,
      [0b000100011] = 4,
      [0b000100101] = 3,
      [0b000100110] = 2,
      [0b000101001] = 9,
      [0b000101010] = 10,
      [0b000101100] = 9,
      [0b000110001] = 8,
      [0b000110010] = 7,
      [0b000110100] = 5,
      [0b000111000] = 11,
      [0b001000011] = 3,
      [0b001000101] = 12,
      [0b001000110] = 13,
      [0b001001001] = 1,
      [0b001001010] = 4,
      [0b001001100] = 13,
      [0b001010001] = 6,
      [0b001010010] = 8,
      [0b001010100] = 14,
      [0b001011000] = 5,
      [0b001100001] = 15,
      [0b001100010] = 16,
      [0b001100100] = 13,
      [0b001101000] = 9,
      [0b001110000] = 8,
      [0b010000011] = 9,
      [0b010000101] = 15,
      [0b010000110] = 9,
      [0b010001001] = 4,
      [0b010001010] = 10,
      [0b010001100] = 16,
      [0b010010001] = 8,
      [0b010010010] = 11,
      [0b010010100] = 8,
      [0b010011000] = 7,
      [0b010100001] = 16,
      [0b010100010] = 10,
      [0b010100100] = 4,
      [0b010101000] = 10,
      [0b010110000] = 7,
      [0b011000001] = 3,
      [0b011000010] = 9,
      [0b011000100] = 13,
      [0b011001000] = 2,
      [0b011010000] = 5,
      [0b011100000] = 4,
      [0b100000011] = 13,
      [0b100000101] = 12,
      [0b100000110] = 3,
      [0b100001001] = 13,
      [0b100001010] = 16,
      [0b100001100] = 15,
      [0b100010001] = 14,
      [0b100010010] = 8,
      [0b100010100] = 6,
      [0b100011000] = 8,
      [0b100100001] = 13,
      [0b100100010] = 4,
      [0b100100100] = 1,
      [0b100101000] = 9,
      [0b100110000] = 5,
      [0b101000001] = 12,
      [0b101000010] = 15,
      [0b101000100] = 12,
      [0b101001000] = 3,
      [0b101010000] = 6,
      [0b101100000] = 3,
      [0b110000001] = 13,
      [0b110000010] = 9,
      [0b110000100] = 3,
      [0b110001000] = 4,
      [0b110010000] = 5,
      [0b110100000] = 2,
      [0b111000000] = 1,

      /* four squares */
      [0b000001111] = 1,
      [0b000010111] = 2,
      [0b000011011] = 3,
      [0b000011101] = 4,
      [0b000011110] = 5,
      [0b000100111] = 1,
      [0b000101011] = 6,
      [0b000101101] = 7,
      [0b000101110] = 6,
      [0b000110011] = 5,
      [0b000110101] = 4,
      [0b000110110] = 3,
      [0b000111001] = 8,
      [0b000111010] = 9,
      [0b000111100] = 8,
      [0b001000111] = 10,
      [0b001001011] = 1,
      [0b001001101] = 10,
      [0b001001110] = 11,
      [0b001010011] = 4,
      [0b001010101] = 12,
      [0b001010110] = 13,
      [0b001011001] = 2,
      [0b001011010] = 5,
      [0b001011100] = 13,
      [0b001100011] = 14,
      [0b001100101] = 15,
      [0b001100110] = 16,
      [0b001101001] = 17,
      [0b001101010] = 18,
      [0b001101100] = 19,
      [0b001110001] = 20,
      [0b001110010] = 21,
      [0b001110100] = 13,
      [0b001111000] = 8,
      [0b010000111] = 17,
      [0b010001011] = 6,
      [0b010001101] = 14,
      [0b010001110] = 18,
      [0b010010011] = 8,
      [0b010010101] = 20,
      [0b010010110] = 8,
      [0b010011001] = 5,
      [0b010011010] = 9,
      [0b010011100] = 21,
      [0b010100011] = 18,
      [0b010100101] = 14,
      [0b010100110] = 6,
      [0b010101001] = 18,
      [0b010101010] = 22,
      [0b010101100] = 18,
      [0b010110001] = 21,
      [0b010110010] = 9,
      [0b010110100] = 5,
      [0b010111000] = 9,
      [0b011000011] = 7,
      [0b011000101] = 15,
      [0b011000110] = 19,
      [0b011001001] = 1,
      [0b011001010] = 6,
      [0b011001100] = 16,
      [0b011010001] = 4,
      [0b011010010] = 8,
      [0b011010100] = 13,
      [0b011011000] = 3,
      [0b011100001] = 14,
      [0b011100010] = 18,
      [0b011100100] = 11,
      [0b011101000] = 6,
      [0b011110000] = 5,
      [0b100000111] = 10,
      [0b100001011] = 16,
      [0b100001101] = 15,
      [0b100001110] = 14,
      [0b100010011] = 13,
      [0b100010101] = 12,
      [0b100010110] = 4,
      [0b100011001] = 13,
      [0b100011010] = 21,
      [0b100011100] = 20,
      [0b100100011] = 11,
      [0b100100101] = 10,
      [0b100100110] = 1,
      [0b100101001] = 19,
      [0b100101010] = 18,
      [0b100101100] = 17,
      [0b100110001] = 13,
      [0b100110010] = 5,
      [0b100110100] = 2,
      [0b100111000] = 8,
      [0b101000011] = 15,
      [0b101000101] = 23,
      [0b101000110] = 15,
      [0b101001001] = 10,
      [0b101001010] = 14,
      [0b101001100] = 15,
      [0b101010001] = 12,
      [0b101010010] = 20,
      [0b101010100] = 12,
      [0b101011000] = 4,
      [0b101100001] = 15,
      [0b101100010] = 14,
      [0b101100100] = 10,
      [0b101101000] = 7,
      [0b101110000] = 4,
      [0b110000011] = 19,
      [0b110000101] = 15,
      [0b110000110] = 7,
      [0b110001001] = 11,
      [0b110001010] = 18,
      [0b110001100] = 14,
      [0b110010001] = 13,
      [0b110010010] = 8,
      [0b110010100] = 4,
      [0b110011000] = 5,
      [0b110100001] = 16,
      [0b110100010] = 6,
      [0b110100100] = 1,
      [0b110101000] = 6,
      [0b110110000] = 3,
      [0b111000001] = 10,
      [0b111000010] = 17,
      [0b111000100] = 10,
      [0b111001000] = 1,
      [0b111010000] = 2,
      [0b111100000] = 1,

      /* five squares */
      [0b000011111] = 1,
      [0b000101111] = 2,
      [0b000110111] = 1,
      [0b000111011] = 3,
      [0b000111101] = 4,
      [0b000111110] = 3,
      [0b001001111] = 5,
      [0b001010111] = 6,
      [0b001011011] = 1,
      [0b001011101] = 6,
      [0b001011110] = 7,
      [0b001100111] = 8,
      [0b001101011] = 9,
      [0b001101101] = 10,
      [0b001101110] = 11,
      [0b001110011] = 12,
      [0b001110101] = 13,
      [0b001110110] = 14,
      [0b001111001] = 15,
      [0b001111010] = 16,
      [0b001111100] = 17,
      [0b010001111] = 9,
      [0b010010111] = 15,
      [0b010011011] = 3,
      [0b010011101] = 12,
      [0b010011110] = 16,
      [0b010100111] = 9,
      [0b010101011] = 18,
      [0b010101101] = 19,
      [0b010101110] = 18,
      [0b010110011] = 16,
      [0b010110101] = 12,
      [0b010110110] = 3,
      [0b010111001] = 16,
      [0b010111010] = 20,
      [0b010111100] = 16,
      [0b011000111] = 10,
      [0b011001011] = 2,
      [0b011001101] = 8,
      [0b011001110] = 11,
      [0b011010011] = 4,
      [0b011010101] = 13,
      [0b011010110] = 17,
      [0b011011001] = 1,
      [0b011011010] = 3,
      [0b011011100] = 14,
      [0b011100011] = 19,
      [0b011100101] = 21,
      [0b011100110] = 11,
      [0b011101001] = 9,
      [0b011101010] = 18,
      [0b011101100] = 11,
      [0b011110001] = 12,
      [0b011110010] = 16,
      [0b011110100] = 7,
      [0b011111000] = 3,
      [0b100001111] = 8,
      [0b100010111] = 6,
      [0b100011011] = 14,
      [0b100011101] = 13,
      [0b100011110] = 12,
      [0b100100111] = 5,
      [0b100101011] = 11,
      [0b100101101] = 10,
      [0b100101110] = 9,
      [0b100110011] = 7,
      [0b100110101] = 6,
      [0b100110110] = 1,
      [0b100111001] = 17,
      [0b100111010] = 16,
      [0b100111100] = 15,
      [0b101000111] = 22,
      [0b101001011] = 8,
      [0b101001101] = 22,
      [0b101001110] = 21,
      [0b101010011] = 13,
      [0b101010101] = 23,
      [0b101010110] = 13,
      [0b101011001] = 6,
      [0b101011010] = 12,
      [0b101011100] = 13,
      [0b101100011] = 21,
      [0b101100101] = 22,
      [0b101100110] = 8,
      [0b101101001] = 10,
      [0b101101010] = 19,
      [0b101101100] = 10,
      [0b101110001] = 13,
      [0b101110010] = 12,
      [0b101110100] = 6,
      [0b101111000] = 4,
      [0b110000111] = 10,
      [0b110001011] = 11,
      [0b110001101] = 21,
      [0b110001110] = 19,
      [0b110010011] = 17,
      [0b110010101] = 13,
      [0b110010110] = 4,
      [0b110011001] = 7,
      [0b110011010] = 16,
      [0b110011100] = 12,
      [0b110100011] = 11,
      [0b110100101] = 8,
      [0b110100110] = 2,
      [0b110101001] = 11,
      [0b110101010] = 18,
      [0b110101100] = 9,
      [0b110110001] = 14,
      [0b110110010] = 3,
      [0b110110100] = 1,
      [0b110111000] = 3,
      [0b111000011] = 10,
      [0b111000101] = 22,
      [0b111000110] = 10,
      [0b111001001] = 5,
      [0b111001010] = 9,
      [0b111001100] = 8,
      [0b111010001] = 6,
      [0b111010010] = 15,
      [0b111010100] = 6,
      [0b111011000] = 1,
      [0b111100001] = 8,
      [0b111100010] = 9,
      [0b111100100] = 5,
      [0b111101000] = 2,
      [0b111110000] = 1,

      /* six squares */
      [0b000111111] = 1,
      [0b001011111] = 2,
      [0b001101111] = 3,
      [0b001110111] = 4,
      [0b001111011] = 5,
      [0b001111101] = 6,
      [0b001111110] = 7,
      [0b010011111] = 5,
      [0b010101111] = 8,
      [0b010110111] = 5,
      [0b010111011] = 9,
      [0b010111101] = 10,
      [0b010111110] = 9,
      [0b011001111] = 3,
      [0b011010111] = 6,
      [0b011011011] = 1,
      [0b011011101] = 4,
      [0b011011110] = 7,
      [0b011100111] = 11,
      [0b011101011] = 8,
      [0b011101101] = 11,
      [0b011101110] = 12,
      [0b011110011] = 10,
      [0b011110101] = 13,
      [0b011110110] = 7,
      [0b011111001] = 5,
      [0b011111010] = 9,
      [0b011111100] = 7,
      [0b100011111] = 4,
      [0b100101111] = 3,
      [0b100110111] = 2,
      [0b100111011] = 7,
      [0b100111101] = 6,
      [0b100111110] = 5,
      [0b101001111] = 14,
      [0b101010111] = 15,
      [0b101011011] = 4,
      [0b101011101] = 15,
      [0b101011110] = 13,
      [0b101100111] = 14,
      [0b101101011] = 11,
      [0b101101101] = 16,
      [0b101101110] = 11,
      [0b101110011] = 13,
      [0b101110101] = 15,
      [0b101110110] = 4,
      [0b101111001] = 6,
      [0b101111010] = 10,
      [0b101111100] = 6,
      [0b110001111] = 11,
      [0b110010111] = 6,
      [0b110011011] = 7,
      [0b110011101] = 13,
      [0b110011110] = 10,
      [0b110100111] = 3,
      [0b110101011] = 12,
      [0b110101101] = 11,
      [0b110101110] = 8,
      [0b110110011] = 7,
      [0b110110101] = 4,
      [0b110110110] = 1,
      [0b110111001] = 7,
      [0b110111010] = 9,
      [0b110111100] = 5,
      [0b111000111] = 16,
      [0b111001011] = 3,
      [0b111001101] = 14,
      [0b111001110] = 11,
      [0b111010011] = 6,
      [0b111010101] = 15,
      [0b111010110] = 6,
      [0b111011001] = 2,
      [0b111011010] = 5,
      [0b111011100] = 4,
      [0b111100011] = 11,
      [0b111100101] = 14,
      [0b111100110] = 3,
      [0b111101001] = 3,
      [0b111101010] = 8,
      [0b111101100] = 3,
      [0b111110001] = 4,
      [0b111110010] = 5,
      [0b111110100] = 2,
      [0b111111000] = 1,

      /* seven squares */
      [0b001111111] = 1,
      [0b010111111] = 2,
      [0b011011111] = 1,
      [0b011101111] = 3,
      [0b011110111] = 4,
      [0b011111011] = 2,
      [0b011111101] = 4,
      [0b011111110] = 5,
      [0b100111111] = 1,
      [0b101011111] = 6,
      [0b101101111] = 7,
      [0b101110111] = 6,
      [0b101111011] = 4,
      [0b101111101] = 8,
      [0b101111110] = 4,
      [0b110011111] = 4,
      [0b110101111] = 3,
      [0b110110111] = 1,
      [0b110111011] = 5,
      [0b110111101] = 4,
      [0b110111110] = 2,
      [0b111001111] = 7,
      [0b111010111] = 8,
      [0b111011011] = 1,
      [0b111011101] = 6,
      [0b111011110] = 4,
      [0b111100111] = 7,
      [0b111101011] = 3,
      [0b111101101] = 7,
      [0b111101110] = 3,
      [0b111110011] = 4,
      [0b111110101] = 6,
      [0b111110110] = 1,
      [0b111111001] = 1,
      [0b111111010] = 2,
      [0b111111100] = 1,

      /* eight squares */
      [0b011111111] = 1,
      [0b101111111] = 2,
      [0b110111111] = 1,
      [0b111011111] = 2,
      [0b111101111] = 3,
      [0b111110111] = 2,
      [0b111111011] = 1,
      [0b111111101] = 2,
      [0b111111110] = 1,

      /* nine squares */
      [0b111111111] = 1,
    };
  return types[signature];
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_type_square_t *app = (struct fv_app_type_square_t *) state->input;
  int type, square;
  switch (key)
    {
    case 'i':
      app->read_square = binary_read_square_from_stream;
      break;
    case 'o':
      app->display_square = display_binary_square_record;
      break;
    case 'f':
      if (sscanf (arg, "%d:%d", &square, &type) != 2)
        argp_error (state, "bad format in -f.  "
                    "\"expecting NUM_SQUARES:TYPE\".");
      else
        if (square < 1 || square > 9)
          argp_error (state, "number of squares must be between 1 and 9.");
        else if (type < 1 || type > 23)
          argp_error (state, "type must be between 1 and 23.");
        else
          {
            app->filter_types =
              realloc (app->filter_types, sizeof (int) * (app->num_filters + 1));
            app->filter_squares =
              realloc (app->filter_squares, sizeof (int) * (app->num_filters + 1));
            app->filter_squares[app->num_filters] = square;
            app->filter_types[app->num_filters] = type;
            app->num_filters++;
          }
      break;
    case 'n':
      app->show_num_squares = 0;
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
  { "-no-show-squares", 'n', 0, 0, "Just show the type, and not the number of squares."},
  { "filter", 'f', "NUM_SQUARES:TYPE", 0, "Show the magic squares that have this number of perfect squares, and type."},
  { 0 }
};

static struct argp argp ={options, parse_opt, 0, "Accept 3x3 magic squares from the standard input, and determine which configuration it is.\vThe nine values must be separated by commas and terminated by a newline. --out-binary is only used with --filter.", 0};

int
fituvalu_type_square (struct fv_app_type_square_t *app,  FILE *in, FILE *out)
{
  int i, j;
  mpz_t a[3][3];
  ssize_t read;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      mpz_init (a[i][j]);

  char* line = NULL;
  size_t len = 0;
  while (!feof (in))
    {
      read = app->read_square (in, &a, &line, &len);
      if (read == -1)
        break;

      int signature = 0;
      int num_squares = 0;
      for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
          {
            signature <<= 1;
            if (mpz_perfect_square_p (a[i][j]))
              {
                num_squares++;
                signature |=  0x1;
              }
          }
      int type = get_type_from_signature (signature, num_squares);
      if (!app->num_filters)
        {
          if (app->show_num_squares)
            fprintf (out, "%d:%d\n", num_squares, type);
          else
            fprintf (out, "%d\n", type);
        }
      else
        {
          for (int i = 0; i < app->num_filters; i++)
            {
              if (app->filter_squares[i] == num_squares &&
                  app->filter_types[i] == type)
                {
                  app->display_square (a, out);
                  break;
                }
            }
        }

    }
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      mpz_clear (a[i][j]);
  if (line)
    free (line);
  return 0;
}

int
main (int argc, char **argv)
{
  struct fv_app_type_square_t app;
  memset (&app, 0, sizeof (app));
  app.read_square = read_square_from_stream;
  app.display_square = display_square_record;
  app.show_num_squares = 1;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_type_square (&app, stdin, stdout);
}
