/* *******************************************************************
 * The main header file for Nanocomsole.
 *     Copyright (C) 2013-2014 Marco Pratesi <marco.pratesi@gmail.com>
 * Nanocomsole is based upon Nanocom by Colin Sauze, Copyright (C) 2005.
 * Nanocom is based upon Microcom by Anca and Lucian Jurubita,
 *     Copyright (C) 1999.  All rights reserved.
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

#ifndef NANOCOM_H
#define NANOCOM_H

#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>

#define MAX_DEVICE_NAME 50 /* maximum length of the serial port /dev name */

#define byte unsigned char
#define TRUE 1
#define FALSE 0

void cook_buf(int fd, char *buf, int num); /* menu.c */
void mux_loop(int pf); /* mux.c */

void display_state();
void init_comm();
void init_stdin(struct termios *sts);
void usage();
void cleanup_termios(int signal);

/* typedef to represent the current line state
 * warning: using int for baud_rate is not safe in 16-bit/8-bit architectures,
 * fine in 32/64-bit, including i386+,alpha,mips,ia64,amd64,arm,sparc,etc. */
typedef struct {
  int baud_rate;      /* 300,1200,2400,4800,9600,19200,38400,57600,115200 */
  byte data_bits;     /* 7 or 8 */
  char parity;        /* e for even, o for odd, n for none */
  char flow_control;  /* h for hardware, s for software, n for none */
  byte stop_bits;     /* 1 or 2 */
  char echo_type;     /* r for remote, l for local, n for none */
  char onlcr_mapping; /* y for yes, n for no */
} state;

state curr_state;

#endif /* NANOCOM_H */

