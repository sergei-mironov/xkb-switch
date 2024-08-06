/*
 * Copyright (C) 2008 by Jay Bromley <jbromley@gmail.com>
 * Copyright (C) 2010-2024 by Sergei Mironov
 *
 * This file is part of Xkb-switch.
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/** Implementation of a class to get keyboard layout information and change layouts */

#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <cstring>

#include <iostream>
#include <string>
#include <sstream>

#include <X11/XKBlib.h>
#include <X11/extensions/XKBrules.h>

#include "XKeyboard.hpp"
#include "Utils.hpp"

using namespace std;

namespace kb {

XKeyboard::XKeyboard(size_t verbose)
  : _display(0), _deviceId(XkbUseCoreKbd), _kbdDescPtr(0), _verbose(verbose)
{
}

void XKeyboard::open_display()
{

  XkbIgnoreExtension(False);

  char* displayName = strdup(""); // allocates memory for string!
  int eventCode;
  int errorReturn;
  int major = XkbMajorVersion;
  int minor = XkbMinorVersion;
  int reasonReturn;

  _display = XkbOpenDisplay(displayName, &eventCode, &errorReturn, &major,
      &minor, &reasonReturn);
  free(displayName);
  switch (reasonReturn) {
    case XkbOD_Success:           break;
    case XkbOD_BadLibraryVersion: THROW_MSG(_verbose, "Bad XKB library version.");
    case XkbOD_ConnectionRefused: THROW_MSG(_verbose, "Connection to X server refused.");
    case XkbOD_BadServerVersion:  THROW_MSG(_verbose, "Bad X11 server version.");
    case XkbOD_NonXkbServer:      THROW_MSG(_verbose, "XKB not present.");
    default:                      THROW_MSG(_verbose, "XKB refused to open the display with reason '" << reasonReturn << "'.");
  }

  _kbdDescPtr = XkbAllocKeyboard();
  if (_kbdDescPtr == NULL) {
    THROW_MSG(_verbose, "Failed to get keyboard description.");
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

  if (_display!=NULL) {
    XCloseDisplay(_display);
  }
}

// XkbRF_VarDefsRec contains heap-allocated C strings, but doesn't provide a
// direct cleanup method. This wrapper privides a workaround.
// See also https://gitlab.freedesktop.org/xorg/lib/libxkbfile/issues/6
struct XkbRF_VarDefsRec_wrapper {

  XkbRF_VarDefsRec _it;

  XkbRF_VarDefsRec_wrapper() {
    std::memset(&_it,0,sizeof(_it));
  }

  ~XkbRF_VarDefsRec_wrapper() {
    if(_it.model) std::free(_it.model);
    if(_it.layout) std::free(_it.layout);
    if(_it.variant) std::free(_it.variant);
    if(_it.options) std::free(_it.options);
  }
};

struct XkbKeyboardWrapper {
  XkbDescPtr ptr;
  XkbKeyboardWrapper(XkbDescPtr p) : ptr(p) {}
  ~XkbKeyboardWrapper() {
    if (ptr) {

      XkbFreeKeyboard(ptr, 0, True);
    }
  }
  // Disable copying
  XkbKeyboardWrapper(const XkbKeyboardWrapper&) = delete;
  XkbKeyboardWrapper& operator=(const XkbKeyboardWrapper&) = delete;
};

struct XGetAtomNameWrapper {
  Display* display;
  char* ptr;
  XGetAtomNameWrapper(Display* d, Atom atom) 
    : display(d), ptr(XGetAtomName(d, atom)) {}
  ~XGetAtomNameWrapper() {
    if (ptr) {
      XFree(ptr);
    }
  }
};

layout_variant_strings XKeyboard::get_layout_variant()
{
  XkbRF_VarDefsRec_wrapper vdr;
  char* tmp = NULL;
  Bool bret;

  bret = XkbRF_GetNamesProp(_display, &tmp, &vdr._it);
  free(tmp);  // return memory allocated by XkbRF_GetNamesProp
  CHECK_MSG(_verbose, bret==True, "Failed to get keyboard properties");

  MSG(_verbose, "raw layout string \"" << vdr._it.layout << "\"");
  if(vdr._it.variant)
    MSG(_verbose, "raw variant string \"" << vdr._it.variant << "\"");
  else
    MSG(_verbose, "No raw variant string");

  return make_pair(string(vdr._it.layout ? vdr._it.layout : "us"),
                   string(vdr._it.variant ? vdr._it.variant : ""));
}

void XKeyboard::build_layout_from(string_vector& out, const layout_variant_strings& lv)
{
  std::istringstream layout(lv.first);
  std::istringstream variant(lv.second);

  out.clear();

  while(true) {
    string l,v;

    getline(layout, l, ',');
    getline(variant, v, ',');
    MSG(_verbose, "layout \"" << l << "\", variant \"" << v << "\"");
    if(!layout && !variant) {
      break;
    }

    if(v!="") {
      v = "(" + v + ")";
    }
    if(l!="") {
      MSG(_verbose, "... accepting with id #" << out.size());
      out.push_back(l+v);
    }
    else {
      MSG(_verbose, "... ignoring");
    }
  }
}


void XKeyboard::build_layout(string_vector& out)
{
  layout_variant_strings lv=this->get_layout_variant();
  build_layout_from(out, lv);
}

void XKeyboard::wait_event()
{
  CHECK(_verbose, _display != 0);

  Bool bret = XkbSelectEventDetails(_display, XkbUseCoreKbd,
      XkbStateNotify, XkbAllStateComponentsMask, XkbGroupStateMask);
  CHECK_MSG(_verbose, bret==True, "XkbSelectEventDetails failed");

  XEvent event;
  int iret = XNextEvent(_display, &event);
  CHECK_MSG(_verbose, iret==0, "XNextEvent failed with " << iret);
}

void XKeyboard::set_group(int groupNum)
{
  Bool result = XkbLockGroup(_display, _deviceId, groupNum);
  CHECK(_verbose, result == True);
  XFlush(_display);
}

int XKeyboard::get_group() const
{
  XkbStateRec xkbState;
  XkbGetState(_display, _deviceId, &xkbState);
  return static_cast<int>(xkbState.group);
}

std::string XKeyboard::get_long_group_name() const
{
  if (_display == nullptr) {
    throw std::runtime_error("Display not opened.");
  }

  XkbStateRec xkbState;

  if (XkbGetState(_display, _deviceId, &xkbState) != Success) {
    throw std::runtime_error("Failed to get keyboard state.");
  }

  XkbDescPtr descPtr = XkbGetKeyboard(_display, XkbAllComponentsMask, _deviceId);
  if (descPtr == nullptr) {
    throw std::runtime_error("Failed to get keyboard description.");
  }

  XkbKeyboardWrapper desc(descPtr);

  if (desc.ptr->names == nullptr) {
    throw std::runtime_error("Failed to get keyboard names.");
  }

  Status ErrorGetControls = XkbGetControls(_display, XkbAllComponentsMask, desc.ptr);
  if (ErrorGetControls != Success || desc.ptr->ctrls == nullptr) {
    throw std::runtime_error("Failed to get keyboard controls.");
  }

  int num_groups = desc.ptr->ctrls->num_groups;
  if (xkbState.group >= num_groups) {
    throw std::runtime_error("Group index out of range.");
  }

  XGetAtomNameWrapper groupName(_display, desc.ptr->names->groups[xkbState.group]);
  if (groupName.ptr == nullptr) {
    throw std::runtime_error("Failed to get group name.");
  }

  std::string longGroupName = groupName.ptr;

  return longGroupName;
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

