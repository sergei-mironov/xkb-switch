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

/** C++ exceptions that wraps X11 errors. */

#ifndef XKBSWITCH_HPP
#define XKBSWITCH_HPP

#include <exception>
#include <stdexcept>
#include <iostream>

#define MSG(verbose, msg) do { \
  if (verbose >= 2) { \
    std::cerr << __FILE__ << ":" << __LINE__ << ": "; \
    std::cerr << msg << std::endl; \
  } \
} while(0)

#define THROW_MSG(verbose, msg) do { \
  std::ostringstream oss; \
  if (verbose >= 2) { \
    oss << __FILE__ << ":" << __LINE__ << ": "; \
  } \
  oss << msg; \
  throw std::runtime_error(oss.str()); \
} while(0)

#define CHECK_MSG(verbose, x, msg) do { \
  if(!(x)) { \
    std::ostringstream oss; \
    if (verbose >= 2) { \
      oss << __FILE__ << ":" << __LINE__ << ": Condition " << #x << " failed. "; \
    } \
    oss << msg; \
    throw std::runtime_error(oss.str()); \
  } \
} while(0)

#define CHECK(verbose, x) CHECK_MSG(verbose, x, "")

#endif
