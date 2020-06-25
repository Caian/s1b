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

#include <s1b/exceptions.hpp>

#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

namespace s1b {
namespace os {

struct functions
{
    static const fd_type FD_Invalid = -1;

    static const mode_type FS_RU = S_IRUSR;
    static const mode_type FS_WU = S_IWUSR;

    static const flags_type MAP_Shared = MAP_SHARED;
    static const flags_type MAP_Read = PROT_READ;
    static const flags_type MAP_Write = PROT_WRITE;
    static const flags_type MAP_Synchronous = MS_SYNC;

    static const flags_type FO_R = O_RDONLY;
    static const flags_type FO_RW = O_RDWR;
    static const flags_type FO_Create = O_CREAT;
    static const flags_type FO_Truncate = O_TRUNC;

    static offset_type seek(
        fd_type fd,
        offset_type position
    )
    {
        offset_type result = ::lseek64(fd, position, SEEK_SET);

        if (result == (offset_type)-1)
        {
            // TODO add fd, offset and whence and remove unecessary filename copies
            EX3_THROW(io_exception()
                << operation_name_ei("lseek64")
                << file_position_ei(position)
                << operation_errno_ei(errno));
        }

        return result;
    }

    static size_t read(
        fd_type fd,
        void* buf,
        size_t count
    )
    {
        ssize_t result = ::read(fd, buf, count);

        if (result == -1)
        {
            EX3_THROW(io_exception()
                << operation_name_ei("read")
                << operation_errno_ei(errno));
        }

        return static_cast<size_t>(result);
    }

    static size_t read(
        fd_type fd,
        void* buf,
        offset_type offset,
        size_t count
    )
    {
        ssize_t result = ::pread64(fd, buf, count, offset);

        if (result == -1)
        {
            EX3_THROW(io_exception()
                << operation_name_ei("pread64")
                << operation_errno_ei(errno));
        }

        return static_cast<size_t>(result);
    }

    static void write(
        fd_type fd,
        const void* buf,
        size_t count
    )
    {
        ssize_t result = ::write(fd, buf, count);

        if (result < 0)
        {
            EX3_THROW(io_exception()
                << operation_name_ei("write")
                << operation_errno_ei(errno));
        }

        size_t uresult = static_cast<size_t>(result);

        if (uresult != count)
        {
            EX3_THROW(incomplete_write_exception()
                << expected_size_ei(count)
                << actual_size_ei(uresult));
        }
    }

    static void write(
        fd_type fd,
        const void* buf,
        offset_type offset,
        size_t count
    )
    {
        ssize_t result = ::pwrite64(fd, buf, count, offset);

        if (result < 0)
        {
            EX3_THROW(io_exception()
                << operation_name_ei("pwrite64")
                << operation_errno_ei(errno));
        }

        size_t uresult = static_cast<size_t>(result);

        if (uresult != count)
        {
            EX3_THROW(incomplete_write_exception()
                << expected_size_ei(count)
                << actual_size_ei(uresult));
        }
    }

    static void fsync(
        fd_type fd
    )
    {
        if (::fsync(fd) < 0)
        {
            EX3_THROW(io_exception()
                << operation_name_ei("fsync")
                << operation_errno_ei(errno));
        }
    }

    static int open(
        const char *pathname,
        flags_type flags,
        mode_type mode
    )
    {
        fd_type fd = ::open64(pathname, flags, mode);

        if (fd < 0)
        {
            EX3_THROW(io_exception()
                << operation_name_ei("open64")
                << file_name_ei(pathname)
                << operation_errno_ei(errno)
                << operation_mode_ei(mode)
                << operation_flags_ei(flags));
        }

        return fd;
    }

    static void close_unchecked(
        fd_type fd
    )
    {
        ::close(fd);
    }

    static void unlink(
        const char *pathname
    )
    {
        if (::unlink(pathname) < 0)
        {
            EX3_THROW(io_exception()
                << operation_name_ei("unlink")
                << file_name_ei(pathname)
                << operation_errno_ei(errno));
        }
    }

    static void *mmap(
        void *addr,
        size_t length,
        int prot,
        flags_type flags,
        fd_type fd,
        off_t offset
    )
    {
        void* ptr = ::mmap64(addr, length, prot, flags, fd, offset);

        if (ptr == MAP_FAILED)
        {
            EX3_THROW(io_exception()
                << operation_name_ei("mmap")
                << operation_errno_ei(errno));
        }

        return ptr;
    }

    static void msync(
        void *addr,
        size_t length,
        flags_type flags
    )
    {
        if (::msync(addr, length, flags) < 0)
        {
            EX3_THROW(io_exception()
                << operation_name_ei("msync")
                << operation_errno_ei(errno));
        }
    }

    static void munmap(
        void *addr,
        size_t length
    )
    {
        if (::munmap(addr, length) != 0)
        {
            EX3_THROW(io_exception()
                << operation_name_ei("munmap")
                << operation_errno_ei(errno));
        }
    }

    static offset_type fsize(
        fd_type fd
    )
    {
        struct stat64 buf;

        if (::fstat64(fd, &buf) != 0)
        {
            EX3_THROW(io_exception()
                << operation_name_ei("fstat64")
                << operation_errno_ei(errno));
        }

        return buf.st_size;
    }
};

}}
