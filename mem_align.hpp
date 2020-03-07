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

#include <stddef.h>

namespace s1b {

struct mem_align
{
    template <foffset_t Align>
    static foffset_t size(
        foffset_t size
    )
    {
        return (size == 0) ? Align : ((((size-1)/Align)+1)*Align);
    }

    template <foffset_t Align, foffset_t Size>
    struct size_s
    {
        static const foffset_t value = (Size == 0) ?
            Align : ((((Size-1)/Align)+1)*Align);
    };

    template <foffset_t Align>
    static bool is_aligned(
        foffset_t value
    )
    {
        return (value % Align) == 0;
    }

    template <foffset_t Align, foffset_t Value>
    struct is_aligned_s
    {
        static const bool value = (Value % Align) == 0;
    };
};

}
