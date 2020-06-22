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
#include "hugetlb_mode.hpp"
#include "hugetlb_state.hpp"
#include "memory_region.hpp"

#include <boost/move/utility_core.hpp>

namespace s1b {

class mapped_buffer
{
private:

   BOOST_MOVABLE_BUT_NOT_COPYABLE(mapped_buffer)

private:

    /** The permission when creating file, user read+write. */
    static const os::mode_type file_sharing_mode = os::functions::FS_RU |
                                                   os::functions::FS_WU;

    /** The base flags when mapping a file to memory, they may be
        changed by the method. */
    static const os::flags_type mapping_flags = os::functions::MAP_Shared;

private:

    path_string _filename;
    bool _can_write;
    memory_region _memory_region;

    memory_region map_fd(
        os::fd_type fd,
        hugetlb_mode htlb_mode
    ) const
    {
        const foffset_t size = os::functions::fsize(fd);

        if (size == 0)
        {
            EX3_THROW(empty_mmap_exception());
        }

        // TODO check / ensure alignment

        const os::flags_type prot = os::functions::MAP_Read | (can_write() ?
                                    os::functions::MAP_Write : 0);
        const os::flags_type flags = mapping_flags;

        hugetlb_state htlb_state;

        void* ptr = 0;

#if !defined(S1B_HUGETLB_FLAG)

            htlb_state = S1B_HUGETLB_UNSUPPORTED;

            if (htlb_mode == S1B_HUGETLB_FORCE)
            {
                EX3_THROW(unsupported_exception()
                    << feature_name_ei("HUGETLB"));
            }

#else

            if (htlb_mode == S1B_HUGETLB_OFF)
            {
                htlb_state = S1B_HUGETLB_NOT_USED;
            }
            else
            {
                try
                {
                    ptr = os::functions::mmap(0, size, prot,
                        flags | S1B_HUGETLB_FLAG, fd, 0);
                }
                catch(const io_exception& e)
                {
                    if (htlb_mode == S1B_HUGETLB_FORCE)
                        EX3_RETHROW(e);
                }
            }

#endif

        if (ptr == 0)
            ptr = os::functions::mmap(0, size, prot, flags, fd, 0);

        return memory_region(reinterpret_cast<char*>(ptr), size, htlb_state);
    }

    os::fd_type open_fd(
        bool new_file
    ) const
    {
        return os::functions::open(
            _filename.c_str(),
            (can_write() ?
                os::functions::FO_RW :
                os::functions::FO_R) |
            (new_file ?
                (os::functions::FO_Truncate | os::functions::FO_Create) :
                 0),
            file_sharing_mode);
    }

    memory_region open_and_map(
        hugetlb_mode htlb_mode
    ) const
    {
        try
        {
            os::fd_type fd = open_fd(false);
            memory_region region = map_fd(fd, htlb_mode);
            os::functions::close_unchecked(fd);
            return region;
        }
        catch (const io_exception& e)
        {
            EX3_RETHROW(e
                << file_name_ei(_filename));
        }
    }

    memory_region create_and_map(
        foffset_t size,
        hugetlb_mode htlb_mode
    ) const
    {
        if (size == 0)
        {
            EX3_THROW(empty_mmap_exception()
                << file_name_ei(_filename));
        }

        try
        {
            os::fd_type fd = open_fd(true);
            os::functions::seek(fd, size-1);
            os::functions::write(fd, "", 1);
            memory_region region = map_fd(fd, htlb_mode);
            os::functions::close_unchecked(fd);
            return region;
        }
        catch (const io_exception& e)
        {
            EX3_RETHROW(e
                << file_name_ei(_filename));
        }
    }

public:

    mapped_buffer(
    ) :
        _filename(),
        _can_write(false),
        _memory_region()
    {
    }

    mapped_buffer(
        const path_string& filename,
        open_mode mode,
        foffset_t size,
        hugetlb_mode htlb_mode=S1B_HUGETLB_OFF
    ) :
        _filename(filename),
        _can_write(
            (mode & S1B_OPEN_WRITE) != 0),
        _memory_region(
            (mode & S1B_OPEN_TRUNC) != 0 ?
                create_and_map(size, htlb_mode) :
                open_and_map(htlb_mode))
    {
    }

    mapped_buffer(
        BOOST_RV_REF(mapped_buffer) other
    ) :
        _filename(boost::move(other._filename)),
        _can_write(other._can_write),
        _memory_region(other._memory_region)
    {
        other._can_write = false;
        other._memory_region = memory_region();
    }

    mapped_buffer& operator=(
        BOOST_RV_REF(mapped_buffer) other
    )
    {
        os::functions::munmap(_memory_region.address, _memory_region.size);

        _filename = boost::move(other._filename);
        _can_write = other._can_write;
        _memory_region = other._memory_region;

        other._can_write = false;
        other._memory_region = memory_region();

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

    const char* address(
    ) const
    {
        return _memory_region.address;
    }

    char* address(
    )
    {
        return _memory_region.address;
    }

    template <typename T>
    const T* ptr(
        size_t offset
    ) const
    {
        return reinterpret_cast<const T*>(address() + offset);
    }

    template <typename T>
    T* ptr(
        size_t offset
    )
    {
        return reinterpret_cast<T*>(address() + offset);
    }

    size_t size(
    ) const
    {
        return _memory_region.size;
    }

    hugetlb_state htlb_state(
    ) const
    {
        return _memory_region.htlb_state;
    }

    void sync(
    ) const
    {
        sync(0, _memory_region.size);
    }

    void sync(
        size_t offset,
        size_t size
    ) const
    {
        if (!_can_write)
            return;

        if (offset > _memory_region.size)
        {
            EX3_THROW(io_exception()
                << file_size_ei(_memory_region.size)
                << file_position_ei(offset)
                << feature_name_ei("HUGETLB"));
        }

        const size_t end = offset + size;
        if (end > _memory_region.size)
        {
            EX3_THROW(io_exception()
                << file_size_ei(_memory_region.size)
                << file_position_ei(end)
                << feature_name_ei("HUGETLB"));
        }

        os::functions::msync(_memory_region.address + offset, size,
            os::functions::MAP_Synchronous);
    }

    virtual ~mapped_buffer(
    )
    {
        if (_memory_region.address != 0)
            os::functions::munmap(_memory_region.address, _memory_region.size);
    }
};

}
