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

#include "traits/metadata_type.hpp"
#include "traits/global_struct_type.hpp"

#include <algorithm>
#include <iterator>
#include <vector>

namespace s1b {

// TODO assert meta check

template <typename MetaAdapter>
class push_metadata : public metadata_base<MetaAdapter>
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
    s1b::uid_t _next_uid;
    foffset_t _next_data_offset;

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
        for ( ; metadata_begin != metadata_end; metadata_begin++, _next_uid++)
        {
            // TODO consistency check: input uid matches

            file_metadata_type elem(*metadata_begin);

            elem.data_offset = _next_data_offset;
            elem.clean_bit = 0;

            base_type::meta_adapter().set_uid(elem, _next_uid);

            _buffer.seek(base_type::get_element_offset_unsafe(_next_uid));
            _buffer.write_object(elem);

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

        _buffer.write_object(elem);

        data_size = base_type::meta_adapter().get_data_size(elem);

        _next_uid++;
        _next_data_offset = data_offset + base_type::
            compute_aligned_data_size(data_size);

        _buffer.seek(base_type::get_element_offset_unsafe(_next_uid));

        return uid;
    }

    s1b::uid_t _get_last_uid(
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
        metadata_base<MetaAdapter>(),
        _buffer(
            filename,
            S1B_OPEN_NEW),
        _global_struct(global_struct),
        _next_uid(FirstUID),
        _next_data_offset(0)
    {
        create_header();
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
        metadata_base<MetaAdapter>(),
        _buffer(
            filename,
            S1B_OPEN_NEW),
        _global_struct(global_struct),
        _next_uid(FirstUID),
        _next_data_offset(0)
    {
        create_header();
        write_global_struct(_global_struct);
        create_meta_check();
        copy_elements(metadata_begin, metadata_end);
        align_file();
    }

    push_metadata(
        const path_string& filename
    ) :
        metadata_base<MetaAdapter>(),
        _buffer(
            filename,
            S1B_OPEN_WRITE),
        _global_struct(read_global_struct()),
        _next_uid(_get_last_uid() + 1),
        _next_data_offset(_get_data_size())
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
