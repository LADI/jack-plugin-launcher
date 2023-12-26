/* -*- Mode: C ; c-basic-offset: 2 -*- */
/*
 * SPDX-FileCopyrightText: Copyright © 2023 Nedko Arnaudov
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include <cdbus/cdbus.h>
#include <cdbus/log.h>

#if HAVE_GITVERSION_H
#include "gitversion.h"
#endif

#include "loader.h"

bool g_quit;
const char * g_dbus_unique_name;
cdbus_object_path g_dbus_object;

extern const struct cdbus_interface_descriptor jpl_appman_cdbus_interface_org_ladish_ApplicationManager0;

#define SERVICE_NAME             DBUS_NAME_BASE
#define DBUS_OBJECT_PATH         DBUS_BASE_PATH

void
jpld_log(
  void * ctx,
  bool error,
  const char * format,
  ...)
{
  va_list ap;
  FILE * stream;

  //printf("jpl_log()\n");

  stream = error ? stderr : stdout;

  va_start(ap, format);
  vfprintf(stream, format, ap);
  va_end(ap);

  fflush(stream);
}

void
jpl_cdbus_log(
  unsigned int level,
  const char * file,
  unsigned int line,
  const char * func,
  const char * format,
  ...)
{
  va_list ap;
  FILE * stream;

  //printf("jpl_cdbus_log()\n");

  stream = stdout;

  va_start(ap, format);
  vfprintf(stream, format, ap);
  va_end(ap);

  fputc('\n', stream);

  fflush(stream);
}

#define log_error(fmt, args...) jpld_log(NULL,  true, fmt "\n", ## args)
#define log_info( fmt, args...) jpld_log(NULL, false, fmt "\n", ## args)
#define log_debug(fmt, args...) jpld_log(NULL, false, fmt "\n", ## args)

static bool connect_dbus(void)
{
  int ret;

  dbus_error_init(&cdbus_g_dbus_error);

  cdbus_g_dbus_connection = dbus_bus_get(DBUS_BUS_SESSION, &cdbus_g_dbus_error);
  if (dbus_error_is_set(&cdbus_g_dbus_error))
  {
    log_error("Failed to get bus: %s", cdbus_g_dbus_error.message);
    dbus_error_free(&cdbus_g_dbus_error);
    goto fail;
  }

  g_dbus_unique_name = dbus_bus_get_unique_name(cdbus_g_dbus_connection);
  if (g_dbus_unique_name == NULL)
  {
    log_error("Failed to read unique bus name");
    goto unref_connection;
  }

  log_info("Connected to local session bus, unique name is \"%s\"", g_dbus_unique_name);

  ret = dbus_bus_request_name(cdbus_g_dbus_connection, SERVICE_NAME, DBUS_NAME_FLAG_DO_NOT_QUEUE, &cdbus_g_dbus_error);
  if (ret == -1)
  {
    log_error("Failed to acquire bus name: %s", cdbus_g_dbus_error.message);
    dbus_error_free(&cdbus_g_dbus_error);
    goto unref_connection;
  }

  if (ret == DBUS_REQUEST_NAME_REPLY_EXISTS)
  {
    log_error("Requested connection name already exists");
    goto unref_connection;
  }

  g_dbus_object = cdbus_object_path_new(
    DBUS_OBJECT_PATH,
    &jpl_appman_cdbus_interface_org_ladish_ApplicationManager0,
    NULL,
    NULL);
  if (g_dbus_object == NULL)
  {
    goto unref_connection;
  }

  if (!cdbus_object_path_register(cdbus_g_dbus_connection, g_dbus_object))
  {
    goto destroy_dbus_object;
  }

  return true;

destroy_dbus_object:
  cdbus_object_path_destroy(cdbus_g_dbus_connection, g_dbus_object);
unref_connection:
  dbus_connection_unref(cdbus_g_dbus_connection);

fail:
  return false;
}

static void disconnect_dbus(void)
{
  cdbus_object_path_destroy(cdbus_g_dbus_connection, g_dbus_object);
  dbus_connection_unref(cdbus_g_dbus_connection);
  cdbus_call_last_error_cleanup();
}

void term_signal_handler(int signum)
{
  log_info("Caught signal %d (%s), terminating", signum, strsignal(signum));
  g_quit = true;
}

bool install_term_signal_handler(int signum, bool ignore_if_already_ignored)
{
  sig_t sigh;

  sigh = signal(signum, term_signal_handler);
  if (sigh == SIG_ERR)
  {
    log_error("signal() failed to install handler function for signal %d.", signum);
    return false;
  }

  if (sigh == SIG_IGN && ignore_if_already_ignored)
  {
    signal(SIGTERM, SIG_IGN);
  }

  return true;
}

int main(void)
{
  int ret;

  ret = EXIT_FAILURE;

  cdbus_log_setup(jpl_cdbus_log);

  log_info("JACK Plugin Launcher daemon, version " JPL_VERSION);
  log_info("Copyleft JACK Plugin Authors");
#if defined(GIT_VERSION)
  log_info("built from " GIT_VERSION);
#endif
  log_info("built on " BUILD_TIMESTAMP);

  /* install the signal handlers */
  install_term_signal_handler(SIGTERM, false);
  install_term_signal_handler(SIGINT, true);
  install_term_signal_handler(SIGHUP, true);
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
  {
    log_error("signal(SIGPIPE, SIG_IGN).");
  }

  if (!connect_dbus())
  {
    log_error("Failed to connect to D-Bus");
    goto exit;
  }

  while (!g_quit)
  {
    dbus_connection_read_write_dispatch(cdbus_g_dbus_connection, 50);
    //jpl_run();
  }

  ret = EXIT_SUCCESS;
  log_debug("Finished, cleaning up");

  disconnect_dbus();

exit:
  return ret;
}
