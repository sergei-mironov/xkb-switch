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
#include <X11/extensions/XKBrules.h>

#include <iostream>
#include <string>
#include <sstream>

namespace kb {

XKeyboard::XKeyboard()
  : _display(0), _deviceId(XkbUseCoreKbd)
{
}

void XKeyboard::open_display()
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


void XKeyboard::build_layout(string_vector& out)
{
  using namespace std;

  XkbRF_VarDefsRec vdr;
  char* tmp = NULL;
  Bool bret;

  bret = XkbRF_GetNamesProp(_display, &tmp, &vdr);
  CHECK_MSG(bret==True, "Failed to get keyboard properties");

  istringstream layout(vdr.layout ? vdr.layout : "us");
  istringstream variant(vdr.variant ? vdr.variant : "");

  while(true) {
    string l,v;

    getline(layout, l, ',');
    getline(variant, v, ',');
    if(!layout && !variant)
      break;

    if(v!="") {
      v = "(" + v + ")";
    }
    if(l!="") {
      out.push_back(l+v);
    }
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

}

