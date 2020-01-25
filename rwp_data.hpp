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

class rwp_data
{
private:

    static const foffset_t Align = S1B_DATA_ALIGNMENT_BYTES;

private:

    foffset_t _slot_size;
    rwp_buffer _buffer;
    unsigned int _num_slots;

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

    unsigned int initialize_num_slots(
        unsigned int num_slots
    )
    {
        if (num_slots == 0)
        {
            EX3_THROW(invalid_num_slots_exception()
                << actual_num_slots_ei(num_slots)
                << file_name_ei(filename()));
        }

        const foffset_t data_size = _slot_size * num_slots;

        if (data_size > 0)
        {
            _buffer.seek(data_size - 1);
            _buffer.write("", 1);
        }

        return num_slots;
    }

    unsigned int compute_num_slots(
        unsigned int expected
    ) const
    {
        const foffset_t data_size = _buffer.get_size();
        const unsigned int num_slots = data_size / _slot_size;
        const foffset_t extra_bytes = data_size % _slot_size;

        if (!mem_align::is_aligned<Align>(data_size))
        {
            EX3_THROW(misaligned_exception()
                << file_size_ei(data_size)
                << expected_alignment_ei(
                    static_cast<foffset_t>(Align))
                << file_name_ei(filename()));
        }

        if (extra_bytes != 0)
        {
            EX3_THROW(extra_slot_bytes_exception()
                << file_size_ei(data_size)
                << actual_slot_size_ei(_slot_size)
                << actual_num_slots_ei(num_slots)
                << file_name_ei(filename()));
        }

        if (expected != 0 && expected != num_slots)
        {
            EX3_THROW(num_slots_mismatch_exception()
                << expected_num_slots_ei(expected)
                << actual_num_slots_ei(num_slots)
                << file_size_ei(data_size)
                << file_name_ei(filename()));
        }

        return num_slots;
    }

public:

    rwp_data(
        const path_string& filename
    ) :
        _slot_size(
            assert_slot_size(0)),
        _buffer(
            filename,
            S1B_OPEN_NEW),
        _num_slots(1)
    {
    }

    template <typename Metadata>
    rwp_data(
        const path_string& filename,
        open_mode mode,
        const Metadata& metadata,
        unsigned int num_slots
    ) :
        _slot_size(
            assert_slot_size(metadata.get_data_size())),
        _buffer(filename, mode),
        _num_slots(((mode & S1B_OPEN_TRUNC) != 0) ?
            initialize_num_slots(num_slots) :
            compute_num_slots(num_slots))
    {
    }

    template <typename Metadata>
    rwp_data(
        const path_string& filename,
        open_mode mode,
        Metadata& metadata,
        unsigned int num_slots
    ) :
        _slot_size(
            assert_slot_size(metadata.get_data_size())),
        _buffer(filename, mode),
        _num_slots(((mode & S1B_OPEN_TRUNC) != 0) ?
            initialize_num_slots(num_slots) :
            compute_num_slots(num_slots))
    {
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
        return _num_slots;
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

    void read(
        void* data,
        foffset_t position,
        foffset_t size,
        unsigned int slot=0
    )
    {
        if (!mem_align::is_aligned<Align>(position))
        {
            EX3_THROW(misaligned_exception()
                << file_position_ei(position)
                << expected_alignment_ei(
                    static_cast<foffset_t>(Align))
                << file_name_ei(filename()));
        }

        if (slot >= _num_slots)
        {
            EX3_THROW(invalid_slot_exception()
                << maximum_slot_ei(_num_slots-1)
                << requested_slot_ei(slot)
                << file_name_ei(filename()));
        }

        position += slot * _slot_size;

        // TODO assert seek returns
        _buffer.seek(position);
        _buffer.read(data, size, true, true);
    }

    void write(
        const void* data,
        foffset_t position,
        foffset_t size,
        unsigned int slot=0
    )
    {
        if (!mem_align::is_aligned<Align>(position))
        {
            EX3_THROW(misaligned_exception()
                << file_position_ei(position)
                << expected_alignment_ei(
                    static_cast<foffset_t>(Align))
                << file_name_ei(filename()));
        }

        if (slot >= _num_slots)
        {
            EX3_THROW(invalid_slot_exception()
                << maximum_slot_ei(_num_slots-1)
                << requested_slot_ei(slot)
                << file_name_ei(filename()));
        }

        position += slot * _slot_size;

        // TODO assert seek returns
        _buffer.seek(position);
        _buffer.write(data, size);
    }

    foffset_t push(
        const void* data,
        foffset_t size
    )
    {
        if (_num_slots != 1)
        {
            EX3_THROW(invalid_num_slots_exception()
                << actual_num_slots_ei(_num_slots)
                << expected_num_slots_ei(1)
                << file_name_ei(filename()));
        }

        const foffset_t position = mem_align::size<Align>(get_size());

        // TODO assert seek returns
        _buffer.seek(position);
        _buffer.write(data, size);

        const foffset_t total_size = position + size;
        const foffset_t total_aligned = mem_align::size<Align>(size);

        if (total_aligned > total_size)
        {
            _buffer.seek(total_aligned - 1);
            _buffer.write("", 1);
        }

        _slot_size = total_aligned;

        return position;
    }

    void sync(
    )
    {
        _buffer.sync();
    }

    virtual ~rwp_data(
    )
    {
    }
};

}
