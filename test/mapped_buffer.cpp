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
 * along with GNU Make; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#define S1B_TEST_NAME MappedBuffer

#include "s1b_test.hpp"

#include <s1b/rwp_buffer.hpp>
#include <s1b/mapped_buffer.hpp>

#include <algorithm>

namespace {

// TODO unlink

S1B_TEST(DefaultConstructor)

    (void)s1b_file_name;

    ASSERT_NO_THROW(s1b::mapped_buffer buffer());
}

S1B_TEST(OpenReadNonExistingZero)

    ASSERT_THROW(s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_DEFAULT,
        0, s1b::S1B_HUGETLB_OFF), s1b::io_exception);
}

S1B_TEST(OpenReadNonExistingNonZero)

    ASSERT_THROW(s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_DEFAULT,
        17, s1b::S1B_HUGETLB_OFF), s1b::io_exception);
}

S1B_TEST(OpenWriteNonExistingZero)

    ASSERT_THROW(s1b::mapped_buffer buffer(s1b_file_name,
        s1b::S1B_OPEN_WRITE, 0, s1b::S1B_HUGETLB_OFF),
        s1b::io_exception);
}

S1B_TEST(OpenWriteNonExistingNonZero)

    ASSERT_THROW(s1b::mapped_buffer buffer(s1b_file_name,
        s1b::S1B_OPEN_WRITE, 17, s1b::S1B_HUGETLB_OFF),
        s1b::io_exception);
}

S1B_TEST(OpenNewNonExistingZero)

    ASSERT_THROW(s1b::mapped_buffer buffer(s1b_file_name,
        s1b::S1B_OPEN_NEW, 0, s1b::S1B_HUGETLB_OFF),
        s1b::empty_mmap_exception);
}

S1B_TEST(OpenNewNonExistingNonZero)

    ASSERT_NO_THROW(s1b::mapped_buffer buffer(s1b_file_name,
        s1b::S1B_OPEN_NEW, 17, s1b::S1B_HUGETLB_OFF));
}

S1B_TEST(OpenReadExistingEmptyZero)

    // Use RWP buffer to create an empty file.
    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW));

    ASSERT_THROW(s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_DEFAULT,
        0, s1b::S1B_HUGETLB_OFF), s1b::empty_mmap_exception);
}

S1B_TEST(OpenReadExistingEmptyNonZero)

    // Use RWP buffer to create an empty file.
    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW));

    ASSERT_THROW(s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_DEFAULT,
        17, s1b::S1B_HUGETLB_OFF), s1b::empty_mmap_exception);
}

S1B_TEST(OpenWriteExistingEmptyZero)

    // Use RWP buffer to create an empty file.
    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW));

    ASSERT_THROW(s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_WRITE,
        0, s1b::S1B_HUGETLB_OFF), s1b::empty_mmap_exception);
}

S1B_TEST(OpenWriteExistingEmptyNonZero)

    // Use RWP buffer to create an empty file.
    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW));

    ASSERT_THROW(s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_WRITE,
        17, s1b::S1B_HUGETLB_OFF), s1b::empty_mmap_exception);
}

S1B_TEST(OpenNewExistingEmptyZero)

    // Use RWP buffer to create an empty file.
    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW));

    ASSERT_THROW(s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW,
        0, s1b::S1B_HUGETLB_OFF), s1b::empty_mmap_exception);
}

S1B_TEST(OpenNewExistingEmptyNonZero)

    // Use RWP buffer to create an empty file.
    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW));

    ASSERT_NO_THROW(s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW,
        17, s1b::S1B_HUGETLB_OFF));
}

S1B_TEST(OpenReadExistingNonEmptyZero)

    ASSERT_NO_THROW(s1b::mapped_buffer buffer(s1b_file_name,
        s1b::S1B_OPEN_NEW, 17, s1b::S1B_HUGETLB_OFF));

    ASSERT_NO_THROW(s1b::mapped_buffer buffer(s1b_file_name,
        s1b::S1B_OPEN_DEFAULT, 0, s1b::S1B_HUGETLB_OFF));
}

S1B_TEST(OpenReadExistingNonEmptyNonZero)

    ASSERT_NO_THROW(s1b::mapped_buffer buffer(s1b_file_name,
        s1b::S1B_OPEN_NEW, 17, s1b::S1B_HUGETLB_OFF));

    ASSERT_NO_THROW(s1b::mapped_buffer buffer(s1b_file_name,
        s1b::S1B_OPEN_DEFAULT, 17, s1b::S1B_HUGETLB_OFF));
}

S1B_TEST(OpenWriteExistingNonEmptyZero)

    ASSERT_NO_THROW(s1b::mapped_buffer buffer(s1b_file_name,
        s1b::S1B_OPEN_NEW, 17, s1b::S1B_HUGETLB_OFF));

    ASSERT_NO_THROW(s1b::mapped_buffer buffer(s1b_file_name,
        s1b::S1B_OPEN_WRITE, 0, s1b::S1B_HUGETLB_OFF));
}

S1B_TEST(OpenWriteExistingNonEmptyNonZero)

    ASSERT_NO_THROW(s1b::mapped_buffer buffer(s1b_file_name,
        s1b::S1B_OPEN_NEW, 17, s1b::S1B_HUGETLB_OFF));

    ASSERT_NO_THROW(s1b::mapped_buffer buffer(s1b_file_name,
        s1b::S1B_OPEN_WRITE, 17, s1b::S1B_HUGETLB_OFF));
}

S1B_TEST(OpenNewExistingNonEmptyZero)

    ASSERT_NO_THROW(s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW,
        17, s1b::S1B_HUGETLB_OFF));

    ASSERT_THROW(s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW,
        0, s1b::S1B_HUGETLB_OFF), s1b::empty_mmap_exception);
}

S1B_TEST(OpenNewExistingNonEmptyNonZero)

    ASSERT_NO_THROW(s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW,
        17, s1b::S1B_HUGETLB_OFF));

    ASSERT_NO_THROW(s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW,
        17, s1b::S1B_HUGETLB_OFF));
}

S1B_TEST(Filename)

    try
    {
        s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW, 17,
            s1b::S1B_HUGETLB_OFF);
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

S1B_TEST(CheckIfClosingFiles)

    ASSERT_NO_THROW(s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW,
        17, s1b::S1B_HUGETLB_OFF));

    for (int i = 0; i < 50000; i++)
    {
        try
        {
            s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_DEFAULT,
                0, s1b::S1B_HUGETLB_OFF);
        }
        catch (const std::exception& e)
        {
            std::cerr
                << boost::current_exception_diagnostic_information()
                << std::endl;
            FAIL();
        }
    }
}

S1B_TEST(WriteOnWriteableFile)

    char data[123];
    float data2 = 0;

    static const size_t total_size = 123 + 11 + sizeof(data2);

    ASSERT_NO_THROW(s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW,
        total_size, s1b::S1B_HUGETLB_OFF));

    try
    {
        s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_WRITE,
            0, s1b::S1B_HUGETLB_OFF);
        ASSERT_EQ(total_size, buffer.size());
        ASSERT_NE(static_cast<void*>(0), buffer.address());
        ASSERT_TRUE(buffer.can_write());
        std::copy(data, data + 0, buffer.ptr<char>(0));
        std::copy(data, data + 123, buffer.ptr<char>(123));
        std::copy(data, data + 11, buffer.ptr<char>(11));
        *buffer.ptr<float>(123 + 11) = data2;
        ASSERT_NO_THROW(buffer.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(WriteOnNewFile)

    char data[123];
    float data2 = 0;

    static const size_t total_size = 123 + 11 + sizeof(data2);

    try
    {
        s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW,
            total_size, s1b::S1B_HUGETLB_OFF);
        ASSERT_EQ(total_size, buffer.size());
        ASSERT_NE(static_cast<void*>(0), buffer.address());
        ASSERT_TRUE(buffer.can_write());
        std::copy(data, data + 0, buffer.ptr<char>(0));
        std::copy(data, data + 123, buffer.ptr<char>(123));
        std::copy(data, data + 11, buffer.ptr<char>(11));
        *buffer.ptr<float>(123 + 11) = data2;
        ASSERT_NO_THROW(buffer.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(MixedRWOperations)

    try
    {
        static const char* value_1 = "Test123";
        static const int size_1 = sizeof(value_1) / sizeof(value_1[0]);
        static const float value_2 = 3.6843;
        static const short value_3 = -20;
        static const size_t total_size = 2 + size_1 + 2 * sizeof(value_2) +
            sizeof(value_3);

        {
            s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW,
                total_size, s1b::S1B_HUGETLB_OFF);
            ASSERT_NE(static_cast<void*>(0), buffer.address());
            ASSERT_EQ(total_size, buffer.size());
            ASSERT_TRUE(buffer.can_write());
            std::copy(value_1, value_1 + size_1, buffer.ptr<char>(0));
            std::copy(value_1, value_1 + size_1, buffer.ptr<char>(2));
            *buffer.ptr<float>(2 + size_1) = value_2;
            *buffer.ptr<short>(2 + size_1 + sizeof(value_2)) = value_3;
            ASSERT_NO_THROW(buffer.sync());
        }

        {
            char data_1[2 + size_1 + 1];
            float data_2;
            short data_3;

            s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_WRITE,
                0, s1b::S1B_HUGETLB_OFF);
            ASSERT_NE(static_cast<void*>(0), buffer.address());
            ASSERT_EQ(total_size, buffer.size());
            ASSERT_TRUE(buffer.can_write());
            const char* p_data1 = buffer.ptr<char>(0);
            std::copy(p_data1, p_data1 + 2 + size_1, data_1);
            data_1[2 + size_1] = 0;
            ASSERT_STREQ("TeTest123", data_1);
            ASSERT_EQ(value_2, *buffer.ptr<float>(2 + size_1));
            ASSERT_EQ(value_3, *buffer.ptr<short>(2 + size_1 + sizeof(data_2)));
            *buffer.ptr<float>(2 + size_1 + sizeof(data_2) + sizeof(data_3)) =
                value_2;
            ASSERT_NO_THROW(buffer.sync());
        }

        {
            char data_1[2 + size_1 + 1];
            float data_2;
            short data_3;

            s1b::mapped_buffer buffer(s1b_file_name, s1b::S1B_OPEN_DEFAULT,
                0, s1b::S1B_HUGETLB_OFF);
            ASSERT_NE(static_cast<void*>(0), buffer.address());
            ASSERT_EQ(total_size, buffer.size());
            ASSERT_FALSE(buffer.can_write());
            const char* p_data1 = buffer.ptr<char>(0);
            std::copy(p_data1, p_data1 + 2 + size_1, data_1);
            data_1[2 + size_1] = 0;
            ASSERT_STREQ("TeTest123", data_1);
            ASSERT_EQ(value_2, *buffer.ptr<float>(2 + size_1));
            ASSERT_EQ(value_3, *buffer.ptr<short>(2 + size_1 + sizeof(data_2)));
            ASSERT_EQ(value_2, *buffer.ptr<float>(2 + size_1 + sizeof(data_2) +
                sizeof(data_3)));
            ASSERT_NO_THROW(buffer.sync());
        }
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

}
