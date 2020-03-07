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

#include <s1b/managed_index.hpp>
#include <s1b/queries/closed_interval.hpp>

#include <boost/sort/pdqsort/pdqsort.hpp>

#include <algorithm> // ? TODO
#include <iterator>
#include <string>

namespace s1b {
namespace indexing {

template <typename Key>
class ordered_sequence
{
public:

    typedef s1b::uid_t indexed_type;

    typedef const indexed_type* walk_iterator;

private:

    // TODO make template parameter of the constructor
    // so it can be used by code outside s1b...
    typedef typename s1b::managed_buffer::managed_allocator
        managed_allocator;

    typedef typename managed_allocator::template rebind<
        indexed_type
        >::other index_allocator;

    typedef boost::interprocess::offset_ptr<
        const indexed_type
        > index_ptr;

private:

    template <typename ITU, typename ITM>
    struct compare_lt
    {
        ITU _uid_begin;
        ITM _metadata_begin;

        compare_lt(
            ITU uid_begin,
            ITM metadata_begin
        ) :
            _uid_begin(uid_begin),
            _metadata_begin(metadata_begin)
        {
        }

        std::pair<Key, s1b::uid_t> make_index(
            const indexed_type& i
        ) const
        {
            ITM it = _metadata_begin;
            s1b::uid_t off = i - *_uid_begin;
            std::advance(it, off);
            return std::make_pair(Key(*it), i);
        }

        bool operator ()(
            const indexed_type& first,
            const indexed_type& second
        ) const
        {
            return make_index(first) < make_index(second);
        }
    };

private:

    s1b::uid_t _num_indices;
    index_ptr _p_indices;

public:

    static std::string get_id(
    )
    {
        return std::string("s1b::indexing::ordered_sequence/") +
            Key::get_id();
    }

    template <typename ITU, typename ITM>
    index_ptr construct(
        ITU uid_begin,
        ITU uid_end,
        ITM metadata_begin,
        index_allocator& allocator
    )
    {
        // Create a temporary pointer that is not const

        typedef boost::interprocess::offset_ptr<
            indexed_type
            > tmp_index_ptr;

        // Allocate the vector of indices

        tmp_index_ptr indices = allocator.allocate(_num_indices);

        // Copy the uids to the indices

        std::copy(uid_begin, uid_end, indices);

        // Sort the indices

        compare_lt<ITU, ITM> compare(uid_begin, metadata_begin);
        boost::sort::pdqsort(indices, indices + _num_indices, compare);

        return indices;
    }

    template <typename ITU, typename ITM>
    ordered_sequence(
        ITU uid_begin,
        ITU uid_end,
        ITM metadata_begin,
        index_allocator char_allocator
    ) :
        _num_indices(std::distance(uid_begin, uid_end)),
        _p_indices(construct(uid_begin, uid_end,
            metadata_begin, char_allocator))
    {
    }

    s1b::uid_t size(
    ) const
    {
        return _num_indices;
    }

    walk_iterator begin(
    ) const
    {
        return _p_indices.get();
    }

    walk_iterator end(
    ) const
    {
        return _p_indices.get() + _num_indices;
    }
};

}}
