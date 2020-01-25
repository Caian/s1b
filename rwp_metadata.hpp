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
#include "metadata.hpp"
#include "mem_align.hpp"
#include "exceptions.hpp"
#include "path_string.hpp"
#include "rwp_buffer.hpp"
#include "metadata_base.hpp"
#include "meta_file_header.hpp"

#include "iterators/uid_iterator.hpp"

#include "traits/metadata_type.hpp"
#include "traits/global_struct_type.hpp"

#include <algorithm>
#include <iterator>
#include <vector>

namespace s1b {

// TODO assert meta check

template <typename MetaAdapter>
class rwp_metadata : public metadata_base<MetaAdapter>
{
public:

    typedef metadata_base<MetaAdapter> base_type;

    typedef typename base_type::metadata_type metadata_type;

    typedef typename base_type::global_struct_type global_struct_type;

protected:

    typedef typename base_type::file_metadata_type file_metadata_type;

private:

    rwp_buffer _buffer;
    global_struct_type _global_struct;

    void assert_header(
    )
    {
        meta_file_header header;

        const foffset_t header_offset = base_type::get_header_offset();

        _buffer.seek(header_offset);
        _buffer.read_object(header, true);

        meta_file_header new_header(
            base_type::Align,
            base_type::get_meta_check_size(),
            base_type::file_metadata_size,
            base_type::global_struct_size);

        try
        {
            new_header.assert_other(header);
        }
        catch (const io_exception& e)
        {
            EX3_RETHROW(e
                << file_position_ei(header_offset)
                << file_name_ei(filename()));
        }
    }

    void assert_meta_check(
    )
    {
        meta_file_header header;

        _buffer.seek(base_type::get_header_offset());
        _buffer.read_object(header, true);

        const foffset_t check_sz = header.checksz;

        const foffset_t ref_check_sz = base_type::get_meta_check_size();

        if (ref_check_sz != check_sz)
        {
            EX3_THROW(check_data_mismatch_exception()
                << expected_size_ei(ref_check_sz)
                << actual_size_ei(check_sz)
                << file_name_ei(filename()));
        }

        std::vector<char> check(check_sz);

        const foffset_t check_offset = base_type::get_meta_check_offset();

        _buffer.seek(check_offset);
        _buffer.read(&check[0], check.size(), true, true);

        const char* const p_check = &check[0];

        const char* const p_ref_check_begin = base_type::meta_adapter().
            get_meta_check_ptr();

        const char* const p_ref_check_end = p_ref_check_begin + check_sz;

        std::pair<const char*, const char*> comp;

        comp = std::mismatch(p_ref_check_begin, p_ref_check_end, p_check);

        if (comp.first != p_ref_check_end)
        {
            const foffset_t offset = std::distance(
                p_ref_check_begin, comp.first);

            EX3_THROW(check_data_mismatch_exception()
                << expected_value_ei(*comp.first)
                << actual_value_ei(*comp.second)
                << offset_ei(offset)
                << file_position_ei(check_offset)
                << file_name_ei(filename()));
        }
    }

    void create_header(
    )
    {
        // TODO refactor to metadata_base
        meta_file_header new_header(
            base_type::Align,
            base_type::get_meta_check_size(),
            base_type::file_metadata_size,
            base_type::global_struct_size);

        _buffer.seek(base_type::get_header_offset());
        _buffer.write_object(new_header);
    }

    void create_meta_check(
    )
    {
        const char* p_src_check = base_type::meta_adapter().
            get_meta_check_ptr();

        foffset_t check_sz = base_type::get_meta_check_size();

        _buffer.seek(base_type::get_meta_check_offset());
        _buffer.write(p_src_check, check_sz);
    }

    global_struct_type read_global_struct(
    )
    {
        global_struct_type glob;
        _buffer.seek(base_type::get_global_struct_offset());
        _buffer.read_object(glob, true);
        return glob;
    }

    void write_global_struct(
        const global_struct_type& glob
    )
    {
        _buffer.seek(base_type::get_global_struct_offset());
        _buffer.write_object(glob);
    }

    bool read_file_element(
        s1b::uid_t uid,
        file_metadata_type& elem,
        bool required
    )
    {
        if (uid < FirstUID)
        {
            EX3_THROW(invalid_uid_exception()
                << requested_uid_ei(uid)
                << file_name_ei(filename()));
        }

        _buffer.seek(base_type::get_element_offset_unsafe(uid));

        try
        {
            if (_buffer.read_object(elem, required) == 0)
                return false;
        }
        catch (const io_exception& e)
        {
            EX3_RETHROW(e
                << requested_uid_ei(uid)
                << file_name_ei(filename()));
        }

        s1b::uid_t file_uid = base_type::meta_adapter().get_uid(elem);

        if (file_uid != uid)
        {
            EX3_THROW(element_mismatch_exception()
                << requested_uid_ei(uid)
                << actual_uid_ei(file_uid)
                << file_name_ei(filename()));
        }

        return true;
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

            _buffer.seek(base_type::get_element_offset_unsafe(uid));
            _buffer.write_object(elem);

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
            _buffer.seek(align_size - 1);
            _buffer.write("", 1);
        }
    }

    const foffset_t get_data_size(
        s1b::uid_t& last_uid
    )
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

public:

    rwp_metadata(
        path_string filename
    ) :
        metadata_base<MetaAdapter>(),
        _buffer(
            filename,
            S1B_OPEN_NEW),
        _global_struct()
    {
        create_header();
        write_global_struct(_global_struct);
        create_meta_check();
        align_file(FirstUID-1);
    }

    template <typename IT>
    rwp_metadata(
        path_string filename,
        IT metadata_begin,
        IT metadata_end
    ) :
        metadata_base<MetaAdapter>(),
        _buffer(
            filename,
            S1B_OPEN_NEW),
        _global_struct()
    {
        create_header();
        write_global_struct(_global_struct);
        create_meta_check();
        copy_elements(metadata_begin, metadata_end);
    }

    rwp_metadata(
        path_string filename,
        bool can_write
    ) :
        metadata_base<MetaAdapter>(),
        _buffer(
            filename,
            can_write ? S1B_OPEN_WRITE : S1B_OPEN_DEFAULT),
        _global_struct(read_global_struct())
    {
        assert_header();
        assert_meta_check();
        // TODO assert valid size
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

    bool read_element(
        s1b::uid_t uid,
        metadata_type& meta,
        foffset_t& data_offset
    )
    {
        file_metadata_type elem;

        if (!read_file_element(uid, elem, false))
            return false;

        meta = elem;
        data_offset = elem.data_offset;

        return true;
    }

    bool read_element(
        s1b::uid_t uid,
        metadata_type& meta
    )
    {
        foffset_t data_offset;
        return read_element(uid, meta, data_offset);
    }

    bool read_data_offset(
        s1b::uid_t uid,
        foffset_t& data_offset
    )
    {
        metadata_type meta;
        return read_element(uid, meta, data_offset);
    }

    foffset_t read_data_offset(
        const metadata_type& elem
    )
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

    void write_element(
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

        _buffer.seek(base_type::get_element_offset_unsafe(uid));
        _buffer.write_object(new_elem);
    }

    s1b::uid_t push_element(
        const metadata_type& meta,
        foffset_t& data_offset, // TODO add data_offset_hint or use_given_data_offset
        foffset_t& data_size
    )
    {
        s1b::uid_t last_uid;
        data_offset = get_data_size(last_uid);

        const s1b::uid_t uid = last_uid + 1;

        file_metadata_type elem(meta);

        elem.data_offset = data_offset;
        elem.clean_bit = 0;

        base_type::meta_adapter().set_uid(elem, uid);

        _buffer.seek(base_type::get_element_offset_unsafe(uid));
        _buffer.write_object(elem);

        align_file(uid);

        data_size = base_type::meta_adapter().get_data_size(elem);

        return uid;
    }

    s1b::uid_t push_element(
        const metadata_type& meta,
        foffset_t& data_offset
    )
    {
        foffset_t data_size;
        return push_element(meta, data_offset, data_size);
    }

    s1b::uid_t push_element(
        const metadata_type& meta
    )
    {
        foffset_t data_offset;
        return push_element(meta, data_offset);
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

    const foffset_t get_data_size(
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
