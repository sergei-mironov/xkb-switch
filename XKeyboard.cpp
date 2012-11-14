// xkeyboard.cpp
// Implementation of a class to get keyboard layout information and change layouts
// Copyright (C) 2008 by Jay Bromley <jbromley@gmail.com>
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
//
// $Id: XKeyboard.cpp 53 2008-07-18 08:38:47Z jay $

#include "XKeyboard.h"
#include "X11Exception.h"
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <cstring>

// XKeyboard -----------------------------------------------------------

XKeyboard::XKeyboard()
    : _display(0), _deviceId(XkbUseCoreKbd)
{

    XkbIgnoreExtension(False);

    char* displayName = strdup("");
    int eventCode;
    int errorReturn;
    int major = XkbMajorVersion;
    int minor = XkbMinorVersion;;
    int reasonReturn;
    _display = XkbOpenDisplay(displayName, &eventCode, &errorReturn, &major,
                              &minor, &reasonReturn);
    switch (reasonReturn) {
    case XkbOD_BadLibraryVersion:
        throw X11Exception("Bad XKB library version.");
        break;
    case XkbOD_ConnectionRefused:
        throw X11Exception("Connection to X server refused.");
        break;
    case XkbOD_BadServerVersion:
        throw X11Exception("Bad X11 server version.");
        break;
    case XkbOD_NonXkbServer:
        throw X11Exception("XKB not present.");
        break;
    case XkbOD_Success:
        break;
    }

    _kbdDescPtr = XkbAllocKeyboard();
    if (_kbdDescPtr == NULL) {
	XCloseDisplay(_display);
        throw X11Exception("Failed to get keyboard description.");
    }

    _kbdDescPtr->dpy = _display;
    if (_deviceId != XkbUseCoreKbd) {
        _kbdDescPtr->device_spec = _deviceId;
    }
}

XKeyboard::~XKeyboard()
{
    if(_kbdDescPtr!=NULL)
	XkbFreeKeyboard(_kbdDescPtr, 0, True);

    XCloseDisplay(_display);
}

StringVector XKeyboard::getSymNames()
{
    XkbGetControls(_display, XkbAllControlsMask, _kbdDescPtr);
    XkbGetNames(_display, XkbSymbolsNameMask, _kbdDescPtr);

    Atom symNameAtom = _kbdDescPtr->names->symbols;
    CHECK(symNameAtom != None);

    char* symNameC = XGetAtomName(_display, symNameAtom);
    std::string symName(symNameC);
    XFree(symNameC);

    CHECK(!symName.empty());

    StringVector symNames;
    XkbSymbolParser symParser;
    symParser.parse(symName, symNames);
    return symNames;
}

void XKeyboard::setGroupByNum(int groupNum)
{
    Bool result = XkbLockGroup(_display, _deviceId, groupNum);
    CHECK(result == True);
}

int XKeyboard::getCurrentGroupNum() const
{
    XkbStateRec xkbState;
    XkbGetState(_display, _deviceId, &xkbState);
    return static_cast<int>(xkbState.group);
}

// XkbSymbolParser -----------------------------------------------------

XkbSymbolParser::XkbSymbolParser()
{
    _nonSymbols.push_back("group");
    _nonSymbols.push_back("inet");
    _nonSymbols.push_back("ctr");
    _nonSymbols.push_back("pc");
    _nonSymbols.push_back("ctrl");
}

XkbSymbolParser::~XkbSymbolParser()
{
    _nonSymbols.clear();
}

void XkbSymbolParser::parse(const std::string& symbols, StringVector& symbolList)
{
    bool inSymbol = false;
    std::string curSymbol;

    for (int i = 0; i < symbols.size(); i++) {
        char ch = symbols[i];
        if (ch == '+') {
            if (inSymbol) {
                if (isXkbLayoutSymbol(curSymbol)) {
                    symbolList.push_back(curSymbol);
                }
                curSymbol.clear();
            } else {
                inSymbol = true;
            }
        } else if (inSymbol && (isalpha(static_cast<int>(ch)) || ch == '_')) {
            curSymbol.append(1, ch);
        } else {
            if (inSymbol) {
                if (isXkbLayoutSymbol(curSymbol) && ch != '(') {
                    symbolList.push_back(curSymbol);
                }
                curSymbol.clear();
                inSymbol = false;
            }
        }
    }

    if (inSymbol && !curSymbol.empty() && isXkbLayoutSymbol(curSymbol)) {
        symbolList.push_back(curSymbol);
    }
}

bool XkbSymbolParser::isXkbLayoutSymbol(const std::string& symbol) {
    StringVectorIter result = find(_nonSymbols.begin(), _nonSymbols.end(), symbol);
    return result == _nonSymbols.end();
}

