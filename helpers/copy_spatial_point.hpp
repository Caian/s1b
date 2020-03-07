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

#include <s1b/traits/num_spatial_dims.hpp>
#include <s1b/traits/get_spatial_dim.hpp>
#include <s1b/traits/set_spatial_dim.hpp>

#include <boost/mpl/integral_c.hpp>

namespace s1b {
namespace helpers {

namespace detail {

template <typename From, typename To>
struct copy_spatial_point_impl
{
    typedef boost::mpl::integral_c<
        unsigned int,
        traits::num_spatial_dims<
            From
            >::value
        > dims_end_type;

    static void apply(
        const From& from,
        To& to
    )
    {
        copy_dim(from, to, boost::mpl::integral_c<unsigned int, 0>());
    }

    template <typename T>
    static void copy_dim(
        const From& from,
        To& to,
        const T&
    )
    {
        static const unsigned int I = T::value;

        traits::set_spatial_dim<I, To>::apply(to,
            traits::get_spatial_dim<I, From>::apply(from));

        copy_dim(from, to, boost::mpl::integral_c<unsigned int, I+1>());
    }

    static void copy_dim(
        const From&,
        To&,
        const dims_end_type&
    )
    {
    }
};

}

template <typename From, typename To>
static void copy_spatial_point(
    const From& from,
    To& to
)
{
    detail::copy_spatial_point_impl<From, To>::apply(from, to);
}

}}
