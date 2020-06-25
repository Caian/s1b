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

#define S1B_TEST_NAME RwpMetadata

#include "s1b_test.hpp"
#include "metadata_test.hpp"

#include <s1b/rwp_metadata.hpp>
#include <s1b/push_metadata.hpp>
#include <s1b/mapped_metadata.hpp>

#include <vector>

#include <omp.h>

#define TEST_OBJ_TYPE test_rwp_metadata
#include "move_test.hpp"

namespace {

typedef s1b::rwp_metadata<test_adapter> test_rwp_metadata;
typedef s1b::push_metadata<test_adapter> test_push_metadata;
typedef s1b::mapped_metadata<test_adapter> test_mapped_metadata;

// TODO unlink

S1B_TEST(OpenReadNonExisting)

    ASSERT_THROW(test_rwp_metadata metadata(s1b_file_name, false),
        s1b::io_exception);
}

S1B_TEST(OpenWriteNonExisting)

    ASSERT_THROW(test_rwp_metadata metadata(s1b_file_name, true),
        s1b::io_exception);
}

S1B_TEST(OpenNewNonExisting)

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));
}

S1B_TEST(FromIteratorNonExisting)

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 10; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end()));
}

S1B_TEST(OpenReadExistingEmpty)

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name, false));
}

S1B_TEST(OpenWriteExistingEmpty)

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name, true));
}

S1B_TEST(OpenNewExistingEmpty)

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));
}

S1B_TEST(FromIteratorExistingEmpty)

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 10; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end()));
}

S1B_TEST(GetSize)

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name);
        MOVE_TEST_OBJ(metadata);
        ASSERT_NO_THROW(metadata.get_size());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(GetDataSize)

    std::vector<test_metadata> meta_vector;

    int size = 0;

    for (int i = 1; i <= 10; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
        size += meta.size;
    }

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name, meta_vector.begin(),
            meta_vector.end());
        MOVE_TEST_OBJ(metadata);

        // Probably will be greater due to alignment
        ASSERT_LE(size, metadata.get_data_size());

    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(GetDataSizeEmpty)

    std::vector<test_metadata> meta_vector;

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name, meta_vector.begin(),
            meta_vector.end());
        MOVE_TEST_OBJ(metadata);

        ASSERT_EQ(0, metadata.get_data_size());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(GetMetaAdapter)

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name);
        MOVE_TEST_OBJ(metadata);
        ASSERT_NO_THROW(metadata.meta_adapter());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(GetGlobalStruct)

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name);
        MOVE_TEST_OBJ(metadata);
        ASSERT_NO_THROW(metadata.global_struct());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(UpdateGlobalStructReadOnlyFile)

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name, false);
        MOVE_TEST_OBJ(metadata);
        test_global_data data;
        ASSERT_NO_THROW(data = metadata.global_struct());
        data.test = 50;
        ASSERT_THROW(metadata.update_global_struct(data),
            s1b::read_only_exception);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(UpdateGlobalStructWriteableFile)

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name, true);
        MOVE_TEST_OBJ(metadata);
        test_global_data data;
        ASSERT_NO_THROW(data = metadata.global_struct());
        data.test = 50;
        ASSERT_NO_THROW(metadata.update_global_struct(data));
        ASSERT_NO_THROW(data = metadata.global_struct());
        ASSERT_EQ(50, data.test);
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
        MAKE_TEST_OBJ(metadata)(s1b_file_name, false);
        MOVE_TEST_OBJ(metadata);
        test_global_data data;
        ASSERT_NO_THROW(data = metadata.global_struct());
        ASSERT_EQ(50, data.test);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(UpdateGlobalStructNewFile)

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name);
        MOVE_TEST_OBJ(metadata);
        test_global_data data;
        ASSERT_NO_THROW(data = metadata.global_struct());
        data.test = 50;
        ASSERT_NO_THROW(metadata.update_global_struct(data));
        ASSERT_EQ(50, data.test);
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
        MAKE_TEST_OBJ(metadata)(s1b_file_name, false);
        MOVE_TEST_OBJ(metadata);
        test_global_data data;
        ASSERT_NO_THROW(data = metadata.global_struct());
        ASSERT_EQ(50, data.test);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(EmptyLastUID)

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name);
        MOVE_TEST_OBJ(metadata);
        ASSERT_EQ(0, metadata.get_last_uid());
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
        MAKE_TEST_OBJ(metadata)(s1b_file_name);
        MOVE_TEST_OBJ(metadata);
        ASSERT_STREQ(s1b_file_name, metadata.filename().c_str());
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

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    for (int i = 0; i < 50000; i++)
    {
        try
        {
            test_rwp_metadata metadata(s1b_file_name, false);
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

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    try
    {
        test_metadata meta;
        s1b::foffset_t offset;

        MAKE_TEST_OBJ(metadata)(s1b_file_name, false);
        MOVE_TEST_OBJ(metadata);
        ASSERT_FALSE(metadata.can_write());
        ASSERT_FALSE(metadata.read(s1b::FirstUID, meta));
        ASSERT_FALSE(metadata.read(s1b::FirstUID, meta, offset));
        ASSERT_FALSE(metadata.read_data_offset(s1b::FirstUID, offset));
        meta.uid = s1b::FirstUID;
        ASSERT_THROW(metadata.read_data_offset(meta),
            s1b::invalid_uid_exception);
        const test_rwp_metadata& cmetadata = metadata;
        ASSERT_FALSE(cmetadata.read(s1b::FirstUID, meta));
        ASSERT_FALSE(cmetadata.read(s1b::FirstUID, meta, offset));
        ASSERT_FALSE(cmetadata.read_data_offset(s1b::FirstUID, offset));
        meta.uid = s1b::FirstUID;
        ASSERT_THROW(cmetadata.read_data_offset(meta),
            s1b::invalid_uid_exception);
        ASSERT_NO_THROW(metadata.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(PushOnReadOnlyFile)

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    try
    {
        test_metadata meta;
        s1b::foffset_t offset;
        s1b::foffset_t size;
        initialize_metadata::small_1(meta);

        MAKE_TEST_OBJ(metadata)(s1b_file_name, false);
        MOVE_TEST_OBJ(metadata);
        ASSERT_FALSE(metadata.can_write());
        ASSERT_THROW(metadata.push(meta),
            s1b::read_only_exception);
        ASSERT_THROW(metadata.push(meta, offset),
            s1b::read_only_exception);
        ASSERT_THROW(metadata.push(meta, offset, size),
            s1b::read_only_exception);
        ASSERT_NO_THROW(metadata.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(PushOnWriteableFile)

    // Create the file beforehand.
    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    try
    {
        test_metadata meta;
        s1b::foffset_t offset;
        s1b::foffset_t size;
        initialize_metadata::small_1(meta);

        MAKE_TEST_OBJ(metadata)(s1b_file_name, true);
        MOVE_TEST_OBJ(metadata);
        ASSERT_TRUE(metadata.can_write());
        ASSERT_EQ(s1b::FirstUID + 0, metadata.push(meta));
        ASSERT_EQ(s1b::FirstUID + 1, metadata.push(meta, offset));
        ASSERT_EQ(s1b::FirstUID + 2, metadata.push(meta, offset, size));
        ASSERT_EQ(s1b::FirstUID + 2, metadata.get_last_uid());
        ASSERT_NO_THROW(metadata.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(PushOnNewFile)

    static const s1b::uid_t uid_0 = s1b::FirstUID + 0;
    static const s1b::uid_t uid_1 = s1b::FirstUID + 1;
    static const s1b::uid_t uid_2 = s1b::FirstUID + 2;

    try
    {
        test_metadata meta;
        s1b::foffset_t offset;
        s1b::foffset_t size;
        initialize_metadata::small_1(meta);

        MAKE_TEST_OBJ(metadata)(s1b_file_name);
        MOVE_TEST_OBJ(metadata);
        ASSERT_TRUE(metadata.can_write());
        ASSERT_EQ(uid_0, metadata.push(meta));
        ASSERT_EQ(uid_1, metadata.push(meta, offset));
        ASSERT_EQ(uid_2, metadata.push(meta, offset, size));
        ASSERT_EQ(uid_2, metadata.get_last_uid());
        ASSERT_NO_THROW(metadata.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(PushFixedOnWriteableFile)

    // Create the file beforehand.
    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    try
    {
        test_metadata meta;
        s1b::foffset_t offset;
        s1b::foffset_t size, size2;
        initialize_metadata::small_1(meta);

        MAKE_TEST_OBJ(metadata)(s1b_file_name, true);
        MOVE_TEST_OBJ(metadata);
        ASSERT_TRUE(metadata.can_write());
        ASSERT_EQ(s1b::FirstUID + 0, metadata.push_fixed(
            meta, 0, size));
        ASSERT_THROW(metadata.push_fixed(meta, 0),
            s1b::data_offset_overlap_exception);
        ASSERT_THROW(metadata.push_fixed(meta, 0, size2),
            s1b::data_offset_overlap_exception);
        offset = s1b::mem_align::size<S1B_DATA_ALIGNMENT_BYTES>(size) + 1;
        ASSERT_THROW(metadata.push_fixed(meta, offset, size),
            s1b::misaligned_exception);
        offset -= 1;
        ASSERT_EQ(s1b::FirstUID + 1, metadata.push_fixed(
            meta, offset, size));
        offset += 10 * s1b::mem_align::size<S1B_DATA_ALIGNMENT_BYTES>(size);
        ASSERT_EQ(s1b::FirstUID + 2, metadata.push_fixed(
            meta, offset));
        ASSERT_THROW(metadata.push_fixed(meta,
            s1b::mem_align::size<S1B_DATA_ALIGNMENT_BYTES>(offset / 2)),
            s1b::data_offset_overlap_exception);
        ASSERT_EQ(s1b::FirstUID + 2, metadata.get_last_uid());
        ASSERT_NO_THROW(metadata.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(PushFixedOnNewFile)

    try
    {
        test_metadata meta;
        s1b::foffset_t offset;
        s1b::foffset_t size, size2;
        initialize_metadata::small_1(meta);

        MAKE_TEST_OBJ(metadata)(s1b_file_name);
        MOVE_TEST_OBJ(metadata);
        ASSERT_TRUE(metadata.can_write());
        ASSERT_EQ(s1b::FirstUID + 0, metadata.push_fixed(
            meta, 0, size));
        ASSERT_THROW(metadata.push_fixed(meta, 0),
            s1b::data_offset_overlap_exception);
        ASSERT_THROW(metadata.push_fixed(meta, 0, size2),
            s1b::data_offset_overlap_exception);
        offset = s1b::mem_align::size<S1B_DATA_ALIGNMENT_BYTES>(size) + 1;
        ASSERT_THROW(metadata.push_fixed(meta, offset, size),
            s1b::misaligned_exception);
        offset -= 1;
        ASSERT_EQ(s1b::FirstUID + 1, metadata.push_fixed(
            meta, offset, size));
        offset += 10 * s1b::mem_align::size<S1B_DATA_ALIGNMENT_BYTES>(size);
        ASSERT_EQ(s1b::FirstUID + 2, metadata.push_fixed(
            meta, offset));
        ASSERT_THROW(metadata.push_fixed(meta,
            s1b::mem_align::size<S1B_DATA_ALIGNMENT_BYTES>(offset / 2)),
            s1b::data_offset_overlap_exception);
        ASSERT_EQ(s1b::FirstUID + 2, metadata.get_last_uid());
        ASSERT_NO_THROW(metadata.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(ReadOnReadOnlyFile)

    static const s1b::uid_t uid_0 = s1b::FirstUID + 0;
    static const s1b::uid_t uid_1 = s1b::FirstUID + 1;
    static const s1b::uid_t uid_2 = s1b::FirstUID + 2;
    static const s1b::uid_t uid_3 = s1b::FirstUID + 3;

    test_metadata meta_ref_1;
    test_metadata meta_ref_2;
    test_metadata meta_ref_3;

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name);
        MOVE_TEST_OBJ(metadata);
        initialize_metadata::small_1(meta_ref_1);
        meta_ref_1.uid = uid_0;
        initialize_metadata::small_2(meta_ref_2);
        meta_ref_2.uid = uid_1;
        initialize_metadata::small_3(meta_ref_3);
        meta_ref_3.uid = uid_2;
        ASSERT_EQ(uid_0, metadata.push(meta_ref_1));
        ASSERT_EQ(uid_1, metadata.push(meta_ref_2));
        ASSERT_EQ(uid_2, metadata.push(meta_ref_3));
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
        test_metadata meta;
        s1b::foffset_t offset;

        MAKE_TEST_OBJ(metadata)(s1b_file_name, false);
        MOVE_TEST_OBJ(metadata);
        ASSERT_FALSE(metadata.can_write());
        ASSERT_TRUE(metadata.read(uid_0, meta, offset));
        ASSERT_TRUE(meta_ref_1 == meta);
        ASSERT_TRUE(metadata.read(uid_1, meta, offset));
        ASSERT_TRUE(meta_ref_2 == meta);
        ASSERT_TRUE(metadata.read(uid_2, meta));
        ASSERT_TRUE(meta_ref_3 == meta);
        ASSERT_FALSE(metadata.read(uid_3, meta));
        ASSERT_FALSE(metadata.read(uid_3, meta, offset));
        const test_rwp_metadata& cmetadata = metadata;
        ASSERT_TRUE(cmetadata.read(uid_0, meta, offset));
        ASSERT_TRUE(meta_ref_1 == meta);
        ASSERT_TRUE(cmetadata.read(uid_1, meta, offset));
        ASSERT_TRUE(meta_ref_2 == meta);
        ASSERT_TRUE(cmetadata.read(uid_2, meta));
        ASSERT_TRUE(meta_ref_3 == meta);
        ASSERT_FALSE(cmetadata.read(uid_3, meta));
        ASSERT_FALSE(cmetadata.read(uid_3, meta, offset));
        ASSERT_NO_THROW(metadata.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(ReadOnWriteableFile)

    static const s1b::uid_t uid_0 = s1b::FirstUID + 0;
    static const s1b::uid_t uid_1 = s1b::FirstUID + 1;
    static const s1b::uid_t uid_2 = s1b::FirstUID + 2;
    static const s1b::uid_t uid_3 = s1b::FirstUID + 3;

    test_metadata meta_ref_1;
    test_metadata meta_ref_2;
    test_metadata meta_ref_3;

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    try
    {
        test_metadata meta;
        s1b::foffset_t offset;

        MAKE_TEST_OBJ(metadata)(s1b_file_name, true);
        MOVE_TEST_OBJ(metadata);
        initialize_metadata::small_1(meta_ref_1);
        meta_ref_1.uid = uid_0;
        initialize_metadata::small_2(meta_ref_2);
        meta_ref_2.uid = uid_1;
        initialize_metadata::small_3(meta_ref_3);
        meta_ref_3.uid = uid_2;
        ASSERT_EQ(uid_0, metadata.push(meta_ref_1));
        ASSERT_EQ(uid_1, metadata.push(meta_ref_2));
        ASSERT_EQ(uid_2, metadata.push(meta_ref_3));
        ASSERT_TRUE(metadata.can_write());
        ASSERT_TRUE(metadata.read(uid_0, meta, offset));
        ASSERT_TRUE(meta_ref_1 == meta);
        ASSERT_TRUE(metadata.read(uid_1, meta, offset));
        ASSERT_TRUE(meta_ref_2 == meta);
        ASSERT_TRUE(metadata.read(uid_2, meta));
        ASSERT_TRUE(meta_ref_3 == meta);
        ASSERT_FALSE(metadata.read(uid_3, meta));
        ASSERT_FALSE(metadata.read(uid_3, meta, offset));
        const test_rwp_metadata& cmetadata = metadata;
        ASSERT_TRUE(cmetadata.read(uid_0, meta, offset));
        ASSERT_TRUE(meta_ref_1 == meta);
        ASSERT_TRUE(cmetadata.read(uid_1, meta, offset));
        ASSERT_TRUE(meta_ref_2 == meta);
        ASSERT_TRUE(cmetadata.read(uid_2, meta));
        ASSERT_TRUE(meta_ref_3 == meta);
        ASSERT_FALSE(cmetadata.read(uid_3, meta));
        ASSERT_FALSE(cmetadata.read(uid_3, meta, offset));
        ASSERT_NO_THROW(metadata.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(ReadOnNewFile)

    static const s1b::uid_t uid_0 = s1b::FirstUID + 0;
    static const s1b::uid_t uid_1 = s1b::FirstUID + 1;
    static const s1b::uid_t uid_2 = s1b::FirstUID + 2;
    static const s1b::uid_t uid_3 = s1b::FirstUID + 3;

    test_metadata meta_ref_1;
    test_metadata meta_ref_2;
    test_metadata meta_ref_3;

    try
    {
        test_metadata meta;
        s1b::foffset_t offset;

        MAKE_TEST_OBJ(metadata)(s1b_file_name);
        MOVE_TEST_OBJ(metadata);
        initialize_metadata::small_1(meta_ref_1);
        meta_ref_1.uid = uid_0;
        initialize_metadata::small_2(meta_ref_2);
        meta_ref_2.uid = uid_1;
        initialize_metadata::small_3(meta_ref_3);
        meta_ref_3.uid = uid_2;
        ASSERT_EQ(uid_0, metadata.push(meta_ref_1));
        ASSERT_EQ(uid_1, metadata.push(meta_ref_2));
        ASSERT_EQ(uid_2, metadata.push(meta_ref_3));
        ASSERT_TRUE(metadata.can_write());
        ASSERT_TRUE(metadata.read(uid_0, meta, offset));
        ASSERT_TRUE(meta_ref_1 == meta);
        ASSERT_TRUE(metadata.read(uid_1, meta, offset));
        ASSERT_TRUE(meta_ref_2 == meta);
        ASSERT_TRUE(metadata.read(uid_2, meta));
        ASSERT_TRUE(meta_ref_3 == meta);
        ASSERT_FALSE(metadata.read(uid_3, meta));
        ASSERT_FALSE(metadata.read(uid_3, meta, offset));
        const test_rwp_metadata& cmetadata = metadata;
        ASSERT_TRUE(cmetadata.read(uid_0, meta, offset));
        ASSERT_TRUE(meta_ref_1 == meta);
        ASSERT_TRUE(cmetadata.read(uid_1, meta, offset));
        ASSERT_TRUE(meta_ref_2 == meta);
        ASSERT_TRUE(cmetadata.read(uid_2, meta));
        ASSERT_TRUE(meta_ref_3 == meta);
        ASSERT_FALSE(cmetadata.read(uid_3, meta));
        ASSERT_FALSE(cmetadata.read(uid_3, meta, offset));
        ASSERT_NO_THROW(metadata.sync());
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

    static const s1b::uid_t uid_0 = s1b::FirstUID + 0;
    static const s1b::uid_t uid_1 = s1b::FirstUID + 1;
    static const s1b::uid_t uid_2 = s1b::FirstUID + 2;
    static const s1b::uid_t uid_3 = s1b::FirstUID + 3;

    test_metadata meta_ref_1;
    test_metadata meta_ref_2;
    test_metadata meta_ref_3;
    test_metadata meta_ref_4;

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name);
        MOVE_TEST_OBJ(metadata);
        initialize_metadata::small_1(meta_ref_1);
        meta_ref_1.uid = uid_0;
        initialize_metadata::small_2(meta_ref_2);
        meta_ref_2.uid = uid_1;
        initialize_metadata::small_3(meta_ref_3);
        meta_ref_3.uid = uid_2;
        initialize_metadata::small_1(meta_ref_4);
        meta_ref_4.uid = uid_3;
        ASSERT_EQ(uid_0, metadata.push(meta_ref_1));
        ASSERT_EQ(uid_1, metadata.push(meta_ref_2));
        ASSERT_EQ(uid_2, metadata.push(meta_ref_3));
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
        MAKE_TEST_OBJ(metadata)(s1b_file_name, false);
        MOVE_TEST_OBJ(metadata);
        ASSERT_FALSE(metadata.can_write());
        ASSERT_THROW(metadata.write(meta_ref_1),
            s1b::read_only_exception);
        ASSERT_THROW(metadata.write(meta_ref_2),
            s1b::read_only_exception);
        ASSERT_THROW(metadata.write(meta_ref_3),
            s1b::read_only_exception);
        ASSERT_THROW(metadata.write(meta_ref_4),
            s1b::io_exception);
        ASSERT_NO_THROW(metadata.sync());
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

    static const s1b::uid_t uid_0 = s1b::FirstUID + 0;
    static const s1b::uid_t uid_1 = s1b::FirstUID + 1;
    static const s1b::uid_t uid_2 = s1b::FirstUID + 2;
    static const s1b::uid_t uid_3 = s1b::FirstUID + 3;

    test_metadata meta_ref_1;
    test_metadata meta_ref_2;
    test_metadata meta_ref_3;
    test_metadata meta_ref_4;

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name, true);
        MOVE_TEST_OBJ(metadata);
        initialize_metadata::small_1(meta_ref_1);
        meta_ref_1.uid = uid_0;
        initialize_metadata::small_2(meta_ref_2);
        meta_ref_2.uid = uid_1;
        initialize_metadata::small_3(meta_ref_3);
        meta_ref_3.uid = uid_2;
        ASSERT_EQ(uid_0, metadata.push(meta_ref_1));
        ASSERT_EQ(uid_1, metadata.push(meta_ref_2));
        ASSERT_EQ(uid_2, metadata.push(meta_ref_3));
        ASSERT_TRUE(metadata.can_write());
        meta_ref_1.x *= 2;
        meta_ref_1.y *= 2;
        meta_ref_1.value1 *= 2;
        meta_ref_1.value2 *= 2;
        ASSERT_NO_THROW(metadata.write(meta_ref_1));
        meta_ref_2.x *= 2;
        meta_ref_2.y *= 2;
        meta_ref_2.value1 *= 2;
        meta_ref_2.value2 *= 2;
        ASSERT_NO_THROW(metadata.write(meta_ref_2));
        meta_ref_3.x *= 2;
        meta_ref_3.y *= 2;
        meta_ref_3.value1 *= 2;
        meta_ref_3.value2 *= 2;
        ASSERT_NO_THROW(metadata.write(meta_ref_3));
        initialize_metadata::small_1(meta_ref_4);
        meta_ref_4.uid = uid_3;
        ASSERT_THROW(metadata.write(meta_ref_4),
            s1b::io_exception);
        meta_ref_1.size *= 2;
        ASSERT_THROW(metadata.write(meta_ref_1),
            s1b::element_mismatch_exception);
        meta_ref_2.size *= 2;
        ASSERT_THROW(metadata.write(meta_ref_2),
            s1b::element_mismatch_exception);
        meta_ref_3.size *= 2;
        ASSERT_THROW(metadata.write(meta_ref_3),
            s1b::element_mismatch_exception);
        ASSERT_NO_THROW(metadata.sync());
        const test_rwp_metadata& cmetadata = metadata;
        ASSERT_TRUE(cmetadata.read(uid_0, meta_ref_1));
        ASSERT_TRUE(cmetadata.read(uid_1, meta_ref_2));
        ASSERT_TRUE(cmetadata.read(uid_2, meta_ref_3));
        ASSERT_FALSE(cmetadata.read(uid_3, meta_ref_4));
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

    static const s1b::uid_t uid_0 = s1b::FirstUID + 0;
    static const s1b::uid_t uid_1 = s1b::FirstUID + 1;
    static const s1b::uid_t uid_2 = s1b::FirstUID + 2;
    static const s1b::uid_t uid_3 = s1b::FirstUID + 3;

    test_metadata meta_ref_1;
    test_metadata meta_ref_2;
    test_metadata meta_ref_3;
    test_metadata meta_ref_4;

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name);
        MOVE_TEST_OBJ(metadata);
        initialize_metadata::small_1(meta_ref_1);
        meta_ref_1.uid = uid_0;
        initialize_metadata::small_2(meta_ref_2);
        meta_ref_2.uid = uid_1;
        initialize_metadata::small_3(meta_ref_3);
        meta_ref_3.uid = uid_2;
        ASSERT_EQ(uid_0, metadata.push(meta_ref_1));
        ASSERT_EQ(uid_1, metadata.push(meta_ref_2));
        ASSERT_EQ(uid_2, metadata.push(meta_ref_3));
        ASSERT_TRUE(metadata.can_write());
        meta_ref_1.x *= 2;
        meta_ref_1.y *= 2;
        meta_ref_1.value1 *= 2;
        meta_ref_1.value2 *= 2;
        ASSERT_NO_THROW(metadata.write(meta_ref_1));
        meta_ref_2.x *= 2;
        meta_ref_2.y *= 2;
        meta_ref_2.value1 *= 2;
        meta_ref_2.value2 *= 2;
        ASSERT_NO_THROW(metadata.write(meta_ref_2));
        meta_ref_3.x *= 2;
        meta_ref_3.y *= 2;
        meta_ref_3.value1 *= 2;
        meta_ref_3.value2 *= 2;
        ASSERT_NO_THROW(metadata.write(meta_ref_3));
        initialize_metadata::small_1(meta_ref_4);
        meta_ref_4.uid = uid_3;
        ASSERT_THROW(metadata.write(meta_ref_4),
            s1b::io_exception);
        meta_ref_1.size *= 2;
        ASSERT_THROW(metadata.write(meta_ref_1),
            s1b::element_mismatch_exception);
        meta_ref_2.size *= 2;
        ASSERT_THROW(metadata.write(meta_ref_2),
            s1b::element_mismatch_exception);
        meta_ref_3.size *= 2;
        ASSERT_THROW(metadata.write(meta_ref_3),
            s1b::element_mismatch_exception);
        ASSERT_NO_THROW(metadata.sync());
        const test_rwp_metadata& cmetadata = metadata;
        ASSERT_TRUE(cmetadata.read(uid_0, meta_ref_1));
        ASSERT_TRUE(cmetadata.read(uid_1, meta_ref_2));
        ASSERT_TRUE(cmetadata.read(uid_2, meta_ref_3));
        ASSERT_FALSE(cmetadata.read(uid_3, meta_ref_4));
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(FromIteratorBadUids)

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta.uid += 500;
        meta_vector.push_back(meta);
    }

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name,
            meta_vector.begin(), meta_vector.end());
        MOVE_TEST_OBJ(metadata);
        ASSERT_TRUE(metadata.can_write());
        for (int i = 1; i <= 1000; i++)
        {
            test_metadata meta;
            test_metadata meta_ref = meta_vector[i-1];
            meta_ref.uid = i;
            ASSERT_TRUE(metadata.read(i, meta));
            ASSERT_TRUE(meta_ref == meta);
        }
        ASSERT_NO_THROW(metadata.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(FromIterator)

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name,
            meta_vector.begin(), meta_vector.end());
        MOVE_TEST_OBJ(metadata);
        ASSERT_TRUE(metadata.can_write());
        for (int i = 1; i <= 1000; i++)
        {
            test_metadata meta;
            test_metadata meta_ref = meta_vector[i-1];
            ASSERT_TRUE(metadata.read(i, meta));
            ASSERT_TRUE(meta_ref == meta);
        }
        ASSERT_NO_THROW(metadata.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(UidIteratorOnReadOnlyFile)

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end()));

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name, false);
        MOVE_TEST_OBJ(metadata);
        ASSERT_FALSE(metadata.can_write());
        s1b::iterators::uid_iterator uid_begin = metadata.uid_begin();
        s1b::iterators::uid_iterator uid_end = metadata.uid_end();
        for (int i = 1; i <= 1000; i++, uid_begin++)
        {
            ASSERT_TRUE(uid_begin != uid_end);
            ASSERT_EQ(i, *uid_begin);
        }
        ASSERT_TRUE(uid_begin == uid_end);
        ASSERT_NO_THROW(metadata.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(UidIteratorOnWriteableFile)

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end()));

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name, true);
        MOVE_TEST_OBJ(metadata);
        ASSERT_TRUE(metadata.can_write());
        s1b::iterators::uid_iterator uid_begin = metadata.uid_begin();
        s1b::iterators::uid_iterator uid_end = metadata.uid_end();
        for (int i = 1; i <= 1000; i++, uid_begin++)
        {
            ASSERT_TRUE(uid_begin != uid_end);
            ASSERT_EQ(i, *uid_begin);
        }
        ASSERT_TRUE(uid_begin == uid_end);
        ASSERT_NO_THROW(metadata.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(UidIteratorOnNewFile)

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name,
            meta_vector.begin(), meta_vector.end());
        MOVE_TEST_OBJ(metadata);
        ASSERT_TRUE(metadata.can_write());
        s1b::iterators::uid_iterator uid_begin = metadata.uid_begin();
        s1b::iterators::uid_iterator uid_end = metadata.uid_end();
        for (int i = 1; i <= 1000; i++, uid_begin++)
        {
            ASSERT_TRUE(uid_begin != uid_end);
            ASSERT_EQ(i, *uid_begin);
        }
        ASSERT_TRUE(uid_begin == uid_end);
        ASSERT_NO_THROW(metadata.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(MixedRWPOperations)

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 20; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        {
            test_metadata meta_ref;
            test_metadata meta;
            MAKE_TEST_OBJ(metadata)(s1b_file_name,
                meta_vector.begin(), meta_vector.end());
            MOVE_TEST_OBJ(metadata);
            ASSERT_TRUE(metadata.can_write());
            for (unsigned int i = 21; i <= 123; i++)
            {
                initialize_metadata::small_i(meta, i);
                ASSERT_EQ(i, metadata.push(meta));
            }
            for (unsigned int i = s1b::FirstUID; i <= 101; i += 3)
            {
                initialize_metadata::small_i(meta_ref, i);
                ASSERT_TRUE(metadata.read(i, meta));
                ASSERT_TRUE(meta_ref == meta);
                meta.size *= 2;
                meta.x *= 2;
                meta.y *= 2;
                meta.value1 *= 2;
                meta.value2 *= 2;
                ASSERT_THROW(metadata.write(meta),
                    s1b::element_mismatch_exception);
                meta.size = meta_ref.size;
                ASSERT_NO_THROW(metadata.write(meta));
            }
            for (unsigned int i = 124; i <= 200; i++)
            {
                initialize_metadata::small_i(meta_ref, i);
                ASSERT_EQ(i, metadata.push(meta_ref));
                ASSERT_TRUE(metadata.read(i, meta));
                ASSERT_TRUE(meta_ref == meta);
            }
            ASSERT_NO_THROW(metadata.sync());
        }

        {
            test_metadata meta_ref;
            test_metadata meta;
            MAKE_TEST_OBJ(metadata)(s1b_file_name, true);
            MOVE_TEST_OBJ(metadata);
            ASSERT_TRUE(metadata.can_write());
            for (unsigned int i = s1b::FirstUID; i <= 101; i++)
            {
                initialize_metadata::small_i(meta_ref, i);
                ASSERT_TRUE(metadata.read(i, meta));
                if ((i - 1) % 3 == 0)
                {
                    meta_ref.x *= 2;
                    meta_ref.y *= 2;
                    meta_ref.value1 *= 2;
                    meta_ref.value2 *= 2;
                    ASSERT_TRUE(meta_ref == meta);
                }
                else
                {
                    ASSERT_TRUE(meta_ref == meta);
                    meta.size *= 2;
                    meta.x *= 2;
                    meta.y *= 2;
                    meta.value1 *= 2;
                    meta.value2 *= 2;
                    ASSERT_THROW(metadata.write(meta),
                        s1b::element_mismatch_exception);
                }
                meta.size = meta_ref.size;
                ASSERT_NO_THROW(metadata.write(meta));
            }
            for (unsigned int i = 124; i <= 200; i++)
            {
                initialize_metadata::small_i(meta_ref, i);
                ASSERT_TRUE(metadata.read(i, meta));
                ASSERT_TRUE(meta_ref == meta);
            }
            for (unsigned int i = 201; i <= 203; i++)
            {
                initialize_metadata::small_i(meta_ref, i);
                ASSERT_EQ(i, metadata.push(meta_ref));
            }
            ASSERT_NO_THROW(metadata.sync());
        }

        {
            test_metadata meta_ref;
            test_metadata meta;
            MAKE_TEST_OBJ(metadata)(s1b_file_name, false);
            MOVE_TEST_OBJ(metadata);
            ASSERT_FALSE(metadata.can_write());
            for (unsigned int i = s1b::FirstUID; i <= 101; i++)
            {
                initialize_metadata::small_i(meta_ref, i);
                ASSERT_TRUE(metadata.read(i, meta));
                meta_ref.x *= 2;
                meta_ref.y *= 2;
                meta_ref.value1 *= 2;
                meta_ref.value2 *= 2;
                ASSERT_TRUE(meta_ref == meta);
            }
            for (unsigned int i = 124; i <= 203; i++)
            {
                initialize_metadata::small_i(meta_ref, i);
                ASSERT_TRUE(metadata.read(i, meta));
                ASSERT_TRUE(meta_ref == meta);
            }
            ASSERT_NO_THROW(metadata.sync());
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

S1B_TEST(UuidIsUniqueAndValid)

    boost::uuids::uuid uuid1, uuid2, uuid3, uuid4;
    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 10; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name);
        MOVE_TEST_OBJ(metadata);
        ASSERT_NO_THROW(uuid1 = metadata.file_uuid());
        ASSERT_FALSE(uuid1.is_nil());
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
        MAKE_TEST_OBJ(metadata)(s1b_file_name);
        MOVE_TEST_OBJ(metadata);
        ASSERT_NO_THROW(uuid2 = metadata.file_uuid());
        ASSERT_FALSE(uuid2.is_nil());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }

    ASSERT_NE(uuid1, uuid2);

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name,
            meta_vector.begin(), meta_vector.end());
        MOVE_TEST_OBJ(metadata);
        ASSERT_NO_THROW(uuid3 = metadata.file_uuid());
        ASSERT_FALSE(uuid3.is_nil());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }

    ASSERT_NE(uuid1, uuid3);
    ASSERT_NE(uuid2, uuid3);

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name, false);
        MOVE_TEST_OBJ(metadata);
        ASSERT_NO_THROW(uuid4 = metadata.file_uuid());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }

    ASSERT_EQ(uuid3, uuid4);

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name, true);
        MOVE_TEST_OBJ(metadata);
        ASSERT_NO_THROW(uuid4 = metadata.file_uuid());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }

    ASSERT_EQ(uuid3, uuid4);
}

void _MappedCompatCreateNew(const char* filename, bool can_write)
{
    std::vector<test_metadata> meta_vector;
    std::vector<s1b::foffset_t> off_vector;
    boost::uuids::uuid uuid;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        test_mapped_metadata metadata(filename, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);
        ASSERT_NO_THROW(uuid = metadata.file_uuid());
        for (int i = 1; i <= 1000; i++)
        {
            ASSERT_NO_THROW(off_vector.push_back(
                metadata.get_data_offset(i)));
        }
        ASSERT_NO_THROW(metadata.sync());
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
        test_metadata meta;
        s1b::foffset_t offset;
        MAKE_TEST_OBJ(metadata)(filename, can_write);
        MOVE_TEST_OBJ(metadata);
        ASSERT_EQ(can_write, metadata.can_write());
        ASSERT_EQ(uuid, metadata.file_uuid());
        for (int i = 1; i <= 1000; i++)
        {
            ASSERT_TRUE(metadata.read(i, meta));
            ASSERT_TRUE(meta_vector[i-1] == meta);
            ASSERT_TRUE(metadata.read(i, meta, offset));
            ASSERT_TRUE(meta_vector[i-1] == meta);
            ASSERT_TRUE(off_vector[i-1] == offset);
            ASSERT_TRUE(metadata.read_data_offset(i, offset));
            ASSERT_TRUE(off_vector[i-1] == offset);
            ASSERT_NO_THROW(offset = metadata.read_data_offset(meta));
            ASSERT_TRUE(off_vector[i-1] == offset);
        }
        ASSERT_NO_THROW(metadata.sync());
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

void _PushCompatCreateNew(const char* filename, bool can_write)
{
    std::vector<test_metadata> meta_vector;
    boost::uuids::uuid uuid;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        test_global_data glob;
        test_push_metadata metadata(filename, meta_vector.begin(),
            meta_vector.end(), glob);
        ASSERT_NO_THROW(uuid = metadata.file_uuid());
        for (int i = 1001; i <= 1010; i++)
        {
            test_metadata meta;
            initialize_metadata::small_i(meta, i);
            meta_vector.push_back(meta);
            metadata.push(meta);
        }
        ASSERT_NO_THROW(metadata.align());
        ASSERT_NO_THROW(metadata.sync());
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
        test_metadata meta;
        s1b::foffset_t offset;
        MAKE_TEST_OBJ(metadata)(filename, can_write);
        MOVE_TEST_OBJ(metadata);
        ASSERT_EQ(can_write, metadata.can_write());
        ASSERT_EQ(uuid, metadata.file_uuid());
        for (int i = 1; i <= 500; i++)
        {
            ASSERT_TRUE(metadata.read(i, meta));
            ASSERT_TRUE(meta_vector[i-1] == meta);
            ASSERT_TRUE(metadata.read(i, meta, offset));
            ASSERT_TRUE(meta_vector[i-1] == meta);
            ASSERT_TRUE(metadata.read_data_offset(i, offset));
            ASSERT_NO_THROW(offset = metadata.read_data_offset(meta));
        }
        if (can_write)
        {
            for (int i = 1011; i <= 1020; i++)
            {
                test_metadata meta;
                initialize_metadata::small_i(meta, i);
                meta_vector.push_back(meta);
                metadata.push(meta);
            }
        }
        for (int i = 501; i <= 1020; i++)
        {
            if (static_cast<unsigned int>(i) > meta_vector.size())
                continue;
            ASSERT_TRUE(metadata.read(i, meta));
            ASSERT_TRUE(meta_vector[i-1] == meta);
            ASSERT_TRUE(metadata.read(i, meta, offset));
            ASSERT_TRUE(meta_vector[i-1] == meta);
            ASSERT_TRUE(metadata.read_data_offset(i, offset));
            ASSERT_NO_THROW(offset = metadata.read_data_offset(meta));
        }
        ASSERT_NO_THROW(metadata.sync());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(PushCompatCreateNewAndOpenReadOnly)

    _PushCompatCreateNew(s1b_file_name, false);
}

S1B_TEST(PushCompatCreateNewAndOpenWrite)

    _PushCompatCreateNew(s1b_file_name, true);
}

S1B_TEST(ParallelIOTest)

    const int N = 10000;

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= N; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        MAKE_TEST_OBJ(metadata)(s1b_file_name,
            meta_vector.begin(), meta_vector.end());
        MOVE_TEST_OBJ(metadata);
        ASSERT_TRUE(metadata.can_write());

        int num_threads = 0;

        #pragma omp parallel for schedule(static) num_threads(100)
        for (size_t i = 1; i <= N; i++)
        {
            #pragma omp critical
            {
                num_threads = omp_get_num_threads();
            }

            for (size_t j = 0; j < 100; j++)
            {
                test_metadata meta;
                metadata.read(i, meta);
                meta.x++;
                meta.y++;
                meta.value1++;
                meta.value2++;
                metadata.write(meta);
            }
        }

        ASSERT_EQ(100, num_threads);
        ASSERT_NO_THROW(metadata.sync());

        std::vector<test_metadata> result(N);

        #pragma omp parallel for schedule(static) num_threads(100)
        for (size_t i = 0; i < N; i++)
        {
            #pragma omp critical
            {
                num_threads = omp_get_num_threads();
            }

            for (size_t j = 0; j < 200; j++)
            {
                metadata.read(i+1, result[i]);
            }
        }

        ASSERT_EQ(100, num_threads);

        for (size_t i = 0; i < N; i++)
        {
            for (size_t j = 0; j < 100; j++)
            {
                meta_vector[i].x++;
                meta_vector[i].y++;
                meta_vector[i].value1++;
                meta_vector[i].value2++;
            }

            ASSERT_EQ(meta_vector[i].x     , result[i].x     );
            ASSERT_EQ(meta_vector[i].y     , result[i].y     );
            ASSERT_EQ(meta_vector[i].value1, result[i].value1);
            ASSERT_EQ(meta_vector[i].value2, result[i].value2);
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
