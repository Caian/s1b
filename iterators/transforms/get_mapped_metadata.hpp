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

#include <s1b/exceptions.hpp>
#include <s1b/traits/metadata_type.hpp>

#include <boost/mpl/if.hpp>

namespace s1b {
namespace iterators {
namespace transforms {

template <typename Metadata, bool Const>
struct get_mapped_metadata
{
    typedef uid_t input_type;

    typedef typename traits::metadata_type<
        Metadata
        >::type value_type;

    typedef typename boost::mpl::if_c<
        Const,
        const value_type&,
        value_type&
        >::type reference;

    typedef typename boost::mpl::if_c<
        Const,
        const Metadata&,
        Metadata&
        >::type MetadataRef;

    typedef typename boost::mpl::if_c<
        Const,
        const Metadata*,
        Metadata*
        >::type MetadataPtr;

    MetadataPtr _metadata;

    reference operator ()(
        const input_type& val
    ) const
    {
        if (_metadata == 0)
        {
            EX3_THROW(not_initialized_exception());
        }

        return _metadata->get_element(val);
    }

    get_mapped_metadata(
    ) :
        _metadata(0)
    {
    }

    get_mapped_metadata(
        MetadataRef metadata
    ) :
        _metadata(&metadata)
    {
    }
};

}}}
