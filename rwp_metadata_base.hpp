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
#include "metadata_base.hpp"
#include "meta_file_header.hpp"

#include "traits/metadata_type.hpp"
#include "traits/global_struct_type.hpp"

#include <iterator>

namespace s1b { // TODO move to detail

template <typename MetaAdapter, typename Buffer>
class rwp_metadata_base : public metadata_base<MetaAdapter> // TODO buffer_metadata_base
{
public:

    typedef metadata_base<MetaAdapter> base_type;

    typedef typename base_type::file_header_type file_header_type;

    typedef typename base_type::metadata_type metadata_type;

    typedef typename base_type::global_struct_type global_struct_type;

protected:

    static const foffset_t Align = base_type::Align;

    typedef typename base_type::file_metadata_type file_metadata_type;

    static const foffset_t file_metadata_size =
        base_type::file_metadata_size;

    static const foffset_t file_metadata_size_align =
        base_type::file_metadata_size_align;

    static const foffset_t global_struct_size =
        base_type::global_struct_size;

protected:

    void assert_header(
        Buffer& buffer
    ) const
    {
        meta_file_header header;

        const foffset_t header_offset = base_type::get_header_offset();

        buffer.seek(header_offset);
        buffer.read_object(header, true);

        meta_file_header new_header(Align, base_type::get_meta_check_size(),
            file_metadata_size, global_struct_size);

        try
        {
            new_header.assert_other(header);
        }
        catch (const io_exception& e)
        {
            EX3_RETHROW(e
                << file_position_ei(header_offset)
                << file_name_ei(buffer.filename()));
        }
    }

    void assert_meta_check(
        Buffer& buffer
    ) const
    {
        meta_file_header header;

        buffer.seek(base_type::get_header_offset());
        buffer.read_object(header, true);

        const foffset_t check_sz = header.checksz;

        const foffset_t ref_check_sz = base_type::get_meta_check_size();

        if (ref_check_sz != check_sz)
        {
            EX3_THROW(check_data_mismatch_exception()
                << expected_size_ei(ref_check_sz)
                << actual_size_ei(check_sz)
                << file_name_ei(buffer.filename()));
        }

        std::vector<char> check(check_sz);

        const foffset_t check_offset = base_type::get_meta_check_offset();

        buffer.seek(check_offset);
        buffer.read(&check[0], check.size(), true, true);

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
                << file_name_ei(buffer.filename()));
        }
    }

    void create_header(
        Buffer& buffer
    ) const
    {
        // TODO refactor to metadata_base
        meta_file_header new_header(Align, base_type::get_meta_check_size(),
            file_metadata_size, global_struct_size);

        buffer.seek(base_type::get_header_offset());
        buffer.write_object(new_header);
    }

    void create_meta_check(
        Buffer& buffer
    ) const
    {
        const char* p_src_check = base_type::meta_adapter().
            get_meta_check_ptr();

        foffset_t check_sz = base_type::get_meta_check_size();

        buffer.seek(base_type::get_meta_check_offset());
        buffer.write(p_src_check, check_sz);
    }

    global_struct_type read_global_struct(
        Buffer& buffer
    ) const
    {
        global_struct_type glob;
        buffer.seek(base_type::get_global_struct_offset());
        buffer.read_object(glob, true);
        return glob;
    }

    void write_global_struct(
        Buffer& buffer,
        const global_struct_type& glob
    ) const
    {
        buffer.seek(base_type::get_global_struct_offset());
        buffer.write_object(glob);
    }

    bool read_file_element(
        Buffer& buffer,
        s1b::uid_t uid,
        file_metadata_type& elem,
        bool required
    ) const
    {
        if (uid < FirstUID)
        {
            EX3_THROW(invalid_uid_exception()
                << requested_uid_ei(uid)
                << file_name_ei(buffer.filename()));
        }

        buffer.seek(base_type::get_element_offset_unsafe(uid));

        try
        {
            if (buffer.read_object(elem, required) == 0)
                return false;
        }
        catch (const io_exception& e)
        {
            EX3_RETHROW(e
                << requested_uid_ei(uid)
                << file_name_ei(buffer.filename()));
        }

        s1b::uid_t file_uid = base_type::meta_adapter().get_uid(elem);

        if (file_uid != uid)
        {
            EX3_THROW(element_mismatch_exception()
                << requested_uid_ei(uid)
                << actual_uid_ei(file_uid)
                << file_name_ei(buffer.filename()));
        }

        return true;
    }

public:

    rwp_metadata_base(
        const MetaAdapter& meta_adapter=MetaAdapter()
    ) :
        metadata_base<MetaAdapter>(meta_adapter)
    {
    }
};

}
