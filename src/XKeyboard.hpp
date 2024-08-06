/*
 * Copyright (C) 2008 by Jay Bromley <jbromley@gmail.com>
 * Copyright (C) 2010-2023 by Sergei Mironov
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

/** Interface for a class to get keyboard layout information and change layouts */

#ifndef XKEYBOARD_HPP
#define XKEYBOARD_HPP

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <map>
#include <string>

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

  // Returns fancy layout name as a string
  std::string get_long_group_name() const;

  // Waits for kb event
  void wait_event();
};

}

#endif

