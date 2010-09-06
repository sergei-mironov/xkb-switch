// xkeyboard.h
// Interface for a class to get keyboard layout information and change layouts
// Copyright (C) 2008 by Jay Bromley <jbromley@gmail.com>
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
//
// $Id: XKeyboard.h 29 2008-04-09 21:37:44Z jay $

#ifndef XKEYBOARD_H_1C79861A_49B3_4A95_88D6_455C22FEB222
#define XKEYBOARD_H_1C79861A_49B3_4A95_88D6_455C22FEB222

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>


typedef std::vector<std::string> StringVector;


#define CHECK_MSG(x,msg) do{ \
		if(!(x)) { \
			std::ostringstream oss; \
			oss << __FILE__ << ":" << __LINE__ << ": Condition " << #x << " failed. " << msg; \
			throw oss.str(); \
		} \
	} while(0)

#define CHECK(x) CHECK_MSG(x,"")


class XKeyboard
{
public:
    XKeyboard();
    ~XKeyboard();
    int getCurrentGroupNum() const;
    void setGroupByNum(int groupNum);
    StringVector getSymNames();

    Display* _display;
    int _deviceId;
    XkbDescRec* _kbdDescPtr;
};


class XkbSymbolParser
{
public:
    typedef std::vector<std::string>::iterator StringVectorIter;

    XkbSymbolParser();
    ~XkbSymbolParser();
    void parse(const std::string& symbols, std::vector<std::string>& symbolList);
	
private:
    bool isXkbLayoutSymbol(const std::string& symbol);
    StringVector _nonSymbols;
};

#endif // XKEYBOARD_H_1C79861A_49B3_4A95_88D6_455C22FEB222

// Local Variables:
// mode: c++
// End:
