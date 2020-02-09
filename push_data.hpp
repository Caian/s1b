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

#include "types.hpp"
#include "macros.hpp"
#include "open_mode.hpp"
#include "mem_align.hpp"
#include "rwp_buffer.hpp"
#include "exceptions.hpp"

namespace s1b {

class push_data
{
private:

    static const foffset_t Align = S1B_DATA_ALIGNMENT_BYTES;

private:

    rwp_buffer _buffer;
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
        _buffer(
            filename,
            create_new ? S1B_OPEN_NEW : S1B_OPEN_WRITE),
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
    ) const
    {
        return _buffer.get_size();
    }

    foffset_t push(
        const void* data,
        foffset_t size
    )
    {
        _buffer.write(data, size);

        const foffset_t position = _slot_size;

        _slot_size += mem_align::size<Align>(size);

        _buffer.seek(_slot_size);

        return position;
    }

    void align(
    )
    {
        const foffset_t file_size = _buffer.get_size();

        if (!mem_align::is_aligned<Align>(_slot_size))
        {
            EX3_THROW(misaligned_exception()
                << actual_slot_size_ei(_slot_size)
                << expected_alignment_ei(
                    static_cast<foffset_t>(Align))
                << file_name_ei(filename()));
        }

        if (_slot_size < file_size)
        {
            EX3_THROW(file_size_mismatch_exception()
                << file_size_ei(file_size)
                << file_position_ei(_slot_size)
                << file_name_ei(filename()));
        }

        if (_slot_size > file_size)
        {
            _buffer.seek(_slot_size - 1);
            _buffer.write("", 1);
        }
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
