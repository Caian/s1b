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

#if defined(TEST_MOVE)
#define MAKE_TEST_OBJ(name) \
    TEST_OBJ_TYPE original_##name
#define MOVE_TEST_OBJ(name) \
    TEST_OBJ_TYPE name(boost::move(original_##name))
#elif defined (TEST_COPY)
#define MAKE_TEST_OBJ(name) \
    TEST_OBJ_TYPE original_##name
#define MOVE_TEST_OBJ(name) \
    TEST_OBJ_TYPE name(original_##name)
#else
#define MAKE_TEST_OBJ(name) TEST_OBJ_TYPE name
#define MOVE_TEST_OBJ(name)
#endif
