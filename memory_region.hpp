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
 * along with s1b.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "types.hpp"
#include "open_mode.hpp"
#include "exceptions.hpp"
#include "path_string.hpp"
#include "os/functions.hpp"
#include "hugetlb_state.hpp"

#include <string>

namespace s1b {

struct memory_region
{
    char* address;
    size_t size;
    hugetlb_state htlb_state;

    memory_region(
    ) :
        address(0),
        size(0),
        htlb_state(S1B_HUGETLB_UNSUPPORTED)
    {
    }

    memory_region(
        char* address,
        size_t size,
        hugetlb_state htlb_state
    ) :
        address(address),
        size(size),
        htlb_state(htlb_state)
    {
    }
};

}
