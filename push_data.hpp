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
#include "open_mode.hpp"
#include "mem_align.hpp"
#include "exceptions.hpp"
#include "push_buffer.hpp"

namespace s1b {

class push_data
{
private:

    static const foffset_t Align = S1B_DATA_ALIGNMENT_BYTES;

private:

    push_buffer _buffer;
    foffset_t _slot_size;

    foffset_t assert_slot_size(
        foffset_t slot_size
    ) const
    {
        if (!mem_align::is_aligned<Align>(slot_size))
        {
            EX3_THROW(misaligned_exception()
                << actual_slot_size_ei(slot_size)
                << expected_alignment_ei(
                    static_cast<foffset_t>(Align))
                << file_name_ei(filename()));
        }

        return slot_size;
    }

public:

    push_data(
        const path_string& filename,
        bool create_new
    ) :
        _buffer(filename, create_new),
        _slot_size(assert_slot_size(_buffer.get_size()))
    {
        _buffer.seek(_slot_size);
    }

    const path_string& filename(
    ) const
    {
        return _buffer.filename();
    }

    bool can_write(
    ) const
    {
        return _buffer.can_write();
    }

    unsigned int num_slots(
    ) const
    {
        return 1;
    }

    foffset_t slot_size(
    ) const
    {
        return _slot_size;
    }

    size_t get_size(
    )
    {
        return _buffer.get_size();
    }

    foffset_t push(
        const void* data,
        foffset_t size
    )
    {
        const foffset_t aligned_size = mem_align::size<Align>(size);
        const foffset_t position = _slot_size;

        _buffer.write(data, size);
        _buffer.skip(aligned_size - size);

        _slot_size += aligned_size;

        return position;
    }

    void align(
    )
    {
        _buffer.sync();
    }

    void sync(
    )
    {
        _buffer.sync();
    }

    virtual ~push_data(
    )
    {
    }
};

}
