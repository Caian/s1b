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

#include <s1b/exceptions.hpp>
#include <s1b/traits/metadata_type.hpp>

namespace s1b {
namespace iterators {
namespace transforms {

template <typename Metadata>
struct get_rwp_metadata
{
    typedef uid_t input_type;

    typedef typename traits::metadata_type<
        Metadata
        >::type value_type;

    typedef value_type reference;

    typedef Metadata& MetadataRef;

    typedef Metadata*  MetadataPtr;

    MetadataPtr _metadata;

    value_type operator ()(
        const input_type& val
    ) const
    {
        if (_metadata == 0)
        {
            EX3_THROW(not_initialized_exception());
        }

        value_type meta;

        if (!_metadata->read(val, meta))
        {
            EX3_THROW(invalid_uid_exception()
                << requested_uid_ei(val)
                << file_name_ei(_metadata->filename()));
        }

        return meta;
    }

    get_rwp_metadata(
    ) :
        _metadata(0)
    {
    }

    get_rwp_metadata(
        MetadataRef metadata
    ) :
        _metadata(&metadata)
    {
    }
};

}}}
