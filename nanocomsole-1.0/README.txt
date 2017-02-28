
----------
Background
----------

Nanocomsole is a lightweight serial port terminal emulator
for POSIX operating systems (such as GNU/Linux) and Android,
based upon Nanocom (http://nanocom.sourceforge.net/).

Nanocom is based upon Microcom (a newer version can be found on
https://launchpad.net/ubuntu/+source/microcom/) but removes
the scripting and logging features while introducing support
for setting local/remote echo, parity and stop bits. 
It also follows a more standard command line style, using the getopt() function.
Internally, much of the code has been rewritten and reformatted;
in particular, the menu system is almost entirely different.

-----
Usage
-----

Options of Nanocomsole can be set either at the command line, or at run-time
through a menu that can be accessed by pressing CTRL+T.

The command line options are shown using the -h "help" options:
"nanocomsole -h".

----
Code
----

Nanocomsole consists of 6 files:
menu.c, mux.c, nanocomsole.c, nanocomsole.h, version.h, and Makefile.

nanocomsole.c contains the main function and all command line parameter
verification code. This contains some of the code from microcom.c.

nanocomsole.h contains prototype definitions for nanocom.c and some general
definitions for the whole program. This is mostly identical to microcom.h.

menu.c contains all the menu related code. This reuses some code from
microcom's help.c file but reimplements the whole menu system.

mux.c contains the main loop of the program which sends data between
the keyboard, users screen and serial port.
This is virtually identical to the same file in microcom.

version.h defines the program version number.

-------
Build
-------

This source can be build with NDK command
ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk

-------
License
-------

Read LICENSE.txt

