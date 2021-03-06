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

#include "metadata_test.hpp"

#include <s1b/managed_index.hpp>

#include <boost/tuple/tuple.hpp>

struct rtree_test_key
{
    typedef float spatial_value_type;

    static const unsigned int num_spatial_dims = 2;

    static std::string get_id(
    )
    {
        return "rtree_test_key";
    }

    spatial_value_type x, y;

    rtree_test_key() :
        x(),
        y()
    {
    }

    rtree_test_key(
        const test_metadata& meta
    ) :
        x(meta.x),
        y(meta.y)
    {
    }

    template <unsigned int I>
    const spatial_value_type& get(
    ) const
    {
        return boost::tie(x, y).template get<I>();
    }

    template <unsigned int I>
    void set(
        const spatial_value_type& value
    )
    {
        boost::tie(x, y).template get<I>() = value;
    }
};
