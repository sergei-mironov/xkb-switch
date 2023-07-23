/*
 * C++ exceptions that wraps X11 errors.
 *
 * Copyright (C) 2008 by Jay Bromley <jbromley@gmail.com>
 * Copyright (C) 2010-2023 by Sergei Mironov
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
