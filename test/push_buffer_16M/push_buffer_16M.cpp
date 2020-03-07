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

#define S1B_PUSH_BUFFER_SIZE 16ULL * 1024ULL * 1024ULL
#define S1B_TEST_NAME PushBuffer16M

#include "../push_buffer_base.hpp"

S1B_TEST(AssertBufferSize)

    (void)s1b_file_name;
    ASSERT_EQ(16ULL * 1024ULL * 1024ULL, S1B_PUSH_BUFFER_SIZE);
}