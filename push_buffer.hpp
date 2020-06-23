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
#include "exceptions.hpp"
#include "path_string.hpp"
#include "os/functions.hpp"

#include <boost/move/utility_core.hpp>

#include <algorithm>

namespace s1b {

class push_buffer
{
private:

   BOOST_MOVABLE_BUT_NOT_COPYABLE(push_buffer)

private:

    /** The permission when creating file, user read+write. */
    static const os::mode_type file_sharing_mode = os::functions::FS_RU |
                                                   os::functions::FS_WU;

    static const foffset_t BufferSize = S1B_PUSH_BUFFER_SIZE;

private:

    path_string _filename;
    os::fd_type _fd;
    foffset_t _buffer_offset;
    char* _data_buffer; // TODO static buffer flag

    void flush(
    )
    {
        if (_buffer_offset == 0)
            return;

        os::functions::write(_fd, _data_buffer, _buffer_offset);

        _buffer_offset = 0;
    }

public:

    push_buffer(
        const path_string& filename,
        bool create_new
    ) :
        _filename(filename),
        _fd(os::functions::open(
            filename.c_str(),
            os::functions::FO_RW |
            (create_new ?
                (os::functions::FO_Truncate | os::functions::FO_Create) :
                 0),
            file_sharing_mode)),
        _buffer_offset(0),
        _data_buffer(new char[BufferSize])
    {
        seek(get_size());
    }

    push_buffer(
        BOOST_RV_REF(push_buffer) other
    ) :
        _filename(boost::move(other._filename)),
        _fd(other._fd),
        _buffer_offset(other._buffer_offset),
        _data_buffer(other._data_buffer)
    {
        other._fd = os::functions::FD_Invalid;
        other._data_buffer = 0;
    }

    push_buffer& operator =(
        BOOST_RV_REF(push_buffer) other
    )
    {
        os::functions::close_unchecked(_fd);

        _filename = boost::move(other._filename);
        _fd = other._fd;
        _buffer_offset = other._buffer_offset;
        _data_buffer = other._data_buffer;

        other._fd = os::functions::FD_Invalid;
        other._data_buffer = 0;

        return *this;
    }

    const path_string& filename(
    ) const
    {
        return _filename;
    }

    bool can_write(
    ) const
    {
        return true;
    }

    foffset_t seek(
        foffset_t position
    )
    {
        flush();

        try
        {
            return os::functions::seek(_fd, position);
        }
        catch (const io_exception& e)
        {
            EX3_RETHROW(e
                << file_name_ei(_filename));
        }
    }

    size_t read(
        void* buf,
        size_t count,
        bool complete,
        bool required
    )
    {
        size_t sr;

        flush();

        try
        {
            sr = os::functions::read(_fd, buf, count);
        }
        catch (const io_exception& e)
        {
            EX3_RETHROW(e
                << file_name_ei(_filename));
        }

        // Check if the read was partial or not did not happen when it was
        // required.
        if ((complete && sr != 0 && sr != count) || (required && sr == 0))
        {
            EX3_THROW(incomplete_read_exception()
                << expected_size_ei(count)
                << actual_size_ei(sr)
                << file_name_ei(_filename));
        }

        return sr;
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

    template <typename T>
    size_t read_object(
        T& o,
        bool required
    )
    {
        const size_t s = sizeof(T);
        char* const po = reinterpret_cast<char*>(&o);
        return read(po, s, true, required);
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

    size_t get_size(
    )
    {
        flush();

        try
        {
            return os::functions::fsize(_fd);
        }
        catch (const io_exception& e)
        {
            EX3_RETHROW(e
                << file_name_ei(_filename));
        }
    }

    void sync(
    )
    {
        flush();
        os::functions::fsync(_fd);
    }

    virtual ~push_buffer(
    )
    {
        delete[] _data_buffer;
        os::functions::close_unchecked(_fd);
    }
};

}
