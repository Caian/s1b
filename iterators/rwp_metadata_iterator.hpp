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

#include "uid_iterator.hpp"

#include "transforms/get_rwp_metadata.hpp"

#include <s1b/macros.hpp>

#include <boost/iterator/transform_iterator.hpp>

namespace s1b {
namespace iterators {

template <typename Metadata>
struct rwp_metadata_iterator_helper
{
    typedef typename transforms::get_rwp_metadata<
        Metadata
        > transform_type;

    typedef boost::transform_iterator<
        transform_type,
        uid_iterator,
        typename transform_type::reference,
        typename transform_type::value_type
        > type;

    typedef type ctype;

    static type begin(
        const Metadata& metadata
    )
    {
        transform_type transform(metadata);
        return type(metadata.uid_begin(), transform);
    }
};

}}
