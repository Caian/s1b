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

#include "transforms/drop_ref.hpp"

#include <boost/iterator/transform_iterator.hpp>

#include <iterator>

namespace s1b {
namespace iterators {

template <typename IT>
struct make_drop_ref_iterator_helper
{
    typedef typename std::iterator_traits<
        IT
        >::value_type value_type;

    typedef typename transforms::drop_ref<
        value_type
        > transform_type;

    typedef boost::transform_iterator<
        transform_type,
        IT,
        typename transform_type::reference,
        typename transform_type::value_type
        > type;

    typedef type ctype;

    static type apply(
        IT it
    )
    {
        return type(it);
    }
};

template <typename IT>
static typename make_drop_ref_iterator_helper<IT>::ctype
make_drop_ref_iterator(
    IT it
)
{
    return make_drop_ref_iterator_helper<IT>::apply(it);
}

}}
