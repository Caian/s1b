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

#define S1B_TEST_NAME RwpData

#include "s1b_test.hpp"
#include "metadata_test.hpp"

#include <s1b/rwp_data.hpp>
#include <s1b/mapped_data.hpp>
#include <s1b/rwp_metadata.hpp>
#include <s1b/mapped_metadata.hpp>

#include <boost/type_traits/is_same.hpp>

#include <vector>
#include <string>

namespace {

typedef s1b::rwp_metadata<test_adapter> test_rwp_metadata;
typedef s1b::mapped_metadata<test_adapter> test_mapped_metadata;

// TODO mapped_data compat

// TODO unlink

struct NoException { };

template <typename Metadata, typename Exception>
void _Open(
    const char* filename,
    s1b::open_mode mode,
    unsigned int slots
)
{
    static const bool throws_exception = boost::is_same<
        Exception,
        NoException>::value == false;

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    std::string meta_filename(filename);
    meta_filename += "_metadata";

    try
    {
        Metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        if (throws_exception)
        {
            ASSERT_THROW(s1b::rwp_data data(filename, mode, metadata, slots),
                Exception);
        }
        else
        {
            ASSERT_NO_THROW(s1b::rwp_data data(filename,
                mode, metadata, slots));
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

template <typename Metadata>
void _OpenRWNonExisting(
    const char* filename,
    bool can_write,
    unsigned int slots
)
{
    _Open<Metadata, s1b::io_exception>(filename, can_write ?
        s1b::S1B_OPEN_WRITE : s1b::S1B_OPEN_DEFAULT, slots);
}

template <typename Metadata>
void _OpenNewNonExisting(
    const char* filename,
    unsigned int slots
)
{
    s1b::open_mode mode = s1b::S1B_OPEN_NEW;

    if (slots != 0)
    {
        _Open<Metadata, NoException>(filename, mode, slots);
    }
    else
    {
        _Open<Metadata, s1b::invalid_num_slots_exception>(filename,
            mode, slots);
    }
}

template <typename Metadata>
void _OpenExistingEmpty(
    const char* filename,
    bool can_write,
    unsigned int slots
)
{
    s1b::open_mode mode = can_write ? s1b::S1B_OPEN_WRITE :
        s1b::S1B_OPEN_DEFAULT;

    if (slots == 0)
    {
        _Open<Metadata, NoException>(filename, mode, slots);
    }
    else
    {
        _Open<Metadata, s1b::num_slots_mismatch_exception>(filename,
            mode, slots);
    }
}

template <typename Metadata>
void _OpenExistingFull(
    const char* filename,
    bool can_write,
    unsigned int slots
)
{
    _Open<Metadata, NoException>(filename, can_write ?
        s1b::S1B_OPEN_WRITE : s1b::S1B_OPEN_DEFAULT, slots);
}

S1B_TEST(OpenReadNonExisting0SlotRWP)

    _OpenRWNonExisting<test_rwp_metadata>(s1b_file_name, false, 0);
}

S1B_TEST(OpenReadNonExisting1SlotRWP)

    _OpenRWNonExisting<test_rwp_metadata>(s1b_file_name, false, 1);
}

S1B_TEST(OpenReadNonExisting3SlotsRWP)

    _OpenRWNonExisting<test_rwp_metadata>(s1b_file_name, false, 3);
}

S1B_TEST(OpenReadNonExisting10SlotsRWP)

    _OpenRWNonExisting<test_rwp_metadata>(s1b_file_name, false, 10);
}

S1B_TEST(OpenWriteNonExisting0SlotRWP)

    _OpenRWNonExisting<test_rwp_metadata>(s1b_file_name, true, 0);
}

S1B_TEST(OpenWriteNonExisting1SlotRWP)

    _OpenRWNonExisting<test_rwp_metadata>(s1b_file_name, true, 1);
}

S1B_TEST(OpenWriteNonExisting3SlotsRWP)

    _OpenRWNonExisting<test_rwp_metadata>(s1b_file_name, true, 3);
}

S1B_TEST(OpenWriteNonExisting10SlotsRWP)

    _OpenRWNonExisting<test_rwp_metadata>(s1b_file_name, true, 10);
}

S1B_TEST(OpenNewNonExisting0SlotRWP)

    _OpenNewNonExisting<test_rwp_metadata>(s1b_file_name, 0);
}

S1B_TEST(OpenNewNonExisting1SlotRWP)

    _OpenNewNonExisting<test_rwp_metadata>(s1b_file_name, 1);
}

S1B_TEST(OpenNewNonExisting3SlotsRWP)

    _OpenNewNonExisting<test_rwp_metadata>(s1b_file_name, 3);
}

S1B_TEST(OpenNewNonExisting10SlotsRWP)

    _OpenNewNonExisting<test_rwp_metadata>(s1b_file_name, 10);
}

S1B_TEST(OpenReadNonExisting0SlotMapped)

    _OpenRWNonExisting<test_mapped_metadata>(s1b_file_name, false, 0);
}

S1B_TEST(OpenReadNonExisting1SlotMapped)

    _OpenRWNonExisting<test_mapped_metadata>(s1b_file_name, false, 1);
}

S1B_TEST(OpenReadNonExisting3SlotsMapped)

    _OpenRWNonExisting<test_mapped_metadata>(s1b_file_name, false, 3);
}

S1B_TEST(OpenReadNonExisting10SlotsMapped)

    _OpenRWNonExisting<test_mapped_metadata>(s1b_file_name, false, 10);
}

S1B_TEST(OpenWriteNonExisting0SlotMapped)

    _OpenRWNonExisting<test_mapped_metadata>(s1b_file_name, true, 0);
}

S1B_TEST(OpenWriteNonExisting1SlotMapped)

    _OpenRWNonExisting<test_mapped_metadata>(s1b_file_name, true, 1);
}

S1B_TEST(OpenWriteNonExisting3SlotsMapped)

    _OpenRWNonExisting<test_mapped_metadata>(s1b_file_name, true, 3);
}

S1B_TEST(OpenWriteNonExisting10SlotsMapped)

    _OpenRWNonExisting<test_mapped_metadata>(s1b_file_name, true, 10);
}

S1B_TEST(OpenNewNonExisting0SlotMapped)

    _OpenNewNonExisting<test_mapped_metadata>(s1b_file_name, 0);
}

S1B_TEST(OpenNewNonExisting1SlotMapped)

    _OpenNewNonExisting<test_mapped_metadata>(s1b_file_name, 1);
}

S1B_TEST(OpenNewNonExisting3SlotsMapped)

    _OpenNewNonExisting<test_mapped_metadata>(s1b_file_name, 3);
}

S1B_TEST(OpenNewNonExisting10SlotsMapped)

    _OpenNewNonExisting<test_mapped_metadata>(s1b_file_name, 10);
}

S1B_TEST(OpenNewNonExisting)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));
}

S1B_TEST(OpenReadExistingEmpty0SlotRWP)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_rwp_metadata>(s1b_file_name, false, 0);
}

S1B_TEST(OpenReadExistingEmpty1SlotRWP)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_rwp_metadata>(s1b_file_name, false, 1);
}

S1B_TEST(OpenReadExistingEmpty3SlotsRWP)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_rwp_metadata>(s1b_file_name, false, 3);
}

S1B_TEST(OpenReadExistingEmpty10SlotsRWP)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_rwp_metadata>(s1b_file_name, false, 10);
}

S1B_TEST(OpenWriteExistingEmpty0SlotRWP)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_rwp_metadata>(s1b_file_name, true, 0);
}

S1B_TEST(OpenWriteExistingEmpty1SlotRWP)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_rwp_metadata>(s1b_file_name, true, 1);
}

S1B_TEST(OpenWriteExistingEmpty3SlotsRWP)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_rwp_metadata>(s1b_file_name, true, 3);
}

S1B_TEST(OpenWriteExistingEmpty10SlotsRWP)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_rwp_metadata>(s1b_file_name, true, 10);
}

S1B_TEST(OpenReadExistingEmpty0SlotMapped)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_mapped_metadata>(s1b_file_name, false, 1);
}

S1B_TEST(OpenReadExistingEmpty1SlotMapped)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_mapped_metadata>(s1b_file_name, false, 1);
}

S1B_TEST(OpenReadExistingEmpty3SlotsMapped)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_mapped_metadata>(s1b_file_name, false, 3);
}

S1B_TEST(OpenReadExistingEmpty10SlotsMapped)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_mapped_metadata>(s1b_file_name, false, 10);
}

S1B_TEST(OpenWriteExistingEmpty0SlotMapped)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_mapped_metadata>(s1b_file_name, true, 0);
}

S1B_TEST(OpenWriteExistingEmpty1SlotMapped)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_mapped_metadata>(s1b_file_name, true, 1);
}

S1B_TEST(OpenWriteExistingEmpty3SlotsMapped)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_mapped_metadata>(s1b_file_name, true, 3);
}

S1B_TEST(OpenWriteExistingEmpty10SlotsMapped)

    ASSERT_NO_THROW(s1b::rwp_data data(s1b_file_name));

    _OpenExistingEmpty<test_mapped_metadata>(s1b_file_name, true, 10);
}

S1B_TEST(OpenReadExistingNonEmpty0SlotRWP)

    _OpenNewNonExisting<test_rwp_metadata>(s1b_file_name, 1);

    _OpenExistingFull<test_rwp_metadata>(s1b_file_name, false, 0);
}

S1B_TEST(OpenReadExistingNonEmpty1SlotRWP)

    _OpenNewNonExisting<test_rwp_metadata>(s1b_file_name, 1);

    _OpenExistingFull<test_rwp_metadata>(s1b_file_name, false, 1);
}

S1B_TEST(OpenReadExistingNonEmpty3SlotsRWP)

    _OpenNewNonExisting<test_rwp_metadata>(s1b_file_name, 3);

    _OpenExistingFull<test_rwp_metadata>(s1b_file_name, false, 3);
}

S1B_TEST(OpenReadExistingNonEmpty10SlotsRWP)

    _OpenNewNonExisting<test_rwp_metadata>(s1b_file_name, 10);

    _OpenExistingFull<test_rwp_metadata>(s1b_file_name, false, 10);
}

S1B_TEST(OpenWriteExistingNonEmpty0SlotRWP)

    _OpenNewNonExisting<test_rwp_metadata>(s1b_file_name, 1);

    _OpenExistingFull<test_rwp_metadata>(s1b_file_name, true, 0);
}

S1B_TEST(OpenWriteExistingNonEmpty1SlotRWP)

    _OpenNewNonExisting<test_rwp_metadata>(s1b_file_name, 1);

    _OpenExistingFull<test_rwp_metadata>(s1b_file_name, true, 1);
}

S1B_TEST(OpenWriteExistingNonEmpty3SlotsRWP)

    _OpenNewNonExisting<test_rwp_metadata>(s1b_file_name, 3);

    _OpenExistingFull<test_rwp_metadata>(s1b_file_name, true, 3);
}

S1B_TEST(OpenWriteExistingNonEmpty10SlotsRWP)

    _OpenNewNonExisting<test_rwp_metadata>(s1b_file_name, 10);

    _OpenExistingFull<test_rwp_metadata>(s1b_file_name, true, 10);
}

S1B_TEST(OpenReadExistingNonEmpty0SlotMapped)

    _OpenNewNonExisting<test_mapped_metadata>(s1b_file_name, 1);

    _OpenExistingFull<test_mapped_metadata>(s1b_file_name, false, 0);
}

S1B_TEST(OpenReadExistingNonEmpty1SlotMapped)

    _OpenNewNonExisting<test_mapped_metadata>(s1b_file_name, 1);

    _OpenExistingFull<test_mapped_metadata>(s1b_file_name, false, 1);
}

S1B_TEST(OpenReadExistingNonEmpty3SlotsMapped)

    _OpenNewNonExisting<test_mapped_metadata>(s1b_file_name, 3);

    _OpenExistingFull<test_mapped_metadata>(s1b_file_name, false, 3);
}

S1B_TEST(OpenReadExistingNonEmpty10SlotsMapped)

    _OpenNewNonExisting<test_mapped_metadata>(s1b_file_name, 10);

    _OpenExistingFull<test_mapped_metadata>(s1b_file_name, false, 10);
}

S1B_TEST(OpenWriteExistingNonEmpty0SlotMapped)

    _OpenNewNonExisting<test_mapped_metadata>(s1b_file_name, 1);

    _OpenExistingFull<test_mapped_metadata>(s1b_file_name, true, 0);
}

S1B_TEST(OpenWriteExistingNonEmpty1SlotMapped)

    _OpenNewNonExisting<test_mapped_metadata>(s1b_file_name, 1);

    _OpenExistingFull<test_mapped_metadata>(s1b_file_name, true, 1);
}

S1B_TEST(OpenWriteExistingNonEmpty3SlotsMapped)

    _OpenNewNonExisting<test_mapped_metadata>(s1b_file_name, 3);

    _OpenExistingFull<test_mapped_metadata>(s1b_file_name, true, 3);
}

S1B_TEST(OpenWriteExistingNonEmpty10SlotsMapped)

    _OpenNewNonExisting<test_mapped_metadata>(s1b_file_name, 10);

    _OpenExistingFull<test_mapped_metadata>(s1b_file_name, true, 10);
}

S1B_TEST(Filename)

    std::vector<test_metadata> meta_vector;

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
            meta_vector.end());

        s1b::rwp_data data(s1b_file_name, s1b::S1B_OPEN_NEW, metadata, 1);

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

    std::vector<test_metadata> meta_vector;

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
            meta_vector.end());

        s1b::rwp_data data(s1b_file_name, s1b::S1B_OPEN_NEW, metadata, 3);

        ASSERT_EQ(3 * data.slot_size(), data.get_size());
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

    std::vector<char> test_data;
    s1b::foffset_t prev_off = -1;
    s1b::foffset_t off;

    try
    {
        s1b::rwp_data data(s1b_file_name);
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
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(OpenEmptyAndPushReadWrite)

    std::vector<char> test_data;
    std::vector<char> read_data;
    s1b::foffset_t prev_off = -1;
    s1b::foffset_t off;

    try
    {
        s1b::rwp_data data(s1b_file_name);
        ASSERT_TRUE(data.can_write());
        ASSERT_EQ(1, data.num_slots());

        for (int i = 19; i < 4411; i += 31 + 2*i, prev_off = off)
        {
            test_data.resize(i);
            for (int j = 0; j < i; j++)
                test_data[j] = 32 + ((i + j) % (127-32));

            ASSERT_NO_THROW(off = data.push(&test_data[0], i));
            ASSERT_NO_THROW(data.sync());
            ASSERT_GT(off, prev_off);

            read_data.resize(i);
            ASSERT_NO_THROW(data.read(&read_data[0], off, i));

            for (int j = 0; j < i; j++)
                ASSERT_TRUE(test_data[j] == read_data[j]);

            for (int j = 0; j < i; j++)
                test_data[j] = 32 + ((i + j + 20) % (127-32));

            ASSERT_NO_THROW(data.write(&test_data[0], off, i));
            ASSERT_NO_THROW(data.read(&read_data[0], off, i));

            for (int j = 0; j < i; j++)
                ASSERT_TRUE(test_data[j] == read_data[j]);
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
}

S1B_TEST(PushMisaligned)

    std::vector<char> test_data;
    s1b::foffset_t prev_off = -1;
    s1b::foffset_t off;

    try
    {
        s1b::rwp_data data(s1b_file_name);
        ASSERT_TRUE(data.can_write());
        ASSERT_EQ(1, data.num_slots());
        test_data.resize(64);
        ASSERT_NO_THROW(data.write(&test_data[0], 0, 3));
        ASSERT_EQ(3, data.get_size());
        ASSERT_THROW(data.push(&test_data[0], test_data.size()),
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

S1B_TEST(ReadWriteMultipleSlotFromMetadata)

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

        s1b::rwp_data data(s1b_file_name, s1b::S1B_OPEN_NEW, metadata, 5);
        ASSERT_TRUE(data.can_write());
        ASSERT_EQ(5, data.num_slots());

        for (int i = 1; i <= 1000; i++, prev_off = off)
        {
            for (int k = 0; k < 5; k++)
            {
                size = meta_vector[i-1].size;
                ASSERT_NO_THROW(off = metadata.get_data_offset(
                    meta_vector[i-1].uid));
                ASSERT_GT(off, prev_off);

                read_data.resize(size);
                ASSERT_NO_THROW(data.read(&read_data[0], off, size, k));

                test_data.resize(size);
                for (int j = 0; j < size; j++)
                    test_data[j] = 32 + ((size + k + j + 20) % (127-32));

                ASSERT_NO_THROW(data.write(&test_data[0], off, size, k));
                ASSERT_NO_THROW(data.read(&read_data[0], off, size, k));

                for (int j = 0; j < size; j++)
                    ASSERT_TRUE(test_data[j] == read_data[j]);
            }
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
        test_mapped_metadata metadata(meta_filename,
            false, s1b::S1B_HUGETLB_OFF);

        s1b::rwp_data data(s1b_file_name, s1b::S1B_OPEN_DEFAULT, metadata, 0);
        ASSERT_FALSE(data.can_write());
        ASSERT_EQ(5, data.num_slots());

        prev_off = -1;

        for (int i = 1; i <= 1000; i++, prev_off = off)
        {
            for (int k = 0; k < 5; k++)
            {
                size = meta_vector[i-1].size;
                ASSERT_NO_THROW(off = metadata.get_data_offset(
                    meta_vector[i-1].uid));
                ASSERT_GT(off, prev_off);

                read_data.resize(size);
                ASSERT_NO_THROW(data.read(&read_data[0], off, size, k));

                test_data.resize(size);
                for (int j = 0; j < size; j++)
                    test_data[j] = 32 + ((size + k + j + 20) % (127-32));

                ASSERT_THROW(data.write(&test_data[0], off, size, k),
                    s1b::read_only_exception);
            }
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
}

S1B_TEST(PushMultipleSlot)

    std::vector<test_metadata> meta_vector;
    std::vector<char> test_data;
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

        s1b::rwp_data data(s1b_file_name, s1b::S1B_OPEN_NEW, metadata, 5);
        ASSERT_TRUE(data.can_write());
        ASSERT_EQ(5, data.num_slots());

        for (int i = 1; i <= 1000; i++)
        {
            size = meta_vector[i-1].size;

            test_data.resize(size);

            ASSERT_THROW(data.push(&test_data[0], size),
                s1b::invalid_num_slots_exception);
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
}

void _MappedCompatCreateNew(const char* filename, bool can_write)
{
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

    std::string meta_filename(filename);
    meta_filename += "_metadata";

    try
    {
        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);

        s1b::mapped_data data(filename, s1b::S1B_OPEN_NEW, metadata, 5,
            s1b::S1B_HUGETLB_OFF);
        ASSERT_TRUE(data.can_write());
        ASSERT_EQ(5, data.num_slots());

        for (int i = 1; i <= 1000; i++, prev_off = off)
        {
            for (int k = 0; k < 5; k++)
            {
                size = meta_vector[i-1].size;
                ASSERT_NO_THROW(off = metadata.get_data_offset(
                    meta_vector[i-1].uid));
                ASSERT_GT(off, prev_off);

                ASSERT_NO_THROW(p_data = data.get_address(off, k));
                ASSERT_TRUE(static_cast<char*>(0) != p_data);

                test_data.resize(size);
                for (int j = 0; j < size; j++)
                    test_data[j] = 32 + ((size + k + j + 20) % (127-32));

                std::copy(&test_data[0], &test_data[0] + size, p_data);
            }
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
        test_mapped_metadata metadata(meta_filename,
            false, s1b::S1B_HUGETLB_OFF);

        s1b::open_mode mode = can_write ? s1b::S1B_OPEN_WRITE :
            s1b::S1B_OPEN_DEFAULT;

        s1b::rwp_data data(filename, mode, metadata, 0);
        ASSERT_EQ(can_write, data.can_write());
        ASSERT_EQ(5, data.num_slots());

        prev_off = -1;

        for (int i = 1; i <= 1000; i++, prev_off = off)
        {
            for (int k = 0; k < 5; k++)
            {
                size = meta_vector[i-1].size;
                ASSERT_NO_THROW(off = metadata.get_data_offset(
                    meta_vector[i-1].uid));
                ASSERT_GT(off, prev_off);

                test_data.resize(size);
                for (int j = 0; j < size; j++)
                    test_data[j] = 32 + ((size + k + j + 20) % (127-32));

                read_data.resize(size);
                ASSERT_NO_THROW(data.read(&read_data[0], off, size, k));

                for (int j = 0; j < size; j++)
                    ASSERT_TRUE(test_data[j] == read_data[j]);

                if (can_write)
                {
                    ASSERT_NO_THROW(data.write(&test_data[0], off, size, k));
                }
            }
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
}

S1B_TEST(MappedCompatCreateNewAndOpenReadOnly)

    _MappedCompatCreateNew(s1b_file_name, false);
}

S1B_TEST(MappedCompatCreateNewAndOpenWrite)

    _MappedCompatCreateNew(s1b_file_name, true);
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
        test_mapped_metadata metadata(meta_filename, false,
            s1b::S1B_HUGETLB_OFF);

        s1b::rwp_data data(s1b_file_name, s1b::S1B_OPEN_WRITE, metadata, 0);
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
