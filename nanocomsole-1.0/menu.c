/* *******************************************************************
 * Implements the menu system for Nanocomsole.
 * Copyright (C) 2013-2014 Marco Pratesi <marco.pratesi@gmail.com>
 * Based on menu.c from Nanocom by Colin Sauze, Copyright (C) 2005.
 * Based on help.c from Microcom by Anca and Lucian Jurubita,
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

extern char device[MAX_DEVICE_NAME]; /* serial device name */

static int menu_state = 0;
static int in_escape = 0;

/* static functions proto-types */
static void main_menu();
static void baud_menu(void);
static void data_bits_menu(void);
static void parity_menu(void);
static void flow_control_menu(void);
static void stop_bits_menu(void);
static void echo_settings_menu(void);
static void onlcr_mapping_menu(void);

static void process_main_menu(int fd, char c);
static void process_baud_menu(int fd, char c);
static void process_data_bits_menu(int fd, char c);
static void process_parity_menu(int fd, char c);
static void process_flow_control_menu(int fd, char c);
static void process_stop_bits_menu(int fd, char c);
static void process_echo_settings_menu(int fd, char c);
static void process_onlcr_mapping_menu(int fd, char c);

static void parse_menu_state(int fd, char buf);
static void reset_state(void);

char err[] = "\nInvalid option\n";
char leaving[] = "\nLeaving menu\n\n";

/* *******************************************************************
 * Menu functions.
 * All functions of the naming convention xxxx_menu just display a menu.
 * All functions of the naming convention process_xxxx_menu perform
 *     the actions for that menu.
 * The main menu is called by pressing CTRL+T, handled in cook_buf()
 * ******************************************************************/

/* the main menu */
void main_menu(void) {
  char str1[] =
    "\n***************** Menu *****************\n"
    "l: display current Line state\n"
    "b: set Bit rate\n"
    "d: set Data bits\n"
    "p: set Parity\n"
    "f: set Flow control\n"
    "s: set Stop bits\n"
    "e: set Echo mode\n"
    "c: send CR (RETURN) to the serial port\n"
    "r: set mapping of NL to CR on output\n"
    "CTRL+B, B: send BREAK to the serial port\n"
    "q, Q, x, X: Quit the program\n"
    "any other key: leave menu\n"
    "****************************************\n";
  write(STDOUT_FILENO, str1, strlen(str1));
}

/* display baud rate menu */
static void baud_menu(void) {
  char str[] =
    "\n***** Set Bit Rate *****\n"
    " a - 300\n"
    " b - 1200\n"
    " c - 2400\n"
    " d - 4800\n"
    " e - 9600\n"
    " f - 19200\n"
    " g - 38400\n"
    " h - 57600\n"
    " i - 115200\n"
    "************************\n"
    "Command: ";
  write(STDOUT_FILENO, str, strlen(str));
}

/* display data bits menu */
static void data_bits_menu(void) {
  char str[] =
    "\n***** Set Number of Data Bits *****\n"
    " 7 - 7 Data Bits\n"
    " 8 - 8 Data bits\n"
    "***********************************\n"
    "Command: ";
  write(STDOUT_FILENO, str, strlen(str));
}

/* display parity menu */
static void parity_menu(void) {
  char str[] =
    "\n***** Set Parity *****\n"
    " n - No Parity\n"
    " e - Even Parity\n"
    " o - Odd Parity\n"
    "**********************\n"
    "Command: ";
  write(STDOUT_FILENO, str, strlen(str));
}

/* display menu for flow control */
static void flow_control_menu(void) {
  char str[] =
    "\n***** Set Flow Control Type *****\n"
    " h - Hardware Flow Control (CTS/RTS)\n"
    " s - Software Flow Control (XON/XOFF)\n"
    " n - No Flow Control\n"
    "*********************************\n"
    "Command: ";
  write(STDOUT_FILENO, str, strlen(str));
}

/* display stop bits menu */
static void stop_bits_menu(void) {
  char str[] =
    "\n***** Set Number of Stop Bits *****\n"
    " 1 - 1 Stop Bit\n"
    " 2 - 2 Data Bits\n"
    "***********************************\n"
    "Command: ";
  write(STDOUT_FILENO, str, strlen(str));
}

/* display menu for echo settings */
static void echo_settings_menu(void) {
  char str[] =
    "\n***** Set Echo Mode *****\n"
    " l - local echo (echo what you type locally)\n"
    " r - remote echo (echo what a remote system sends you, back to that system)\n"
    " n - No Echoing\n"
    "*************************\n"
    "Command: ";
  write(STDOUT_FILENO, str, strlen(str));
}

/* display menu for onlcr setting */
static void onlcr_mapping_menu(void) {
  char str[] =
    "\n***** Set Mapping of NL to CR on Output *****\n"
    " y - yes\n"
    " n - no\n"
    "*********************************************\n"
    "Command: ";
  write(STDOUT_FILENO, str, strlen(str));
}

/* process function for menu_state == 0 */
static void process_main_menu(int fd, char c) {
  menu_state = c;

  switch (c) {
    case 'l':
      /* display current line settings */
      display_state();
      /* exit the menu */
      reset_state();
      break;
    /* baud rate */
    case 'b':
      /* display baud rate menu */
      baud_menu();
      break;
    case 'd':
      /* data bits */
      data_bits_menu();
      break;
    case 'p':
      /* parity */
      parity_menu();
      break;
    case 'f':
      /* flow control */
      flow_control_menu();
      break;
    case 's':
      /* stop bits */
      stop_bits_menu();
      break;
    case 'e':
      /* echo settings */
      echo_settings_menu();
      break;
    case 'c':
      /* send CR to the serial port */
      write(fd, "\r", 1);
      reset_state();
      break;
    case 'r':
      /* onlcr mapping */
      onlcr_mapping_menu();
      break;
    case 0x02:
    case 'B':
      /* send break to the serial port */
      tcsendbreak(fd, 0);
      reset_state();
      break;
    case 'q':
    case 'Q':
    case 'x':
    case 'X':
      /* quit the program */
      /* restore termios settings and exit */
      write(STDOUT_FILENO, "\n", 1);
      cleanup_termios(0);
      break;
    default:
      /* quit menu */
      reset_state();
      break;
  }
}

/* process responses to select a new baud rate */
static void process_baud_menu(int fd, char c) {
  switch (c) {
    case 'a':
      curr_state.baud_rate = 300;
      init_comm();
      break;
    case 'b':
      curr_state.baud_rate = 1200;
      init_comm();
      break;
    case 'c':
      curr_state.baud_rate = 2400;
      init_comm();
      break;
    case 'd':
      curr_state.baud_rate = 4800;
      init_comm();
      break;
    case 'e':
      curr_state.baud_rate = 9600;
      init_comm();
      break;
    case 'f':
      curr_state.baud_rate = 19200;
      init_comm();
      break;
    case 'g':
      curr_state.baud_rate = 38400;
      init_comm();
      break;
    case 'h':
      curr_state.baud_rate = 57600;
      init_comm();
      break;
    case 'i':
      curr_state.baud_rate = 115200;
      init_comm();
      break;
    default:
      write(STDOUT_FILENO, err, strlen(err));
      break;
  }
}

/* process responses to select number of data bits */
static void process_data_bits_menu(int fd, char c) {
  switch (c) {
    case '7':
    case '8':
      curr_state.data_bits = c;
      init_comm();
      break;
    default:
      write(STDOUT_FILENO, err, strlen(err));
      break;
  }
}

/* process responses to select parity settings */
static void process_parity_menu(int fd, char c) {
  switch (c) {
    case 'n':
    case 'e':
    case 'o':
      curr_state.parity = c;
      init_comm();
      break;
    default:
      write(STDOUT_FILENO, err, strlen(err));
      break;
  }
}

/* process responses to select flow control type */
static void process_flow_control_menu(int fd, char c) {
  switch (c) {
    case 'n':
    case 'h':
    case 's':
      curr_state.flow_control = c;
      init_comm();
      break;
    default:
      write(STDOUT_FILENO, err, strlen(err));
      break;
  }
}

/* process responses to select number of stop bits */
static void process_stop_bits_menu(int fd, char c) {
  switch (c) {
    case '1':
    case '2':
      curr_state.stop_bits = c;
      init_comm();
      break;
    default:
      write(STDOUT_FILENO, err, strlen(err));
      break;
  }
}

/* process responses to select echo mode */
static void process_echo_settings_menu(int fd, char c) {
  switch (c) {
    case 'n':
    case 'l':
    case 'r':
      curr_state.echo_type = c;
      init_comm();
      break;
    default:
      write(STDOUT_FILENO, err, strlen(err));
      break;
  }
}

/* process responses to select onlcr mapping */
static void process_onlcr_mapping_menu(int fd, char c) {
  switch (c) {
    case 'y':
    case 'n':
      curr_state.onlcr_mapping = c;
      init_comm();
      break;
    default:
      write(STDOUT_FILENO, err, strlen(err));
      break;
  }
}

/* resets the menu state so we aren't left in a deeper level of the menu */
static void reset_state(void) {
  in_escape = 0;
  menu_state = 0;
  write(STDOUT_FILENO, leaving, strlen(leaving));
}

/* launch the right menu processor, based on the current menu state */
static void parse_menu_state(int fd, char buf)
{
  switch (menu_state) {
    case 0:
      process_main_menu(fd, buf);
      break;
    /* no need for case 'l': it just displays the settings, no input required */
    case 'b':
      process_baud_menu(fd, buf);
      reset_state();
      break;
    case 'd':
      process_data_bits_menu(fd, buf);
      reset_state();
      break;
    case 'p':
      process_parity_menu(fd, buf);
      reset_state();
      break;
    case 'f':
      process_flow_control_menu(fd, buf);
      reset_state();
      break;
    case 's':
      process_stop_bits_menu(fd, buf);
      reset_state();
      break;
    case 'e':
      process_echo_settings_menu(fd, buf);
      reset_state();
      break;
    /* there is no need for cases c,q,Q,x,X,default, */
    /* as we'll have exited before getting to this menu */
    case 'r': /* entered the 'r' character */
      process_onlcr_mapping_menu(fd, buf);
      reset_state();
      break;
  }
}

/* *******************************************************************
 * Redirect escaped characters to the menu handling functions;
 * the function is called from mux.c in the main character processing routine;
 * - fd: file handle for the comm port
 * - buf: pointer to the character buffer
 * - num: number of valid characters in the buffer
 ****************************************************************** */
void cook_buf(int fd, char *buf, int num) {
  int current = 0;

  if (in_escape) {
    /* cook_buf last called with an incomplete escape sequence */
    parse_menu_state(fd, buf[current]);
    num--; /* advance to the next character in the buffer */
    buf++;
  }

  while (current < num) { /* big while loop, to process all the charactes in buffer */
    /* look for the next escape character */
    /* Reference: http://www.unix-manuals.com/refs/misc/ascii-table.html */
    while ((current < num)
           && !(buf[current] == 0x14) /* 0x14 is the ASCII code for CTRL+t */
           && !(buf[current] == 0x0a  /* LF, i.e. '\n' */
                && curr_state.onlcr_mapping == 'y')
          ) {
      current++;
    }
    /* and write to the serial port the sequence preceding the escape character */
    if (current)
      write(fd, buf, current);

    if (current < num) { /* process an escape sequence */
      /* found an escape character */
      switch (buf[current]) {
        case 0x0a:
          buf[current] = 0x0d; /* replace NL with CR */
          write(fd, "\r", 1);
          break;
        default:
          if (menu_state == 0) {
            /* not already in menu, display the main one */
            main_menu();
          }
          current++;
          if (current >= num) {
            /* interpret first character of next sequence */
            in_escape = 1;
            return;
          }
          parse_menu_state(fd, buf[current]);
          break;
      } /* end switch */
      current++;
      if (current >= num)
        return;
    } /* if - end of processing escape sequence */
    num -= current;
    buf += current;
    current = 0;
  } /* while - end of processing all the charactes in the buffer */

  return;
}

