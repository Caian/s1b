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
#include "mem_align.hpp"

#include <boost/uuid/uuid.hpp>

namespace s1b {

class data_base
{
protected:

    static const foffset_t Align = S1B_DATA_ALIGNMENT_BYTES;

protected:

    foffset_t get_uuid_offset(
    ) const
    {
        return 0;
    }

    foffset_t get_data_offset(
    ) const
    {
        return
            get_uuid_offset() +
            mem_align::size_s<Align, sizeof(boost::uuids::uuid)>::value;
    }
};

}
