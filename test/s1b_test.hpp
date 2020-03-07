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

#include <boost/preprocessor/stringize.hpp>

#include <gtest/gtest.h>

#define S1B_TEST(Name) \
    TEST(S1B_TEST_NAME, Name) { \
        const char* s1b_file_name = \
            BOOST_PP_STRINGIZE(S1B_TEST_NAME) \
            BOOST_PP_STRINGIZE(Name);
