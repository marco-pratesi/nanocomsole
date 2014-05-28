/* *******************************************************************
 * Implements the main program loop.
 * Copyright (C) 2013-2014 Marco Pratesi <marco.pratesi@gmail.com>
 * Based on mux.c from Nanocom by Colin Sauze, Copyright (C) 2005.
 * Based upon code from Microcom by Anca and Lucian Jurubita,
 *     Copyright (C) 1999. All rights reserved.
 *********************************************************************
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 ****************************************************************** */

#include "nanocomsole.h"

#define BUFSIZE 1024

/* main program loop */
void mux_loop(int pf) {
  fd_set ready; /* used for select */
  int i = 0;    /* used in the multiplex loop */
  int done = 0;
  char buf[BUFSIZE];

  do { /* loop forever */
    FD_ZERO(&ready);
    FD_SET(STDIN_FILENO, &ready);
    FD_SET(pf, &ready);

    select(pf+1, &ready, NULL, NULL, NULL);

    if (FD_ISSET(pf, &ready)) {
      /* pf has characters for us */
      i = read(pf, buf, BUFSIZE);
      if (i > 0)
        write(STDOUT_FILENO, buf, i);
      else
        done = 1;
    } /* end if */

    if (FD_ISSET(STDIN_FILENO, &ready)) {
      /* standard input has characters for us */
      i = read(STDIN_FILENO, buf, BUFSIZE);
        if (i > 0)
          cook_buf(pf, buf, i);
        else
          done = 1;
    } /* if */
  } while (!done); /* do */
}

