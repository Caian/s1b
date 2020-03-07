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

#include "metadata_test.hpp"

#include <s1b/managed_index.hpp>

struct test_key
{
    static std::string get_id(
    )
    {
        return "test_key";
    }

    float x;
    char value2;

    test_key(
    ) :
        x(),
        value2()
    {
    }

    test_key(
        const test_metadata& meta
    ) :
        x(meta.x),
        value2(meta.value2)
    {
    }

    bool operator <(
        const test_key& other
    ) const
    {
        return (x != other.x ? (x < other.x) : (value2 < other.value2));
    }
};
