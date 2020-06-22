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
#include "push_buffer.hpp"
#include "meta_file_header.hpp"
#include "rwp_metadata_base.hpp"

#include "traits/metadata_type.hpp"
#include "traits/global_struct_type.hpp"

#include "helpers/seekrw_proxy.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/move/utility_core.hpp>

#include <algorithm>
#include <iterator>
#include <vector>

namespace s1b {

// TODO assert meta check

template <typename MetaAdapter>
class push_metadata : public rwp_metadata_base<MetaAdapter,
#if defined(S1B_DISABLE_ATOMIC_RW)
    push_buffer
#else
    s1b::helpers::seekrw_proxy<push_buffer>
#endif
    >
{
private:

   BOOST_MOVABLE_BUT_NOT_COPYABLE(push_metadata)

public:

    typedef
#if defined(S1B_DISABLE_ATOMIC_RW)
        push_buffer
#else
        s1b::helpers::seekrw_proxy<push_buffer>
#endif
        meta_buffer_type;

    typedef rwp_metadata_base<MetaAdapter, meta_buffer_type> base_type;

    typedef typename base_type::metadata_type metadata_type;

    typedef typename base_type::global_struct_type global_struct_type;

protected:

    typedef typename base_type::file_metadata_type file_metadata_type;

    static const foffset_t file_metadata_size_align =
        base_type::file_metadata_size_align;

    static const foffset_t file_metadata_size =
        base_type::file_metadata_size;

private:

    push_buffer _buffer;
    const boost::uuids::uuid _uuid;
    global_struct_type _global_struct;
    s1b::uid_t _next_uid;
    foffset_t _next_data_offset;

    boost::uuids::uuid assert_header(
    )
    {
        return base_type::assert_header(_buffer);
    }

    void assert_meta_check(
    )
    {
        base_type::assert_meta_check(_buffer);
    }

    boost::uuids::uuid create_header(
    )
    {
#if defined(S1B_DISABLE_ATOMIC_RW)
        meta_buffer_type& proxy = _buffer;
#else
        meta_buffer_type proxy = _buffer;
#endif
        return base_type::create_header(proxy);
    }

    void create_meta_check(
    )
    {
#if defined(S1B_DISABLE_ATOMIC_RW)
        meta_buffer_type& proxy = _buffer;
#else
        meta_buffer_type proxy = _buffer;
#endif
        base_type::create_meta_check(proxy);
    }

    global_struct_type read_global_struct(
    )
    {
        return base_type::read_global_struct(_buffer);
    }

    void write_global_struct(
        const global_struct_type& glob
    )
    {
#if defined(S1B_DISABLE_ATOMIC_RW)
        meta_buffer_type& proxy = _buffer;
#else
        meta_buffer_type proxy = _buffer;
#endif
        base_type::write_global_struct(proxy, glob);
    }

    bool read_file_element(
        s1b::uid_t uid,
        file_metadata_type& elem,
        bool required
    )
    {
        return base_type::read_file_element(_buffer, uid, elem, required);
    }

    template <typename IT>
    void copy_elements(
        IT metadata_begin,
        IT metadata_end
    )
    {
        align_file();

        for ( ; metadata_begin != metadata_end; metadata_begin++, _next_uid++)
        {
            // TODO consistency check: input uid matches

            file_metadata_type elem(*metadata_begin);

            elem.data_offset = _next_data_offset;
            elem.clean_bit = 0;

            base_type::meta_adapter().set_uid(elem, _next_uid);

            _buffer.write_object(elem); // TODO assert file_metadata_size
            _buffer.skip(file_metadata_size_align - file_metadata_size);

            const foffset_t size = base_type::meta_adapter().
                get_data_size(elem);

            _next_data_offset += base_type::compute_aligned_data_size(size);
        }
    }

    void align_file(
    )
    {
        const foffset_t file_size = _buffer.get_size();
        const foffset_t align_size = base_type::
            get_element_offset_unsafe(_next_uid);

        if (align_size < file_size)
        {
            EX3_THROW(file_size_mismatch_exception()
                << file_size_ei(file_size)
                << file_position_ei(align_size)
                << file_name_ei(filename()));
        }

        if (align_size > file_size)
        {
            _buffer.seek(align_size - 1);
            _buffer.write("", 1);
            _buffer.sync();
        }
    }

    s1b::uid_t push(
        const metadata_type& meta,
        foffset_t& data_offset,
        foffset_t& data_size,
        bool force_data_offset
    )
    {
        static const s1b::foffset_t Align = base_type::Align;

        if (force_data_offset)
        {
            if (!mem_align::is_aligned<Align>(data_offset))
            {
                EX3_THROW(misaligned_exception()
                    << file_size_ei(_next_data_offset)
                    << offset_ei(data_offset)
                    << expected_alignment_ei(
                        static_cast<foffset_t>(Align))
                    << file_name_ei(filename()));
            }

            if (data_offset < _next_data_offset)
            {
                EX3_THROW(data_offset_overlap_exception()
                    << file_size_ei(_next_data_offset)
                    << offset_ei(data_offset)
                    << file_name_ei(filename()));
            }
        }
        else
        {
            data_offset = _next_data_offset;
        }

        const s1b::uid_t uid = _next_uid;

        file_metadata_type elem(meta);

        elem.data_offset = data_offset;
        elem.clean_bit = 0;

        base_type::meta_adapter().set_uid(elem, uid);

        _buffer.write_object(elem); // TODO assert file_metadata_size
        _buffer.skip(file_metadata_size_align - file_metadata_size);

        data_size = base_type::meta_adapter().get_data_size(elem);

        _next_uid++;
        _next_data_offset = data_offset + base_type::
            compute_aligned_data_size(data_size);

        return uid;
    }

    s1b::uid_t _get_last_uid(
    )
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

    foffset_t _get_data_size(
    )
    {
        // Get the data offset and size of the last element and compute the
        // total size of the data file

        // WARNING: Data offsets must not be reordered for this to work!

        s1b::uid_t last_uid = _next_uid - 1;

        if (last_uid == FirstUID-1)
            return 0;

        file_metadata_type last;
        read_file_element(last_uid, last, true);

        const foffset_t last_data_offset = last.data_offset;
        const foffset_t size = base_type::meta_adapter().get_data_size(last);
        return last_data_offset + base_type::compute_aligned_data_size(size);
    }

public:

    push_metadata(
        const path_string& filename,
        const global_struct_type& global_struct
    ) :
        rwp_metadata_base<MetaAdapter, meta_buffer_type>(),
        _buffer(filename, true),
        _uuid(create_header()),
        _global_struct(global_struct),
        _next_uid(FirstUID),
        _next_data_offset(0)
    {
        write_global_struct(_global_struct);
        create_meta_check();
        align_file();
    }

    template <typename IT>
    push_metadata(
        const path_string& filename,
        IT metadata_begin,
        IT metadata_end,
        const global_struct_type& global_struct
    ) :
        rwp_metadata_base<MetaAdapter, meta_buffer_type>(),
        _buffer(filename, true),
        _uuid(create_header()),
        _global_struct(global_struct),
        _next_uid(FirstUID),
        _next_data_offset(0)
    {
        write_global_struct(_global_struct);
        create_meta_check();
        copy_elements(metadata_begin, metadata_end);
        align_file();
    }

    push_metadata(
        const path_string& filename
    ) :
        rwp_metadata_base<MetaAdapter, meta_buffer_type>(),
        _buffer(filename, false),
        _uuid(assert_header()),
        _global_struct(read_global_struct()),
        _next_uid(_get_last_uid() + 1),
        _next_data_offset(_get_data_size())
    {
        assert_meta_check();
        // TODO assert valid size
        align_file();
    }

    push_metadata(
        BOOST_RV_REF(push_metadata) other
    ) :
        _buffer(boost::move(other._buffer)),
        _uuid(boost::move(other._uuid)),
        _global_struct(boost::move(other._global_struct)),
        _next_uid(other._next_uid),
        _next_data_offset(other._next_data_offset)
    {
    }

    push_metadata& operator=(
        BOOST_RV_REF(push_metadata) other
    )
    {
        _buffer = boost::move(other._buffer);
        _uuid = boost::move(other._uuid);
        _global_struct = boost::move(other._global_struct);
        _next_uid = other._next_uid;
        _next_data_offset = other._next_data_offset;

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
    )
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
        return _next_uid - 1;
    }

    foffset_t get_data_size(
    ) const
    {
        return _next_data_offset;
    }

    void align(
    )
    {
        align_file();
    }

    void sync(
    )
    {
        _buffer.sync();
    }

    virtual ~push_metadata(
    )
    {
    }
};

}
