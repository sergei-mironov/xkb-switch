
*Unfortunately, I'm going to stop supporting this project. Please contact me if you need some cosmetic changes in code.*
/Sergey/


XKB-SWITCH 
===========
by J. Bromley, S. Mironov, Alexei Rad'kov

xkb-switch is a C++ program that allows to query and change the XKB layout state.
Originally ruby-based code written by J.Broomley.

* XKeyboard.cpp  Implementation for XKB query/set class
* XKbSwitch.cpp  Main program
* XKbSwitchApi.cpp The Vim API bindings

The C++ class has no special dependencies on anything outside of
X-related libraries, so it can be easily used with other software.

Xkb-switch is licensed under the GNU GPLv3, see COPYING for details.

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

VIM integration
---------------

Xkb-switch now contains a library libxkbswitch.so which can be called from
within Vim scripts. See this [article in Russian](http://lin-techdet.blogspot.ru/2012/12/vim-xkb-switch-libcall.html) for
details. Thanks to lyokha for contribution.

Layout groups
-------------

xkb-group.sh can help you to manage layout groups. Just run it and send some
input at it's stdin every time you want to trigger layouts from primary to
secondary and back. For example:

	$ xkb-group.sh us ru
	switch # switch from us to ru or from current layout to us
	switch # switch from ru to us or from us to ru

	(from other terminal)
	$ xkb-switch -s de # switch to 'de' layout, change secondary layout to 'de'
	
	(back to terminal running xkb-group.sh)
	switch # switch from de to us
	switch # switch from us to de
	
Bugs or Problems
----------------

Admittedly, I only tested with a few different layouts that I used. If you find
any bugs let me know at Github or ierton@gmail.com. 

Xkb-switch contains some testing facilities, please include output of
`xkb-switch -x' command in the report.

Sergey.

