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

#define XKB_SWITCH_VERSION "1.3.1"

using namespace std;
using namespace kb;


void usage()
{
	cerr << "Usage: xkb-switch -s ARG            Sets current layout group to ARG" << endl;
	cerr << "       xkb-switch -l|--list         Displays all layout groups" << endl;
	cerr << "       xkb-switch -h|--help         Displays this message" << endl;
	cerr << "       xkb-switch -v|--version      Shows version number" << endl;
	cerr << "       xkb-switch -w|--wait [-p]    Waits for group change and exits" << endl;
	cerr << "       xkb-switch -W                Infinitely waits for group change" << endl;
	cerr << "       xkb-switch -n|--next         Switch to the next layout group" << endl;
	cerr << "       xkb-switch -x                Print X layout string" << endl;
	cerr << "       xkb-switch [-p]              Displays current layout group" << endl;
	cerr << "       xkb-switch --test            Run some internal tests" << endl;
}

string_vector (*parse)(const std::string&, const string_vector&) = parse3;

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

int run_tests()
{
	string kbs;
	string_vector sv;

	try {
		kbs = "us+sk1(qwerty1):2+at:3+us(alt-intl):4+inet(evdev)+compose(ralt)";
		sv = parse(kbs, nonsyms());
		CHECK(sv.at(0) == "us");
		CHECK(sv.at(1) == "sk1(qwerty1)");
		CHECK(sv.at(2) == "at");
		CHECK(sv.at(3) == "us(alt-intl)");
		cout << kbs << " " << print_layouts(sv) << endl;

		kbs = "us+sk(qwerty):2+at:3+us(alt-intl):4+inet(evdev)+compose(ralt)";
		sv = parse(kbs, nonsyms());
		CHECK(sv.at(0) == "us");
		CHECK(sv.at(1) == "sk(qwerty)");
		CHECK(sv.at(2) == "at");
		CHECK(sv.at(3) == "us(alt-intl)");
		cout << kbs << " " << print_layouts(sv) << endl;

		kbs = "pc+us+ru:2+inet(evdev)+group(alt_space_toggle)+ctrl(nocaps)+ctrl(swapcaps)+eurosign(e)";
		sv = parse(kbs, nonsyms());
		CHECK(sv.at(0) == "us");
		CHECK(sv.at(1) == "ru");
		cout << kbs << " " << print_layouts(sv) << endl;

		kbs = "pc_us_ru_2_inet(evdev)_group(switch)_group(alt_shift_toggle)_compose(rwin)_terminate(ctrl_alt_bksp)";
		sv = parse(kbs, nonsyms());
		CHECK(sv.at(0) == "us");
		CHECK(sv.at(1) == "ru");
		cout << kbs << " " << print_layouts(sv) << endl;

		kbs = "pc+us+inet(evdev)";
		sv = parse(kbs, nonsyms());
		CHECK(sv.at(0) == "us");
		cout << kbs << " " << print_layouts(sv) << endl;
		return 0;
	}
	catch (exception & e) {
		cerr << "xkb-switch: test failed: " << e.what() << endl;
		cerr << "xkb-switch: kbs: " << kbs << endl;
		cerr << "xkb-switch: layouts: " << print_layouts(sv) << endl;
		return 1;
	}
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
		int m_x = 0;
		int m_test = 0;
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
			else if(arg == "-x") {
				m_x = 1;
				m_cnt++;
			}
			else if(arg == "--test") {
				m_test = 1;
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

		if(m_test) {
			return run_tests();
		}

		if(m_wait) {
			xkb.wait_event();
		}

		if(m_lwait) {
			while(true) {
				xkb.wait_event();
				syms = parse(xkb.get_kb_string(), nonsyms());
				cout << syms.at(xkb.get_group()) << endl;
			}
		}

		string kbs = xkb.get_kb_string();
		if (m_x) {
			cout << kbs << endl;
		}

		syms = parse(kbs, nonsyms());

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
