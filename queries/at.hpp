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

#include "closed_interval.hpp"

#include <s1b/exceptions.hpp>
#include <s1b/traits/query_iterator.hpp>

#include <iterator>

namespace s1b {
namespace queries {

template <typename Index, typename T>
typename traits::query_iterator<Index>::type at(
    const Index& index,
    const T& point
)
{
    typename traits::query_iterator<Index>::type begin, end, next;
    index.query(make_closed_interval(point, point), begin, end);

    if (begin == end)
    {
        EX3_THROW(not_found_exception()
            << index_id_ei(index.get_id())
            << expected_count_ei(1)
            << actual_count_ei(0));
    }

    next = begin;
    next++;

    if (next != end)
    {
        int64_t count = std::distance(begin, end);

        EX3_THROW(multiple_results_exception()
            << index_id_ei(index.get_id())
            << expected_count_ei(1)
            << actual_count_ei(count));
    }

    return begin;
}

}}
