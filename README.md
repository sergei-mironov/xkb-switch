XKB-SWITCH 
===========
by J. Bromley, S. Mironov

xkb-switch is a C++ program that allows to query and change the XKB layout state.
Originally ruby-based code written by J.Broomley.

* XKeyboard.cpp  Implementation for XKB query/set class
* XKbSwitch.cpp  Main program

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
           xkb-switch -x                Print X layout string
	       xkb-switch [-p]              Displays current layout group
	       xkb-switch --test            Run some internal tests

Bugs or Problems
----------------

Admittedly, I only tested with a few different layouts that I used. If you find
any bugs let me know at Github or ierton@gmail.com. 

Xkb-switch contains some testing facilities, please include output of
`xkb-switch -x' command in the report.

Sergey.

