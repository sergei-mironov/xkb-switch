XKB-SWITCH 
===========
by J. Bromley, S. Mironov

xkb-switch is a C++ program that allows to query and change the XKB layout state.
Originally ruby-based code written by J.Broomley.

XKeyboard.cpp  Implementation for XKB query/set class
XKbSwitch.cpp  Main program

The C++ class has no special dependencies on anything outside of
X-related libraries, so it can be easily used with other software.

Building
--------

To build the program manually, do the following.

 1. Unpack the tarball and cd to source directory
 2. cmake .
 3. make
 4. sudo make install

Usage
-----
	
	$ xkb-switch --help

	Usage: xkb-switch -s ARG            Sets current layout group to ARG
	       xkb-switch -l|--list         Displays all layout groups
	       xkb-switch -h|--help         Displays this message
	       xkb-switch -v|--version      Shows version number
	       xkb-switch -w|--wait [-p]    Waits for group change and exits
	       xkb-switch -W                Infinitely waits for group change
	       xkb-switch -n|--next         Switch to the next layout group
	       xkb-switch [-p]              Displays current layout group



Bugs or Problems
----------------

If you find any bugs please let me know at
ierton@gmail.com. Admittedly, I only tested with a few
different layouts that I used, so if you've got some odd layout or set
of layouts there may be problems. Please let me know.

Sergey.

