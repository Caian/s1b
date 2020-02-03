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

#include <s1b/types.hpp>
#include <s1b/exceptions.hpp>
#include <s1b/managed_index.hpp>
#include <s1b/queries/closed_interval.hpp>

#include <boost/sort/pdqsort/pdqsort.hpp>

#include <algorithm>
#include <iterator>
#include <string>

namespace s1b {
namespace indexing {

template <typename Key>
class ordered_index
{
public:

    typedef std::pair<Key, s1b::uid_t> indexed_type;

    typedef const indexed_type* walk_iterator;
    typedef const indexed_type* query_iterator;

private:

    // TODO make template parameter of the constructor
    // so it can be used by code outside s1b
    typedef typename s1b::managed_buffer::managed_allocator
        managed_allocator;

    typedef typename managed_allocator::template rebind<
        indexed_type
        >::other index_allocator;

    typedef boost::interprocess::offset_ptr<
        const indexed_type
        > index_ptr;

private:

    struct compare_lt
    {
        bool operator ()(
            const indexed_type& first,
            const indexed_type& second
        ) const
        {
            return first.first < second.first;
        }
    };

private:

    s1b::uid_t _num_indices;
    index_ptr _p_indices;

public:

    static std::string get_id(
    )
    {
        return std::string("s1b::indexing::ordered_index/") +
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

        // Copy the data to the indices

        for (s1b::uid_t i = 0; i < _num_indices;
            i++, uid_begin++, metadata_begin++)
        {
            indices[i].first = Key(*metadata_begin);
            indices[i].second = *uid_begin;
        }

        // Sort the indices

        boost::sort::pdqsort(indices, indices + _num_indices);

        return indices;
    }

    template <typename ITU, typename ITM>
    ordered_index(
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

    std::pair<indexed_type, indexed_type> range(
    ) const
    {
        walk_iterator first = begin();
        walk_iterator last = end();

        if (first == last)
        {
            EX3_THROW(empty_index_exception()
                << index_id_ei(get_id()));
        }

        last--;

        return std::make_pair(*first, *last);
    }

    void range(
        indexed_type& index_min,
        indexed_type& index_max
    ) const
    {
        std::pair<indexed_type, indexed_type> minmax = range();
        index_min = minmax.first;
        index_max = minmax.second;
    }

    void query(
        const s1b::queries::closed_interval<Key>& predicates, // TODO rename interval
        query_iterator& begin,
        query_iterator& end
    ) const
    {
        const compare_lt compare;

        walk_iterator ibegin = this->begin();
        walk_iterator iend = this->end();

        const indexed_type point_min(predicates.point_min, s1b::uid_t());
        const indexed_type point_max(predicates.point_max, s1b::uid_t());

        begin = std::lower_bound(ibegin, iend, point_min, compare);
        end = std::upper_bound(begin, iend, point_max, compare);
    }
};

}}
