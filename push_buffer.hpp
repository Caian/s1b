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

#include "types.hpp"
#include "macros.hpp"
#include "open_mode.hpp"
#include "rwp_buffer.hpp"

#include <algorithm>

namespace s1b {

class push_buffer
{
private:

    static const foffset_t BufferSize = S1B_PUSH_BUFFER_SIZE;

private:

    rwp_buffer _buffer;
    foffset_t _buffer_offset;
    char* _data_buffer;

    void flush(
    )
    {
        if (_buffer_offset == 0)
            return;

        _buffer.write(_data_buffer, _buffer_offset);
        _buffer_offset = 0;
    }

public:

    push_buffer(
        const path_string& filename,
        bool create_new
    ) :
        _buffer(
            filename,
            create_new ? S1B_OPEN_NEW : S1B_OPEN_WRITE),
        _buffer_offset(0),
        _data_buffer(new char[BufferSize])
    {
        _buffer.seek(_buffer.get_size());
    }

    const path_string& filename(
    ) const
    {
        return _buffer.filename();
    }

    bool can_write(
    ) const
    {
        return _buffer.can_write();
    }

    foffset_t seek(
        foffset_t position
    )
    {
        flush();
        return _buffer.seek(position);
    }

    size_t read(
        void* buf,
        size_t count,
        bool complete,
        bool required
    )
    {
        flush();
        return _buffer.read(buf, count, complete, required);
    }

    void write(
        const void* data,
        foffset_t size
    )
    {
        if (data == 0)
        {
            EX3_THROW(null_pointer_exception()
                << file_name_ei(filename()));
        }

        if (size < 0)
        {
            EX3_THROW(s1b_exception()
                << actual_size_ei(size));
        }

        while (size > 0)
        {
            if (_buffer_offset == BufferSize)
                flush();

            foffset_t count = std::min(BufferSize - _buffer_offset, size);
            const char* src = reinterpret_cast<const char*>(data);
            char* dest = _data_buffer + _buffer_offset;
            std::copy(src, src + count, dest);
            _buffer_offset += count;
            size -= count;
        }
    }

    void skip(
        foffset_t size
    )
    {
        if (size < 0)
        {
            EX3_THROW(s1b_exception()
                << actual_size_ei(size));
        }

        while (size > 0)
        {
            if (_buffer_offset == BufferSize)
                flush();

            foffset_t count = std::min(BufferSize - _buffer_offset, size);
            char* dest = _data_buffer + _buffer_offset;
            std::fill(dest, dest + count, 0);
            _buffer_offset += count;
            size -= count;
        }
    }

    template <typename T>
    size_t read_object(
        T& o,
        bool required
    )
    {
        flush();
        return _buffer.read_object(o, required);
    }

    template <typename T>
    size_t write_object(
        const T& o
    )
    {
        const size_t s = sizeof(T);
        const char* const po = reinterpret_cast<const char*>(&o);
        write(po, s);
        return s;
    }

    size_t get_size(
    )
    {
        flush();
        return _buffer.get_size();
    }

    void sync(
    )
    {
        flush();
        _buffer.sync();
    }

    virtual ~push_buffer(
    )
    {
        delete[] _data_buffer;
    }
};

}
