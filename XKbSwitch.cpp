/*
 * Plain launcher program for Jay Bromley's xkb library.
 *
 * Copyright (C) 2010 by Sergey Mironov <ierton@gmail.com>
 *
 */

#include <iostream>
#include <algorithm>
#include "XKeyboard.h"
#include "X11Exception.h"

#define XKB_SWITCH_VERSION "1.0"

int main( int argc, char* argv[] ) 
{
	using namespace std;

	try {
		XKeyboard xkb;

		StringVector groups;
		groups = xkb.groupSymbols();

		for(int i=1; i<argc; i) {
			string arg(argv[i++]);
			if(arg == "-s") {
				if(i>=argc) throw string("Argument expected");
				string newgrp(argv[i++]);
				if(i!=argc) throw string("Too many arguments");
				StringVector::iterator i = find(groups.begin(), groups.end(), newgrp);
				if(i==groups.end()) throw string("Group '") + newgrp + 
					"' is not supported by current layout. Try xkb-switch -l.";
				return xkb.setGroupByNum(i-groups.begin());
			}
			else if(arg == "-l" || arg == "--list") {
				if(i!=argc) throw string("Too many arguments");
				for(int i=0; i<groups.size(); i++) {
					cout << groups[i] << endl;
				}
				return 0;
			}
			else if(arg == "-h" || arg == "--help") {
				if(i!=argc) throw string("Too many arguments");
				cerr << "Usage: xkb-switch -s ARG        Sets current layout group to ARG" << endl;
				cerr << "       xkb-switch -l|--list     Displays all layout groups" << endl;
				cerr << "       xkb-switch               Displays current layout group" << endl;
				cerr << "       xkb-switch -h|--help     Displays this message" << endl;
				cerr << "       xkb-switch -v|--version  Shows version number" << endl;
				return 1;
			}
			else if(arg == "-v" || arg=="--version") {
				if(i!=argc) throw string("Too many arguments");
				cerr << "xkb-switch " << XKB_SWITCH_VERSION << endl;
				return 0;
			}
			else {
				throw string("Invalid argument: " + arg);
			}
		}

		cout << xkb.currentGroupSymbol() << endl;
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
