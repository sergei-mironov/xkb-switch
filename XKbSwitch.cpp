/*
 * Plain launcher program for Jay Bromley's Xkeyboard library.
 *
 * Copyright (C) 2010 by Sergey Mironov <ierton@gmail.com>
 *
 * This file is part of Xkb-switch.
 * Xkb-switch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xkb-switch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xkb-switch. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <algorithm>
#include "XKeyboard.h"
#include "X11Exception.h"
#include <X11/XKBlib.h>
#include <sstream>

#define XKB_SWITCH_VERSION "1.4.0"

using namespace std;
using namespace kb;


void usage()
{
	cerr << "Usage: xkb-switch -s ARG            Sets current layout group to ARG" << endl;
	cerr << "       xkb-switch -l|--list         Displays all layout groups" << endl;
	cerr << "       xkb-switch -h|--help         Displays this message" << endl;
	cerr << "       xkb-switch -v|--version      Shows version number" << endl;
	cerr << "       xkb-switch -w|--wait [-p]    Waits for group change" << endl;
	cerr << "       xkb-switch -W                Infinitely waits for group change, prints group names to stdout" << endl;
	cerr << "       xkb-switch -n|--next         Switch to the next layout group" << endl;
	cerr << "       xkb-switch [-p]              Displays current layout group" << endl;
}

string print_layouts(const string_vector& sv)
{
	ostringstream oss;
	bool fst = true;

	oss << "[";
	for(string_vector::const_iterator i=sv.begin(); i!=sv.end(); i++) {
		if(!fst) oss << " ";
		oss << *i;
		fst = false;
	}
	oss << "]";
	return oss.str();
}

int main( int argc, char* argv[] )
{
	string_vector syms;

	using namespace std;
	try {
		int m_cnt = 0;
		int m_wait = 0;
		int m_lwait = 0;
		int m_print = 0;
		int m_next = 0;
		int m_list = 0;
		string newgrp;

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

		XKeyboard xkb;
    xkb.open_display();

		if(m_wait) {
			xkb.wait_event();
		}

		if(m_lwait) {
			while(true) {
				xkb.wait_event();
				xkb.build_layout(syms);
				cout << syms.at(xkb.get_group()) << endl;
			}
		}

		if (m_lwait)
			syms.clear();

		xkb.build_layout(syms);

		if (m_next) {
			CHECK_MSG(!syms.empty(), "No layout groups configured");
			const string nextgrp = syms.at(xkb.get_group());
			string_vector::iterator i = find(syms.begin(), syms.end(), nextgrp);
			if (++i == syms.end()) i = syms.begin();
			xkb.set_group(i - syms.begin());
		}
		else if(!newgrp.empty()) {
			string_vector::iterator i = find(syms.begin(), syms.end(), newgrp);
			CHECK_MSG(i!=syms.end(),
				"Group " << newgrp << "' is not supported by current layout. Try xkb-switch -l.");
			xkb.set_group(i-syms.begin());
		}

		if(m_print) {
			cout << syms.at(xkb.get_group()) << endl;
		}

		if(m_list) {
			for(int i=0; i<syms.size(); i++) {
				cout << syms[i] << endl;
			}
		}
		return 0;
	}
	catch(X11Exception &err) {
		cerr << "xkb-switch: " << err.what() << endl;
		cerr << "xkb-switch: layouts: " << print_layouts(syms) << endl;
		return 2;
	}
	catch(std::string & err) {
		cerr << "xkb-switch: " << err << endl;
		cerr << "xkb-switch: layouts: " << print_layouts(syms) << endl;
		return 2;
	}
	catch(std::exception & err) {
		cerr << "xkb-switch: " << err.what() << endl;
		cerr << "xkb-switch: layouts: " << print_layouts(syms) << endl;
	}
}
