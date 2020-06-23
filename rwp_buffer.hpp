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

namespace s1b {

class rwp_buffer
{
private:

   BOOST_MOVABLE_BUT_NOT_COPYABLE(rwp_buffer)

private:

    /** The permission when creating file, user read+write. */
    static const os::mode_type file_sharing_mode = os::functions::FS_RU |
                                                   os::functions::FS_WU;

private:

    path_string _filename;
    bool _can_write;
    os::fd_type _fd;

public:

    rwp_buffer(
    ) :
        _filename(),
        _can_write(false),
        _fd(os::functions::FD_Invalid)
    {
    }

    rwp_buffer(
        const path_string& filename,
        open_mode mode
    ) :
        _filename(filename),
        _can_write((mode & S1B_OPEN_WRITE) != 0),
        _fd(os::functions::open(
            filename.c_str(),
            (can_write() ?
                os::functions::FO_RW :
                os::functions::FO_R) |
            (((mode & S1B_OPEN_TRUNC) != 0) ?
                (os::functions::FO_Truncate | os::functions::FO_Create) :
                 0),
            file_sharing_mode))
    {
    }

    rwp_buffer(
        BOOST_RV_REF(rwp_buffer) other
    ) :
        _filename(boost::move(other._filename)),
        _can_write(other._can_write),
        _fd(other._fd)
    {
        other._can_write = false;
        other._fd = os::functions::FD_Invalid;
    }

    rwp_buffer& operator =(
        BOOST_RV_REF(rwp_buffer) other
    )
    {
        os::functions::close_unchecked(_fd);

        _filename = boost::move(other._filename);
        _can_write = other._can_write;
        _fd = other._fd;

        other._can_write = false;
        other._fd = os::functions::FD_Invalid;

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
        return _can_write;
    }

    foffset_t seek(
        foffset_t position
    )
    {
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
#if !defined(S1B_DISABLE_ATOMIC_RW)
        foffset_t offset,
#endif
        size_t count,
        bool complete,
        bool required
    ) S1B_READ_METHOD_QUALIFIER
    {
        size_t sr;

        try
        {
            sr = os::functions::read(_fd, buf,
#if !defined(S1B_DISABLE_ATOMIC_RW)
                offset,
#endif
                count);
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

    size_t write(
        const void* buf,
#if !defined(S1B_DISABLE_ATOMIC_RW)
        foffset_t offset,
#endif
        size_t count
    )
    {
        if (!_can_write)
        {
            EX3_THROW(read_only_exception()
                << file_name_ei(_filename));
        }

        try
        {
            os::functions::write(_fd, buf,
#if !defined(S1B_DISABLE_ATOMIC_RW)
                offset,
#endif
                count);
        }
        catch (const io_exception& e)
        {
            EX3_RETHROW(e
                << file_name_ei(_filename));
        }

        return count;
    }

    template <typename T>
    size_t read_object(
        T& o,
#if !defined(S1B_DISABLE_ATOMIC_RW)
        foffset_t offset,
#endif
        bool required
    ) S1B_READ_METHOD_QUALIFIER
    {
        const size_t s = sizeof(T);
        char* const po = reinterpret_cast<char*>(&o);
        return read(po,
#if !defined(S1B_DISABLE_ATOMIC_RW)
            offset,
#endif
            s, true, required);
    }

    template <typename T>
    size_t write_object(
        const T& o
#if !defined(S1B_DISABLE_ATOMIC_RW)
        , foffset_t offset
#endif
    )
    {
        const size_t s = sizeof(T);
        const char* const po = reinterpret_cast<const char*>(&o);
        write(po,
#if !defined(S1B_DISABLE_ATOMIC_RW)
        offset,
#endif
        s);
        return s;
    }

    foffset_t get_size(
    ) const
    {
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
        if (_can_write)
            os::functions::fsync(_fd);
    }

    virtual ~rwp_buffer(
    )
    {
        os::functions::close_unchecked(_fd);
    }
};

}
