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
#include <argp.h>
#include <gmp.h>
#include "magicsquareutil.h"

struct fv_gen_progression_t
{
  unsigned long long incr;
  int in_binary;
  four_square_progression_t *force_four_square_prog;
  nine_progression_t *nine_prog;
  four_square_progression_t *four_square_prog;
  void (*display_record) (mpz_t *progression, FILE *out);
  void (*four_to_nine_prog)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t);
  int (*count_prog) (mpz_t *);
  int filter;
  mpz_t start, finish, oneshot;
  int num_args;
  FILE *out;
};

static void
extend_and_display_progression (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four, void *data)
{
  struct fv_gen_progression_t *app = (struct fv_gen_progression_t *) data;
  app->four_to_nine_prog (progression, one, two, three, four);
  app->display_record (progression, app->out);
}

static void
extend_and_count_squares_and_display_progression (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four, void *data)
{
  struct fv_gen_progression_t *app = (struct fv_gen_progression_t *) data;
  app->four_to_nine_prog (progression, one, two, three, four);
  int count = app->count_prog (progression);
  if (count >= app->filter)
    app->display_record (progression, app->out);
}

static void
generate_progression_starting_at (mpz_t i, struct fv_gen_progression_t *app)
{
  static void (*func) (mpz_t *, mpz_t one, mpz_t two, mpz_t three, mpz_t four, void *);
  if (app->filter)
    func = extend_and_count_squares_and_display_progression;
  else
    func = extend_and_display_progression;
  void (*progression) (mpz_t, mpz_t, mpz_t, unsigned long long, void (*)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, void *), void *) = app->four_square_prog->func;
  progression (i, i, app->finish, app->incr, func, app);
}

static void
generate_progression_from_input (struct fv_gen_progression_t *app, FILE *in)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t i;
  mpz_init (i);
  static void (*func) (mpz_t *, mpz_t one, mpz_t two, mpz_t three, mpz_t four, void *);
  if (app->filter)
    func = extend_and_count_squares_and_display_progression;
  else
    func = extend_and_display_progression;
  
  void (*progression) (mpz_t, mpz_t, mpz_t, unsigned long long, void (*)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, void *), void *) = app->four_square_prog->func;
  while (1)
    {
      read = fv_getline (&line, &len, in);
      if (read == -1)
        break;
      char *end = strchr (line, '\n');
      if (end)
        *end = '\0';
      mpz_set_str (i, line, 10);

      progression (i, i, app->finish, app->incr, func, app);
    }
  mpz_clear (i);
  free (line);
}

static void
generate_progression_from_binary_input (struct fv_gen_progression_t *app, FILE *in)
{
  ssize_t read;
  mpz_t i;
  mpz_init (i);
  static void (*func) (mpz_t *, mpz_t one, mpz_t two, mpz_t three, mpz_t four, void *);
  if (app->filter)
    func = extend_and_count_squares_and_display_progression;
  else
    func = extend_and_display_progression;
  
  void (*progression) (mpz_t, mpz_t, mpz_t, unsigned long long, void (*)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, void *), void *) = app->four_square_prog->func;
  while (1)
    {
      read = mpz_inp_raw (i, in);
      if (!read)
        break;
      progression (i, i, app->finish, app->incr, func, app);
    }
  mpz_clear (i);
}

static void
generate_progression (struct fv_gen_progression_t *app)
{
  static void (*func) (mpz_t *, mpz_t one, mpz_t two, mpz_t three, mpz_t four, void *);
  if (app->filter)
    func = extend_and_count_squares_and_display_progression;
  else
    func = extend_and_display_progression;
  void (*progression) (mpz_t, mpz_t, mpz_t, unsigned long long, void (*)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, void *), void *) = app->four_square_prog->func;
  mpz_t root, i;
  mpz_inits (root, i, NULL);
  mpz_set (i, app->start);
  mpz_sqrt (root, i);
  mpz_mul (i, root, root);
  progression (i, app->start, app->finish, app->incr, func, app);
  while (1)
    {
      for (int j = 0; j < app->incr; j++)
        {
          mpz_add (i, i, root);
          mpz_add (i, i, root);
          mpz_add_ui (i, i, 1);
          mpz_add_ui (root, root, 1);
        }
      if (mpz_cmp (i, app->finish) >= 0)
        break;
      progression (i, app->start, app->finish, app->incr, func, app);
    }
  mpz_clears (root, i, NULL);
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_gen_progression_t *app = (struct fv_gen_progression_t *) state->input;
  char *end = NULL;
  switch (key)
    {
    case 'I':
      app->incr = strtoull (arg, &end, 10);
      break;
    case '1':
      mpz_set_str (app->oneshot, arg, 10);
      break;
    case 'o':
      app->display_record = display_binary_nine_record;
      break;
    case 'i':
      app->in_binary = 1;
      break;
    case '4':
        {
          four_square_progression_t *p =
            lookup_four_square_progression_by_name (arg);
          if (p)
            app->force_four_square_prog = p;
          else
            {
              char *types = generate_list_of_four_square_progression_types ();
              argp_error (state, "--force-4sq must be one of %s.", types);
              free (types);
            }
        }
      break;
    case 'f':
      app->filter = atoi (arg);
      break;
    case 't':
        {
          nine_progression_t *p =
            lookup_nine_progression_by_name (arg);
          if (p)
            app->nine_prog = p;
          else
            {
              char *types = generate_list_of_nine_progression_types ();
              argp_error (state, "--type must be one of %s.", types);
              free (types);
            }
        }
      break;
    case ARGP_KEY_ARG:
      if (app->num_args == 2)
        argp_error (state, "too many arguments.");
      else
        {
          if (app->num_args == 0)
            mpz_init_set_str (app->finish, arg, 10);
          else
            {
              mpz_init_set (app->start, app->finish);
              mpz_init_set_str (app->finish, arg, 10);
            }
          app->num_args++;
        }
      break;
    case ARGP_KEY_INIT:
      setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
      break;
    case ARGP_KEY_FINI:
      if (app->num_args == 0)
        argp_error (state, "too few arguments.");

      app->four_to_nine_prog = app->nine_prog->progfunc;
      app->count_prog = app->nine_prog->countfunc;
      app->four_square_prog =
        lookup_four_square_progression_by_kind
        (app->nine_prog->four_square_progression);
      if (app->force_four_square_prog)
        {
          if (app->force_four_square_prog->kind != app->nine_prog->four_square_progression)
            argp_error (state, "forcing the wrong kind of progression.");
          else
            app->four_square_prog = app->force_four_square_prog;
        }
      break;
    }
  return 0;
}

int
fituvalu_gen_progression (struct fv_gen_progression_t *app, FILE *in)
{
  if (app->num_args == 1)
    {
      if (app->in_binary)
        generate_progression_from_binary_input (app, in);
      else
        generate_progression_from_input (app, in);
    }
  else
    {
      if (mpz_cmp_ui (app->oneshot, 0) != 0)
        generate_progression_starting_at (app->oneshot, app);
      else
        generate_progression (app);
    }
  return 0;
}

static char *
help_filter (int key, const char *text, void *input)
{
  if (key == ARGP_KEY_HELP_POST_DOC)
    {
      char *s = generate_list_of_nine_progression_timelines ();
      char *q = NULL;
      asprintf (&q, "\n%s", s);
      free (s);
      char *new_text = NULL;
      if (asprintf (&new_text, text, q) != -1)
        return new_text;
    }
  return (char *) text;
}

static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "type", 't', "NAME", 0, "Use NAME as the progression strategy"},
  { "filter", 'f', "NUM", 0, "Only show progressions that have at least NUM perfect squares" },
  { "force-4sq", '4', "NAME", 0, "Force the use of an alternative four square progression strategy"},
  { NULL, '1', "NUM", 0, "Do one iteration with NUM as first square"},
  { "increment", 'I', "NUM", 0, "Advance by NUM squares instead of 1"},
  { 0 }
};

static struct argp
argp =
{
  options, parse_opt, "START FINISH\nFINISH",
  "Generate progressions of 9 numbers that contain 4 squares or more.  If only FINISH is specified, read perfect squares from the standard input.  Continue iterating until the progression reaches FINISH.\v--type must be one of:%s",
  0,
  help_filter
};

int
main (int argc, char **argv)
{
  struct fv_gen_progression_t app;
  memset (&app, 0, sizeof (app));
  app.incr = 1;
  app.nine_prog = &nine_progressions[0];
  app.display_record = display_nine_record;
  app.out = stdout;
  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_gen_progression (&app, stdin);
}

