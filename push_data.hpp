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
#include "exceptions.hpp"
#include "push_buffer.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/move/utility_core.hpp>

namespace s1b {

class push_data : public data_base
{
private:

   BOOST_MOVABLE_BUT_NOT_COPYABLE(push_data)

private:

    static const foffset_t Align = S1B_DATA_ALIGNMENT_BYTES;

private:

    push_buffer _buffer;
    boost::uuids::uuid _meta_uuid;
    foffset_t _slot_size;

    foffset_t assert_slot_size(
        foffset_t data_size
    ) const
    {
        const foffset_t slot_size = data_size - data_base::get_data_offset();

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

    const boost::uuids::uuid& write_uuid(
        const boost::uuids::uuid& uuid
    )
    {
        const foffset_t position = data_base::get_uuid_offset();

        // TODO assert seek returns
        _buffer.seek(position);

        const foffset_t size = _buffer.write_object(uuid);
        const foffset_t total_size = position + size;
        const foffset_t total_aligned = data_base::get_data_offset();

        _buffer.skip(total_aligned - total_size);

        return uuid;
    }

    const boost::uuids::uuid& assert_uuid(
        const boost::uuids::uuid& meta_uuid
    )
    {
        boost::uuids::uuid stored_uuid;

        const foffset_t position = data_base::get_uuid_offset();

        // TODO assert seek returns
        _buffer.seek(position);
        _buffer.read_object(stored_uuid, true);

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
    push_data(
        const path_string& filename,
        bool create_new,
        const Metadata& metadata
    ) :
        _buffer(filename, create_new),
        _meta_uuid(create_new ?
            write_uuid(metadata.file_uuid()) :
            assert_uuid(metadata.file_uuid())),
        _slot_size(assert_slot_size(_buffer.get_size())) // TODO fail if size mismatch
    {
        _buffer.seek(_slot_size + data_base::get_data_offset());
    }

    push_data(
        BOOST_RV_REF(push_data) other
    ) :
        _buffer(boost::move(other._buffer)),
        _meta_uuid(boost::move(other._meta_uuid)),
        _slot_size(other._slot_size)
    {
    }

    push_data& operator =(
        BOOST_RV_REF(push_data) other
    )
    {
        _buffer = boost::move(other._buffer);
        _meta_uuid = boost::move(other._meta_uuid);
        _slot_size = other._slot_size;

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
        return 1;
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
