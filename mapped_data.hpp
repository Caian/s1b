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
#include "mem_align.hpp"
#include "exceptions.hpp"
#include "mapped_buffer.hpp"

namespace s1b {

class mapped_data
{
private:

    static const foffset_t Align = S1B_DATA_ALIGNMENT_BYTES;

private:

    size_t _slot_size;
    mapped_buffer _buffer;
    unsigned int _num_slots;

    size_t assert_slot_size(
        size_t slot_size
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

    unsigned int compute_num_slots(
        unsigned int expected
    ) const
    {
        const size_t data_size = _buffer.size();
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

    size_t compute_offset(
        size_t offset,
        unsigned int slot
    ) const
    {
        if (slot >= _num_slots)
        {
            EX3_THROW(invalid_slot_exception()
                << maximum_slot_ei(_num_slots-1)
                << requested_slot_ei(slot)
                << file_name_ei(filename()));
        }

        const size_t full_offset = slot * _slot_size + offset;

        if (!mem_align::is_aligned<Align>(full_offset))
        {
            EX3_THROW(misaligned_exception()
                << file_position_ei(offset)
                << actual_slot_size_ei(_slot_size)
                << requested_slot_ei(slot)
                << expected_alignment_ei(
                    static_cast<foffset_t>(Align))
                << file_name_ei(filename()));
        }

        const size_t data_size = _buffer.size();

        if (full_offset >= data_size)
        {
            EX3_THROW(io_exception()
                << file_position_ei(offset)
                << file_size_ei(data_size)
                << actual_slot_size_ei(_slot_size)
                << requested_slot_ei(slot)
                << expected_alignment_ei(
                    static_cast<foffset_t>(Align))
                << file_name_ei(filename()));
        }

        return full_offset;
    }

public:

    template <typename Metadata>
    mapped_data(
        const path_string& filename,
        open_mode mode,
        const Metadata& metadata,
        unsigned int num_slots,
        hugetlb_mode htlb_mode=S1B_HUGETLB_OFF
    ) :
        _slot_size(assert_slot_size(metadata.get_data_size())),
        _buffer(filename, mode, _slot_size*num_slots, htlb_mode),
        _num_slots(compute_num_slots(num_slots))
    {
    }

    template <typename Metadata>
    mapped_data(
        const path_string& filename,
        open_mode mode,
        Metadata& metadata,
        unsigned int num_slots,
        hugetlb_mode htlb_mode=S1B_HUGETLB_OFF
    ) :
        _slot_size(assert_slot_size(metadata.get_data_size())),
        _buffer(filename, mode, _slot_size*num_slots, htlb_mode),
        _num_slots(compute_num_slots(num_slots))
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

    template <typename T>
    const T* get_ptr(
        size_t offset,
        unsigned int slot=0
    ) const
    {
        return reinterpret_cast<const T*>(get_address(offset, slot));
    }

    template <typename T>
    T* get_ptr(
        size_t offset,
        unsigned int slot=0
    )
    {
        return reinterpret_cast<T*>(get_address(offset, slot));
    }

    const char* get_address(
        size_t offset,
        unsigned int slot=0
    ) const
    {
        return _buffer.address() + compute_offset(offset, slot);
    }

    char* get_address(
        size_t offset,
        unsigned int slot=0
    )
    {
        return _buffer.address() + compute_offset(offset, slot);
    }

    unsigned int num_slots(
    ) const
    {
        return _num_slots;
    }

    size_t slot_size(
    ) const
    {
        return _slot_size;
    }

    size_t size(
    ) const
    {
        return _buffer.size();
    }

    hugetlb_state htlb_state(
    ) const
    {
        return _buffer.htlb_state();
    }

    void sync(
    )
    {
        _buffer.sync();
    }

    virtual ~mapped_data(
    )
    {
    }
};

}
