
*******************************************************************
* List of changes made by Marco Pratesi <marco.pratesi@gmail.com> *
*     to obtain Nanocomsole 1.0 from Nanocom                      *
*******************************************************************

-----------------------------------------------------
Changes to manage a Cisco device via its console port
-----------------------------------------------------

Removed the ICRNL c_iflag translation: it caused visualization
of a blank line after each text line of the output on screen.

Removed the ONLCR c_oflag mapping: it caused sending double newlines;
apart from a non optimal user interface, this prevented e.g. showing
the running-config, because, after "show run" and RETURN, an additional
different character was sent, causing the "show" command to quit.
Removed the OPOST c_oflag mapping, too.
At this point, ENTER was sending NL, i.e. "\n" and it seems there was
no way to send CR, i.e. "\r"; in many cases, "\n" is good for Cisco devices,
but "\r" is needed e.g. to get the CLI prompt after exit or timeout.
Hence, added a menu item just to send a CR.
Furthermore, added a command line option to map NL to CR on output,
enabled by default; added a menu item to change this setting at runtime, too.
With this option enabled, you can work with Cisco devices as usual.

Sending a BREAK signal to the Cisco console is needed e.g. for password
recovery and to kill ping, traceroute, lookups, etc. without waiting for
the expiration of a timeout.
Hence, added a menu option to send a BREAK.

---------------------------------------------------
Changes to the command line options and to the menu
---------------------------------------------------

Explicitly setting each option was mandatory and no defaults were foreseen.
Furthermore, the device had to be set as the second argument, before all other
options, without any option identifier; this way, getopt() failed on Android.
Changed the syntax:
- now the device is specified the same way as all other options;
- options may be specified in whatever order;
- no option is mandatory and defaults match the Cisco console port defaults;
- added a help "-h" option, that also reports default options.

Menu options were associated to numeric digits.
Now each menu option is associated to the same char identifying
the corresponding option.

-----------------------------
Changes to the user interface
-----------------------------

Many changes to text outputs, for better readability on small screens,
e.g. on 80-columns terminals.
Added the program version to the output text shown at startup
and to the help message.

-------------
Code cleaning
-------------

Removed some unused variables/structures/function prototypes.
A more "smart" Makefile.
Changed some names to better match the current code.
Reformatted almost all the code to standardize the indentation
and for better readability.
Minor changes to the README.txt

---------------
Android support
---------------

Nanocomsole works on Android, too.

