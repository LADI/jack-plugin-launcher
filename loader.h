/* -*- Mode: C ; c-basic-offset: 2 -*- */
/*
 * SPDX-FileCopyrightText: Copyright © 2009-2023 Nedko Arnaudov
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 **************************************************************************
 * This file contains interface to the code that starts programs
 **************************************************************************
 */

#ifndef JPL_H__6D6AF97A_DA41_46D6_954B_A121B25E96CE__INCLUDED
#define JPL_H__6D6AF97A_DA41_46D6_954B_A121B25E96CE__INCLUDED

/** Handle to object describing child */
typedef struct jpl_child_tag { int unused; } * jpl_child_handle;

typedef
void
(* jpl_on_child_exit)(
  void * ctx,
  int exit_status);

typedef
void
(* jpl_log_callback)(
  void * ctx,
  bool error,
  const char * format,
  ...)
  __attribute__((format(printf, 3, 4)));

void
jpl_init(
  jpl_on_child_exit on_child_exit,
  jpl_log_callback log_callback);

bool
jpl_execute(
  void * ctx,
  const char * working_dir,
  bool run_in_terminal,
  const char * commandline,
  const char * const * env_vars,
  const char * const * ldpreload,
  pid_t * pid_ptr,
  jpl_child_handle * child_handle_ptr);

void
jpl_run(
  void);

void
jpl_uninit(
  void);

unsigned int
jpl_get_app_count(
  void);

#endif /* #ifndef JPL_H__6D6AF97A_DA41_46D6_954B_A121B25E96CE__INCLUDED */
