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
#ifndef MAGIC_SQUARE_UTIL_H
#define MAGIC_SQUARE_UTIL_H
#include <stdio.h>
#include <gmp.h>
#define SIZE 9
void display_nine_record (mpz_t *progression, FILE *out);
void display_three_record (mpz_t *progression, FILE *out);
void display_three_record_with_root (mpz_t *progression, mpz_t *root, FILE *out);
void display_binary_three_record_with_root (mpz_t *progression, mpz_t *root, FILE *out);
void display_four_record (mpz_t *progression, FILE *out);
void display_binary_four_record (mpz_t *progression, FILE *out);
void display_binary_nine_record (mpz_t *progression, FILE *out);
void display_binary_two_record (mpz_t *one, mpz_t *two, FILE *out);
void display_two_record (mpz_t *one, mpz_t *two, FILE *out);
void display_square_record (mpz_t s[3][3], FILE *out);
void display_binary_square_record (mpz_t s[3][3], FILE *out);
void is_magic_square_init ();
int is_magic_square (mpz_t a[3][3], int diag);
int count_squares (mpz_t a[3][3]);
int read_square_from_stream (FILE *stream, mpz_t (*a)[3][3], char **line, size_t *len);
int binary_read_square_from_stream (FILE *stream, mpz_t (*a)[3][3], char **s, size_t *len);
int read_numbers_from_stream (FILE *stream, mpz_t *a, int size, char **line, size_t *len);
int binary_read_numbers_from_stream (FILE *stream, mpz_t *a, int size, char **line, size_t *len);
void small_read_square_and_run (FILE *, void (*)(unsigned long long, unsigned long long, unsigned long long, void (*)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE*), FILE *), void (*)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), unsigned long long, unsigned long long, FILE *);
void read_square_and_run (FILE *, void (*)(mpz_t,mpz_t, mpz_t, unsigned long long, void (*)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE *), FILE *), void (*)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE *), mpz_t, mpz_t, unsigned long long, FILE *);
void binary_read_square_and_run (FILE *, void (*iterfunc)(mpz_t, mpz_t, mpz_t, unsigned long long, void (*)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *), void (*check)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE*), mpz_t, mpz_t, unsigned long long, FILE *);
void loop_and_run (void (*)(mpz_t, mpz_t, mpz_t, unsigned long long, void (*)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE *), FILE *), void (*)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE *), mpz_t, mpz_t, unsigned long long, FILE *);
void small_loop_and_run (void (*func)(unsigned long long, unsigned long long, unsigned long long, void (*)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), FILE *), void (*)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), unsigned long long start, unsigned long long finish, FILE *out);
int small_is_square (long long num);

void fwd_4sq_progression1 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE *), FILE *);
void fwd_4sq_progression2 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE *), FILE *);
void fwd_4sq_progression3 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE *), FILE *);
void fwd_4sq_progression4 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE *), FILE *);
void fwd_4sq_progression5 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE *), FILE *out);
void fwd_4sq_progression6 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE *), FILE *out);
void rev_4sq_progression1 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE *), FILE *);
void optimized_fwd_4sq_progression1 (mpz_t i, mpz_t start, mpz_t finish, unsigned long long incr, void (*func)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE *), FILE *out);

void small_fwd_4sq_progression1 (unsigned long long i, unsigned long long start, unsigned long long finish, void (*func)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), FILE *);
void small_fwd_4sq_progression2 (unsigned long long i, unsigned long long start, unsigned long long finish, void (*func)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), FILE *);
void small_fwd_4sq_progression3 (unsigned long long i, unsigned long long start, unsigned long long finish, void (*func)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), FILE *);
void small_fwd_4sq_progression4 (unsigned long long i, unsigned long long start, unsigned long long finish, void (*func)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), FILE *);
void small_fwd_4sq_progression5 (unsigned long long i, unsigned long long start, unsigned long long finish, void (*func)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), FILE *);
void small_fwd_4sq_progression6 (unsigned long long i, unsigned long long start, unsigned long long finish, void (*func)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), FILE *);
void small_rev_4sq_progression1 (unsigned long long i, unsigned long long start, unsigned long long finish, void (*func)(unsigned long long *, unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE *), FILE *);

void four_square_to_nine_number_step_progression1 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four);
void four_square_to_nine_number_step_progression2 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four);
void four_square_to_nine_number_step_progression3 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four);
void four_square_to_nine_number_step_progression4 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four);
void four_square_to_nine_number_reverse_step_progression1 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four);
void four_square_to_nine_number_fulcrum_progression1 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four);
void four_square_to_nine_number_fulcrum_progression2 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four);
void four_square_to_nine_number_fulcrum_progression3 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four);
void four_square_to_nine_number_fulcrum_progression4 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four);
void four_square_to_nine_number_fulcrum_progression5 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four);
void four_square_to_nine_number_reverse_fulcrum_progression1 (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four);

int count_squares_in_step_progression1 (mpz_t *progression);
int count_squares_in_step_progression2 (mpz_t *progression);
int count_squares_in_step_progression3 (mpz_t *progression);
int count_squares_in_step_progression4 (mpz_t *progression);
int count_squares_in_reverse_step_progression1 (mpz_t *progression);
int count_squares_in_fulcrum_progression1 (mpz_t *progression);
int count_squares_in_fulcrum_progression2 (mpz_t *progression);
int count_squares_in_fulcrum_progression3 (mpz_t *progression);
int count_squares_in_fulcrum_progression4 (mpz_t *progression);
int count_squares_in_fulcrum_progression5 (mpz_t *progression);
int count_squares_in_reverse_fulcrum_progression1 (mpz_t *progression);

typedef enum four_square_progression_enum_t {
  FWD_PROGRESSION_1,
  FWD_PROGRESSION_2,
  FWD_PROGRESSION_3,
  FWD_PROGRESSION_4,
  FWD_PROGRESSION_5,
  FWD_PROGRESSION_6,
  REV_PROGRESSION_1,
}four_square_progression_enum_t;

typedef struct four_square_progression_t {
  four_square_progression_enum_t kind;
  char *name;
  void (*func) (mpz_t, mpz_t, mpz_t, unsigned long long, void (*)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, FILE *), FILE *);
  char *timeline;
} four_square_progression_t;

char * generate_list_of_four_square_progression_types ();
char * generate_list_of_four_square_progression_timelines ();
four_square_progression_t * lookup_four_square_progression_by_name (char *);
four_square_progression_t * lookup_four_square_progression_by_kind (four_square_progression_enum_t);

typedef struct nine_progression_t {
  char *name;
  four_square_progression_enum_t four_square_progression;
  void (*progfunc) (mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t);
  int (*countfunc)(mpz_t *);
  char *timeline;
} nine_progression_t;

char * generate_list_of_nine_progression_types ();
nine_progression_t * lookup_nine_progression_by_name (char *name);
char * generate_list_of_nine_progression_timelines ();
extern four_square_progression_t four_square_progressions[];
extern nine_progression_t nine_progressions[];

int small_morgenstern_search (unsigned long long max, FILE *in, void (*search) (unsigned long long, unsigned long long, unsigned long long, unsigned long long, FILE*), FILE *out);
int morgenstern_search (mpz_t max, FILE *in, void (*search) (mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *out);
int morgenstern_search_from_binary (mpz_t max, FILE *in, void (*search) (mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *out);
void reduce_three_square_progression (mpz_t *progression);
int binary_read_three_numbers_from_stream (FILE *stream, mpz_t *a, char **line, size_t *len);
int read_three_numbers_from_stream (FILE *stream, mpz_t *a, char **line, size_t *len);
int read_four_numbers_from_stream (FILE *stream, mpz_t *a, char **line, size_t *len);
int binary_read_four_numbers_from_stream (FILE *stream, mpz_t *a, char **line, size_t *len);
void display_binary_three_record (mpz_t *progression, FILE *out);
void morgenstern_search_dual (FILE *in1, FILE *in2, void (*search) (mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *out);
void morgenstern_search_dual_binary (FILE *in1, FILE *in2, void (*search) (mpz_t, mpz_t, mpz_t, mpz_t, FILE*), FILE *out);
void display_textual_number (mpz_t *i, FILE *out);
void display_binary_number (mpz_t *i, FILE *out);
void disp_binary_record (mpz_t *vec, int size, FILE *out);
void disp_record (mpz_t *vec, int size, FILE *out);
int fv_getline (char **line, size_t *len, FILE *stream);
int fv_getdelim (char **line, size_t *len, int delim, FILE *stream);
void reduce_square (mpz_t a[3][3]);
#endif
