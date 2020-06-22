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

#define S1B_TEST_NAME ManagedBuffer

#include "s1b_test.hpp"
#include "managed_test.hpp"

#define TEST_OBJ_TYPE s1b::managed_buffer
#include "move_test.hpp"

namespace {

// TODO unlink

S1B_TEST(OpenNonExisting)

    ASSERT_THROW(s1b::managed_buffer buffer(s1b_file_name),
        s1b::io_exception);
}

S1B_TEST(OpenNewNonExisting)

    DummyInitializer initializer;
    ASSERT_NO_THROW(s1b::managed_buffer buffer(s1b_file_name, 1024,
        initializer, 1, 1024));
}

S1B_TEST(OpenExisting)

    DummyInitializer initializer;
    ASSERT_NO_THROW(s1b::managed_buffer buffer(s1b_file_name, 1024,
        initializer, 1, 1024));

    ASSERT_NO_THROW(s1b::managed_buffer buffer(s1b_file_name));
}

S1B_TEST(ConstructNoRetries)

    static const size_t initial_size = 1024*1024 + 1024;

    FixedSizeInitializer initializer(1024*1024);
    ASSERT_NO_THROW(s1b::managed_buffer buffer(s1b_file_name, initial_size,
        initializer, 0, 0));
}

S1B_TEST(ConstructInvalidRetries)

    static const size_t initial_size = 1024*1024 + 1024;

    FixedSizeInitializer initializer(1024*1024);
    ASSERT_NO_THROW(s1b::managed_buffer buffer(s1b_file_name, initial_size,
        initializer, 0, 1024));
    ASSERT_THROW(s1b::managed_buffer buffer(s1b_file_name, initial_size,
        initializer, 1, 0), s1b::empty_size_increment_exception);
}

S1B_TEST(ConstructWithRetry)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 1024*1024;

    FixedSizeInitializer initializer(1024*1024);
    ASSERT_NO_THROW(s1b::managed_buffer buffer(s1b_file_name, initial_size,
        initializer, 1, size_increment));
}

S1B_TEST(ConstructNotEnoughRetries)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 1024*1024 / 4;

    FixedSizeInitializer initializer(1024*1024);
    ASSERT_THROW(s1b::managed_buffer buffer(s1b_file_name, initial_size,
        initializer, 2, size_increment), s1b::index_size_too_big_exception);
}

S1B_TEST(Filename)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 1024*1024;

    FixedSizeInitializer initializer(1024*1024);

    try
    {
        MAKE_TEST_OBJ(buffer)(s1b_file_name, initial_size,
            initializer, 1, size_increment);
        MOVE_TEST_OBJ(buffer);
        ASSERT_STREQ(s1b_file_name, buffer.filename().c_str());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(Size)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 1024*1024;

    FixedSizeInitializer initializer(1024*1024);

    try
    {
        MAKE_TEST_OBJ(buffer)(s1b_file_name, initial_size,
            initializer, 1, size_increment);
        MOVE_TEST_OBJ(buffer);
        ASSERT_EQ(initial_size + size_increment, buffer.size());
        ASSERT_GE(initial_size + size_increment, buffer.get_used_bytes());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(AlwaysReadOnly)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 1024*1024;

    FixedSizeInitializer initializer(1024*1024);

    try
    {
        MAKE_TEST_OBJ(buffer)(s1b_file_name, initial_size,
            initializer, 1, size_increment);
        MOVE_TEST_OBJ(buffer);
        ASSERT_EQ(false, buffer.can_write());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(Data)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 1024*1024;

    RandomDataInitializer initializer(size_increment);

    try
    {
        const char* cp_data = 0;
        MAKE_TEST_OBJ(buffer)(s1b_file_name, initial_size,
            initializer, 1, size_increment);
        MOVE_TEST_OBJ(buffer);
        ASSERT_EQ(false, buffer.can_write());
        ASSERT_NO_THROW(cp_data = buffer.data());
        ASSERT_NE(static_cast<char*>(0), cp_data);
        for (size_t j = 0; j < size_increment; j++)
            ASSERT_TRUE(initializer.make_byte(j) == cp_data[j]);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(CustomInitializationException)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 1024*1024;

    ThrowInitializer initializer;
    DerivedThrowInitializer dinitializer;

    ASSERT_THROW(s1b::managed_buffer buffer(s1b_file_name, initial_size,
            initializer, 1, size_increment), CustomException);

    ASSERT_THROW(s1b::managed_buffer buffer(s1b_file_name, initial_size,
            dinitializer, 1, size_increment), s1b::io_exception);
}

}
