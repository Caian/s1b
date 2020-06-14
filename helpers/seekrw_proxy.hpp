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

#include <s1b/types.hpp>
#include <s1b/path_string.hpp>

namespace s1b {
namespace helpers {

// TODO detail?

template <class Buffer>
class seekrw_proxy
{
private:

    Buffer* _buffer;

public:

    seekrw_proxy(
        Buffer& buffer
    ) :
        _buffer(&buffer)
    {
    }

    const path_string& filename(
    ) const
    {
        return _buffer->filename();
    }

    foffset_t seek(
        foffset_t position
    )
    {
        return _buffer->seek(position);
    }

    size_t read(
        void* buf,
        foffset_t offset,
        size_t count,
        bool complete,
        bool required
    ) const
    {
        _buffer->seek(offset);
        return _buffer->read(buf, count, complete, required);
    }

    void write(
        const void* buf,
        foffset_t offset,
        size_t count
    )
    {
        _buffer->seek(offset);
        _buffer->write(buf, count);
    }

    template <typename T>
    size_t read_object(
        T& o,
        foffset_t offset,
        bool required
    ) const
    {
        _buffer->seek(offset);
        return _buffer->read_object(o, required);
    }

    template <typename T>
    size_t write_object(
        const T& o,
        foffset_t offset
    )
    {
        _buffer->seek(offset);
        return _buffer->write_object(o);
    }
};

}}
