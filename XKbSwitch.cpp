/*
 * Plain launcher program for Jay Bromley's Xkeyboard library.
 *
 * Copyright (C) 2010 by Sergey Mironov <ierton@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <iostream>
#include <algorithm>
#include "XKeyboard.h"
#include "X11Exception.h"
#include <X11/XKBlib.h>
#include <sstream>

#define XKB_SWITCH_VERSION "1.1"

using namespace std;

void xkb_wait_event(XKeyboard &xkb)
{
	CHECK(xkb._display != 0);

	Bool bret = XkbSelectEventDetails(xkb._display, XkbUseCoreKbd, 
		XkbStateNotify, XkbAllStateComponentsMask, XkbGroupStateMask);
	CHECK_MSG(bret==True, "XkbSelectEventDetails failed");

	XEvent event;
	int iret = XNextEvent(xkb._display, &event);
	CHECK_MSG(iret==0, "XNextEvent failed with " << iret);
}


void usage()
{
	cerr << "Usage: xkb-switch -s ARG            Sets current layout group to ARG" << endl;
	cerr << "       xkb-switch -l|--list         Displays all layout groups" << endl;
	cerr << "       xkb-switch -h|--help         Displays this message" << endl;
	cerr << "       xkb-switch -v|--version      Shows version number" << endl;
	cerr << "       xkb-switch -w|--wait [-p]    Waits for group change and exits" << endl;
	cerr << "       xkb-switch -W                Infinitely waits for group change" << endl;
	cerr << "       xkb-switch -n|--next         Switch to the next layout group" << endl;
	cerr << "       xkb-switch [-p]              Displays current layout group" << endl;
}

int main( int argc, char* argv[] ) 
{
	using namespace std;
	try {
		int m_cnt = 0;
		int m_wait = 0;
		int m_lwait = 0;
		int m_print = 0;
		int m_next = 0;
		int m_list = 0;
		string newgrp;

		XKeyboard xkb;

		for(int i=1; i<argc; i) {
			string arg(argv[i++]);
			if(arg == "-s") {
				CHECK_MSG(i<argc, "Argument expected");
				newgrp=argv[i++];
				m_cnt++;
			}
			else if(arg == "-l" || arg == "--list") {
				m_list = 1;
				m_cnt++;
			}
			else if(arg == "-v" || arg=="--version") {
				cerr << "xkb-switch " << XKB_SWITCH_VERSION << endl;
				return 0;
			}
			else if(arg == "-w" || arg == "--wait") {
				m_wait = 1;
				m_cnt++;
			}
			else if(arg == "-W" || arg == "--longwait") {
				m_lwait = 1;
				m_cnt++;
			}
			else if(arg == "-p" || arg == "--print") {
				m_print = 1;
				m_cnt++;
			}
			else if(arg == "-n" || arg == "--next") {
				m_next = 1;
				m_cnt++;
			}
			else if(arg == "-h" || arg == "--help") {
				usage();
				return 1;
			}
			else {
				throw string("Invalid argument: " + arg);
			}
		}

		if(m_list || m_lwait || !newgrp.empty() || m_next)
			CHECK_MSG(m_cnt==1, "Invalid flag combination. Try --help.");

		// Default action
		if(m_cnt==0) 
			m_print = 1;

		if(m_wait) {
			xkb_wait_event(xkb);
		}

		if(m_lwait) {
			while(true) {
				xkb_wait_event(xkb);
				StringVector syms = xkb.getSymNames();
				cout << syms.at(xkb.getCurrentGroupNum()) << endl;
			}
		}

		StringVector syms = xkb.getSymNames();

		if(!newgrp.empty()) {
			StringVector::iterator i = find(syms.begin(), syms.end(), newgrp);
			if(i==syms.end()) throw string("Group '") + newgrp + 
				"' is not supported by current layout. Try xkb-switch -l.";
			xkb.setGroupByNum(i-syms.begin());
		}

		if (m_next) {
			if (syms.empty()) throw string("No layout groups configured");
			const string nextgrp = syms.at(xkb.getCurrentGroupNum());
			StringVector::iterator i = find(syms.begin(), syms.end(), nextgrp);
			if (++i == syms.end()) i = syms.begin();
			xkb.setGroupByNum(i - syms.begin());
		}

		if(m_print) {
			cout << syms.at(xkb.getCurrentGroupNum()) << endl;
		}

		if(m_list) {
			for(int i=0; i<syms.size(); i++) {
				cout << syms[i] << endl;
			}
		}
		return 0;
	}
	catch(X11Exception &e) {
		cerr << "xkb-switch: " << e.what() << endl;
		return 2;
	}
	catch(std::string & err) {
		cerr << "xkb-switch: " << err << endl;
		return 2;
	}
}
