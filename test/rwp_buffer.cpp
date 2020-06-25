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

#define S1B_TEST_NAME RwpBuffer

#include "s1b_test.hpp"

#include <s1b/rwp_buffer.hpp>

#include <omp.h>

#define TEST_OBJ_TYPE s1b::rwp_buffer
#include "move_test.hpp"

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
        MAKE_TEST_OBJ(buffer)(s1b_file_name, s1b::S1B_OPEN_NEW);
        MOVE_TEST_OBJ(buffer);
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
        MAKE_TEST_OBJ(buffer)(s1b_file_name, s1b::S1B_OPEN_NEW);
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
        s1b::foffset_t position = 0;
        MAKE_TEST_OBJ(buffer)(s1b_file_name, s1b::S1B_OPEN_DEFAULT);
        MOVE_TEST_OBJ(buffer);
        ASSERT_FALSE(buffer.can_write());
        ASSERT_EQ(0, position += buffer.read(data, position, 0, false, false));
        ASSERT_EQ(0, position += buffer.read(data, position, 128, false, false));
        ASSERT_EQ(0, position += buffer.read(data, position, 128, true, false));
        ASSERT_THROW(position += buffer.read(data, position, 0, false, true),
            s1b::incomplete_read_exception);
        ASSERT_THROW(position += buffer.read(data, position, 128, false, true),
            s1b::incomplete_read_exception);
        ASSERT_THROW(position += buffer.read(data, position, 0, true, true),
            s1b::incomplete_read_exception);
        ASSERT_THROW(position += buffer.read(data, position, 128, true, true),
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
        s1b::foffset_t position = 0;
        MAKE_TEST_OBJ(buffer)(s1b_file_name, s1b::S1B_OPEN_DEFAULT);
        MOVE_TEST_OBJ(buffer);
        ASSERT_FALSE(buffer.can_write());
        ASSERT_THROW(position += buffer.write(data, position, 0),
            s1b::read_only_exception);
        ASSERT_THROW(position += buffer.write(data, position, 128),
            s1b::read_only_exception);
        ASSERT_THROW(position += buffer.write(data, position, 11),
            s1b::read_only_exception);
        ASSERT_THROW(position += buffer.write(data, position, 21),
            s1b::read_only_exception);
        ASSERT_THROW(position += buffer.write_object(data2, position),
            s1b::read_only_exception);
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
        s1b::foffset_t position = 0;
        MAKE_TEST_OBJ(buffer)(filename, mode);
        MOVE_TEST_OBJ(buffer);
        ASSERT_TRUE(buffer.can_write());
        ASSERT_EQ(0, position += buffer.write(data, position, 0));
        ASSERT_EQ(123, buffer.write(data, position, 123));
        position += 123;
        ASSERT_EQ(11, buffer.write(data, position, 11));
        position += 11;
        ASSERT_EQ(sizeof(data2), buffer.write_object(data2, position));
        position += sizeof(data2);
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
            s1b::foffset_t position = 0;
            MAKE_TEST_OBJ(buffer)(s1b_file_name, s1b::S1B_OPEN_NEW);
            MOVE_TEST_OBJ(buffer);
            ASSERT_TRUE(buffer.can_write());
            ASSERT_EQ(size_1, buffer.write(value_1, position, size_1));
#if defined(S1B_DISABLE_ATOMIC_RW)
            ASSERT_EQ(2, buffer.seek(2));
#endif
            position = 2;
            ASSERT_EQ(size_1, buffer.write(value_1, position, size_1));
            position += size_1;
            ASSERT_EQ(sizeof(value_2), buffer.write_object(value_2, position));
            position += sizeof(value_2);
            ASSERT_EQ(sizeof(value_3), buffer.write_object(value_3, position));
            position += sizeof(value_3);
            ASSERT_EQ(2 + size_1 + sizeof(value_2) + sizeof(value_3),
                buffer.get_size());
            ASSERT_NO_THROW(buffer.sync());
        }

        {
            char data_1[2 + size_1 + 1];
            float data_2;
            short data_3;

            s1b::foffset_t position = 0;
            MAKE_TEST_OBJ(buffer)(s1b_file_name, s1b::S1B_OPEN_WRITE);
            MOVE_TEST_OBJ(buffer);
            ASSERT_TRUE(buffer.can_write());
            ASSERT_EQ(2 + size_1, buffer.read(data_1, position, 2 + size_1, true, true));
            position += 2 + size_1;
            data_1[2 + size_1] = 0;
            ASSERT_STREQ("TeTest123", data_1);
            ASSERT_EQ(sizeof(data_2), buffer.read_object(data_2, position, true));
            position += sizeof(data_2);
            ASSERT_EQ(value_2, data_2);
            ASSERT_EQ(sizeof(data_3), buffer.read_object(data_3, position, true));
            position += sizeof(data_3);
            ASSERT_EQ(value_3, data_3);
            ASSERT_EQ(2 + size_1 + sizeof(value_2) + sizeof(value_3),
                buffer.get_size());
            position = 2 + size_1 + sizeof(value_2) + sizeof(value_3);
#if defined(S1B_DISABLE_ATOMIC_RW)
            ASSERT_EQ(position, buffer.seek(position));
#endif
            ASSERT_EQ(sizeof(value_2), buffer.write_object(value_2, position));
            ASSERT_NO_THROW(buffer.sync());
        }

        {
            char data_1[2 + size_1 + 1];
            float data_2;
            short data_3;

            s1b::foffset_t position = 0;
            MAKE_TEST_OBJ(buffer)(s1b_file_name, s1b::S1B_OPEN_DEFAULT);
            MOVE_TEST_OBJ(buffer);
            ASSERT_FALSE(buffer.can_write());
            ASSERT_EQ(2 + size_1, buffer.read(data_1, position, 2 + size_1, true, true));
            position += 2 + size_1;
            data_1[2 + size_1] = 0;
            ASSERT_STREQ("TeTest123", data_1);
            ASSERT_EQ(sizeof(data_2), buffer.read_object(data_2, position, true));
            position += sizeof(data_2);
            ASSERT_EQ(value_2, data_2);
            ASSERT_EQ(sizeof(data_3), buffer.read_object(data_3, position, true));
            position += sizeof(data_3);
            ASSERT_EQ(value_3, data_3);
            ASSERT_EQ(sizeof(data_2), buffer.read_object(data_2, position, true));
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

S1B_TEST(ParallelIOTest)

    const int N = 10000;

    try
    {
        MAKE_TEST_OBJ(buffer)(s1b_file_name, s1b::S1B_OPEN_NEW);
        MOVE_TEST_OBJ(buffer);
        ASSERT_TRUE(buffer.can_write());

        int num_threads = 0;

        #pragma omp parallel for schedule(static) num_threads(100)
        for (size_t i = 0; i < N; i++)
        {
            const s1b::foffset_t position = i;

            #pragma omp critical
            {
                num_threads = omp_get_num_threads();
            }

            {
                const char value = 0;
                buffer.write(&value, position, 1);
            }

            for (size_t j = 0; j < 100; j++)
            {
                char value;
                buffer.read(&value, position, 1, true, true);
                value++;
                buffer.write(&value, position, 1);
            }
        }

        ASSERT_EQ(100, num_threads);
        ASSERT_NO_THROW(buffer.sync());

        std::vector<char> result(N);

        #pragma omp parallel for schedule(static) num_threads(100)
        for (size_t i = 0; i < N; i++)
        {
            const s1b::foffset_t position = i;

            #pragma omp critical
            {
                num_threads = omp_get_num_threads();
            }

            for (size_t j = 0; j < 200; j++)
            {
                buffer.read(&result[i], position, 1, true, true);
            }
        }

        ASSERT_EQ(100, num_threads);

        for (size_t i = 0; i < N; i++)
        {
            ASSERT_EQ(100, result[i]);
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
