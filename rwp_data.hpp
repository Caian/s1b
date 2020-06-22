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
#include "data_base.hpp"
#include "rwp_buffer.hpp"
#include "exceptions.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/move/utility_core.hpp>

namespace s1b {

class rwp_data : public data_base
{
private:

   BOOST_MOVABLE_BUT_NOT_COPYABLE(rwp_data)

private:

    static const foffset_t Align = data_base::Align;

private:

    foffset_t _slot_size;
    rwp_buffer _buffer;
    unsigned int _num_slots;
    boost::uuids::uuid _meta_uuid;

    foffset_t assert_slot_size(
        const path_string& filename,
        foffset_t slot_size
    ) const
    {
        if (!mem_align::is_aligned<Align>(slot_size))
        {
            EX3_THROW(misaligned_exception()
                << actual_slot_size_ei(slot_size)
                << expected_alignment_ei(
                    static_cast<foffset_t>(Align))
                << file_name_ei(filename));
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
            const foffset_t aligned_position = data_size - 1 +
                data_base::get_data_offset();

#if defined(S1B_DISABLE_ATOMIC_RW)
            _buffer.seek(aligned_position);
#endif
            _buffer.write("",
#if !defined(S1B_DISABLE_ATOMIC_RW)
                aligned_position,
#endif
                1);
        }

        return num_slots;
    }

    unsigned int compute_num_slots(
        unsigned int expected
    ) const
    {
        const size_t file_size = _buffer.get_size();
        const size_t data_size = file_size - data_base::get_data_offset();

        if (data_size == 0 && _slot_size == 0)
        {
            EX3_THROW(invalid_data_layout_exception()
                << file_size_ei(data_size)
                << actual_slot_size_ei(_slot_size)
                << file_name_ei(filename()));
        }

        const unsigned int num_slots = data_size / _slot_size;
        const foffset_t extra_bytes = data_size % _slot_size;

        if (!mem_align::is_aligned<Align>(data_size))
        {
            EX3_THROW(misaligned_exception()
                << file_size_ei(file_size)
                << expected_alignment_ei(
                    static_cast<foffset_t>(Align))
                << file_name_ei(filename()));
        }

        if (extra_bytes != 0)
        {
            EX3_THROW(extra_slot_bytes_exception()
                << file_size_ei(file_size)
                << actual_slot_size_ei(_slot_size)
                << actual_num_slots_ei(num_slots)
                << file_name_ei(filename()));
        }

        if (expected != 0 && expected != num_slots)
        {
            EX3_THROW(num_slots_mismatch_exception()
                << expected_num_slots_ei(expected)
                << actual_num_slots_ei(num_slots)
                << file_size_ei(file_size)
                << file_name_ei(filename()));
        }

        return num_slots;
    }

    const boost::uuids::uuid& write_uuid(
        const boost::uuids::uuid& uuid
    )
    {
        const foffset_t position = data_base::get_uuid_offset();

        // TODO assert seek returns
#if defined(S1B_DISABLE_ATOMIC_RW)
        _buffer.seek(position);
#endif
        const foffset_t size = _buffer.write_object(uuid
#if !defined(S1B_DISABLE_ATOMIC_RW)
            , position
#endif
            );

        const foffset_t total_size = position + size;
        const foffset_t total_aligned = data_base::get_data_offset();

        if (total_aligned > total_size)
        {
            const foffset_t aligned_position = total_aligned - 1;

#if defined(S1B_DISABLE_ATOMIC_RW)
            _buffer.seek(aligned_position);
#endif
            _buffer.write("",
#if !defined(S1B_DISABLE_ATOMIC_RW)
                aligned_position,
#endif
                1);
        }

        return uuid;
    }

    const boost::uuids::uuid& assert_uuid(
        const boost::uuids::uuid& meta_uuid
    )
    {
        boost::uuids::uuid stored_uuid;

        const foffset_t position = data_base::get_uuid_offset();

        // TODO assert seek returns
#if defined(S1B_DISABLE_ATOMIC_RW)
        _buffer.seek(position);
#endif
        _buffer.read_object(stored_uuid,
#if !defined(S1B_DISABLE_ATOMIC_RW)
            position,
#endif
            true);

        if (meta_uuid != stored_uuid)
        {
            EX3_THROW(uuid_mismatch_exception()
                << metadata_uuid_ei(meta_uuid)
                << stored_uuid_ei(stored_uuid)
                << file_name_ei(filename()));
        }

        return meta_uuid;
    }

public:

    template <typename Metadata>
    rwp_data(
        const path_string& filename,
        open_mode mode,
        const Metadata& metadata,
        unsigned int num_slots
    ) :
        _slot_size(
            assert_slot_size(filename, metadata.get_data_size())),
        _buffer(filename, mode),
        _num_slots(((mode & S1B_OPEN_TRUNC) != 0) ?
            initialize_num_slots(num_slots) :
            compute_num_slots(num_slots)),
        _meta_uuid(((mode & S1B_OPEN_TRUNC) != 0) ?
            write_uuid(metadata.file_uuid()) :
            assert_uuid(metadata.file_uuid()))
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
            assert_slot_size(filename, metadata.get_data_size())),
        _buffer(filename, mode),
        _num_slots(((mode & S1B_OPEN_TRUNC) != 0) ?
            initialize_num_slots(num_slots) :
            compute_num_slots(num_slots)),
        _meta_uuid(((mode & S1B_OPEN_TRUNC) != 0) ?
            write_uuid(metadata.file_uuid()) :
            assert_uuid(metadata.file_uuid()))
    {
    }

    rwp_data(
        BOOST_RV_REF(rwp_data) other
    ) :
        _slot_size(other._slot_size),
        _buffer(boost::move(other._buffer)),
        _num_slots(other._num_slots),
        _meta_uuid(boost::move(other._meta_uuid))
    {
    }

    rwp_data& operator=(
        BOOST_RV_REF(rwp_data) other
    )
    {
        _slot_size = other._slot_size;
        _buffer = boost::move(other._buffer);
        _num_slots = other._num_slots;
        _meta_uuid = boost::move(other._meta_uuid);

        return *this;
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

    const boost::uuids::uuid& metadata_uuid(
    ) const
    {
        return _meta_uuid;
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
    ) S1B_READ_METHOD_QUALIFIER
    {
        // TODO position < 0 invalid
        position += data_base::get_data_offset();

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
#if defined(S1B_DISABLE_ATOMIC_RW)
        _buffer.seek(position);
#endif
        _buffer.read(data,
#if !defined(S1B_DISABLE_ATOMIC_RW)
            position,
#endif
            size, true, true);
    }

    void write(
        const void* data,
        foffset_t position,
        foffset_t size,
        unsigned int slot=0
    )
    {
        position += data_base::get_data_offset();

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
#if defined(S1B_DISABLE_ATOMIC_RW)
        _buffer.seek(position);
#endif
        _buffer.write(data,
#if !defined(S1B_DISABLE_ATOMIC_RW)
            position,
#endif
            size);
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

        const foffset_t position = get_size();

        if (!mem_align::is_aligned<Align>(position))
        {
            EX3_THROW(misaligned_exception()
                << file_size_ei(position)
                << expected_alignment_ei(
                    static_cast<foffset_t>(Align))
                << file_name_ei(filename()));
        }

        // TODO assert seek returns
#if defined(S1B_DISABLE_ATOMIC_RW)
        _buffer.seek(position);
#endif
        _buffer.write(data,
#if !defined(S1B_DISABLE_ATOMIC_RW)
            position,
#endif
            size);

        const foffset_t total_size = position + size;
        const foffset_t total_aligned = mem_align::size<Align>(total_size);

        if (total_aligned > total_size)
        {
            const foffset_t aligned_position = total_aligned - 1;

#if defined(S1B_DISABLE_ATOMIC_RW)
            _buffer.seek(aligned_position);
#endif
            _buffer.write("",
#if !defined(S1B_DISABLE_ATOMIC_RW)
                aligned_position,
#endif
                1);
        }

        _slot_size = total_aligned;

        return position - data_base::get_data_offset();
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
