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

#include <s1b/types.hpp>
#include <s1b/metadata.hpp>

#include <string>

struct test_metadata
{
    int uid;
    int size;
    float x;
    float y;
    short value1;
    char value2;

    bool operator ==(
        const test_metadata& other
    ) const
    {
        return
            this->uid == other.uid &&
            this->size == other.size &&
            this->x == other.x &&
            this->y == other.y &&
            this->value1 == other.value1 &&
            this->value2 == other.value2;
    }
};

struct test_global_data
{
    int test;

    test_global_data(
    ) :
        test(5)
    {
    }
};

struct test_adapter
{
    typedef test_metadata metadata_type;
    typedef test_global_data global_struct_type;

    const std::string check;

    s1b::uid_t get_uid(
        const metadata_type& p_meta
    ) const
    {
        return static_cast<s1b::uid_t>(p_meta.uid);
    }

    void set_uid(
        metadata_type& p_meta,
        const s1b::uid_t& uid
    ) const
    {
        p_meta.uid = static_cast<int>(uid);
    }

    size_t get_data_size(
        const metadata_type& p_meta
    ) const
    {
        return static_cast<size_t>(p_meta.size);
    }

    s1b::c_meta_check_ptr_t get_meta_check_ptr(
    ) const
    {
        return check.c_str();
    }

    size_t get_meta_check_size(
    ) const
    {
        return check.size() + 1;
    }

    test_adapter(
    ) :
        check("s1b_metadata_test")
    {
    }
};

struct initialize_metadata
{
    static void small_1(
        test_metadata& meta
    )
    {
        meta.size = 17;
        meta.x = 5;
        meta.y = -8;
        meta.value1 = 500;
        meta.value2 = -79;
    }

    static void small_2(
        test_metadata& meta
    )
    {
        meta.size = 17;
        meta.x = -7.9456;
        meta.y = 12.267;
        meta.value1 = -500;
        meta.value2 = 121;
    }

    static void small_3(
        test_metadata& meta
    )
    {
        meta.size = 29;
        meta.x = -14.24;
        meta.y = -1.77;
        meta.value1 = 200;
        meta.value2 = 25;
    }

    static void small_i(
        test_metadata& meta,
        unsigned int i
    )
    {
        float fi = static_cast<float>(i % 100);

        meta.uid = i;
        meta.size = 15 + (i % 23);
        meta.x = 3 * fi * fi -77.24 * fi - 9;
        meta.y = -1.77 * fi * fi + 12 * fi + 20;
        meta.value1 = -10000 + (i % 20000);
        meta.value2 = -50 + (i % 100);
    }

    static void large_1(
        test_metadata& meta
    )
    {
        meta.size = 12745;
        meta.x = 53845.23;
        meta.y = 1746.98;
        meta.value1 = 3;
        meta.value2 = -1;
    }
};
