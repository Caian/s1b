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

#include "s1b_test.hpp"

#include <s1b/push_buffer.hpp>

namespace {

// TODO unlink

S1B_TEST(OpenWriteNonExisting)

    ASSERT_THROW(s1b::push_buffer buffer(s1b_file_name, false),
        s1b::io_exception);
}

S1B_TEST(OpenNewNonExisting)

    ASSERT_NO_THROW(s1b::push_buffer buffer(s1b_file_name, true));
}

S1B_TEST(OpenWriteExistingEmpty)

    ASSERT_NO_THROW(s1b::push_buffer buffer(s1b_file_name, true));

    ASSERT_NO_THROW(s1b::push_buffer buffer(s1b_file_name, false));
}

S1B_TEST(OpenNewExistingEmpty)

    ASSERT_NO_THROW(s1b::push_buffer buffer(s1b_file_name, true));

    ASSERT_NO_THROW(s1b::push_buffer buffer(s1b_file_name, true));
}

S1B_TEST(EmptySize)

    try
    {
        s1b::push_buffer buffer(s1b_file_name, true);
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
        s1b::push_buffer buffer(s1b_file_name, true);
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

    ASSERT_NO_THROW(s1b::push_buffer buffer(s1b_file_name, true));

    for (int i = 0; i < 50000; i++)
    {
        try
        {
            s1b::push_buffer buffer(s1b_file_name, false);
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

    ASSERT_NO_THROW(s1b::push_buffer buffer(s1b_file_name, true));

    try
    {
        s1b::push_buffer buffer(s1b_file_name, false);
        ASSERT_TRUE(buffer.can_write());
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

void _WriteOn(const char* filename, bool create_new)
{
    char data[123];
    float data2 = 0;

    try
    {
        s1b::push_buffer buffer(filename, create_new);
        ASSERT_TRUE(buffer.can_write());
        ASSERT_NO_THROW(buffer.write(data, 0));
        ASSERT_NO_THROW(buffer.write(data, 123));
        ASSERT_NO_THROW(buffer.write(data, 11));
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
    ASSERT_NO_THROW(s1b::push_buffer buffer(s1b_file_name, true));

    _WriteOn(s1b_file_name, false);
}

S1B_TEST(WriteOnNewFile)

    _WriteOn(s1b_file_name, true);
}

S1B_TEST(FillBuffer)

    static const int data_size = 1031;
    static const int num_pushes = S1B_PUSH_BUFFER_SIZE / data_size;

    try
    {
        char test_data[data_size];
        char read_data[data_size];
        s1b::push_buffer buffer(s1b_file_name, true);
        ASSERT_TRUE(buffer.can_write());
        for (int i = 0; i <= num_pushes; i++)
        {
            for (int j = 0; j < data_size; j++)
                test_data[j] = 32 + ((7*i + j) % (127-32));

            ASSERT_NO_THROW(buffer.write(test_data, data_size));
        }
        // Try to read before sync to force it to happen anyway.
        buffer.seek(0);
        for (int i = 0; i <= num_pushes; i++)
            ASSERT_EQ(data_size, buffer.read(read_data, data_size, true, true));
        ASSERT_NO_THROW(buffer.sync());

    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }

    try
    {
        char read_data[data_size];
        s1b::push_buffer buffer(s1b_file_name, false);
        ASSERT_TRUE(buffer.can_write());
        buffer.seek(0);
        for (int i = 0; i <= num_pushes; i++)
            ASSERT_EQ(data_size, buffer.read(read_data, data_size, true, true));
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

S1B_TEST(SkipBuffer)

    static const int data_size = 1031;
    static const int num_pushes = S1B_PUSH_BUFFER_SIZE / data_size;

    try
    {
        char read_data[data_size];
        s1b::push_buffer buffer(s1b_file_name, true);
        ASSERT_TRUE(buffer.can_write());
        for (int i = 0; i <= num_pushes; i++)
            ASSERT_NO_THROW(buffer.skip(data_size));
        // Try to read before sync to force it to happen anyway.
        buffer.seek(0);
        for (int i = 0; i <= num_pushes; i++)
            ASSERT_EQ(data_size, buffer.read(read_data, data_size, true, true));
        ASSERT_NO_THROW(buffer.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }

    try
    {
        char read_data[data_size];
        s1b::push_buffer buffer(s1b_file_name, false);
        ASSERT_TRUE(buffer.can_write());
        buffer.seek(0);
        for (int i = 0; i <= num_pushes; i++)
            ASSERT_EQ(data_size, buffer.read(read_data, data_size, true, true));
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

        {
            s1b::push_buffer buffer(s1b_file_name, true);
            ASSERT_TRUE(buffer.can_write());
            ASSERT_NO_THROW(buffer.write(value_1, size_1));
            ASSERT_EQ(2, buffer.seek(2));
            ASSERT_NO_THROW(buffer.write(value_1, size_1));
            ASSERT_EQ(sizeof(value_2), buffer.write_object(value_2));
            ASSERT_EQ(sizeof(value_3), buffer.write_object(value_3));
            ASSERT_EQ(2 + size_1 + sizeof(value_2) + sizeof(value_3),
                buffer.get_size());
            ASSERT_NO_THROW(buffer.sync());
        }

        {
            s1b::push_buffer buffer(s1b_file_name, false);
            ASSERT_TRUE(buffer.can_write());
            ASSERT_NO_THROW(buffer.write(value_1, size_1));
            ASSERT_EQ(4 + size_1 + sizeof(value_2) + sizeof(value_3),
                buffer.seek(4 + size_1 + sizeof(value_2) + sizeof(value_3)));
            ASSERT_NO_THROW(buffer.write(value_1, size_1));
            ASSERT_EQ(sizeof(value_2), buffer.write_object(value_2));
            ASSERT_EQ(sizeof(value_3), buffer.write_object(value_3));
            ASSERT_EQ(2 * (2 + size_1 + sizeof(value_2) + sizeof(value_3)),
                buffer.get_size());
            ASSERT_NO_THROW(buffer.sync());
        }

        {
            char data_1[2 + size_1 + 1];
            float data_2;
            short data_3;

            s1b::push_buffer buffer(s1b_file_name, false);
            ASSERT_TRUE(buffer.can_write());
            ASSERT_NO_THROW(buffer.seek(0));
            ASSERT_EQ(2 + size_1, buffer.read(data_1, 2 + size_1, true, true));
            data_1[2 + size_1] = 0;
            ASSERT_STREQ("TeTest123", data_1);
            ASSERT_EQ(sizeof(data_2), buffer.read_object(data_2, true));
            ASSERT_EQ(value_2, data_2);
            ASSERT_EQ(sizeof(data_3), buffer.read_object(data_3, true));
            ASSERT_EQ(value_3, data_3);
            ASSERT_EQ(2 + size_1 + sizeof(value_2) + sizeof(value_3),
                buffer.seek(2 + size_1 + sizeof(value_2) + sizeof(value_3)));
            ASSERT_EQ(sizeof(value_2), buffer.write_object(value_2));
            ASSERT_EQ(2 * (2 + size_1 + sizeof(value_2) + sizeof(value_3)),
                buffer.get_size());
            ASSERT_EQ(2 + size_1 + sizeof(value_2) + sizeof(value_3),
                buffer.seek(2 + size_1 + sizeof(value_2) + sizeof(value_3)));
            ASSERT_EQ(sizeof(data_2), buffer.read_object(data_2, true));
            ASSERT_EQ(value_2, data_2);
            ASSERT_EQ(size_1 - 2, buffer.read(data_1, size_1 - 2, true, true));
            data_1[size_1 - 2] = 0;
            ASSERT_STREQ("st123", data_1);
            ASSERT_EQ(sizeof(data_2), buffer.read_object(data_2, true));
            ASSERT_EQ(value_2, data_2);
            ASSERT_EQ(sizeof(data_3), buffer.read_object(data_3, true));
            ASSERT_EQ(value_3, data_3);
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
