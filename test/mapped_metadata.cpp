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

#define S1B_TEST_NAME MappedMetadata

#include "s1b_test.hpp"
#include "metadata_test.hpp"

#include <s1b/rwp_metadata.hpp>
#include <s1b/push_metadata.hpp>
#include <s1b/mapped_metadata.hpp>

#include <vector>

namespace {

typedef s1b::rwp_metadata<test_adapter> test_rwp_metadata;
typedef s1b::push_metadata<test_adapter> test_push_metadata;
typedef s1b::mapped_metadata<test_adapter> test_mapped_metadata;

// TODO unlink

S1B_TEST(OpenReadNonExisting)

    ASSERT_THROW(test_mapped_metadata metadata(s1b_file_name, false,
        s1b::S1B_HUGETLB_OFF), s1b::io_exception);
}

S1B_TEST(OpenWriteNonExisting)

    ASSERT_THROW(test_mapped_metadata metadata(s1b_file_name, true,
        s1b::S1B_HUGETLB_OFF), s1b::io_exception);
}

S1B_TEST(FromIteratorNonExisting)

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 10; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    ASSERT_NO_THROW(test_mapped_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end(), s1b::S1B_HUGETLB_OFF));
}

S1B_TEST(OpenReadExistingEmpty)

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    ASSERT_NO_THROW(test_mapped_metadata metadata(s1b_file_name, false,
        s1b::S1B_HUGETLB_OFF));
}

S1B_TEST(OpenWriteExistingEmpty)

    ASSERT_NO_THROW(test_rwp_metadata metadata(s1b_file_name));

    ASSERT_NO_THROW(test_mapped_metadata metadata(s1b_file_name, true,
        s1b::S1B_HUGETLB_OFF));
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

    ASSERT_NO_THROW(test_mapped_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end(), s1b::S1B_HUGETLB_OFF));
}

S1B_TEST(GetSize)

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 10; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        test_mapped_metadata metadata(s1b_file_name, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);
        ASSERT_NO_THROW(metadata.size());
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
        test_mapped_metadata metadata(s1b_file_name, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);

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
        test_mapped_metadata metadata(s1b_file_name, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);

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

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 10; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        test_mapped_metadata metadata(s1b_file_name, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);
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

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 10; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        test_mapped_metadata metadata(s1b_file_name, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);
        const test_mapped_metadata& cmetadata = metadata;
        ASSERT_NO_THROW(metadata.global_struct());
        ASSERT_NO_THROW(cmetadata.global_struct());
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

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 10; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    ASSERT_NO_THROW(test_mapped_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end(), s1b::S1B_HUGETLB_OFF));

    try
    {
        test_mapped_metadata metadata(s1b_file_name, true,
            s1b::S1B_HUGETLB_OFF);
        ASSERT_TRUE(metadata.can_write());
        ASSERT_NO_THROW(metadata.global_struct().test = 50);
        ASSERT_EQ(50, metadata.global_struct().test);
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
        test_mapped_metadata metadata(s1b_file_name, false);
        test_global_data data;
        ASSERT_FALSE(metadata.can_write());
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

S1B_TEST(UpdateGlobalStructFromIterator)

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 10; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        test_mapped_metadata metadata(s1b_file_name, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);
        ASSERT_TRUE(metadata.can_write());
        ASSERT_NO_THROW(metadata.global_struct().test = 50);
        ASSERT_EQ(50, metadata.global_struct().test);
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
        test_mapped_metadata metadata(s1b_file_name, false,
            s1b::S1B_HUGETLB_OFF);
        test_global_data data;
        ASSERT_FALSE(metadata.can_write());
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

S1B_TEST(GetLastUID)

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 10; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        test_mapped_metadata metadata(s1b_file_name, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);
        ASSERT_EQ(10, metadata.get_last_uid());
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

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 10; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        test_mapped_metadata metadata(s1b_file_name, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);
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

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 10; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    ASSERT_NO_THROW(test_mapped_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end(), s1b::S1B_HUGETLB_OFF));

    for (int i = 0; i < 50000; i++)
    {
        try
        {
            test_mapped_metadata metadata(s1b_file_name, false,
                s1b::S1B_HUGETLB_OFF);
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
        s1b::foffset_t offset;
        test_metadata meta;
        meta.uid = s1b::FirstUID;

        test_mapped_metadata metadata(s1b_file_name, false,
            s1b::S1B_HUGETLB_OFF);
        const test_mapped_metadata& cmetadata = metadata;
        ASSERT_FALSE(metadata.can_write());
        ASSERT_THROW(metadata.get_element(s1b::FirstUID),
            s1b::invalid_uid_exception);
        ASSERT_THROW(metadata.get_element(s1b::FirstUID, offset),
            s1b::invalid_uid_exception);
        ASSERT_THROW(metadata.get_data_offset(s1b::FirstUID),
            s1b::invalid_uid_exception);
        ASSERT_THROW(metadata.get_data_offset(meta),
            s1b::invalid_uid_exception);
        ASSERT_THROW(cmetadata.get_element(s1b::FirstUID),
            s1b::invalid_uid_exception);
        ASSERT_THROW(cmetadata.get_element(s1b::FirstUID, offset),
            s1b::invalid_uid_exception);
        ASSERT_THROW(cmetadata.get_data_offset(s1b::FirstUID),
            s1b::invalid_uid_exception);
        ASSERT_THROW(cmetadata.get_data_offset(meta),
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

S1B_TEST(ReadOnReadOnlyFile)

    static const s1b::uid_t uid_0 = s1b::FirstUID + 0;
    static const s1b::uid_t uid_1 = s1b::FirstUID + 1;
    static const s1b::uid_t uid_2 = s1b::FirstUID + 2;
    static const s1b::uid_t uid_3 = s1b::FirstUID + 3;

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 3; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    ASSERT_NO_THROW(test_mapped_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end(), s1b::S1B_HUGETLB_OFF));

    try
    {
        test_metadata meta;
        s1b::foffset_t offset;

        test_mapped_metadata metadata(s1b_file_name, false,
            s1b::S1B_HUGETLB_OFF);
        const test_mapped_metadata& cmetadata = metadata;
        ASSERT_FALSE(metadata.can_write());
        ASSERT_NO_THROW(meta = metadata.get_element(uid_0, offset));
        ASSERT_TRUE(meta_vector[0] == meta);
        ASSERT_NO_THROW(meta = metadata.get_element(uid_1, offset));
        ASSERT_TRUE(meta_vector[1] == meta);
        ASSERT_NO_THROW(meta = metadata.get_element(uid_2));
        ASSERT_TRUE(meta_vector[2] == meta);
        ASSERT_THROW(meta = metadata.get_element(uid_3),
            s1b::invalid_uid_exception);
        ASSERT_THROW(meta = metadata.get_element(uid_3, offset),
            s1b::invalid_uid_exception);
        ASSERT_NO_THROW(meta = cmetadata.get_element(uid_0, offset));
        ASSERT_TRUE(meta_vector[0] == meta);
        ASSERT_NO_THROW(meta = cmetadata.get_element(uid_1, offset));
        ASSERT_TRUE(meta_vector[1] == meta);
        ASSERT_NO_THROW(meta = cmetadata.get_element(uid_2));
        ASSERT_TRUE(meta_vector[2] == meta);
        ASSERT_THROW(meta = cmetadata.get_element(uid_3),
            s1b::invalid_uid_exception);
        ASSERT_THROW(meta = cmetadata.get_element(uid_3, offset),
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

S1B_TEST(ReadOnWriteableFile)

    static const s1b::uid_t uid_0 = s1b::FirstUID + 0;
    static const s1b::uid_t uid_1 = s1b::FirstUID + 1;
    static const s1b::uid_t uid_2 = s1b::FirstUID + 2;
    static const s1b::uid_t uid_3 = s1b::FirstUID + 3;

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 3; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    ASSERT_NO_THROW(test_mapped_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end(), s1b::S1B_HUGETLB_OFF));

    try
    {
        test_metadata meta;
        s1b::foffset_t offset;

        test_mapped_metadata metadata(s1b_file_name, true,
            s1b::S1B_HUGETLB_OFF);
        const test_mapped_metadata& cmetadata = metadata;
        ASSERT_TRUE(metadata.can_write());
        ASSERT_NO_THROW(meta = metadata.get_element(uid_0, offset));
        ASSERT_TRUE(meta_vector[0] == meta);
        ASSERT_NO_THROW(meta = metadata.get_element(uid_1, offset));
        ASSERT_TRUE(meta_vector[1] == meta);
        ASSERT_NO_THROW(meta = metadata.get_element(uid_2));
        ASSERT_TRUE(meta_vector[2] == meta);
        ASSERT_THROW(meta = metadata.get_element(uid_3),
            s1b::invalid_uid_exception);
        ASSERT_THROW(meta = metadata.get_element(uid_3, offset),
            s1b::invalid_uid_exception);
        ASSERT_NO_THROW(meta = cmetadata.get_element(uid_0, offset));
        ASSERT_TRUE(meta_vector[0] == meta);
        ASSERT_NO_THROW(meta = cmetadata.get_element(uid_1, offset));
        ASSERT_TRUE(meta_vector[1] == meta);
        ASSERT_NO_THROW(meta = cmetadata.get_element(uid_2));
        ASSERT_TRUE(meta_vector[2] == meta);
        ASSERT_THROW(meta = cmetadata.get_element(uid_3),
            s1b::invalid_uid_exception);
        ASSERT_THROW(meta = cmetadata.get_element(uid_3, offset),
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

S1B_TEST(ReadOnNewFile)

    static const s1b::uid_t uid_0 = s1b::FirstUID + 0;
    static const s1b::uid_t uid_1 = s1b::FirstUID + 1;
    static const s1b::uid_t uid_2 = s1b::FirstUID + 2;
    static const s1b::uid_t uid_3 = s1b::FirstUID + 3;

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 3; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        test_metadata meta;
        s1b::foffset_t offset;

        test_mapped_metadata metadata(s1b_file_name, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);
        const test_mapped_metadata& cmetadata = metadata;
        ASSERT_TRUE(metadata.can_write());
        ASSERT_NO_THROW(meta = cmetadata.get_element(uid_0, offset));
        ASSERT_TRUE(meta_vector[0] == meta);
        ASSERT_NO_THROW(meta = cmetadata.get_element(uid_1, offset));
        ASSERT_TRUE(meta_vector[1] == meta);
        ASSERT_NO_THROW(meta = cmetadata.get_element(uid_2));
        ASSERT_TRUE(meta_vector[2] == meta);
        ASSERT_THROW(meta = cmetadata.get_element(uid_3),
            s1b::invalid_uid_exception);
        ASSERT_THROW(meta = cmetadata.get_element(uid_3, offset),
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

S1B_TEST(WriteOnWriteableFile)

    static const s1b::uid_t uid_0 = s1b::FirstUID + 0;
    static const s1b::uid_t uid_1 = s1b::FirstUID + 1;
    static const s1b::uid_t uid_2 = s1b::FirstUID + 2;
    static const s1b::uid_t uid_3 = s1b::FirstUID + 3;

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 3; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta.uid = i;
        meta_vector.push_back(meta);
    }

    ASSERT_NO_THROW(test_mapped_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end(), s1b::S1B_HUGETLB_OFF));

    try
    {
        test_mapped_metadata metadata(s1b_file_name, true,
            s1b::S1B_HUGETLB_OFF);
        const test_mapped_metadata& cmetadata = metadata;
        ASSERT_EQ(meta_vector[0], cmetadata.get_element(uid_0));
        ASSERT_EQ(meta_vector[1], cmetadata.get_element(uid_1));
        ASSERT_EQ(meta_vector[2], cmetadata.get_element(uid_2));
        ASSERT_TRUE(metadata.can_write());
        meta_vector[0].x *= 2;
        meta_vector[0].y *= 2;
        meta_vector[0].value1 *= 2;
        meta_vector[0].value2 *= 2;
        ASSERT_NO_THROW(metadata.get_element(uid_0) = meta_vector[0]);
        meta_vector[1].x *= 2;
        meta_vector[1].y *= 2;
        meta_vector[1].value1 *= 2;
        meta_vector[1].value2 *= 2;
        ASSERT_NO_THROW(metadata.get_element(uid_1) = meta_vector[1]);
        meta_vector[2].x *= 2;
        meta_vector[2].y *= 2;
        meta_vector[2].value1 *= 2;
        meta_vector[2].value2 *= 2;
        ASSERT_NO_THROW(metadata.get_element(uid_2) = meta_vector[2]);
        ASSERT_THROW(metadata.get_element(uid_3) = meta_vector[0],
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

S1B_TEST(WriteOnNewFile)

    static const s1b::uid_t uid_0 = s1b::FirstUID + 0;
    static const s1b::uid_t uid_1 = s1b::FirstUID + 1;
    static const s1b::uid_t uid_2 = s1b::FirstUID + 2;
    static const s1b::uid_t uid_3 = s1b::FirstUID + 3;

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 3; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta.uid = i;
        meta_vector.push_back(meta);
    }

    try
    {
        test_mapped_metadata metadata(s1b_file_name, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);
        const test_mapped_metadata& cmetadata = metadata;
        ASSERT_EQ(meta_vector[0], cmetadata.get_element(uid_0));
        ASSERT_EQ(meta_vector[1], cmetadata.get_element(uid_1));
        ASSERT_EQ(meta_vector[2], cmetadata.get_element(uid_2));
        ASSERT_TRUE(metadata.can_write());
        meta_vector[0].x *= 2;
        meta_vector[0].y *= 2;
        meta_vector[0].value1 *= 2;
        meta_vector[0].value2 *= 2;
        ASSERT_NO_THROW(metadata.get_element(uid_0) = meta_vector[0]);
        meta_vector[1].x *= 2;
        meta_vector[1].y *= 2;
        meta_vector[1].value1 *= 2;
        meta_vector[1].value2 *= 2;
        ASSERT_NO_THROW(metadata.get_element(uid_1) = meta_vector[1]);
        meta_vector[2].x *= 2;
        meta_vector[2].y *= 2;
        meta_vector[2].value1 *= 2;
        meta_vector[2].value2 *= 2;
        ASSERT_NO_THROW(metadata.get_element(uid_2) = meta_vector[2]);
        ASSERT_THROW(metadata.get_element(uid_3) = meta_vector[0],
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
        test_mapped_metadata metadata(s1b_file_name, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);
        ASSERT_TRUE(metadata.can_write());
        for (int i = 1; i <= 1000; i++)
        {
            test_metadata meta;
            test_metadata meta_ref = meta_vector[i-1];
            meta_ref.uid = i;
            ASSERT_NO_THROW(meta = metadata.get_element(i));
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
        test_mapped_metadata metadata(s1b_file_name, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);
        ASSERT_TRUE(metadata.can_write());
        for (int i = 1; i <= 1000; i++)
        {
            test_metadata meta;
            test_metadata meta_ref = meta_vector[i-1];
            ASSERT_NO_THROW(meta = metadata.get_element(i));
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

    ASSERT_NO_THROW(test_mapped_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end(), s1b::S1B_HUGETLB_OFF));

    try
    {
        test_mapped_metadata metadata(s1b_file_name, false,
            s1b::S1B_HUGETLB_OFF);
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

    ASSERT_NO_THROW(test_mapped_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end(), s1b::S1B_HUGETLB_OFF));

    try
    {
        test_mapped_metadata metadata(s1b_file_name, true,
            s1b::S1B_HUGETLB_OFF);
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
        test_mapped_metadata metadata(s1b_file_name, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);
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

    for (int i = 1; i <= 200; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        {
            test_metadata meta_ref;
            test_metadata* p_meta;
            test_mapped_metadata metadata(s1b_file_name, meta_vector.begin(),
                meta_vector.end(), s1b::S1B_HUGETLB_OFF);
            ASSERT_TRUE(metadata.can_write());
            for (unsigned int i = s1b::FirstUID; i <= 101; i += 3)
            {
                initialize_metadata::small_i(meta_ref, i);
                ASSERT_NO_THROW(p_meta = &metadata.get_element(i));
                ASSERT_TRUE(meta_ref == *p_meta);
                p_meta->x *= 2;
                p_meta->y *= 2;
                p_meta->value1 *= 2;
                p_meta->value2 *= 2;
            }
            ASSERT_NO_THROW(metadata.sync());
        }

        {
            test_metadata meta_ref;
            test_metadata* p_meta;
            const test_metadata* cp_meta;
            test_mapped_metadata metadata(s1b_file_name, true,
                s1b::S1B_HUGETLB_OFF);
            ASSERT_TRUE(metadata.can_write());
            for (unsigned int i = s1b::FirstUID; i <= 101; i++)
            {
                initialize_metadata::small_i(meta_ref, i);
                if ((i - 1) % 3 == 0)
                {
                    ASSERT_NO_THROW(cp_meta = &metadata.get_element(i));
                    meta_ref.x *= 2;
                    meta_ref.y *= 2;
                    meta_ref.value1 *= 2;
                    meta_ref.value2 *= 2;
                    ASSERT_TRUE(meta_ref == *cp_meta);
                }
                else
                {
                    ASSERT_NO_THROW(p_meta = &metadata.get_element(i));
                    ASSERT_TRUE(meta_ref == *p_meta);
                    p_meta->x *= 2;
                    p_meta->y *= 2;
                    p_meta->value1 *= 2;
                    p_meta->value2 *= 2;
                }
            }
            for (unsigned int i = 124; i <= 200; i++)
            {
                initialize_metadata::small_i(meta_ref, i);
                ASSERT_NO_THROW(cp_meta = &metadata.get_element(i));
                ASSERT_TRUE(meta_ref == *cp_meta);
            }
            ASSERT_NO_THROW(metadata.sync());
        }

        {
            test_metadata meta_ref;
            const test_metadata* cp_meta;
            test_mapped_metadata metadata(s1b_file_name, false,
                s1b::S1B_HUGETLB_OFF);
            ASSERT_FALSE(metadata.can_write());
            for (unsigned int i = s1b::FirstUID; i <= 101; i++)
            {
                initialize_metadata::small_i(meta_ref, i);
                ASSERT_NO_THROW(cp_meta = &metadata.get_element(i));
                meta_ref.x *= 2;
                meta_ref.y *= 2;
                meta_ref.value1 *= 2;
                meta_ref.value2 *= 2;
                ASSERT_TRUE(meta_ref == *cp_meta);
            }
            for (unsigned int i = 124; i <= 200; i++)
            {
                initialize_metadata::small_i(meta_ref, i);
                ASSERT_NO_THROW(cp_meta = &metadata.get_element(i));
                ASSERT_TRUE(meta_ref == *cp_meta);
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

void _RWPCompatCreateNew(const char* filename, bool can_write)
{
    std::vector<test_metadata> meta_vector;
    std::vector<s1b::foffset_t> off_vector;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    try
    {
        test_rwp_metadata metadata(filename,
            meta_vector.begin(), meta_vector.end());
        for (int i = 1; i <= 1000; i++)
        {
            s1b::foffset_t offset;
            ASSERT_NO_THROW(metadata.read_data_offset(i, offset));
            off_vector.push_back(offset);
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
        const test_metadata* cp_meta;
        s1b::foffset_t offset;
        test_mapped_metadata metadata(filename, can_write,
            s1b::S1B_HUGETLB_OFF);
        ASSERT_EQ(can_write, metadata.can_write());
        for (int i = 1; i <= 1000; i++)
        {
            ASSERT_NO_THROW(cp_meta = &metadata.get_element(i));
            ASSERT_TRUE(meta_vector[i-1] == *cp_meta);
            ASSERT_NO_THROW(offset = metadata.get_data_offset(i));
            ASSERT_TRUE(off_vector[i-1] == offset);
            ASSERT_NO_THROW(offset = metadata.get_data_offset(*cp_meta));
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

S1B_TEST(RWPCompatCreateNewAndOpenReadOnly)

    _RWPCompatCreateNew(s1b_file_name, false);
}

S1B_TEST(RWPCompatCreateNewAndOpenWrite)

    _RWPCompatCreateNew(s1b_file_name, true);
}

void _PushCompatCreateNew(const char* filename, bool can_write)
{
    std::vector<test_metadata> meta_vector;

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
        const test_metadata* cp_meta;
        test_mapped_metadata metadata(filename, can_write,
            s1b::S1B_HUGETLB_OFF);
        ASSERT_EQ(can_write, metadata.can_write());
        for (int i = 1; i <= 1000; i++)
        {
            ASSERT_NO_THROW(cp_meta = &metadata.get_element(i));
            ASSERT_TRUE(meta_vector[i-1] == *cp_meta);
            ASSERT_NO_THROW(metadata.get_data_offset(i));
            ASSERT_NO_THROW(metadata.get_data_offset(*cp_meta));
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

}
