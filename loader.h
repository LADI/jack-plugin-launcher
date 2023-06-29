/* -*- Mode: C ; c-basic-offset: 2 -*- */
/*
 * SPDX-FileCopyrightText: Copyright © 2009-2023 Nedko Arnaudov
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 **************************************************************************
 * This file contains interface to the code that starts programs
 **************************************************************************
 */

#ifndef __LASHD_LOADER_H__
#define __LASHD_LOADER_H__

void loader_init(void (* on_child_exit)(pid_t pid, int exit_status));

bool
loader_execute(
  //const char * vgraph_name,
  //const char * project_name,
  //const char * app_name,
  const char * working_dir,
  //const char * session_dir,
  bool run_in_terminal,
  const char * commandline,
  //bool set_env_vars,
  pid_t * pid_ptr);

void loader_run(void);

void loader_uninit(void);

unsigned int loader_get_app_count(void);

#endif /* __LASHD_LOADER_H__ */
