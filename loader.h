/* -*- Mode: C ; c-basic-offset: 2 -*- */
/*
 * SPDX-FileCopyrightText: Copyright © 2009-2023 Nedko Arnaudov
 * SPDX-License-Identifier: GPL-2.0-or-later */
/**
 **************************************************************************
 * @file loader.h
 * @brief Interface to the code that starts programs (launcher module)
 **************************************************************************/

#ifndef JPL_H__6D6AF97A_DA41_46D6_954B_A121B25E96CE__INCLUDED
#define JPL_H__6D6AF97A_DA41_46D6_954B_A121B25E96CE__INCLUDED

/** Handle to object describing child */
typedef struct jpl_child_tag
{
  int unused; /**< opaque handle quazi-member */
} * jpl_child_handle;

/**
 * @brief function type for callback called on child exit
 *
 * @param ctx context pointer with value as supplied by call to jpl_execute()
 * @param exit_status Exit status of the child process
 */
typedef
void
(* jpl_on_child_exit)(
  void * ctx,
  int exit_status);

/**
 * @brief function type for callback called on child exit
 *
 * @param ctx context pointer with value as supplied by call to jpl_execute()
 * @param error whether line from stderr (true) is being logged or from stdout (false)
 * @param line char string of line being logged
 */
typedef
void
(* jpl_log_callback)(
  void * ctx,
  bool error,
  const char * line);

/**
 * @brief initialize the launcher module
 *
 * @param on_child_exit function to call back when child exits
 * @param log_callback function to call back child produces a line
 */
void
jpl_init(
  jpl_on_child_exit on_child_exit,
  jpl_log_callback log_callback);

/**
 * @brief schedule child program for execution
 *
 * @param ctx context pointer that will be passed as parameter when functions,
 *        previously specified int the jpl_init() call, are called.
 * @param working_dir working dir where the program is started
 * @param run_in_terminal whether to run the child program in a new terminal
 * @param commandline command-line to use for starting the child process
 * @param env_vars optional (can be NULL) array of strings for
 *        environment variables to be set in the child process
 *        the array is terminated by empty string element.
 * @param ldpreload optional (can be NULL) array of strings for LD_PRELOAD-ed libraries.
 *        the array is terminated by empty string element.
 * @param pid_ptr pointer to variable where, upon successful return,
 *        the child process identifier value will be stored.
 * @param child_handle_ptr pointer to variable where, upon successful return,
 *        child handle is stored.
 *
 * @return success status
 */
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

/**
 * @brief Read subprocess output and call the callbacks
 */
void
jpl_run(
  void);

/**
 * uninitialize the launcher module
 */
void
jpl_uninit(
  void);

/**
 * get number of child processes
 */
unsigned int
jpl_get_app_count(
  void);

#endif /* #ifndef JPL_H__6D6AF97A_DA41_46D6_954B_A121B25E96CE__INCLUDED */
