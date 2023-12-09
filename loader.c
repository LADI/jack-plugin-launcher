/* -*- Mode: C ; c-basic-offset: 2 -*- */
/*
 * SPDX-FileCopyrightText: Copyright © 2008-2023 Nedko Arnaudov
 * SPDX-FileCopyrightText: Copyright © 2008 Juuso Alasuutari <juuso.alasuutari@gmail.com>
 * SPDX-FileCopyrightText: Copyright © 2002 Robert Ham <rah@bash.sh>
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 **************************************************************************
 * This file contains code that starts programs
 **************************************************************************
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pty.h>                /* forkpty() */
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "loader.h"
#include "klist.h"

#define XTERM_COMMAND_EXTENSION "&& sh || sh"

#define CLIENT_OUTPUT_BUFFER_SIZE 2048

#include <assert.h>
#define ASSERT assert

#include "catdup.h"

struct jpl_child
{
  struct list_head  siblings;

  void * ctx;

  bool dead;
  int exit_status;
  pid_t pid;

  bool terminal;

  int stdout;
  char stdout_buffer[CLIENT_OUTPUT_BUFFER_SIZE];
  char stdout_last_line[CLIENT_OUTPUT_BUFFER_SIZE];
  unsigned int stdout_last_line_repeat_count;
  char * stdout_buffer_ptr;

  int stderr;
  char stderr_buffer[CLIENT_OUTPUT_BUFFER_SIZE];
  char stderr_last_line[CLIENT_OUTPUT_BUFFER_SIZE];
  unsigned int stderr_last_line_repeat_count;
  char * stderr_buffer_ptr;
};

static jpl_on_child_exit jpl_g_on_child_exit;
static jpl_log_callback jpl_g_on_log;
static struct list_head jpl_g_children;

#define jpl_log(format, args...) jpl_g_on_log(NULL, false, format, ## args);
#define jpl_error(format, args...) jpl_g_on_log(NULL, true, format, ## args);

#define jpl_child_log(ctx, format, args...) jpl_g_on_log(ctx, false, format, ## args);
#define jpl_child_error(ctx, format, args...) jpl_g_on_log(ctx, true, format, ## args);

static
struct jpl_child *
jpl_child_find(
  pid_t pid)
{
  struct list_head *node_ptr;
  struct jpl_child *child_ptr;

  list_for_each (node_ptr, &jpl_g_children)
  {
    child_ptr = list_entry(node_ptr, struct jpl_child, siblings);
    if (child_ptr->pid == pid)
    {
      return child_ptr;
    }
  }

  return NULL;
}

static
void
jpl_check_line_repeat_end(
  struct jpl_child * child_ptr,
  bool error,
  unsigned int last_line_repeat_count)
{
  if (last_line_repeat_count >= 2)
  {
    jpl_g_on_log(
      child_ptr->ctx,
      error,
      "%s line repeated %u times",
      error ? "stderr" : "stdout",
      last_line_repeat_count);
  }
}

static
void
jpl_childs_bury(
  void)
{
  struct list_head *node_ptr;
  struct list_head *next_ptr;
  struct jpl_child *child_ptr;

  list_for_each_safe (node_ptr, next_ptr, &jpl_g_children)
  {
    child_ptr = list_entry(node_ptr, struct jpl_child, siblings);
    if (child_ptr->dead)
    {
      jpl_check_line_repeat_end(
        child_ptr,
        false,
        child_ptr->stdout_last_line_repeat_count);

      jpl_check_line_repeat_end(
        child_ptr,
        true,
        child_ptr->stderr_last_line_repeat_count);

      list_del(&child_ptr->siblings);

      if (!child_ptr->terminal)
      {
        close(child_ptr->stdout);
        close(child_ptr->stderr);
      }

      jpl_g_on_child_exit(child_ptr->ctx, child_ptr->exit_status);
      free(child_ptr);
    }
  }
}

static
void
jpl_sigchld_handler(
  int signum)
{
  int status;
  pid_t pid;
  struct jpl_child *child_ptr;
  int signal;

  ASSERT(signum == SIGCHLD);

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
  {
    child_ptr = jpl_child_find(pid);

    if (!child_ptr)
    {
      jpl_error("Termination of unknown child process with PID %llu detected", (unsigned long long)pid);
    }
    else
    {
      jpl_log("Termination of child process with PID %llu detected", (unsigned long long)pid);
      child_ptr->dead = true;
      child_ptr->exit_status = status;
    }

    if (WIFEXITED(status))
    {
      jpl_log("Child exited, code=%d", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
      signal = WTERMSIG(status);
      switch (signal)
      {
      case SIGILL:
      case SIGABRT:
      case SIGSEGV:
      case SIGFPE:
        jpl_error("Child was killed by signal %d", signal);
        break;
      default:
        jpl_log("Child was killed by signal %d", signal);
      }
    }
    else if (WIFSTOPPED(status))
    {
      jpl_log("Child was stopped by signal %d", WSTOPSIG(status));
    }
  }
}

void
jpl_init(
  jpl_on_child_exit on_child_exit,
  jpl_log_callback log_callback)
{
  jpl_g_on_child_exit = on_child_exit;
  jpl_g_on_log = log_callback,
  signal(SIGCHLD, jpl_sigchld_handler);
  INIT_LIST_HEAD(&jpl_g_children);
}

void
jpl_uninit(
  void)
{
  jpl_childs_bury();
}

#if 0
static void jpl_exec_program_in_xterm(const char * const * argv)
{
  char * dst_ptr;
  const char * const * src_ptr_ptr;
  size_t len;

  log_debug("Executing program '%s' with PID %llu in terminal", argv[0], (unsigned long long)getpid());

  /* Calculate the command string length */
  len = strlen(XTERM_COMMAND_EXTENSION) + 1;
  for (src_ptr_ptr = argv; *src_ptr_ptr != NULL; src_ptr_ptr++)
  {
    len += strlen(*src_ptr_ptr) + 3; /* three additional chars per argument: two double quotes and a space */
  }

  char buf[len];                /* dynamically allocate in stack */

  /* Create the command string */
  src_ptr_ptr = argv;
  dst_ptr = buf;
  while (*src_ptr_ptr != NULL)
  {
    len = strlen(*src_ptr_ptr);
    dst_ptr[0] = '"';
    memcpy(dst_ptr + 1, src_ptr_ptr, len);
    dst_ptr[1 + len] = '"';
    dst_ptr[1 + len + 1] = ' ';
    dst_ptr += len + 3;
    src_ptr_ptr++;
  }

  strcat(dst_ptr, XTERM_COMMAND_EXTENSION);

  /* Execute the command */
  execlp("xterm", "xterm", "-e", "/bin/sh", "-c", buf, NULL);

  log_error("Failed to execute command '%s' in terminal: %s", buf, strerror(errno));

  exit(1);
}
#endif

static
void
jpl_exec_program(
  const char * commandline,
  const char * working_dir,
  bool run_in_terminal,
  const char * const * env_vars)
{
  const char * argv[8];
  unsigned int i;

  /* for non terminal processes we use forkpty() that calls login_tty() that calls setsid() */
  /* we can successful call setsid() only once */
  if (run_in_terminal)
  {
    /* no longer anything to do with lashd */
    if (setsid() == -1)
    {
      fprintf(stderr, "Could not create new process group: %s\n", strerror(errno));
    }
  }

  /* change the working dir */
  if (chdir(working_dir) == -1)
  {
    fprintf(stderr, "Could not change directory to working dir '%s': %s\n", working_dir, strerror(errno));
  }

  if (env_vars)
  {
    while (env_vars[0] != NULL)
    {
      setenv(env_vars[0], env_vars[1], true);
      env_vars += 2;
    }
  }

  i = 0;

  if (run_in_terminal)
  {
    //if (!conf_get(LADISH_CONF_KEY_DAEMON_TERMINAL, argv + i))
    {
      argv[i] = "xterm";
    }
    i++;

#if 0
    if (strcmp(argv[0], "xterm") == 0 &&
        strchr(app_name, '"') == NULL &&
        strchr(app_name, '\'') == NULL &&
        strchr(app_name, '`') == NULL)
    {
      argv[i++] = "-T";
      argv[i++] = app_name;
    }
#endif

    argv[i++] = "-e";
  }

  //if (!conf_get(LADISH_CONF_KEY_DAEMON_SHELL, argv + i))
  {
    argv[i] = "sh";
  }
  i++;

  argv[i++] = "-c";

  argv[i++] = commandline;
  argv[i++] = NULL;

  printf("Executing '%s' with PID %llu\n", commandline, (unsigned long long)getpid());

  /* Execute it */
  execvp(argv[0], (char **)argv);

  fprintf(stderr, "Executing program '%s' failed: %s\n", argv[0], strerror(errno));

  exit(1);
}

static
void
jpl_read_child_output(
  struct jpl_child * child_ptr,
  int fd,
  bool error,
  char * buffer_ptr,
  char ** buffer_ptr_ptr,
  char * last_line,
  unsigned int * last_line_repeat_count)
{
  ssize_t ret;
  char *char_ptr;
  char *eol_ptr;
  size_t left;
  size_t max_read;

  do
  {
    max_read = CLIENT_OUTPUT_BUFFER_SIZE - 1 - (*buffer_ptr_ptr - buffer_ptr);
    ret = read(fd, *buffer_ptr_ptr, max_read);
    if (ret > 0)
    {
      (*buffer_ptr_ptr)[ret] = 0;
      char_ptr = buffer_ptr;

      while ((eol_ptr = strchr(char_ptr, '\n')) != NULL)
      {
        *eol_ptr = 0;

        if (*last_line_repeat_count > 0 && strcmp(last_line, char_ptr) == 0)
        {
          if (*last_line_repeat_count == 1)
          {
            if (error)
            {
              jpl_child_error(child_ptr->ctx, "last stderr line repeating...");
            }
            else
            {
              jpl_child_log(child_ptr->ctx, "last stdout line repeating...");
            }
          }

          (*last_line_repeat_count)++;
        }
        else
        {
          jpl_check_line_repeat_end(child_ptr, error, *last_line_repeat_count);

          strcpy(last_line, char_ptr);
          *last_line_repeat_count = 1;

          if (error)
          {
            jpl_child_error(child_ptr->ctx, "%s", char_ptr);
          }
          else
          {
            jpl_child_log(child_ptr->ctx, "%s", char_ptr);
          }
        }

        char_ptr = eol_ptr + 1;
      }

      left = ret - (char_ptr - *buffer_ptr_ptr);
      if (left != 0)
      {
        /* last line does not end with newline */

        if (left == CLIENT_OUTPUT_BUFFER_SIZE - 1)
        {
          /* line is too long to fit in buffer */
          /* print it like it is, rest (or more) of it will be logged on next interation */

          if (error)
          {
            jpl_child_error(child_ptr->ctx, "%s", char_ptr);
            jpl_child_error(child_ptr->ctx, "last line was truncated");
          }
          else
          {
            jpl_child_log(child_ptr->ctx, "%s", char_ptr);
            jpl_child_log(child_ptr->ctx, "last line was truncated");
          }

          left = 0;
        }
        else
        {
          memmove(buffer_ptr, char_ptr, left);
        }
      }

      *buffer_ptr_ptr = buffer_ptr + left;
    }
  }
  while ((size_t)ret == max_read);      /* if we have read everything as much as we can, then maybe there is more to read */
}

static
void
jpl_read_childs_output(
  void)
{
  struct list_head * node_ptr;
  struct jpl_child * child_ptr;

  list_for_each (node_ptr, &jpl_g_children)
  {
    child_ptr = list_entry(node_ptr, struct jpl_child, siblings);

    if (!child_ptr->terminal)
    {
      jpl_read_child_output(
        child_ptr,
        child_ptr->stdout,
        false,
        child_ptr->stdout_buffer,
        &child_ptr->stdout_buffer_ptr,
        child_ptr->stdout_last_line,
        &child_ptr->stdout_last_line_repeat_count);

      jpl_read_child_output(
        child_ptr,
        child_ptr->stderr,
        true,
        child_ptr->stderr_buffer,
        &child_ptr->stderr_buffer_ptr,
        child_ptr->stderr_last_line,
        &child_ptr->stderr_last_line_repeat_count);
    }
  }
}

void
void
jpl_run(
  void)
{
  jpl_read_childs_output();
  jpl_childs_bury();
}

static void set_ldpreload(const char * const * ldpreload)
{
  const char * old;
  char * new;
  char * add;

  if (ldpreload == NULL) return;

  add = catdup_array(ldpreload, " ");
  if (new == NULL)
  {
    fprintf(stderr, "Memory allocation failure. Cannot catdup LD_PRELOAD array.\n");
    exit(1);
  }

  old = getenv("LD_PRELOAD");
  if (old != NULL)
  {
    new = catdup3(add, " ", old);
    if (new == NULL)
    {
      fprintf(stderr, "Memory allocation failure. Cannot hook libalsapid.so through LD_PRELOAD.\n");
      exit(1);
    }
  }
  else
  {
    new = add;
  }

  printf("LD_PRELOAD set to \"%s\"\n", new);
  setenv("LD_PRELOAD", new, 1);

  if (old) free(new);
  free(add);
}

bool
jpl_execute(
  void * ctx,
  const char * working_dir,
  bool run_in_terminal,
  const char * commandline,
  const char * const * env_vars,
  const char * const * ldpreload,
  pid_t * pid_ptr,
  jpl_child_handle * child_handle_ptr)
{
  pid_t pid;
  struct jpl_child * child_ptr;
  int stderr_pipe[2];

  child_ptr = malloc(sizeof(struct jpl_child));
  if (child_ptr == NULL)
  {
    jpl_error("malloc() failed to allocate struct jpl_child");
    goto fail;
  }

  child_ptr->dead = false;
  child_ptr->terminal = run_in_terminal;
  child_ptr->stdout_buffer_ptr = child_ptr->stdout_buffer;
  child_ptr->stderr_buffer_ptr = child_ptr->stderr_buffer;
  child_ptr->stdout_last_line_repeat_count = 0;
  child_ptr->stderr_last_line_repeat_count = 0;

  if (!run_in_terminal)
  {
    if (pipe(stderr_pipe) == -1)
    {
      jpl_error("Failed to create stderr pipe");
    }
    else
    {
      child_ptr->stderr = stderr_pipe[0];

      if (fcntl(child_ptr->stderr, F_SETFL, O_NONBLOCK) == -1)
      {
        jpl_child_error(
          child_ptr->ctx,
          "Failed to set nonblocking mode on "
          "stderr reading end: %s",
          strerror(errno));
        close(stderr_pipe[0]);
        close(stderr_pipe[1]);
      }
    }
  }

  list_add_tail(&child_ptr->siblings, &jpl_g_children);

  if (!run_in_terminal)
  {
    /* We need pty to disable libc buffering of stdout */
    pid = forkpty(&child_ptr->stdout, NULL, NULL, NULL);
  }
  else
  {
    pid = fork();
  }

  if (pid == -1)
  {
    jpl_error("Could not fork to exec program: %s", strerror(errno));
    list_del(&child_ptr->siblings); /* fork failed so it is not really a child process to watch for. */
    goto free_struct;
  }

  if (pid == 0)
  {
    /* Need to close all open file descriptors except the std ones */
    struct rlimit max_fds;
    rlim_t fd;

    getrlimit(RLIMIT_NOFILE, &max_fds);

    for (fd = 3; fd < max_fds.rlim_cur; ++fd)
    {
      close(fd);
    }

    if (!run_in_terminal)
    {
      /* In child, close unused reading end of pipe */
      close(stderr_pipe[0]);

      dup2(stderr_pipe[1], fileno(stderr));
    }

    set_ldpreload(ldpreload);

    jpl_exec_program(commandline, working_dir, run_in_terminal, env_vars);

    return false;  /* We should never get here */
  }

  if (!run_in_terminal)
  {
    /* In parent, close unused writing ends of pipe */
    close(stderr_pipe[1]);

    if (fcntl(child_ptr->stdout, F_SETFL, O_NONBLOCK) == -1)
    {
      jpl_child_error(child_ptr->ctx,
                      "Could not set noblocking mode on stdout "
                      "- pty: %s", strerror(errno));
      close(stderr_pipe[0]);
      close(child_ptr->stdout);
    }
  }

  *pid_ptr = child_ptr->pid = pid;

  return true;

free_struct:
  free(child_ptr);

fail:
  return false;
}

unsigned int
jpl_get_app_count(
  void)
{
  struct list_head * node_ptr;
  unsigned int count;

  count = 0;

  list_for_each(node_ptr, &jpl_g_children)
  {
    count++;
  }

  return count;
}
