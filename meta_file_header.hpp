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
#include "macros.hpp"
#include "exceptions.hpp"

#include <algorithm>

namespace s1b {

#pragma pack(push, 4)
struct meta_file_header
{
    char         magic[8]; //  0 - 7
    unsigned int one;      //  8 - 11
    unsigned int uintsz;   // 12 - 15
    unsigned int fofsz;    // 16 - 19
    unsigned int version;  // 20 - 23
    unsigned int revision; // 24 - 27
    unsigned int align;    // 28 - 31
    unsigned int checksz;  // 32 - 35
    unsigned int metasz;   // 36 - 39
    unsigned int globsz;   // 40 - 44

    meta_file_header(
    ) :
        magic(),
        one(0),
        uintsz(0),
        fofsz(0),
        version(0),
        revision(0),
        align(0),
        checksz(0),
        metasz(0),
        globsz(0)
    {
        magic[0] = 0;
        magic[1] = 0;
        magic[2] = 0;
        magic[3] = 0;
        magic[4] = 0;
        magic[5] = 0;
        magic[6] = 0;
        magic[7] = 0;
    }

    meta_file_header(
        unsigned int align,
        unsigned int checksz,
        unsigned int metasz,
        unsigned int globsz
    ) :
        magic(),
        one(1),
        uintsz(sizeof(unsigned int)),
        fofsz(sizeof(foffset_t)),
        version((S1B_VERSION_MAJOR << 16) | S1B_VERSION_MINOR),
        revision(S1B_VERSION_REVISION),
        align(align),
        checksz(checksz),
        metasz(metasz),
        globsz(globsz)
    {
        magic[0] = S1B_MAGIC0;
        magic[1] = S1B_MAGIC1;
        magic[2] = S1B_MAGIC2;
        magic[3] = S1B_MAGIC3;
        magic[4] = S1B_MAGIC4;
        magic[5] = S1B_MAGIC5;
        magic[6] = S1B_MAGIC6;
        magic[7] = S1B_MAGIC7;
    }

    void assert_other(
        const meta_file_header& other
    ) const
    {
        // TODO assert each field

        const char* const p_other = reinterpret_cast
            <const char*>(&other);

        const char* const p_this_begin = reinterpret_cast
            <const char*>(this);

        const char* const p_this_end = p_this_begin +
            sizeof(meta_file_header);

        std::pair<const char*, const char*> comp;

        comp = std::mismatch(p_this_begin, p_this_end, p_other);

        if (comp.first != p_this_end)
        {
            EX3_THROW(header_mismatch_exception()
                << expected_value_ei(*comp.first)
                << actual_value_ei(*comp.second)
                << offset_ei(std::distance(p_this_begin, comp.first)));
        }

    }
};
#pragma pack(pop)

}
