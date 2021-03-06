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

#include <s1b/traits/set_spatial_dim.hpp>
#include <s1b/traits/num_spatial_dims.hpp>
#include <s1b/traits/spatial_value_type.hpp>

#include <boost/geometry/geometries/point.hpp>

namespace s1b {
namespace traits {

template <unsigned int I, typename T, std::size_t N, typename C>
struct set_spatial_dim<I, boost::geometry::model::point<T, N, C> >
{
    typedef T value_type;

    static void apply(
        boost::geometry::model::point<T, N, C>& who,
        const value_type& what
    )
    {
        who.template set<I>(what);
    }
};

template <typename T, std::size_t N, typename C>
struct num_spatial_dims<boost::geometry::model::point<T, N, C> >
{
    static const unsigned int value = N;
};

template <typename T, std::size_t N, typename C>
struct spatial_value_type<boost::geometry::model::point<T, N, C> >
{
    typedef T type;
};

}}
