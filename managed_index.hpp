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
#include "exceptions.hpp"
#include "path_string.hpp"
#include "managed_buffer.hpp"
#include "mapped_metadata.hpp"
#include "rwp_metadata.hpp"

#include "iterators/mapped_metadata_iterator.hpp"
#include "iterators/rwp_metadata_iterator.hpp"

#include <s1b/traits/indexed_type.hpp>

#include <boost/interprocess/offset_ptr.hpp>

#include <algorithm>
#include <string>

namespace s1b {

template <typename Index>
class managed_index
{
private:

    struct MANAGED_DATA
    {
        boost::interprocess::offset_ptr<char> p_id;
        boost::interprocess::offset_ptr<int> p_id_sz;
        boost::interprocess::offset_ptr<Index> p_index;
    };

    template <typename ITU, typename ITM>
    struct INDEX_INITIALIZER
    {
        ITU _uid_begin;
        ITU _uid_end;
        ITM _metadata_begin;

        INDEX_INITIALIZER(
            ITU uid_begin,
            ITU uid_end,
            ITM metadata_begin
        ) :
            _uid_begin(uid_begin),
            _uid_end(uid_end),
            _metadata_begin(metadata_begin)
        {
        }

        const char* operator ()(
            managed_buffer::managed_file& mfile
        ) const
        {
            // Create the allocator for the managed memory

            managed_buffer::managed_allocator allocator(
                mfile.get_segment_manager());

            // Create an id to verify if the index inside the file matches
            // the type being used to open it

            std::string id(get_id());

            // Construct a struct to hold the pointers

            MANAGED_DATA* p_data = mfile.construct<MANAGED_DATA>(
                boost::interprocess::anonymous_instance)();

            // Construct the verification string and the index itself

            p_data->p_id_sz = reinterpret_cast<int*>(
                allocator.allocate(sizeof(int)).get());

            *p_data->p_id_sz = id.size();

            p_data->p_id = allocator.allocate(id.size() + 1);

            std::copy(id.begin(), id.end(), p_data->p_id);
            p_data->p_id[id.size()] = '\0';

            p_data->p_index = mfile.construct<Index>(boost::interprocess::
                anonymous_instance)(_uid_begin, _uid_end,
                    _metadata_begin, allocator);

            return reinterpret_cast<const char*>(p_data);
        }
    };

    void assert_index_id(
    ) const
    {
        // Retrieve the id from the index file and compare it to the id of
        // the Index type

        std::string index_id(get_id());

        const size_t check_sz = *_p_data->p_id_sz;
        const size_t ref_check_sz = index_id.size();

        if (ref_check_sz != check_sz)
        {
            EX3_THROW(index_id_mismatch_exception()
                << expected_size_ei(ref_check_sz)
                << actual_size_ei(check_sz)
                << file_name_ei(filename()));
        }

        const char* const p_check = _p_data->p_id.get();

        const char* const p_ref_check_begin = index_id.c_str();

        const char* const p_ref_check_end = p_ref_check_begin + check_sz;

        std::pair<const char*, const char*> comp;

        comp = std::mismatch(p_ref_check_begin, p_ref_check_end, p_check);

        if (comp.first != p_ref_check_end)
        {
            const foffset_t offset = std::distance(
                p_ref_check_begin, comp.first);

            EX3_THROW(index_id_mismatch_exception()
                << expected_value_ei(*comp.first)
                << actual_value_ei(*comp.second)
                << offset_ei(offset)
                << file_name_ei(filename()));
        }
    }

private:

    static std::string get_id(
    )
    {
        // TODO fix when put in right place
        return std::string("s1b::managed_index/") + Index::get_id();
    }

    template <typename ITU, typename ITM>
    static INDEX_INITIALIZER<ITU, ITM> create_initializer(
        ITU uid_begin,
        ITU uid_end,
        ITM metadata_begin
    )
    {
        return INDEX_INITIALIZER<ITU, ITM>(uid_begin,
            uid_end, metadata_begin);
    }

private:

    managed_buffer _buffer;
    const MANAGED_DATA* _p_data;

public:

    template <typename Adapter>
    managed_index(
        const path_string& filename,
        const mapped_metadata<Adapter>& metadata,
        size_t initial_size,
        unsigned int resize_attempts,
        size_t size_increment
    ) :
        _buffer(filename, initial_size, create_initializer(
            metadata.uid_begin(),
            metadata.uid_end(),
            iterators::mapped_metadata_iterator_helper<
                mapped_metadata<Adapter>
                >::begin(metadata)
            ), resize_attempts, size_increment),
        _p_data(reinterpret_cast<const MANAGED_DATA*>(
            _buffer.data()))
    {
    }

    template <typename Adapter>
    managed_index(
        const path_string& filename,
        rwp_metadata<Adapter>& metadata,
        size_t initial_size,
        unsigned int resize_attempts,
        size_t size_increment
    ) :
        _buffer(filename, initial_size, create_initializer(
            metadata.uid_begin(),
            metadata.uid_end(),
            iterators::rwp_metadata_iterator_helper<
                rwp_metadata<Adapter>
                >::begin(metadata)
        ), resize_attempts, size_increment),
        _p_data(reinterpret_cast<const MANAGED_DATA*>(
            _buffer.data()))
    {
    }

    managed_index(
        const path_string& filename
    ) :
        _buffer(filename),
        _p_data(reinterpret_cast<const MANAGED_DATA*>(
            _buffer.data()))
    {
        assert_index_id();
    }

    const path_string& filename(
    ) const
    {
        return _buffer.filename();
    }

    bool can_write(
    ) const
    {
        return false;
    }

    const Index* get_index(
    ) const
    {
        return _p_data->p_index.get();
    }

    const char* get_index_id(
    ) const
    {
        return _p_data->p_id.get();
    }

    size_t size(
    ) const
    {
        return _buffer.size();
    }

    size_t get_used_bytes(
    ) const
    {
        return _buffer.get_used_bytes();
    }

    size_t get_free_bytes(
    ) const
    {
        return _buffer.get_free_bytes();
    }

    virtual ~managed_index(
    )
    {
    }
};

namespace traits
{

template <typename Index>
struct indexed_type<managed_index<Index> >
{
    typedef typename indexed_type<Index>::type type;
};

}

}
