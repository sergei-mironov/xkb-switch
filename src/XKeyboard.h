/*
 * Interface for a class to get keyboard layout information and change layouts
 *
 * Copyright (C) 2008 by Jay Bromley <jbromley@gmail.com>
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

#ifndef XKEYBOARD_H
#define XKEYBOARD_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <map>
#include <string>

using std::string;

namespace kb {

typedef std::vector<std::string> string_vector;
typedef std::pair<std::string,std::string> layout_variant_strings;

class XKeyboard
{
public:

  Display* _display;
  int _deviceId;
  XkbDescRec* _kbdDescPtr;
  size_t _verbose;

  XKeyboard(size_t verbose);
  ~XKeyboard();

  // Opens display (or throw std::runtime_error)
  void open_display(void);

  // Gets the current layout
  int get_group() const;

  // Sets the layout
  void set_group(int num);

  // Return layout/variant strings
  layout_variant_strings get_layout_variant();

  // Returns keyboard layout string
  void build_layout_from(string_vector& vec, const layout_variant_strings& lv);
  void build_layout(string_vector& vec);

  // Waits for kb event
  void wait_event();
};

}

#endif

