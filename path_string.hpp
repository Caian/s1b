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

#include "exceptions.hpp"

#if !defined(S1B_STATIC_STRING_SIZE)
#define S1B_STATIC_STRING_SIZE 259
#endif

#include <algorithm>
#include <string>

#if defined(S1B_USE_STATIC_STRINGS)
#include <string.h>
#endif

namespace s1b {

#if defined(S1B_USE_STATIC_STRINGS)

class path_string
{
private:

    size_t _size;
    char _str[S1B_STATIC_STRING_SIZE+1];

public:

    path_string(
    ) :
        _size(0)
    {
        _str[_size] = 0;
    }

    path_string(
        const char* str
    ) :
        _size(strlen(str))
    {
        if (_size > S1B_STATIC_STRING_SIZE)
        {
            EX3_THROW(file_name_too_long_exception()
                << file_name_ei(str)
                << maximum_size_ei(S1B_STATIC_STRING_SIZE)
                << actual_size_ei(_size));
        }

        std::copy(str, str + _size, _str);
        _str[_size] = 0;
    }

    path_string(
        const std::string& str
    ) :
        _size(str.size())
    {
        const char* const c_str = str.c_str();

        if (_size > S1B_STATIC_STRING_SIZE)
        {
            EX3_THROW(file_name_too_long_exception()
                << file_name_ei(str)
                << maximum_size_ei(S1B_STATIC_STRING_SIZE)
                << actual_size_ei(_size));
        }

        std::copy(c_str, c_str + _size, _str);
        _str[_size] = 0;
    }

    path_string(
        const path_string& other
    ) :
        _size(other._size)
    {
        const char* const c_str = other.c_str();

        if (_size > S1B_STATIC_STRING_SIZE)
        {
            EX3_THROW(file_name_too_long_exception()
                << file_name_ei(c_str)
                << maximum_size_ei(S1B_STATIC_STRING_SIZE)
                << actual_size_ei(_size));
        }

        std::copy(c_str, c_str + _size, _str);
        _str[_size] = 0;
    }

    size_t size(
    ) const
    {
        return _size;
    }

    const char* c_str(
    ) const
    {
        return _str;
    }

    operator std::string() const
    {
        return std::string(_str);
    }
};

#else

class path_string
{
private:

    std::string _str;

public:

    path_string(
    ) :
        _str()
    {
    }

    path_string(
        const char* str
    ) :
        _str(str)
    {
    }

    path_string(
        const std::string& str
    ) :
        _str(str)
    {
    }

    path_string(
        const path_string& other
    ) :
        _str(other._str)
    {
    }

    size_t size(
    ) const
    {
        return _str.size();
    }

    const char* c_str(
    ) const
    {
        return _str.c_str();
    }

    operator std::string() const
    {
        return _str;
    }
};

#endif

}
