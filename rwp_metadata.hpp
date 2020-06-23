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
#include "metadata.hpp"
#include "mem_align.hpp"
#include "exceptions.hpp"
#include "path_string.hpp"
#include "rwp_buffer.hpp"
#include "meta_file_header.hpp"
#include "rwp_metadata_base.hpp"

#include "iterators/uid_iterator.hpp"

#include "traits/metadata_type.hpp"
#include "traits/global_struct_type.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/move/utility_core.hpp>

#include <algorithm>
#include <iterator>
#include <vector>

namespace s1b {

// TODO assert meta check

template <typename MetaAdapter>
class rwp_metadata : public rwp_metadata_base<MetaAdapter, rwp_buffer>
{
private:

   BOOST_MOVABLE_BUT_NOT_COPYABLE(rwp_metadata)

public:

    typedef rwp_metadata_base<MetaAdapter, rwp_buffer> base_type;

    typedef typename base_type::metadata_type metadata_type;

    typedef typename base_type::global_struct_type global_struct_type;

protected:

    typedef typename base_type::file_metadata_type file_metadata_type;

private:

    rwp_buffer _buffer;
    const boost::uuids::uuid _uuid;
    global_struct_type _global_struct;

    boost::uuids::uuid assert_header(
    ) S1B_READ_METHOD_QUALIFIER
    {
        return base_type::assert_header(_buffer);
    }

    void assert_meta_check(
    ) S1B_READ_METHOD_QUALIFIER
    {
        base_type::assert_meta_check(_buffer);
    }

    boost::uuids::uuid create_header(
    )
    {
        return base_type::create_header(_buffer);

    }

    void create_meta_check(
    )
    {
        base_type::create_meta_check(_buffer);

    }

    global_struct_type read_global_struct(
    ) S1B_READ_METHOD_QUALIFIER
    {
        return base_type::read_global_struct(_buffer);
    }

    void write_global_struct(
        const global_struct_type& glob
    )
    {
        base_type::write_global_struct(_buffer, glob);
    }

    bool read_file_element(
        s1b::uid_t uid,
        file_metadata_type& elem,
        bool required
    ) S1B_READ_METHOD_QUALIFIER
    {
        return base_type::read_file_element(_buffer, uid, elem, required);
    }

    template <typename IT>
    void copy_elements(
        IT metadata_begin,
        IT metadata_end
    )
    {
        s1b::uid_t uid = FirstUID;
        foffset_t offset = 0;

        for ( ; metadata_begin != metadata_end; metadata_begin++, uid++)
        {
            // TODO consistency check: input uid matches

            file_metadata_type elem(*metadata_begin);

            elem.data_offset = offset;
            elem.clean_bit = 0;

            base_type::meta_adapter().set_uid(elem, uid);

            const foffset_t element_offset = base_type::
                get_element_offset_unsafe(uid);

#if defined(S1B_DISABLE_ATOMIC_RW)
            _buffer.seek(element_offset);
#endif
            _buffer.write_object(elem
#if !defined(S1B_DISABLE_ATOMIC_RW)
                , element_offset
#endif
                );

            const foffset_t size = base_type::meta_adapter().
                get_data_size(elem);

            offset += base_type::compute_aligned_data_size(size);
        }

        align_file(uid-1);
    }

    void align_file(
        s1b::uid_t last_id
    )
    {
        foffset_t file_size = _buffer.get_size();
        foffset_t align_size = base_type::get_element_offset_unsafe(
            last_id + 1);

        if (align_size < file_size)
        {
            EX3_THROW(file_size_mismatch_exception()
                << file_size_ei(file_size)
                << file_position_ei(align_size)
                << file_name_ei(filename()));
        }

        if (align_size > file_size)
        {
            const foffset_t aligned_position = align_size - 1;

#if defined(S1B_DISABLE_ATOMIC_RW)
            _buffer.seek(aligned_position);
#endif
            _buffer.write("",
#if !defined(S1B_DISABLE_ATOMIC_RW)
                aligned_position,
#endif
                1);
        }
    }

    foffset_t get_data_size(
        s1b::uid_t& last_uid
    ) S1B_READ_METHOD_QUALIFIER
    {
        // Get the data offset and size of the last element and compute the
        // total size of the data file

        // WARNING: Data offsets must not be reordered for this to work!

        last_uid = get_last_uid();

        if (last_uid == FirstUID-1)
            return 0;

        file_metadata_type last;
        read_file_element(last_uid, last, true);

        const foffset_t last_data_offset = last.data_offset;
        const foffset_t size = base_type::meta_adapter().get_data_size(last);
        return last_data_offset + base_type::compute_aligned_data_size(size);
    }

    s1b::uid_t push(
        const metadata_type& meta,
        foffset_t& data_offset,
        foffset_t& data_size,
        bool force_data_offset
    )
    {
        static const s1b::foffset_t Align = base_type::Align;

        s1b::uid_t last_uid;
        const foffset_t curr_data_offset = get_data_size(last_uid);

        if (force_data_offset)
        {
            if (!mem_align::is_aligned<Align>(data_offset))
            {
                EX3_THROW(misaligned_exception()
                    << file_size_ei(curr_data_offset)
                    << offset_ei(data_offset)
                    << expected_alignment_ei(
                        static_cast<foffset_t>(Align))
                    << file_name_ei(filename()));
            }

            if (data_offset < curr_data_offset)
            {
                EX3_THROW(data_offset_overlap_exception()
                    << file_size_ei(curr_data_offset)
                    << offset_ei(data_offset)
                    << file_name_ei(filename()));
            }
        }
        else
        {
            data_offset = curr_data_offset;
        }

        const s1b::uid_t uid = last_uid + 1;

        file_metadata_type elem(meta);

        elem.data_offset = data_offset;
        elem.clean_bit = 0;

        base_type::meta_adapter().set_uid(elem, uid);

        const foffset_t element_offset = base_type::
            get_element_offset_unsafe(uid);

#if defined(S1B_DISABLE_ATOMIC_RW)
        _buffer.seek(element_offset);
#endif
        _buffer.write_object(elem
#if !defined(S1B_DISABLE_ATOMIC_RW)
            , element_offset
#endif
            );

        align_file(uid);

        data_size = base_type::meta_adapter().get_data_size(elem);

        return uid;
    }

public:

    rwp_metadata(
        const path_string& filename
    ) :
        rwp_metadata_base<MetaAdapter, rwp_buffer>(),
        _buffer(
            filename,
            S1B_OPEN_NEW),
        _uuid(create_header()),
        _global_struct()
    {
        write_global_struct(_global_struct);
        create_meta_check();
        align_file(FirstUID-1);
    }

    template <typename IT>
    rwp_metadata(
        const path_string& filename,
        IT metadata_begin,
        IT metadata_end
    ) :
        rwp_metadata_base<MetaAdapter, rwp_buffer>(),
        _buffer(
            filename,
            S1B_OPEN_NEW),
        _uuid(create_header()),
        _global_struct()
    {
        write_global_struct(_global_struct);
        create_meta_check();
        copy_elements(metadata_begin, metadata_end);
    }

    rwp_metadata(
        const path_string& filename,
        bool can_write
    ) :
        rwp_metadata_base<MetaAdapter, rwp_buffer>(),
        _buffer(
            filename,
            can_write ? S1B_OPEN_WRITE : S1B_OPEN_DEFAULT),
        _uuid(assert_header()),
        _global_struct(read_global_struct())
    {
        assert_meta_check();
        // TODO assert valid size
    }

    rwp_metadata(
        BOOST_RV_REF(rwp_metadata) other
    ) :
        _buffer(boost::move(other._buffer)),
        _uuid(boost::move(other._uuid)),
        _global_struct(boost::move(other._global_struct))
    {
    }

    rwp_metadata& operator =(
        BOOST_RV_REF(rwp_metadata) other
    )
    {
        _buffer = boost::move(other._buffer);
        _uuid = boost::move(other._uuid);
        _global_struct = boost::move(other._global_struct);

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

    foffset_t get_size(
    ) const
    {
        return _buffer.get_size();
    }

    const MetaAdapter& meta_adapter(
    ) const
    {
        return base_type::meta_adapter();
    }

    const boost::uuids::uuid& file_uuid(
    ) const
    {
        return _uuid;
    }

    const global_struct_type& global_struct(
    ) const
    {
        return _global_struct;
    }

    void update_global_struct(
        const global_struct_type& glob
    )
    {
        write_global_struct(glob);
        _global_struct = glob;
    }

    bool read(
        s1b::uid_t uid,
        metadata_type& meta,
        foffset_t& data_offset
    ) S1B_READ_METHOD_QUALIFIER
    {
        file_metadata_type elem;

        if (!read_file_element(uid, elem, false))
            return false;

        meta = elem;
        data_offset = elem.data_offset;

        return true;
    }

    bool read(
        s1b::uid_t uid,
        metadata_type& meta
    ) S1B_READ_METHOD_QUALIFIER
    {
        foffset_t data_offset;
        return read(uid, meta, data_offset);
    }

    bool read_data_offset(
        s1b::uid_t uid,
        foffset_t& data_offset
    ) S1B_READ_METHOD_QUALIFIER
    {
        metadata_type meta;
        return read(uid, meta, data_offset);
    }

    foffset_t read_data_offset(
        const metadata_type& elem
    ) S1B_READ_METHOD_QUALIFIER
    {
        const s1b::uid_t uid = base_type::meta_adapter().get_uid(elem);

        foffset_t data_offset;

        if (!read_data_offset(uid, data_offset))
        {
            EX3_THROW(invalid_uid_exception()
                << requested_uid_ei(uid)
                << file_name_ei(filename()));
        }

        return data_offset;
    }

    void write(
        const metadata_type& meta
    )
    {
        const s1b::uid_t uid = base_type::meta_adapter().get_uid(meta);

        file_metadata_type elem;
        file_metadata_type new_elem(meta);

        read_file_element(uid, elem, true);

        const foffset_t size = base_type::meta_adapter().
            get_data_size(elem);

        const foffset_t new_size = base_type::meta_adapter().
            get_data_size(new_elem);

        if (size != new_size)
        {
            EX3_THROW(element_mismatch_exception()
                << expected_size_ei(size)
                << actual_size_ei(new_size)
                << file_name_ei(filename()));
        }

        new_elem.data_offset = elem.data_offset;
        new_elem.clean_bit = elem.clean_bit;

        const foffset_t element_offset = base_type::
            get_element_offset_unsafe(uid);

#if defined(S1B_DISABLE_ATOMIC_RW)
        _buffer.seek(element_offset);
#endif
        _buffer.write_object(new_elem
#if !defined(S1B_DISABLE_ATOMIC_RW)
            , element_offset
#endif
            );
    }

    s1b::uid_t push(
        const metadata_type& meta,
        foffset_t& data_offset,
        foffset_t& data_size
    )
    {
        return push(meta, data_offset, data_size, false);
    }

    s1b::uid_t push(
        const metadata_type& meta,
        foffset_t& data_offset
    )
    {
        foffset_t data_size;
        return push(meta, data_offset, data_size);
    }

    s1b::uid_t push(
        const metadata_type& meta
    )
    {
        foffset_t data_offset;
        return push(meta, data_offset);
    }

    s1b::uid_t push_fixed(
        const metadata_type& meta,
        foffset_t data_offset,
        foffset_t& data_size
    )
    {
        return push(meta, data_offset, data_size, true);
    }

    s1b::uid_t push_fixed(
        const metadata_type& meta,
        foffset_t data_offset
    )
    {
        foffset_t data_size;
        return push_fixed(meta, data_offset, data_size);
    }

    s1b::uid_t get_last_uid(
    ) const
    {
        try
        {
            return base_type::compute_num_elements(_buffer.get_size());
        }
        catch (const io_exception& e)
        {
            EX3_RETHROW(e
                << file_name_ei(filename()));
        }
    }

    iterators::uid_iterator uid_begin(
    ) const
    {
        return iterators::uid_iterator(FirstUID);
    }

    iterators::uid_iterator uid_end(
    ) const
    {
        return iterators::uid_iterator(get_last_uid() + 1);
    }

    foffset_t get_data_size(
    )
    {
        s1b::uid_t last_uid;
        return get_data_size(last_uid);
    }

    void sync(
    )
    {
        _buffer.sync();
    }

    virtual ~rwp_metadata(
    )
    {
    }
};

}
