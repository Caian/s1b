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

#define S1B_TEST_NAME PushMetadata

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

S1B_TEST(OpenWriteNonExisting)

    ASSERT_THROW(test_push_metadata metadata(s1b_file_name),
        s1b::io_exception);
}

S1B_TEST(OpenNewNonExisting)

    test_global_data glob;
    ASSERT_NO_THROW(test_push_metadata metadata(s1b_file_name, glob));
}

S1B_TEST(FromIteratorNonExisting)

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 10; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    test_global_data glob;
    ASSERT_NO_THROW(test_push_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end(), glob));
}

S1B_TEST(OpenWriteExistingEmpty)

    test_global_data glob;
    ASSERT_NO_THROW(test_push_metadata metadata(s1b_file_name, glob));

    ASSERT_NO_THROW(test_push_metadata metadata(s1b_file_name));
}

S1B_TEST(OpenNewExistingEmpty)

    test_global_data glob;

    ASSERT_NO_THROW(test_push_metadata metadata(s1b_file_name, glob));

    ASSERT_NO_THROW(test_push_metadata metadata(s1b_file_name, glob));
}

S1B_TEST(FromIteratorExistingEmpty)

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 10; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    test_global_data glob;
    ASSERT_NO_THROW(test_push_metadata metadata(s1b_file_name, glob));

    ASSERT_NO_THROW(test_push_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end(), glob));
}

S1B_TEST(GetSize)

    try
    {
        test_global_data glob;
        test_push_metadata metadata(s1b_file_name, glob);
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

S1B_TEST(GetMetaAdapter)

    try
    {
        test_global_data glob;
        test_push_metadata metadata(s1b_file_name, glob);
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
        test_global_data glob;
        test_push_metadata metadata(s1b_file_name, glob);
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

S1B_TEST(EmptyLastUID)

    try
    {
        test_global_data glob;
        test_push_metadata metadata(s1b_file_name, glob);
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

S1B_TEST(OpenAligned) // TODO add to others

    try
    {
        test_metadata meta;
        test_global_data glob;
        test_push_metadata metadata(s1b_file_name, glob);
        ASSERT_TRUE(sizeof(meta) % 64 != 0);
        ASSERT_NO_THROW(metadata.push(meta));
        ASSERT_NO_THROW(metadata.align());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }

    ASSERT_NO_THROW(test_push_metadata metadata(s1b_file_name));
}

S1B_TEST(OpenMisligned) // TODO add to others

    try
    {
        test_metadata meta;
        test_global_data glob;
        test_push_metadata metadata(s1b_file_name, glob);
        ASSERT_TRUE(sizeof(meta) % 64 != 0);
        ASSERT_NO_THROW(metadata.push(meta));
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }

    ASSERT_THROW(test_push_metadata metadata(s1b_file_name),
        s1b::misaligned_exception);
}

S1B_TEST(Filename)

    try
    {
        test_global_data glob;
        test_push_metadata metadata(s1b_file_name, glob);
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

    test_global_data glob;
    ASSERT_NO_THROW(test_push_metadata metadata(s1b_file_name, glob));

    for (int i = 0; i < 50000; i++)
    {
        try
        {
            test_push_metadata metadata(s1b_file_name);
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

S1B_TEST(PushOnWriteableFile)

    // Create the file beforehand.
    test_global_data glob;
    ASSERT_NO_THROW(test_push_metadata metadata(s1b_file_name, glob));

    try
    {
        test_metadata meta;
        s1b::foffset_t offset;
        s1b::foffset_t size;
        initialize_metadata::small_1(meta);

        test_push_metadata metadata(s1b_file_name);
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

        test_global_data glob;
        test_push_metadata metadata(s1b_file_name, glob);
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
    test_global_data glob;
    ASSERT_NO_THROW(test_push_metadata metadata(s1b_file_name, glob));

    try
    {
        test_metadata meta;
        s1b::foffset_t offset;
        s1b::foffset_t size, size2;
        initialize_metadata::small_1(meta);

        test_push_metadata metadata(s1b_file_name);
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

        test_global_data glob;
        test_push_metadata metadata(s1b_file_name, glob);
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

S1B_TEST(FromIteratorBadUids)

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta.uid += 500;
        meta_vector.push_back(meta);
    }

    test_global_data glob;
    ASSERT_NO_THROW(test_push_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end(), glob));

    try
    {
        test_rwp_metadata metadata(s1b_file_name, false);
        ASSERT_FALSE(metadata.can_write());
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

    test_global_data glob;
    ASSERT_NO_THROW(test_push_metadata metadata(s1b_file_name,
        meta_vector.begin(), meta_vector.end(), glob));

    try
    {
        test_rwp_metadata metadata(s1b_file_name, false);
        ASSERT_FALSE(metadata.can_write());
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

S1B_TEST(MixedPushOperations)

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
            test_global_data glob;
            test_push_metadata metadata(s1b_file_name, meta_vector.begin(),
                meta_vector.end(), glob);
            ASSERT_TRUE(metadata.can_write());
            for (unsigned int i = 21; i <= 123; i++)
            {
                initialize_metadata::small_i(meta, i);
                ASSERT_EQ(i, metadata.push(meta));
            }
            for (unsigned int i = 124; i <= 200; i++)
            {
                initialize_metadata::small_i(meta_ref, i);
                ASSERT_EQ(i, metadata.push(meta_ref));
            }
            ASSERT_NO_THROW(metadata.align());
            ASSERT_NO_THROW(metadata.sync());
        }

        {
            test_metadata meta_ref;
            test_push_metadata metadata(s1b_file_name);
            ASSERT_TRUE(metadata.can_write());
            for (unsigned int i = 201; i <= 203; i++)
            {
                initialize_metadata::small_i(meta_ref, i);
                ASSERT_EQ(i, metadata.push(meta_ref));
            }
            ASSERT_NO_THROW(metadata.align());
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

S1B_TEST(MappedCompatCreateNew)

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
        test_mapped_metadata metadata(s1b_file_name, meta_vector.begin(),
            meta_vector.end(), s1b::S1B_HUGETLB_OFF);
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
        test_push_metadata metadata(s1b_file_name);
        ASSERT_EQ(true, metadata.can_write());
        for (unsigned int i = 1001; i <= 1500; i++)
        {
            initialize_metadata::small_i(meta, i);
            ASSERT_EQ(i, metadata.push(meta));
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

S1B_TEST(RWPCompatCreateNew)

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
        test_rwp_metadata metadata(s1b_file_name,
            meta_vector.begin(), meta_vector.end());
        for (int i = 1001; i <= 1010; i++)
        {
            test_metadata meta;
            initialize_metadata::small_i(meta, i);
            meta_vector.push_back(meta);
            metadata.push(meta);
        }
        for (int i = 1; i <= 1010; i++)
        {
            s1b::foffset_t off;
            ASSERT_TRUE(metadata.read_data_offset(i, off));
            ASSERT_NO_THROW(off_vector.push_back(off));
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
        test_push_metadata metadata(s1b_file_name);
        ASSERT_EQ(true, metadata.can_write());
        for (unsigned int i = 1011; i <= 1500; i++)
        {
            initialize_metadata::small_i(meta, i);
            ASSERT_EQ(i, metadata.push(meta));
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

}
