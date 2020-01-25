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
#include "meta_file_header.hpp"

#include "traits/metadata_type.hpp"
#include "traits/global_struct_type.hpp"

#include <iterator>

namespace s1b {

template <typename MetaAdapter>
class metadata_base
{
public:

    typedef meta_file_header file_header_type;

    typedef typename traits::metadata_type<
        MetaAdapter
    >::type metadata_type;

    typedef typename traits::global_struct_type<
        MetaAdapter
    >::type global_struct_type;

protected:

    static const foffset_t Align = S1B_META_ALIGNMENT_BYTES;

    typedef s1b::file_metadata<
        metadata_type
    > file_metadata_type;

    static const foffset_t file_metadata_size = sizeof(file_metadata_type);

    static const foffset_t file_metadata_size_align =
        mem_align::size_s<Align, file_metadata_size>::value;

    static const foffset_t global_struct_size = sizeof(global_struct_type);

private:

    MetaAdapter _meta_adapter;

protected:

    foffset_t get_header_offset(
    ) const
    {
        return 0;
    }

    foffset_t get_global_struct_offset(
    ) const
    {
        return
            get_header_offset() +
            mem_align::size_s<Align, sizeof(file_header_type)>::value;
    }

    foffset_t get_meta_check_offset(
    ) const
    {
        return
            get_global_struct_offset() +
            mem_align::size_s<Align, global_struct_size>::value;
    }

    foffset_t get_metadata_offset(
    ) const
    {
        return
            get_meta_check_offset() +
            mem_align::size<Align>(get_meta_check_size());
    }

    foffset_t get_element_offset_unsafe(
        s1b::uid_t uid
    ) const
    {
        return
            get_metadata_offset() +
            get_element_base_offset_unsafe(uid);
    }

    foffset_t get_element_base_offset_unsafe(
        s1b::uid_t uid
    ) const
    {
        // TODO check uid==0

        return file_metadata_size_align * (uid - 1);
    }

    foffset_t get_meta_check_size(
    ) const
    {
        return _meta_adapter.get_meta_check_size();
    }

    template <typename IT>
    foffset_t compute_file_size(
        IT metadata_begin,
        IT metadata_end
    ) const
    {
        foffset_t num_elements = std::distance(metadata_begin, metadata_end);
        return get_element_offset_unsafe(num_elements + 1);
    }

    foffset_t compute_aligned_data_size(
        foffset_t size
    ) const
    {
        return mem_align::size<S1B_DATA_ALIGNMENT_BYTES>(size);
    }

    s1b::uid_t compute_num_elements(
        foffset_t file_size
    ) const
    {
        const foffset_t elements_offset = get_element_offset_unsafe(FirstUID);
        const foffset_t elements_size = file_size - elements_offset;

        if (!mem_align::is_aligned<Align>(file_size))
        {
            EX3_THROW(misaligned_exception()
                << file_size_ei(file_size)
                << expected_alignment_ei(
                    static_cast<foffset_t>(Align)));
        }

        if (elements_size < 0)
        {
            EX3_THROW(file_size_mismatch_exception()
                << file_size_ei(file_size)
                << file_position_ei(elements_offset));
        }

        // Compute the number of elements
        s1b::uid_t num_elements = elements_size / file_metadata_size_align;

        // Check if the metadata size is multiple of the metadata size
        const foffset_t extra_bytes = elements_size %
            file_metadata_size_align;

        if (extra_bytes != 0)
        {
            EX3_THROW(extra_elem_bytes_exception()
                << file_size_ei(file_size)
                << expected_alignment_ei(
                    static_cast<foffset_t>(file_metadata_size_align))
                << file_position_ei(elements_offset)
                << actual_size_ei(elements_size));
        }

        return num_elements;
    }

public:

    metadata_base(
        const MetaAdapter& meta_adapter=MetaAdapter()
    ) :
        _meta_adapter(meta_adapter)
    {
    }

    const MetaAdapter& meta_adapter(
    ) const
    {
        return _meta_adapter;
    }
};

}
