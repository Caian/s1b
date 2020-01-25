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

#include "uid_iterator.hpp"

#include "transforms/get_mapped_metadata.hpp"

#include <boost/iterator/transform_iterator.hpp>

namespace s1b {
namespace iterators {

template <typename Metadata>
struct mapped_metadata_iterator_helper
{
    typedef typename transforms::get_mapped_metadata<
        Metadata,
        false
        > transform_type;

    typedef typename transforms::get_mapped_metadata<
        Metadata,
        true
        > transform_ctype;

    typedef boost::transform_iterator<
        transform_type,
        uid_iterator,
        typename transform_type::reference,
        typename transform_type::value_type
        > type;

    typedef boost::transform_iterator<
        transform_ctype,
        uid_iterator,
        typename transform_ctype::reference,
        typename transform_ctype::value_type
        > ctype;

    static type begin(
        Metadata& metadata
    )
    {
        transform_type transform(metadata);
        return type(metadata.uid_begin(), transform);
    }

    static type end(
        Metadata& metadata
    )
    {
        transform_type transform(metadata);
        return type(metadata.uid_end(), transform);
    }

    static ctype begin(
        const Metadata& metadata
    )
    {
        transform_ctype transform(metadata);
        return ctype(metadata.uid_begin(), transform);
    }

    static ctype end(
        const Metadata& metadata
    )
    {
        transform_ctype transform(metadata);
        return ctype(metadata.uid_end(), transform);
    }
};

}}
