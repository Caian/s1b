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

namespace s1b {
namespace queries {

template <typename Point>
struct closed_interval
{
    typedef closed_interval<Point> this_type;

    Point point_min;
    Point point_max;

    closed_interval(
        const Point& point_min,
        const Point& point_max
    ) :
        point_min(point_min),
        point_max(point_max)
    {
    }

    closed_interval(
        const this_type& other
    ) :
        point_min(other.point_min),
        point_max(other.point_max)
    {
    }
};

template <typename T>
closed_interval<T> make_closed_interval(
    const T& point_min,
    const T& point_max
)
{
    return closed_interval<T>(point_min, point_max);
}

}}
