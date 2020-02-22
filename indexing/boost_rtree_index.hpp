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

#include "detail/boost_point_traits.hpp"

#include <s1b/exceptions.hpp>
#include <s1b/queries/nearest.hpp>
#include <s1b/queries/closed_interval.hpp>
#include <s1b/traits/num_spatial_dims.hpp>
#include <s1b/traits/spatial_value_type.hpp>
#include "s1b/helpers/copy_spatial_point.hpp"
#include <s1b/iterators/spatial_point_iterator.hpp>
#include <s1b/iterators/make_drop_ref_iterator.hpp>
#include <s1b/iterators/transforms/get_first_by_val.hpp>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/fusion/adapted/std_pair.hpp>

#include <string>
#include <utility>
#include <iterator>

namespace s1b {
namespace indexing {

template <
    typename Key,
    typename Params,
    typename CoordSys,
    typename Allocator
    >
class boost_rtree_index
{
public:

    static std::string get_id(
    )
    {
        return std::string("s1b::indexing::boost_rtree_index/") +
            Key::get_id();
    }

public:

    typedef Params params_t;

    static const unsigned int num_spatial_dims =
        traits::num_spatial_dims<Key>::value;

    typedef typename traits::spatial_value_type<
        Key
        >::type spatial_value_type;

    typedef boost::geometry::model::point<
        spatial_value_type,
        num_spatial_dims,
        CoordSys
        > spatial_point_type;

    typedef std::pair<spatial_point_type, s1b::uid_t> indexed_type;

    typedef boost::geometry::index::equal_to<indexed_type> equal_to_t;

    typedef typename Allocator::template rebind<
        indexed_type
        >::other allocator_t;

    typedef iterators::transforms::get_first_by_val<
        spatial_point_type
        > indexable_t;

    typedef boost::geometry::index::rtree<
        indexed_type,
        params_t,
        indexable_t,
        equal_to_t,
        allocator_t
        > rtree;

    typedef typename rtree::const_iterator walk_iterator;
    typedef typename rtree::const_query_iterator query_iterator;

private:

    rtree _rtree;

private:

    template <typename ITU, typename ITM> // TODO Add metadata_end to function
    ITM get_metadata_end(
        ITU uid_begin,
        ITU uid_end,
        ITM metadata_begin
    ) const
    {
        ITM metadata_end = metadata_begin;
        std::advance(metadata_end, std::distance(uid_begin, uid_end));
        return metadata_end;
    }

    Key point_to_key(
        const spatial_point_type& point
    ) const
    {
        Key key;
        helpers::copy_spatial_point(point, key);
        return key;
    }

    spatial_point_type key_to_point(
        const Key& key
    ) const
    {
        spatial_point_type point;
        helpers::copy_spatial_point(key, point);
        return point;
    }

public:

    template <typename ITU, typename ITM>
    boost_rtree_index(
        ITU uid_begin,
        ITU uid_end,
        ITM metadata_begin,
        Allocator allocator
    ) :
        _rtree(
            boost::make_zip_iterator(
                std::make_pair(
                    iterators::make_spatial_point_iterator<
                        Key,
                        spatial_point_type
                        >(metadata_begin),
                    iterators::make_drop_ref_iterator(
                        uid_begin))),
            boost::make_zip_iterator(
                std::make_pair(
                    iterators::make_spatial_point_iterator<
                        Key,
                        spatial_point_type
                        >(get_metadata_end(
                            uid_begin, uid_end, metadata_begin)),
                    iterators::make_drop_ref_iterator(
                        uid_end))),
            params_t() /* TODO pass as parameter */,
            indexable_t(), equal_to_t(), allocator)
    {
    }

    s1b::uid_t size(
    ) const
    {
        return _rtree.size();
    }

    walk_iterator begin(
    ) const
    {
        return _rtree.begin();
    }

    walk_iterator end(
    ) const
    {
        return _rtree.end();
    }

    std::pair<Key, Key> get_bounds(
    ) const
    {
        if (_rtree.size() == 0)
        {
            EX3_THROW(empty_index_exception()
                << index_id_ei(get_id()));
        }

        typename rtree::bounds_type bounds = _rtree.bounds();
        Key point_min = point_to_key(bounds.min_corner());
        Key point_max = point_to_key(bounds.max_corner());
        return std::make_pair(point_min, point_max);
    }

    void get_bounds(
        Key& point_min,
        Key& point_max
    ) const
    {
        boost::tie(point_min, point_max) = get_bounds();
    }

    template<typename Predicates>
    void query(
        const Predicates& predicates,
        query_iterator& begin,
        query_iterator& end
    ) const
    {
        begin = _rtree.qbegin(predicates);
        end = _rtree.qend();
    }

    void query(
        const queries::closed_interval<Key>& predicates,
        query_iterator& begin,
        query_iterator& end
    ) const
    {
        using namespace boost::geometry::index;
        using namespace boost::geometry::model;

        const Key& key_min = predicates.point_min;
        const Key& key_max = predicates.point_max;

        const spatial_point_type point_min = key_to_point(key_min);
        const spatial_point_type point_max = key_to_point(key_max);

        begin = _rtree.qbegin(
            intersects(box<spatial_point_type>(point_min, point_max))
        );

        end = _rtree.qend();
    }

    void query(
        const queries::nearest<Key>& predicates,
        query_iterator& begin,
        query_iterator& end
    ) const
    {
        using namespace boost::geometry::index;

        const spatial_point_type& point = predicates.point;
        const unsigned int k = predicates.k;

        begin = _rtree.qbegin(
            nearest(point, k)
        );

        end = _rtree.qend();
    }
};

}}
