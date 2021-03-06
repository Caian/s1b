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

#define S1B_TEST_NAME PushData

#include "s1b_test.hpp"
#include "metadata_test.hpp"

#include <s1b/rwp_data.hpp>
#include <s1b/push_data.hpp>
#include <s1b/mapped_data.hpp>
#include <s1b/rwp_metadata.hpp>
#include <s1b/mapped_metadata.hpp>

#include <boost/type_traits/is_same.hpp>

#include <vector>
#include <string>

#define TEST_OBJ_TYPE s1b::push_data
#include "move_test.hpp"

namespace {

typedef s1b::rwp_metadata<test_adapter> test_rwp_metadata;
typedef s1b::mapped_metadata<test_adapter> test_mapped_metadata;

// TODO mapped_data compat

// TODO unlink align

S1B_TEST(OpenPushNonExisting)

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_rwp_metadata metadata(meta_filename);

        ASSERT_THROW(s1b::push_data data(s1b_file_name, false, metadata),
            s1b::io_exception);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(OpenNewNonExisting)

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_rwp_metadata metadata(meta_filename);

        ASSERT_NO_THROW(s1b::push_data data(s1b_file_name, true, metadata));
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(OpenPushExistingEmpty)

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_rwp_metadata metadata(meta_filename);

        ASSERT_NO_THROW(s1b::push_data data(s1b_file_name, true, metadata));

        ASSERT_NO_THROW(s1b::push_data data(s1b_file_name, false, metadata));
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(OpenAligned) // TODO add to others

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_rwp_metadata metadata(meta_filename);

        MAKE_TEST_OBJ(data)(s1b_file_name, true, metadata);
        MOVE_TEST_OBJ(data);
        data.align();
        data.sync();
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
        char test_data[128]; // TODO use S1B_DATA_ALIGNMENT_BYTES?

        s1b::rwp_buffer data(s1b_file_name, s1b::S1B_OPEN_WRITE);
        const s1b::foffset_t off = data.get_size();
        ASSERT_NO_THROW(data.write(test_data, off, sizeof(test_data)));
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
        test_rwp_metadata metadata(meta_filename, false);

        ASSERT_NO_THROW(s1b::push_data data(s1b_file_name, false, metadata));
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(OpenMisligned) // TODO add to others

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_rwp_metadata metadata(meta_filename);

        MAKE_TEST_OBJ(data)(s1b_file_name, true, metadata);
        MOVE_TEST_OBJ(data);
        data.align();
        data.sync();
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
        char test_data[129];

        s1b::rwp_buffer data(s1b_file_name, s1b::S1B_OPEN_WRITE);
        const s1b::foffset_t off = data.get_size();
        ASSERT_NO_THROW(data.write(test_data, off, sizeof(test_data)));
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
        test_rwp_metadata metadata(meta_filename, false);

        ASSERT_THROW(s1b::push_data data(s1b_file_name, false, metadata),
            s1b::misaligned_exception);
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

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_rwp_metadata metadata(meta_filename);

        MAKE_TEST_OBJ(data)(s1b_file_name, true, metadata);
        MOVE_TEST_OBJ(data);

        ASSERT_STREQ(s1b_file_name, data.filename().c_str());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(SizeAndSlotSize)

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    s1b::foffset_t initial_size;

    try
    {
        test_rwp_metadata metadata(meta_filename);

        MAKE_TEST_OBJ(data)(s1b_file_name, true, metadata);
        MOVE_TEST_OBJ(data);
        data.align();
        data.sync();
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
        char test_data[512];

        s1b::rwp_buffer data(s1b_file_name, s1b::S1B_OPEN_WRITE);
        ASSERT_NO_THROW(initial_size = data.get_size());
        const s1b::foffset_t off = data.get_size();
        ASSERT_NO_THROW(data.write(test_data, off, sizeof(test_data)));
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
        test_rwp_metadata metadata(meta_filename, false);

        MAKE_TEST_OBJ(data)(s1b_file_name, false, metadata);
        MOVE_TEST_OBJ(data);

        ASSERT_EQ(512, data.slot_size());
        ASSERT_EQ(512 + initial_size, data.get_size());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(OpenEmptyAndPush)

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    std::vector<char> test_data;
    s1b::foffset_t prev_off = -1;
    s1b::foffset_t off;
    s1b::foffset_t initial_size;

    try
    {
        test_rwp_metadata metadata(meta_filename);

        MAKE_TEST_OBJ(data)(s1b_file_name, true, metadata);
        MOVE_TEST_OBJ(data);
        ASSERT_TRUE(data.can_write());
        ASSERT_EQ(1, data.num_slots());
        ASSERT_EQ(0, data.slot_size());
        ASSERT_NO_THROW(initial_size = data.get_size());

        for (int i = 17; i < 4359; i += 33 + 2*i, prev_off = off)
        {
            test_data.resize(i);
            for (int j = 0; j < i; j++)
                test_data[j] = 32 + ((i + j) % (127-32));

            ASSERT_NO_THROW(off = data.push(&test_data[0], i));
            ASSERT_GT(off, prev_off);
        }

        ASSERT_NO_THROW(data.align());
        ASSERT_EQ(data.get_size(), initial_size + data.slot_size());
        ASSERT_NO_THROW(data.align());
        ASSERT_EQ(data.get_size(), initial_size + data.slot_size());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(AlignAndReOpen)

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    std::vector<char> test_data;
    s1b::foffset_t prev_off = -1;
    s1b::foffset_t off;
    s1b::foffset_t initial_size = 0;

    try
    {
        test_rwp_metadata metadata(meta_filename);

        MAKE_TEST_OBJ(data)(s1b_file_name, true, metadata);
        MOVE_TEST_OBJ(data);
        ASSERT_TRUE(data.can_write());
        ASSERT_EQ(1, data.num_slots());
        ASSERT_EQ(0, data.slot_size());
        ASSERT_NO_THROW(initial_size = data.get_size());

        for (int i = 17; i < 4359; i += 33 + 2*i, prev_off = off)
        {
            test_data.resize(i);
            for (int j = 0; j < i; j++)
                test_data[j] = 32 + ((i + j) % (127-32));

            ASSERT_NO_THROW(off = data.push(&test_data[0], i));
            ASSERT_GT(off, prev_off);
        }

        ASSERT_NO_THROW(data.align());
        ASSERT_EQ(data.get_size(), data.slot_size() + initial_size);
        ASSERT_NO_THROW(data.align());
        ASSERT_EQ(data.get_size(), data.slot_size() + initial_size);
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
        test_rwp_metadata metadata(meta_filename, false);

        MAKE_TEST_OBJ(data)(s1b_file_name, false, metadata);
        MOVE_TEST_OBJ(data);
        ASSERT_TRUE(data.can_write());
        ASSERT_EQ(1, data.num_slots());
        ASSERT_EQ(data.get_size(), data.slot_size() + initial_size);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(MappedCompatCreateNewAndPush)

    std::vector<test_metadata> meta_vector;
    std::vector<char> test_data;
    std::vector<char> read_data;
    char* p_data;
    s1b::foffset_t prev_off = -1;
    s1b::foffset_t off;
    s1b::foffset_t size;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);

        s1b::mapped_data data(s1b_file_name, s1b::S1B_OPEN_NEW, metadata, 1,
            s1b::S1B_HUGETLB_OFF);
        ASSERT_TRUE(data.can_write());
        ASSERT_EQ(1, data.num_slots());

        for (int i = 1; i <= 1000; i++, prev_off = off)
        {
            size = meta_vector[i-1].size;
            ASSERT_NO_THROW(off = metadata.get_data_offset(
                meta_vector[i-1].uid));
            ASSERT_GT(off, prev_off);

            ASSERT_NO_THROW(p_data = data.get_address(off));
            ASSERT_TRUE(static_cast<char*>(0) != p_data);

            test_data.resize(size);
            for (int j = 0; j < size; j++)
                test_data[j] = 32 + ((size + j + 20) % (127-32));

            std::copy(&test_data[0], &test_data[0] + size, p_data);
        }

        ASSERT_NO_THROW(data.sync());
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
        test_mapped_metadata metadata(meta_filename, false);

        MAKE_TEST_OBJ(data)(s1b_file_name, false, metadata);
        MOVE_TEST_OBJ(data);
        ASSERT_TRUE(data.can_write());
        ASSERT_EQ(1, data.num_slots());

        for (int i = 17; i < 4359; i += 33 + 2*i, prev_off = off)
        {
            test_data.resize(i);
            for (int j = 0; j < i; j++)
                test_data[j] = 32 + ((i + j) % (127-32));

            ASSERT_NO_THROW(off = data.push(&test_data[0], i));
            ASSERT_GT(off, prev_off);
        }

        // ASSERT_NO_THROW(data.align());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(RwpCompatCreateNewAndPush)

    std::vector<test_metadata> meta_vector;
    std::vector<char> test_data;
    std::vector<char> read_data;
    s1b::foffset_t prev_off = -1;
    s1b::foffset_t off;
    s1b::foffset_t size;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);

        s1b::rwp_data data(s1b_file_name, s1b::S1B_OPEN_NEW, metadata, 1);
        ASSERT_TRUE(data.can_write());
        ASSERT_EQ(1, data.num_slots());

        for (int i = 1; i <= 1000; i++, prev_off = off)
        {
            size = meta_vector[i-1].size;
            ASSERT_NO_THROW(off = metadata.get_data_offset(
                meta_vector[i-1].uid));
            ASSERT_GT(off, prev_off);

            test_data.resize(size);
            for (int j = 0; j < size; j++)
                test_data[j] = 32 + ((size + j + 20) % (127-32));

            data.write(&test_data[0], off, size);
        }

        ASSERT_NO_THROW(data.sync());
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
        test_mapped_metadata metadata(meta_filename, false);

        MAKE_TEST_OBJ(data)(s1b_file_name, false, metadata);
        MOVE_TEST_OBJ(data);
        ASSERT_TRUE(data.can_write());
        ASSERT_EQ(1, data.num_slots());

        for (int i = 17; i < 4359; i += 33 + 2*i, prev_off = off)
        {
            test_data.resize(i);
            for (int j = 0; j < i; j++)
                test_data[j] = 32 + ((i + j) % (127-32));

            ASSERT_NO_THROW(off = data.push(&test_data[0], i));
            ASSERT_GT(off, prev_off);
        }

        // ASSERT_NO_THROW(data.align());
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
