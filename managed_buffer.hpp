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
#include "exceptions.hpp"
#include "path_string.hpp"

#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/move/utility_core.hpp>

#include <utility>

namespace s1b {

class managed_buffer
{
private:

   BOOST_MOVABLE_BUT_NOT_COPYABLE(managed_buffer)

private:

    static const char* get_data_name(
    )
    {
        return ".data";
    }

public:

    typedef boost::interprocess::managed_mapped_file managed_file;

    typedef boost::interprocess::offset_ptr<const char> managed_pointer;

    typedef managed_file::segment_manager segment_manager;

    typedef boost::interprocess::allocator<
        char,
        segment_manager
        > managed_allocator;

private:

    path_string _filename;
    managed_file _file;
    const char* _data;

    managed_file open_existing(
    ) const
    {
        using namespace boost::interprocess;

        try
        {
            // Force open the file
            return managed_file(open_read_only, _filename.c_str());
        }
        catch (const std::exception& ex)
        {
            EX3_THROW(s1b::io_exception() << file_name_ei(filename()));
        }
        catch (const boost::exception& ex)
        {
            EX3_THROW(s1b::io_exception() << file_name_ei(filename()));
        }
    }

    const char* get_data(
    )
    {
        using namespace boost::interprocess;

        std::pair<managed_pointer*, size_t> p;

        try
        {
            // Retrieve the pointer holding the data
            p = _file.find<
                managed_pointer
                >(get_data_name());

        }
        catch (const std::exception& ex)
        {
            EX3_THROW(s1b::io_exception() << file_name_ei(filename()));
        }
        catch (const boost::exception& ex)
        {
            EX3_THROW(s1b::io_exception() << file_name_ei(filename()));
        }

        // Shouldn't happen because we are opening a valid file
        if (p.first == 0 || p.second != 1)
        {
            EX3_THROW(invalid_managed_data_exception()
                << actual_pointer_ei(p.first)
                << actual_size_ei(p.second)
                << file_name_ei(filename()));
        }

        return p.first->get();
    }

    template <typename Initializer>
    managed_file construct_and_open(
        size_t initial_size,
        const Initializer& initializer,
        unsigned int resize_attempts,
        size_t size_increment
    ) const
    {
        using namespace boost::interprocess;

        if (resize_attempts > 0 && size_increment == 0)
        {
            EX3_THROW(empty_size_increment_exception()
                << file_name_ei(filename()));
        }

        size_t file_size = 0;
        unsigned int attempt;

        for (attempt = 0; attempt <= resize_attempts; attempt++)
        {
            // Remove the old file
            file_mapping::remove(_filename.c_str());

            // Compute the new mapped file size
            file_size = initial_size + attempt * size_increment;

            // Force create the new file
            managed_file mfile(create_only,
                _filename.c_str(), file_size);

            try
            {
                // Construct the pointer that will hold the data
                managed_pointer* ptr = mfile.construct<
                    managed_pointer
                    >(get_data_name())();

                // Initialize the contents of the mapped file
                *ptr = initializer(mfile);
            }
            catch (const bad_alloc&)
            {
                // Try again
                continue;
            }
            catch (const s1b::s1b_exception& ex)
            {
                EX3_RETHROW(ex << file_name_ei(filename()));
            }
            catch (const std::exception& ex)
            {
                EX3_THROW(s1b::io_exception() << file_name_ei(filename()));
            }
            catch (const boost::exception& ex)
            {
                EX3_THROW(s1b::io_exception() << file_name_ei(filename()));
            }

            // TODO remove file after throw

            return open_existing();
        }

        // TODO remove reference to dataset and index
        // The maximum number of attempts was reached
        EX3_THROW(index_size_too_big_exception()
            << maximum_attempts_ei(attempt)
            << file_size_ei(file_size)
            << file_name_ei(filename()));
    }

public:

    template <typename Initializer>
    managed_buffer(
        const path_string& filename,
        size_t initial_size,
        const Initializer& initializer,
        unsigned int resize_attempts,
        size_t size_increment
    ) :
        _filename(filename),
        _file(construct_and_open(
            initial_size, initializer,
            resize_attempts,
            size_increment)),
        _data(get_data())
    {
    }

    managed_buffer(
        const path_string& filename
    ) :
        _filename(filename),
        _file(open_existing()),
        _data(get_data())
    {
    }

    managed_buffer(
        BOOST_RV_REF(managed_buffer) other
    ) :
        _filename(boost::move(other._filename)),
        _file(boost::move(other._file)),
        _data(other._data)
    {
    }

    managed_buffer& operator=(
        BOOST_RV_REF(managed_buffer) other
    )
    {
        _filename = boost::move(other._filename);
        _file = boost::move(other._file);
        _data = other._data;

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
        return false;
    }

    const char* address(
    ) const
    {
        return reinterpret_cast<const char*>(_file.get_address());
    }

    const char* data(
    ) const
    {
        return _data;
    }

    size_t size(
    ) const
    {
        return _file.get_size();
    }

    size_t get_used_bytes(
    ) const
    {
        return _file.get_segment_manager()->get_size() - get_free_bytes();
    }

    size_t get_free_bytes(
    ) const
    {
        return _file.get_segment_manager()->get_free_memory();
    }

    virtual ~managed_buffer(
    )
    {
    }
};

}
