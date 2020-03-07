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

#include "transforms/get_spatial_point.hpp"

#include <boost/iterator/transform_iterator.hpp>

#include <iterator>

namespace s1b {
namespace iterators {

template <typename Key, typename Point, typename ITM>
struct spatial_point_iterator_helper
{
    typedef typename std::iterator_traits<
        ITM
        >::value_type metadata_type;

    typedef typename transforms::get_spatial_point<
        Key,
        Point,
        metadata_type
        > transform_type;

    typedef boost::transform_iterator<
        transform_type,
        ITM,
        typename transform_type::reference,
        typename transform_type::value_type
        > type;

    typedef type ctype;

    static type apply(
        ITM it
    )
    {
        return type(it);
    }
};

template <typename Key, typename Point, typename ITM>
static typename spatial_point_iterator_helper<Key, Point, ITM>::ctype
make_spatial_point_iterator(
    ITM it
)
{
    return spatial_point_iterator_helper<Key, Point, ITM>::apply(it);
}

}}
