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
#include "exceptions.hpp"
#include "path_string.hpp"
#include "mapped_buffer.hpp"
#include "metadata_base.hpp"
#include "meta_file_header.hpp"

#include "iterators/uid_iterator.hpp"

#include "traits/metadata_type.hpp"
#include "traits/global_struct_type.hpp"

#include <boost/uuid/uuid.hpp>

#include <algorithm>
#include <iterator>

namespace s1b {

// TODO assert meta check

/*
 * TODO
 */
template <typename MetaAdapter>
class mapped_metadata : public metadata_base<MetaAdapter>
{
public:

    typedef metadata_base<MetaAdapter> base_type;

    typedef typename base_type::metadata_type metadata_type;

    typedef typename base_type::global_struct_type global_struct_type;

protected:

    typedef typename base_type::file_metadata_type file_metadata_type;

private:

    mapped_buffer _buffer;
    const boost::uuids::uuid* const _uuid;
    s1b::uid_t _num_elements;

    const boost::uuids::uuid* assert_header(
    ) const
    {
        const foffset_t header_offset = base_type::get_header_offset();

        const meta_file_header* const p_header = _buffer.ptr<
            meta_file_header
            >(header_offset);

        meta_file_header new_header(
            base_type::Align,
            base_type::get_meta_check_size(),
            base_type::file_metadata_size,
            base_type::global_struct_size);

        try
        {
            new_header.assert_other(*p_header);
        }
        catch (const io_exception& e)
        {
            EX3_RETHROW(e
                << file_position_ei(header_offset)
                << file_name_ei(filename()));
        }

        return &p_header->uuid;
    }

    void assert_meta_check(
    ) const
    {
        const meta_file_header* const p_header = _buffer.ptr<
            meta_file_header
            >(base_type::get_header_offset());

        const size_t check_sz = p_header->checksz;

        const size_t ref_check_sz = base_type::get_meta_check_size();

        if (ref_check_sz != check_sz)
        {
            EX3_THROW(check_data_mismatch_exception()
                << expected_size_ei(ref_check_sz)
                << actual_size_ei(check_sz)
                << file_name_ei(filename()));
        }

        const foffset_t check_offset = base_type::get_meta_check_offset();

        const char* const p_check = _buffer.ptr<char>(check_offset);

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

    const boost::uuids::uuid* create_header(
    )
    {
        meta_file_header* const p_header = _buffer.ptr<meta_file_header>(
            base_type::get_header_offset());

        meta_file_header new_header(
            base_type::Align,
            base_type::get_meta_check_size(),
            base_type::file_metadata_size,
            base_type::global_struct_size);

        *p_header = new_header;

        return &p_header->uuid;
    }

    void create_global_struct(
    )
    {
        global_struct_type* const p_glob = _buffer.ptr<global_struct_type>(
            base_type::get_global_struct_offset());

        *p_glob = global_struct_type();
    }

    void create_meta_check(
    )
    {
        const char* const p_src_check = base_type::meta_adapter().
            get_meta_check_ptr();

        char* const p_dst_check = _buffer.ptr<char>(
            base_type::get_meta_check_offset());

        const size_t check_sz = base_type::get_meta_check_size();

        std::copy(p_src_check, p_src_check + check_sz, p_dst_check);
    }

    s1b::uid_t compute_num_elements(
        foffset_t file_size
    ) const
    {
        try
        {
            return base_type::compute_num_elements(file_size);
        }
        catch (const io_exception& e)
        {
            EX3_RETHROW(e
                << file_name_ei(filename()));
        }
    }

    void set_metadata(
        metadata_type& dest,
        const metadata_type& src
    ) const
    {
        dest = src;
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

            file_metadata_type* const p_elem = reinterpret_cast<
                file_metadata_type*
                >(_buffer.address() +
                base_type::get_element_offset_unsafe(uid));

            set_metadata(*p_elem, *metadata_begin);

            p_elem->data_offset = offset;
            p_elem->clean_bit = 0;

            base_type::meta_adapter().set_uid(*p_elem, uid);

            const size_t size = base_type::meta_adapter().
                get_data_size(*p_elem);

            offset += base_type::compute_aligned_data_size(size);
        }
    }

    void assert_uid_in_file(
        s1b::uid_t uid
    ) const
    {
        if (uid < FirstUID || uid > _num_elements)
        {
            EX3_THROW(invalid_uid_exception()
                << maximum_uid_ei(_num_elements)
                << requested_uid_ei(uid)
                << file_name_ei(filename()));
        }
    }

public:

    template <typename IT>
    mapped_metadata(
        const path_string& filename,
        IT metadata_begin,
        IT metadata_end,
        hugetlb_mode htlb_mode=S1B_HUGETLB_OFF
    ) :
        metadata_base<MetaAdapter>(),
        _buffer(
            filename,
            S1B_OPEN_NEW,
            base_type::compute_file_size(metadata_begin, metadata_end),
            htlb_mode),
        _uuid(create_header()),
        _num_elements(compute_num_elements(_buffer.size()))
    {
        create_global_struct();
        create_meta_check();
        copy_elements(metadata_begin, metadata_end);
    }

    mapped_metadata(
        const path_string& filename,
        bool can_write,
        hugetlb_mode htlb_mode=S1B_HUGETLB_OFF
    ) :
        metadata_base<MetaAdapter>(),
        _buffer(
            filename,
            can_write ? S1B_OPEN_WRITE : S1B_OPEN_DEFAULT,
            0,
            htlb_mode),
        _uuid(assert_header()),
        _num_elements(compute_num_elements(_buffer.size()))
    {
        assert_meta_check();
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

    const MetaAdapter& meta_adapter(
    ) const
    {
        return base_type::meta_adapter();
    }

    const boost::uuids::uuid& file_uuid(
    ) const
    {
        return *_uuid;
    }

    const global_struct_type& global_struct(
    ) const
    {
        return *reinterpret_cast<const global_struct_type*>(
            _buffer.address() + base_type::get_global_struct_offset());
    }

    global_struct_type& global_struct(
    )
    {
        return *reinterpret_cast<global_struct_type*>(_buffer.address() +
            base_type::get_global_struct_offset());
    }

    const metadata_type& get_element(
        s1b::uid_t uid,
        foffset_t& data_offset
    ) const
    {
        assert_uid_in_file(uid);

        const file_metadata_type* const p_elem = reinterpret_cast<
            const file_metadata_type*>(_buffer.address() +
            base_type::get_element_offset_unsafe(uid));

        // TODO consistency check uid matches

        data_offset = p_elem->data_offset;

        return *p_elem;
    }

    metadata_type& get_element(
        s1b::uid_t uid,
        foffset_t& data_offset
    )
    {
        assert_uid_in_file(uid);

        file_metadata_type* const p_elem = reinterpret_cast<
            file_metadata_type*>(_buffer.address() +
            base_type::get_element_offset_unsafe(uid));

        data_offset = p_elem->data_offset;

        return *p_elem;
    }

    const metadata_type& get_element(
        s1b::uid_t uid
    ) const
    {
        foffset_t data_offset;
        return get_element(uid, data_offset);
    }

    metadata_type& get_element(
        s1b::uid_t uid
    )
    {
        foffset_t data_offset;
        return get_element(uid, data_offset);
    }

    foffset_t get_data_offset(
        s1b::uid_t uid
    ) const
    {
        foffset_t data_offset;
        get_element(uid, data_offset);
        return data_offset;
    }

    foffset_t get_data_offset(
        const metadata_type& elem
    ) const
    {
        return get_data_offset(base_type::meta_adapter().get_uid(elem));
    }

    s1b::uid_t get_last_uid(
    ) const
    {
        return _num_elements;
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
    ) const
    {
        // Get the data offset and size of the last element and compute the
        // total size of the data file

        // WARNING: Data offsets must not be reordered for this to work!

        const s1b::uid_t last_uid = get_last_uid();

        if (last_uid == FirstUID-1)
            return 0;

        foffset_t last_data_offset;
        const metadata_type& last = get_element(last_uid, last_data_offset);
        const size_t size = base_type::meta_adapter().get_data_size(last);
        return last_data_offset + base_type::compute_aligned_data_size(size);
    }

    void sync(
    )
    {
        _buffer.sync();
    }

    virtual ~mapped_metadata(
    )
    {
    }
};

}
