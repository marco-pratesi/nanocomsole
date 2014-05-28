/* *******************************************************************
 * Nanocomsole is a tiny text-only serial port terminal emulator
 *     for POSIX systems and Android.
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

#include "nanocomsole.h"
#include "version.h"
#include <getopt.h>

char device[MAX_DEVICE_NAME]; /* serial device name */
int pf = 0;                   /* port file descriptor */
struct termios pots;          /* old port termios settings to restore */
struct termios sots;          /* old stdout/in termios settings to restore */

/* displays the current terminal settings */
void display_state() {
  fprintf(stdout,
    "\n****************************** Line State ******************************\n"
    "Serial Port Device: -D %s   \tBit Rate: -b %d bps\n"
    "Data Bits: -d %c    Parity: -p %c    Flow Control: -f %c    Stop Bits: -s %c\n"
    "Echo Mode: -e %c    Map NL to CR on output: -r %c\n"
    "************************************************************************\n",
    device, curr_state.baud_rate,
    curr_state.data_bits, curr_state.parity, curr_state.flow_control, curr_state.stop_bits,
    curr_state.echo_type, curr_state.onlcr_mapping
  );
}

/* setup the terminal according to settings in curr_state */
void init_comm() {
  struct termios pts;
  tcgetattr(pf, &pts);
  /* make sure pts is all blank and no residual values set
       safer than modifying current settings */
  pts.c_lflag = 0; /* implies non-canonical mode */
  pts.c_iflag = 0;
  pts.c_oflag = 0;
  pts.c_cflag = 0;

  switch (curr_state.baud_rate) {
    case 300:
      pts.c_cflag |= B300;
      break;
    case 1200:
      pts.c_cflag |= B1200;
      break;
    case 2400:
      pts.c_cflag |= B2400;
      break;
    case 4800:
      pts.c_cflag |= B4800;
      break;
    case 9600:
      pts.c_cflag |= B9600;
      break;
    case 19200:
      pts.c_cflag |= B19200;
      break;
    case 38400:
      pts.c_cflag |= B38400;
      break;
    case 57600:
      pts.c_cflag |= B57600;
      break;
    case 115200:
      pts.c_cflag |= B115200;
      break;
    default:
      curr_state.baud_rate = 9600;
      pts.c_cflag |= B9600;
      break;
  }

  switch (curr_state.data_bits) {
    case '7':
      pts.c_cflag |= CS7;
      break;
    case '8':
      pts.c_cflag |= CS8;
      break;
    default:
      curr_state.data_bits = '8';
      pts.c_cflag |= CS8;
      break;
  }

  switch (curr_state.parity) {
    case 'n':
      /* no parity */
      break;
    case 'e':
      /* turn on parity checking on input */
      pts.c_iflag |= INPCK;
      /* set parity on the cflag */
      pts.c_cflag |= PARENB;
      /* even parity default, no need to set it */
      break;
    case 'o':
      /* turn on parity checking on input */
      pts.c_iflag |= INPCK;
      /* set parity on the cflag */
      pts.c_cflag |= PARENB;
      /* set parity to odd */
      pts.c_cflag |= PARODD;
      break;
    default:
      /* no parity */
      curr_state.parity = 'n';
      break;
  }

  switch (curr_state.flow_control) {
    case 'h':
      /* hardware flow control */
      pts.c_cflag |= CRTSCTS;
      break;
    case 's':
      /* software flow control */
      pts.c_iflag |= IXON | IXOFF | IXANY;
      break;
    case 'n':
      /* no flow control */
      break;
    default:
      /* no flow control */
      curr_state.flow_control = 'n';
      break;
  }

  /* ignore modem lines like hangup */
  pts.c_cflag |= CLOCAL;
  /* let us read from this device! */
  pts.c_cflag |= CREAD;

  switch (curr_state.stop_bits) {
    case '1':
      /* 1 stop bit is the default setting - no action required */
      break;
    case '2':
      /* 2 stop bits */
      pts.c_cflag |= CSTOPB;
      break;
    default:
      /* 1 stop bit */
      curr_state.stop_bits = '1';
      break;
  }

  switch (curr_state.echo_type) {
    case 'r':
      pts.c_lflag |= (ECHO | ECHOCTL | ECHONL);
      break;
    case 'n':
      /* handled in init_stdin */
      break;
    case 'l':
      break;
    default:
      /* no echoing */
      curr_state.echo_type = 'n';
      break;
  }

  switch (curr_state.onlcr_mapping) {
    case 'y':
    case 'n':
      /* already set */
      break;
    default:
      curr_state.onlcr_mapping = 'y';
      break;
  }

  /* perform newline mapping, useful when dealing with ms-dos/ms-windows systems */
  //pts.c_oflag = OPOST;
  /* pay attention to hangup line */
  pts.c_cflag |= HUPCL;
  /* don't wait between receiving characters */
  pts.c_cc[VMIN] = 1;
  pts.c_cc[VTIME] = 0;
  /* translate incoming CR to LF, helps with ms-dos/ms-windows clients */
  //pts.c_iflag |= ICRNL;
  /* translate outgoing CR to CRLF */
  //pts.c_oflag |= ONLCR;
  /* set new attributes */
  tcsetattr(pf, TCSANOW, &pts);
}

void init_stdin(struct termios *sts) {
  /* again, some arbitrary things */
  sts->c_iflag &= ~BRKINT;
  sts->c_iflag |= IGNBRK;
  sts->c_lflag &= ~ISIG;
  sts->c_cc[VMIN] = 1;
  sts->c_cc[VTIME] = 0;
  sts->c_lflag &= ~ICANON;

  /* enable local echo */
  if (curr_state.echo_type=='l')
    sts->c_lflag |= (ECHO | ECHOCTL | ECHONL);
  else
    sts->c_lflag &= ~(ECHO | ECHOCTL | ECHONL);
}

/* Display a usage message */
void usage() {
  fprintf(stdout,
    "\nNanocomsole " VERSION "\n"
    "Usage: nanocomsole [options]\n"
    "  -D device [/dev/ttyUSB0]: the serial port to use.\n"
    "       (/dev/ttyS0, /dev/ttyS1, or /dev/ttyUSB0 on most systems.)\n"
    "  -b bit rate [9600]: valid options are 300, 1200, 2400, 4800, 9600,\n"
    "                                        19200, 38400, 57600, 115200.\n"
    "  -d data bits [8]: the number of data bits (7 or 8).\n"
    "  -p parity [n]: n for none, e for even, o for odd.\n"
    "       (It must be the same as the remote system.)\n"
    "  -f flow control [n]: h for hardware (RTS/CTS), s for software (Xon/Xoff),\n"
    "                       n for none.\n"
    "  -s stop bits [1]: the number of stop bits (1 or 2).\n"
    "  -e echo mode [n]:\n"
    "    l: local echo, echos everything you type,\n"
    "                   use if you don't see what you type;\n"
    "    r: lemote echo, if remote system doesn't see what is typed, use it\n"
    "                    OR enable local echo on remote system;\n"
    "    n: no echoing.\n"
    "  -r map NL to CR on output [y]: y for yes, n for no.\n"
    "\n");
}

/* restore original terminal settings and exit */
void cleanup_termios(int signal) {
  /*flush any buffers*/
  tcflush(pf,TCIFLUSH);
  /*restore settings*/
  tcsetattr(pf, TCSANOW, &pots);
  tcsetattr(STDIN_FILENO, TCSANOW, &sots);
  exit(0);
}

int main(int argc, char *argv[]) {
  struct termios pts;    /* termios settings on port */
  struct termios sts;    /* termios settings on stdout/in */
  struct sigaction sact; /* used to initialize the signal handler */
  int opt;

  strcpy(device, "/dev/ttyUSB0");

  while ( (opt = getopt(argc, argv, "h?D:b:d:p:f:s:e:r:")) != -1 ) {
    switch (opt) {
      /* help message */
      case 'h':
        usage();
        return 0;
        break;
      /* set the serial device name */
      case 'D':
        strcpy(device, optarg);
        break;
      /* set the baud rate */
      case 'b':
        curr_state.baud_rate = atoi(optarg);
        break;
      /* data bits */
      case 'd':
        curr_state.data_bits = optarg[0];
        break;
      /* parity */
      case 'p':
        curr_state.parity = optarg[0];
        break;
      /* setup flow control */
      case 'f':
        curr_state.flow_control = optarg[0];
        break;
      /* stop bits */
      case 's':
        curr_state.stop_bits = optarg[0];
        break;
      /* echo type */
      case 'e':
        curr_state.echo_type = optarg[0];
        break;
      /* NL to CR mapping on output */
      case 'r':
        curr_state.onlcr_mapping = optarg[0];
        break;
      default:
        break;
    } /* end switch */
  }  /* end while */

  /* open the device */
  pf = open(device, O_RDWR);
  if (pf < 0) {
    fprintf(stderr, "\n  ERROR: cannot open device %s\n\n", device);
    exit(1);
  }
  /* save old serial port config */
  tcgetattr(pf, &pts);
  memcpy(&pots, &pts, sizeof(pots));
  /* setup serial port with new settings */
  init_comm(&pts);
  fprintf(stdout, "\nWelcome to Nanocomsole " VERSION);
  display_state();
  fprintf(stdout, "Press CTRL+T for menu options\n\n");

  /* Now deal with the local terminal side */
  tcgetattr(STDIN_FILENO, &sts);
  memcpy(&sots, &sts, sizeof(sots)); /* to be used upon exit */
  init_stdin(&sts);
  tcsetattr(STDIN_FILENO, TCSANOW, &sts);

  /* set the signal handler to restore the old termios handler */
  sact.sa_handler = cleanup_termios;
  sigaction(SIGHUP, &sact, NULL);
  sigaction(SIGINT, &sact, NULL);
  sigaction(SIGPIPE, &sact, NULL);
  sigaction(SIGTERM, &sact, NULL);

  /* run the main program loop */
  mux_loop(pf);

  /* unreachable line ?? */
  cleanup_termios(0);

  return 0;
}

