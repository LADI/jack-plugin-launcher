/* -*- Mode: C ; c-basic-offset: 2 -*- */
/* Copyleft JACK Plugin Authors, see COPYING file for more info
 * This file is part of JACK Plugin Launcher (jpl) */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#if HAVE_GITVERSION_H
#include "gitversion.h"
#endif

int main(void)
{
  printf("JACK Plugin Launcher, version " JPL_VERSION
#if (HAVE_GITVERSION_H)
         " (" GIT_VERSION ")"
#endif
         " built on " BUILD_TIMESTAMP
         "\n"
    );
  printf("Copyleft 2022 Nedko Arnaudov\n");
  return 0;
}
