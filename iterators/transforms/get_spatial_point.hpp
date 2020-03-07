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

#include <s1b/helpers/copy_spatial_point.hpp>

namespace s1b {
namespace iterators {
namespace transforms {

template <typename Key, typename Point, typename MetadataType>
struct get_spatial_point
{
    typedef MetadataType input_type;

    typedef Point value_type;

    typedef value_type reference;

    value_type operator ()(
        const input_type& val
    ) const
    {
        Key key(val);
        Point point;
        helpers::copy_spatial_point(key, point);
        return point;
    }
};

}}}
