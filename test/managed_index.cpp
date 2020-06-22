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

#define S1B_TEST_NAME ManagedIndex

#include "s1b_test.hpp"
#include "managed_test.hpp"
#include "metadata_test.hpp"

#include <s1b/rwp_metadata.hpp>
#include <s1b/mapped_metadata.hpp>

#define TEST_OBJ_TYPE test_managed_index_2
#include "move_test.hpp"

namespace {

// TODO unlink

typedef s1b::rwp_metadata<test_adapter> test_rwp_metadata;
typedef s1b::mapped_metadata<test_adapter> test_mapped_metadata;

struct NoException { };

template <typename Metadata, typename Exception>
void _Open(
    const char* filename,
    size_t initial_size,
    unsigned int num_retries,
    size_t size_increment
)
{
    static const bool throws_exception = boost::is_same<
        Exception,
        NoException>::value == false;

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 1024; i++)
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

#if !defined(S1B_DISABLE_ATOMIC_RW)
        const
#endif
        Metadata& rmetadata = metadata;

        if (throws_exception)
        {
            ASSERT_THROW(test_managed_index_2 index(filename, rmetadata,
                initial_size, num_retries, size_increment), Exception);
        }
        else
        {
            ASSERT_NO_THROW(test_managed_index_2 index(filename,
                rmetadata, initial_size, num_retries, size_increment));
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

S1B_TEST(OpenNonExisting)

    ASSERT_THROW(test_managed_index_1 index(s1b_file_name),
        s1b::io_exception);
}

S1B_TEST(OpenNewNonExistingRWP)

    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        1024*1024, 1, 1024*1024);
}

S1B_TEST(OpenExistingRWP)

    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        1024*1024, 1, 1024*1024);
    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        1024*1024, 1, 1024*1024);
}

S1B_TEST(ConstructNoRetriesRWP)

    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        1024 + 1024*1024, 0, 0);
}

S1B_TEST(ConstructInvalidRetriesRWP)

    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        1024 + 1024*1024, 0, 1024);
    _Open<test_rwp_metadata, s1b::empty_size_increment_exception>(
        s1b_file_name, 1024 + 1024*1024, 1, 0);
}

S1B_TEST(ConstructWithRetryRWP)

    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        1024, 1, 1024*1024);
}

S1B_TEST(ConstructNotEnoughRetriesRWP)

    _Open<test_rwp_metadata, s1b::index_size_too_big_exception>(
        s1b_file_name, 1024, 2, 1024*1024 / 4);
}

S1B_TEST(OpenNewNonExistingMapped)

    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        1024*1024, 1, 1024*1024);
}

S1B_TEST(OpenExistingMapped)

    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        1024*1024, 1, 1024*1024);
    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        1024*1024, 1, 1024*1024);
}

S1B_TEST(ConstructNoRetriesMapped)

    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        1024 + 1024*1024, 0, 0);
}

S1B_TEST(ConstructInvalidRetriesMapped)

    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        1024 + 1024*1024, 0, 1024);
    _Open<test_mapped_metadata, s1b::empty_size_increment_exception>(
        s1b_file_name, 1024 + 1024*1024, 1, 0);
}

S1B_TEST(ConstructWithRetryMapped)

    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        1024, 1, 1024*1024);
}

S1B_TEST(ConstructNotEnoughRetriesMapped)

    _Open<test_mapped_metadata, s1b::index_size_too_big_exception>(
        s1b_file_name, 1024, 2, 1024*1024 / 4);
}

S1B_TEST(Filename)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 1024*1024;

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

        MAKE_TEST_OBJ(index)(s1b_file_name, metadata,
            initial_size, 1, size_increment);
        MOVE_TEST_OBJ(index);

        ASSERT_STREQ(s1b_file_name, index.filename().c_str());
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

        MAKE_TEST_OBJ(index)(s1b_file_name, metadata,
            initial_size, 1, size_increment);
        MOVE_TEST_OBJ(index);

        ASSERT_EQ(initial_size + size_increment, index.size());
        ASSERT_GE(initial_size + size_increment, index.get_used_bytes());
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(IndexId)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 1024*1024;

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

        MAKE_TEST_OBJ(index)(s1b_file_name, metadata,
            initial_size, 1, size_increment);
        MOVE_TEST_OBJ(index);

        ASSERT_STREQ("s1b::managed_index/simple", index.get_index_id());
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

        MAKE_TEST_OBJ(index)(s1b_file_name, metadata,
            initial_size, 1, size_increment);
        MOVE_TEST_OBJ(index);

        ASSERT_EQ(false, index.can_write());
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

        ASSERT_THROW(test_managed_index_throw index(s1b_file_name, metadata,
            initial_size, 1, size_increment), CustomException);

        ASSERT_THROW(test_managed_index_dthrow index(s1b_file_name, metadata,
            initial_size, 1, size_increment), s1b::io_exception);
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
