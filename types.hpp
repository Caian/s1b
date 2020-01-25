/*
 * Copyright (C) 2020 Caian Benedicto <caianbene@gmail.com>
 *
 * This file is part of s1b.
 *
 * s1b is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * s1b is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Make; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#pragma once

#include "os/types.hpp"

#include <stdint.h>
#include <stdlib.h> // For size_t

namespace s1b {

// TODO rename to s1buid_t;
typedef uint64_t uid_t; /** The type used to uniquely identify
                            an element in the dataset. */

typedef int64_t foffset_t; /** The type used to handle offsets
                               in the files and data. */

static const uid_t FirstUID = 1;

}
