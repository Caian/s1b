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

#include <s1b/managed_buffer.hpp>
#include <s1b/managed_index.hpp>

#include <exception>
#include <iterator>

struct DummyInitializer
{
    typedef s1b::managed_buffer::managed_file managed_file;
    char* operator ()(
        managed_file&
    ) const
    {
        return 0;
    }
};

struct FixedSizeInitializer
{
    typedef s1b::managed_buffer::managed_file managed_file;
    typedef boost::interprocess::allocator<
        char,
        managed_file::segment_manager
        > allocator_type;

    size_t _size;

    FixedSizeInitializer(
        size_t size
    ) :
        _size(size)
    {
    }

    size_t size(
    ) const
    {
        return _size;
    }

    char* operator ()(
        managed_file& file
    ) const
    {
        allocator_type allocator(file.get_segment_manager());
        return reinterpret_cast<char*>(allocator.allocate(_size).get());
    }
};

struct RandomDataInitializer
{
    FixedSizeInitializer _initializer;

    RandomDataInitializer(
        size_t size
    ) :
        _initializer(size)
    {
    }

    char make_byte(
        size_t i
    ) const
    {
        return static_cast<char>(32 + ((_initializer.size() +
            i + 23) % (127-32)));
    }

    char* operator ()(
        FixedSizeInitializer::managed_file& file
    ) const
    {
        char* p_data = _initializer(file);
        for (size_t i = 0; i < _initializer.size(); i++)
            p_data[i] = make_byte(i);
        return p_data;
    }
};

struct CustomException
{
};

struct CustomDerivedException : public std::exception
{
};

struct ThrowInitializer
{
    typedef s1b::managed_buffer::managed_file managed_file;
    char* operator ()(
        managed_file&
    ) const
    {
        throw CustomException();
    }
};

struct DerivedThrowInitializer
{
    typedef s1b::managed_buffer::managed_file managed_file;
    char* operator ()(
        managed_file&
    ) const
    {
        throw CustomDerivedException();
    }
};

class EmptyTestIndex
{
public:

    static const char* get_id(
    )
    {
        return "";
    }

    template <typename ITU, typename ITM, typename Allocator>
    EmptyTestIndex(
        ITU,
        ITU,
        ITM,
        Allocator
    )
    {
    }
};

class SimpleTestIndex
{
private:

    void* p_data;

public:

    static const char* get_id(
    )
    {
        return "simple";
    }

    template <typename ITU, typename ITM, typename Allocator>
    SimpleTestIndex(
        ITU begin,
        ITU end,
        ITM,
        Allocator allocator
    ) :
        p_data(allocator.allocate(1024 * std::distance(begin, end)).get())
    {
    }
};

class ThrowTestIndex
{
public:

    static const char* get_id(
    )
    {
        return "throw1";
    }

    template <typename ITU, typename ITM, typename Allocator>
    ThrowTestIndex(
        ITU,
        ITU,
        ITM,
        Allocator
    )
    {
        throw CustomException();
    }
};

class DerivedThrowTestIndex
{
public:

    static const char* get_id(
    )
    {
        return "throw1";
    }

    template <typename ITU, typename ITM, typename Allocator>
    DerivedThrowTestIndex(
        ITU,
        ITU,
        ITM,
        Allocator
    )
    {
        throw CustomDerivedException();
    }
};

typedef s1b::managed_index<EmptyTestIndex> test_managed_index_1;
typedef s1b::managed_index<SimpleTestIndex> test_managed_index_2;
typedef s1b::managed_index<ThrowTestIndex> test_managed_index_throw;
typedef s1b::managed_index<DerivedThrowTestIndex> test_managed_index_dthrow;
