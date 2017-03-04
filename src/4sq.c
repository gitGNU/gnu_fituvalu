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

struct fv_app_4sq_t
{
  unsigned long long incr;
  int in_binary;
  void (*func)(mpz_t, mpz_t, mpz_t, unsigned long long, void (*)(mpz_t*, mpz_t, mpz_t, mpz_t, mpz_t, void*), void*);
  mpz_t start, finish, oneshot;
  int num_args;
  void (*display_record)(mpz_t *, mpz_t, mpz_t, mpz_t, mpz_t, void *);
  FILE *out;
};

static void
display_record (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four, void *data)
{
  struct fv_app_4sq_t *app = (struct fv_app_4sq_t *) data;
  mpz_set (progression[0], one);
  mpz_set (progression[1], two);
  mpz_set (progression[2], three);
  mpz_set (progression[3], four);
  display_four_record (progression, app->out);
}

static void
display_binary_record (mpz_t *progression, mpz_t one, mpz_t two, mpz_t three, mpz_t four, void *data)
{
  struct fv_app_4sq_t *app = (struct fv_app_4sq_t *) data;
  mpz_set (progression[0], one);
  mpz_set (progression[1], two);
  mpz_set (progression[2], three);
  mpz_set (progression[3], four);
  display_binary_four_record (progression, app->out);
}

void
generate_4sq_from_binary_input (struct fv_app_4sq_t *app, FILE *in)
{
  ssize_t read;
  mpz_t i;
  mpz_init (i);
  while (1)
    {
      read = mpz_inp_raw (i, in);
      if (!read)
        break;
      app->func (i, i, app->finish, app->incr, app->display_record, app);
    }
  mpz_clear (i);
}

void
generate_4sq_from_input (struct fv_app_4sq_t *app, FILE *in)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  mpz_t i;
  mpz_init (i);
  while (1)
    {
      read = fv_getline (&line, &len, in);
      if (read == -1)
        break;
      char *end = strchr (line, '\n');
      if (end)
        *end = '\0';
      mpz_set_str (i, line, 10);
      app->func (i, i, app->finish, app->incr, app->display_record, app);
    }
  mpz_clear (i);
  free (line);
}

void
generate_4sq (struct fv_app_4sq_t *app)
{
  mpz_t i;
  mpz_init (i);
  mpz_t root;
  mpz_inits (root, NULL);
  mpz_sqrt (root, app->start);
  mpz_mul (i, root, root);
  app->func (i, app->start, app->finish, app->incr, app->display_record, app);
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
      app->func (i, app->start, app->finish, app->incr, app->display_record, app);
    }
  mpz_clears (root, NULL);
  mpz_clear (i);
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct fv_app_4sq_t *app = (struct fv_app_4sq_t *) state->input;
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
      app->display_record = display_binary_record;
      break;
    case 'i':
      app->in_binary = 1;
      break;
    case 't':
        {
          four_square_progression_t *p =
            lookup_four_square_progression_by_name (arg);
          if (p)
            app->func = p->func;
          else
            {
              char *types = generate_list_of_four_square_progression_types ();
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
      mpz_init (app->start);
      mpz_init (app->finish);
      app->display_record = display_record;
      setenv ("ARGP_HELP_FMT", "no-dup-args-note", 1);
      break;
    case ARGP_KEY_FINI:
      if (app->num_args == 0)
        argp_error (state, "too few arguments.");
      break;
    }
  return 0;
}

static char *
help_filter (int key, const char *text, void *input)
{
  if (key == ARGP_KEY_HELP_POST_DOC)
    {
      char *s = generate_list_of_four_square_progression_timelines ();
      char *q = NULL;
      asprintf (&q, "\n%s", s);
      free (s);
      char *new_text = NULL;
      if (asprintf (&new_text, text, q) != -1)
        return new_text;
    }
  return (char *) text;
}

int
fituvalu_4sq (struct fv_app_4sq_t *app, FILE *in)
{
  if (app->num_args == 1)
    {
      if (app->in_binary)
        generate_4sq_from_binary_input (app, in);
      else
        generate_4sq_from_input (app, in);
    }
  else
    {
      if (mpz_cmp_ui (app->oneshot, 0) != 0)
        app->func (app->oneshot, app->start, app->finish, app->incr, app->display_record, app);
      else
        generate_4sq (app);
    }
  return 0;
}


static struct argp_option
options[] =
{
  { "in-binary", 'i', 0, 0, "Input raw GMP numbers instead of text"},
  { "out-binary", 'o', 0, 0, "Output raw GMP numbers instead of text"},
  { "type", 't', "NAME", 0, "Use NAME as the progression strategy"},
  { NULL, '1', "NUM", 0, "Do one iteration with NUM as first square"},
  { "increment", 'I', "NUM", 0, "Advance by NUM squares instead of 1"},
  { 0 }
};

static struct argp
argp =
{
  options, parse_opt, "START FINISH\nFINISH",
  "Generate progressions of 4 perfect squares.  If only FINISH is specified, read the starting perfect square from the standard input.  Continue iterating until the progression reaches FINISH.\vAn arithmetic progression of 4 squares in a row is impossible (e.g. the distance between the squares is the same), so we iterate over 4 squares with a different gap between one or more of the squares.  --type must be one of:%s",
  0,
  help_filter
};

int
main (int argc, char **argv)
{
  struct fv_app_4sq_t app;
  memset (&app, 0, sizeof (app));
  app.incr = 1;
  app.func = fwd_4sq_progression1;
  app.out = stdout;

  argp_parse (&argp, argc, argv, 0, 0, &app);
  return fituvalu_4sq (&app, stdin);
}

