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

#define S1B_TEST_NAME RwpBuffer

#include "s1b_test.hpp"

#include <s1b/rwp_buffer.hpp>

namespace {

// TODO unlink

S1B_TEST(DefaultConstructor)

    (void)s1b_file_name;

    ASSERT_NO_THROW(s1b::rwp_buffer buffer());
}

S1B_TEST(OpenReadNonExisting)

    ASSERT_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_DEFAULT),
        s1b::io_exception);
}

S1B_TEST(OpenWriteNonExisting)

    ASSERT_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_WRITE),
        s1b::io_exception);
}

S1B_TEST(OpenNewNonExisting)

    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW));
}

S1B_TEST(OpenReadExistingEmpty)

    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW));

    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_DEFAULT));
}

S1B_TEST(OpenWriteExistingEmpty)

    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW));

    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_WRITE));
}

S1B_TEST(OpenNewExistingEmpty)

    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW));

    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW));
}

S1B_TEST(EmptySize)

    try
    {
        s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW);
        ASSERT_EQ(0, buffer.get_size());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(Filename)

    try
    {
        s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW);
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

    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW));

    for (int i = 0; i < 50000; i++)
    {
        try
        {
            s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_DEFAULT);
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

S1B_TEST(ReadNothing)

    char data[128];

    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW));

    try
    {
        s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_DEFAULT);
        ASSERT_FALSE(buffer.can_write());
        ASSERT_EQ(0, buffer.read(data, 0, false, false));
        ASSERT_EQ(0, buffer.read(data, 128, false, false));
        ASSERT_EQ(0, buffer.read(data, 128, true, false));
        ASSERT_THROW(buffer.read(data, 0, false, true),
            s1b::incomplete_read_exception);
        ASSERT_THROW(buffer.read(data, 128, false, true),
            s1b::incomplete_read_exception);
        ASSERT_THROW(buffer.read(data, 0, true, true),
            s1b::incomplete_read_exception);
        ASSERT_THROW(buffer.read(data, 128, true, true),
            s1b::incomplete_read_exception);
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

S1B_TEST(WriteOnReadOnlyFile)

    char data[128];
    float data2 = 0;

    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW));

    try
    {
        s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_DEFAULT);
        ASSERT_FALSE(buffer.can_write());
        ASSERT_THROW(buffer.write(data, 0), s1b::read_only_exception);
        ASSERT_THROW(buffer.write(data, 128), s1b::read_only_exception);
        ASSERT_THROW(buffer.write(data, 11), s1b::read_only_exception);
        ASSERT_THROW(buffer.write(data, 21), s1b::read_only_exception);
        ASSERT_THROW(buffer.write_object(data2), s1b::read_only_exception);
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

void _WriteOn(const char* filename, s1b::open_mode mode)
{
    char data[123];
    float data2 = 0;

    try
    {
        s1b::rwp_buffer buffer(filename, mode);
        ASSERT_TRUE(buffer.can_write());
        ASSERT_EQ(0, buffer.write(data, 0));
        ASSERT_EQ(123, buffer.write(data, 123));
        ASSERT_EQ(11, buffer.write(data, 11));
        ASSERT_EQ(sizeof(data2), buffer.write_object(data2));
        ASSERT_EQ(123 + 11 + sizeof(data2), buffer.get_size());
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

S1B_TEST(WriteOnWriteableFile)

    // Create the file beforehand.
    ASSERT_NO_THROW(s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW));

    _WriteOn(s1b_file_name, s1b::S1B_OPEN_WRITE);
}

S1B_TEST(WriteOnNewFile)

    _WriteOn(s1b_file_name, s1b::S1B_OPEN_NEW);
}

S1B_TEST(MixedRWOperations)

    try
    {
        static const char* value_1 = "Test123";
        static const int size_1 = sizeof(value_1) / sizeof(value_1[0]);
        static const float value_2 = 3.6843;
        static const short value_3 = -20;

        {
            s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_NEW);
            ASSERT_TRUE(buffer.can_write());
            ASSERT_EQ(size_1, buffer.write(value_1, size_1));
            ASSERT_EQ(2, buffer.seek(2));
            ASSERT_EQ(size_1, buffer.write(value_1, size_1));
            ASSERT_EQ(sizeof(value_2), buffer.write_object(value_2));
            ASSERT_EQ(sizeof(value_3), buffer.write_object(value_3));
            ASSERT_EQ(2 + size_1 + sizeof(value_2) + sizeof(value_3),
                buffer.get_size());
            ASSERT_NO_THROW(buffer.sync());
        }

        {
            char data_1[2 + size_1 + 1];
            float data_2;
            short data_3;

            s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_WRITE);
            ASSERT_TRUE(buffer.can_write());
            ASSERT_EQ(2 + size_1, buffer.read(data_1, 2 + size_1, true, true));
            data_1[2 + size_1] = 0;
            ASSERT_STREQ("TeTest123", data_1);
            ASSERT_EQ(sizeof(data_2), buffer.read_object(data_2, true));
            ASSERT_EQ(value_2, data_2);
            ASSERT_EQ(sizeof(data_3), buffer.read_object(data_3, true));
            ASSERT_EQ(value_3, data_3);
            ASSERT_EQ(2 + size_1 + sizeof(value_2) + sizeof(value_3),
                buffer.get_size());
            ASSERT_EQ(2 + size_1 + sizeof(value_2) + sizeof(value_3),
                buffer.seek(2 + size_1 + sizeof(value_2) + sizeof(value_3)));
            ASSERT_EQ(sizeof(value_2), buffer.write_object(value_2));
            ASSERT_NO_THROW(buffer.sync());
        }

        {
            char data_1[2 + size_1 + 1];
            float data_2;
            short data_3;

            s1b::rwp_buffer buffer(s1b_file_name, s1b::S1B_OPEN_DEFAULT);
            ASSERT_FALSE(buffer.can_write());
            ASSERT_EQ(2 + size_1, buffer.read(data_1, 2 + size_1, true, true));
            data_1[2 + size_1] = 0;
            ASSERT_STREQ("TeTest123", data_1);
            ASSERT_EQ(sizeof(data_2), buffer.read_object(data_2, true));
            ASSERT_EQ(value_2, data_2);
            ASSERT_EQ(sizeof(data_3), buffer.read_object(data_3, true));
            ASSERT_EQ(value_3, data_3);
            ASSERT_EQ(sizeof(data_2), buffer.read_object(data_2, true));
            ASSERT_EQ(value_2, data_2);
            ASSERT_EQ(2 + size_1 + 2 * sizeof(value_2) + sizeof(value_3),
                buffer.get_size());
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
