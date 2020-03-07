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

#include <s1b/path_string.hpp>
#include <gtest/gtest.h>

#include <utility>

namespace {

void test_compare_strings(
    const char* s,
    const size_t ssz,
    const s1b::path_string& filename
)
{
    size_t sz;
    ASSERT_NO_THROW(sz = filename.size());
    ASSERT_EQ(ssz, sz);

    const char* cstr;
    ASSERT_NO_THROW(cstr = filename.c_str());
    ASSERT_STREQ(s, cstr);

    std::string str;
    ASSERT_NO_THROW(str = filename);
    ASSERT_EQ(ssz, str.size());
    ASSERT_STREQ(s, str.c_str());
}

void initialization_test(
    const s1b::path_string& filename
)
{
}

TEST(S1B_TEST_NAME, InitializationTest)
{
    const char* s = "hello, world!";
    std::string ss(s);

    ASSERT_NO_THROW(initialization_test("hello, world!"));
    ASSERT_NO_THROW(initialization_test(s));
    ASSERT_NO_THROW(initialization_test(ss));
}

TEST(S1B_TEST_NAME, Empty)
{
    const char* s = "";
    const size_t ssz = 0;

    try
    {
        s1b::path_string filename;
        test_compare_strings(s, ssz, filename);
    }
    catch(
        ...
    )
    {
        FAIL() << "Unexpected exception thrown by path_string constructor!";
    }
}

TEST(S1B_TEST_NAME, EmptyCopy)
{
    const char* s = "";
    const size_t ssz = 0;

    try
    {
        s1b::path_string other;
        s1b::path_string filename(other);
        test_compare_strings(s, ssz, filename);
    }
    catch(
        ...
    )
    {
        FAIL() << "Unexpected exception thrown by path_string constructor!";
    }
}

TEST(S1B_TEST_NAME, Valid)
{
    const size_t ssz = S1B_STATIC_STRING_SIZE;
    char s[ssz+1];

    std::fill(s, s+ssz, 'A');
    s[ssz] = 0;

    try
    {
        s1b::path_string filename(s);
        test_compare_strings(s, ssz, filename);
    }
    catch(
        ...
    )
    {
        FAIL() << "Unexpected exception thrown by path_string constructor!";
    }
}

TEST(S1B_TEST_NAME, ValidStdString)
{
    const size_t ssz = S1B_STATIC_STRING_SIZE;
    char s[ssz+1];

    std::fill(s, s+ssz, 'A');
    s[ssz] = 0;

    try
    {
        std::string ss(s);
        s1b::path_string filename(ss);
        test_compare_strings(s, ssz, filename);
    }
    catch(
        ...
    )
    {
        FAIL() << "Unexpected exception thrown by path_string constructor!";
    }
}

TEST(S1B_TEST_NAME, ValidCopy)
{
    const size_t ssz = S1B_STATIC_STRING_SIZE;
    char s[ssz+1];

    std::fill(s, s+ssz, 'A');
    s[ssz] = 0;

    try
    {
        s1b::path_string other(s);
        s1b::path_string filename(other);
        test_compare_strings(s, ssz, filename);
    }
    catch(
        ...
    )
    {
        FAIL() << "Unexpected exception thrown by path_string constructor!";
    }
}

}
