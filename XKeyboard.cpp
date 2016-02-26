/*
 * Implementation of a class to get keyboard layout information and change layouts
 *
 * Copyright (C) 2008 by Jay Bromley <jbromley@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 */

#include "XKeyboard.h"
#include "X11Exception.h"
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <X11/XKBlib.h>
#include "X11/extensions/XKBrules.h"

#ifndef DFLT_XKB_LAYOUT
#define	DFLT_XKB_LAYOUT "us"
#endif

#ifndef NO_KEYBOARD
#define	NO_KEYBOARD "no keyboard"
#endif


#include <iostream>
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

namespace kb {

XKeyboard::XKeyboard()
  : _display(0), _deviceId(XkbUseCoreKbd)
{

  XkbIgnoreExtension(False);

  char* displayName = strdup("");
  int eventCode;
  int errorReturn;
  int major = XkbMajorVersion;
  int minor = XkbMinorVersion;
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


void XKeyboard::BuildLayout(string_vector& vec) {
	XkbRF_VarDefsRec vdr;
	int i = 0;
	char str[20] = {0};
	const char s[2] = ",";
	char *token;
	char* tmp = NULL;

  	strcpy(str, (
    !_display ? NO_KEYBOARD :
    (XkbRF_GetNamesProp(_display, &tmp, &vdr) && vdr.layout) ?
    vdr.layout : DFLT_XKB_LAYOUT));

	/* get the first token */
	token = strtok(str, s);

	/* walk through other tokens */
	while( token != NULL )
	{
	  vec.push_back(token);
	  token = strtok(NULL, s);
	}
}



void XKeyboard::wait_event()
{
  CHECK(_display != 0);

  Bool bret = XkbSelectEventDetails(_display, XkbUseCoreKbd, 
      XkbStateNotify, XkbAllStateComponentsMask, XkbGroupStateMask);
  CHECK_MSG(bret==True, "XkbSelectEventDetails failed");

  XEvent event;
  int iret = XNextEvent(_display, &event);
  CHECK_MSG(iret==0, "XNextEvent failed with " << iret);
}

void XKeyboard::set_group(int groupNum)
{
  Bool result = XkbLockGroup(_display, _deviceId, groupNum);
  CHECK(result == True);
}

int XKeyboard::get_group() const
{
  XkbStateRec xkbState;
  XkbGetState(_display, _deviceId, &xkbState);
  return static_cast<int>(xkbState.group);
}

// returns true if symbol is ok
bool filter(const string_vector& nonsyms, const std::string& symbol)
{
  if(symbol.empty())
    return false;

  // Filter out all prohibited words
  string_vector::const_iterator r = find(nonsyms.begin(), nonsyms.end(), symbol);
  if(r != nonsyms.end())
    return false;

  // Filter out all numbers groups started with number
  if(isdigit(symbol[0]))
    return false;

  return true;
}

string_vector parse1(const std::string& symbols, const string_vector& nonsyms)
{
  bool inSymbol = false;
  std::string sym;
  string_vector symlist;

  for (size_t i = 0; i < symbols.size(); i++) {
    char ch = symbols[i];
    if (ch == '+') {
      if (inSymbol && !sym.empty() && filter(nonsyms, sym)) {
        symlist.push_back(sym);
      }
      inSymbol = true;
      sym.clear();
    }
    else if (inSymbol && ch == '(') {
      inSymbol = false;
    }
    else if (inSymbol && (isalpha(static_cast<int>(ch)) || ch == '_')) {
      sym.append(1, ch);
    }
    else {
      if (inSymbol && !sym.empty() && filter(nonsyms, sym)) {
        symlist.push_back(sym);
      }
      inSymbol = false;
    }
  }

  if (inSymbol && !sym.empty() && filter(nonsyms, sym)) {
    symlist.push_back(sym);
  }

  return symlist;
}

void safe_push_back(string_vector& v, std::string s, std::string note)
{
  if(s.empty()) return;
  if(!note.empty()) {
    s += "(" + note + ")";
  }
  v.push_back(s);
}

bool goodchar(char ch)
{
  return (isdigit(ch) || isalpha(static_cast<int>(ch)) || ch == '_' || ch == '-');
}

string_vector parse2(const std::string& symbols, const string_vector& nonsyms)
{
  enum{ok,skip,broken} state = ok;
  int paren = 0;
  std::string sym;
  // Words between optional '(' ')'
  std::string note;
  string_vector symlist;

  for (size_t i = 0; i < symbols.size(); i++) {
    char ch = symbols[i];

    if (ch == '+') {
      if (state != broken && paren == 0 && filter(nonsyms, sym)) {
        safe_push_back(symlist, sym, note);
      }
      state = ok;
      paren = 0;
      sym.clear();
      note.clear();
    }
    else if (state == ok && ch == '(') {
      paren++;
    }
    else if (state == ok && ch == ')') {
      paren--;
    }
    else if (state == ok && ch == ':') {
      state = skip;
    }
    else if (state == ok && goodchar(ch)) {
      if (paren == 0)
        sym.append(1, ch);
      else
        note.append(1, ch);
    }
    else if(state == ok) {
      state = broken;
    }
  }

  if (state != broken && paren == 0 && filter(nonsyms, sym)) {
    safe_push_back(symlist, sym, note);
  }

  return symlist;
}

string_vector parse3(const std::string& symbols, const string_vector& nonsyms)
{
  enum{ok,skip,broken} state = ok;
  int paren = 0;
  std::string sym;
  // Words between optional '(' ')'
  std::string note;
  string_vector symlist;

  for (size_t i = 0; i < symbols.size(); i++) {
    char ch = symbols[i];

    if (ch == '+' || ch == '_') {
      if(paren == 0) {
        if (state != broken && paren == 0 && filter(nonsyms, sym)) {
          safe_push_back(symlist, sym, note);
        }
        state = ok;
        sym.clear();
        note.clear();
      }
    }
    else if (state == ok && ch == '(') {
      paren++;
    }
    else if (state == ok && ch == ')') {
      paren--;
    }
    else if (state == ok && ch == ':') {
      state = skip;
    }
    else if (state == ok && goodchar(ch)) {
      if (paren == 0)
        sym.append(1, ch);
      else
        note.append(1, ch);
    }
    else if(state == ok) {
      state = broken;
    }
  }

  if (state != broken && paren == 0 && filter(nonsyms, sym)) {
    safe_push_back(symlist, sym, note);
  }

  return symlist;
}

}

