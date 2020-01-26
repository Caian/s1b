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

#include <s1b/path_string.hpp>
#include <gtest/gtest.h>

#include <utility>

namespace {

TEST(S1B_TEST_NAME, LittleBitLong)
{
    const size_t ssz = S1B_STATIC_STRING_SIZE+1;
    char s[ssz+1];

    std::fill(s, s+ssz, 'A');
    s[ssz] = 0;

    try
    {
        s1b::path_string filename(s);

        FAIL() << "path_string constructor did not throw any exception!";
    }
    catch (
        const s1b::file_name_too_long_exception& ex
    )
    {
        const std::string* file_name;
        file_name = boost::get_error_info<s1b::file_name_ei>(ex);

        ASSERT_NE((void*)0, file_name);
        ASSERT_STREQ(s, file_name->c_str());

        const s1b::foffset_t* maximum_size;
        maximum_size = boost::get_error_info<s1b::maximum_size_ei>(ex);

        ASSERT_NE((void*)0, maximum_size);
        ASSERT_EQ(S1B_STATIC_STRING_SIZE, *maximum_size);

        const s1b::foffset_t* actual_size;
        actual_size = boost::get_error_info<s1b::actual_size_ei>(ex);

        ASSERT_NE((void*)0, actual_size);
        ASSERT_EQ(ssz, *actual_size);
    }
    catch (...)
    {
        FAIL() << "Unexpected exception thrown by path_string constructor!";
    }
}

}
