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

#define S1B_TEST_NAME OrderedSequence

#include "s1b_test.hpp"
#include "index_test.hpp"

#include <s1b/traits/walk_iterator.hpp>
#include <s1b/indexing/ordered_sequence.hpp>
#include <s1b/rwp_metadata.hpp>
#include <s1b/mapped_metadata.hpp>

namespace {

// TODO unlink
// TODO query

typedef s1b::rwp_metadata<test_adapter> test_rwp_metadata;
typedef s1b::mapped_metadata<test_adapter> test_mapped_metadata;
typedef s1b::indexing::ordered_sequence<test_key> test_ordered_sequence;
typedef s1b::managed_index<test_ordered_sequence> test_index;
typedef s1b::traits::walk_iterator<test_index>::type test_walk_iterator;

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

        if (throws_exception)
        {
            ASSERT_THROW(test_index index(filename, metadata,
                initial_size, num_retries, size_increment), Exception);
        }
        else
        {
            ASSERT_NO_THROW(test_index index(filename,
                metadata, initial_size, num_retries, size_increment));
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

    ASSERT_THROW(test_index index(s1b_file_name),
        s1b::io_exception);
}

S1B_TEST(OpenNewNonExistingRWP)

    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        8*1024, 1, 8*1024);
}

S1B_TEST(OpenExistingRWP)

    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        8*1024, 1, 8*1024);
    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        8*1024, 1, 8*1024);
}

S1B_TEST(ConstructNoRetriesRWP)

    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        1024 + 8*1024, 0, 0);
}

S1B_TEST(ConstructInvalidRetriesRWP)

    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        1024 + 8*1024, 0, 1024);
    _Open<test_rwp_metadata, s1b::empty_size_increment_exception>(
        s1b_file_name, 1024 + 8*1024, 1, 0);
}

S1B_TEST(ConstructWithRetryRWP)

    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        1024, 1, 8*1024);
}

S1B_TEST(ConstructNotEnoughRetriesRWP)

    _Open<test_rwp_metadata, s1b::index_size_too_big_exception>(
        s1b_file_name, 1024, 2, 8*1024 / 4);
}

S1B_TEST(OpenNewNonExistingMapped)

    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        8*1024, 1, 8*1024);
}

S1B_TEST(OpenExistingMapped)

    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        8*1024, 1, 8*1024);
    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        8*1024, 1, 8*1024);
}

S1B_TEST(ConstructNoRetriesMapped)

    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        1024 + 8*1024, 0, 0);
}

S1B_TEST(ConstructInvalidRetriesMapped)

    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        1024 + 8*1024, 0, 1024);
    _Open<test_mapped_metadata, s1b::empty_size_increment_exception>(
        s1b_file_name, 1024 + 8*1024, 1, 0);
}

S1B_TEST(ConstructWithRetryMapped)

    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        1024, 1, 8*1024);
}

S1B_TEST(ConstructNotEnoughRetriesMapped)

    _Open<test_mapped_metadata, s1b::index_size_too_big_exception>(
        s1b_file_name, 1024, 2, 8*1024 / 4);
}

S1B_TEST(Filename)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 8*1024;

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

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

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
    static const size_t size_increment = 8*1024;

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

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

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

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        ASSERT_STREQ("s1b::managed_index/s1b::indexing::ordered_sequence/"
            "test_key", index.get_index_id());
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

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

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

S1B_TEST(EmptyIndex)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 8024;

    std::vector<test_metadata> meta_vector;

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_walk_iterator begin, end;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        ASSERT_EQ(0, index.get_index()->size());
        ASSERT_NO_THROW(begin = index.get_index()->begin());
        ASSERT_NO_THROW(end = index.get_index()->end());
        ASSERT_TRUE(begin == end);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(ValidIndex)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 8024;

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
        test_walk_iterator begin, end;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        ASSERT_EQ(1000, index.get_index()->size());
        ASSERT_NO_THROW(begin = index.get_index()->begin());
        ASSERT_NO_THROW(end = index.get_index()->end());
        ASSERT_TRUE(begin != end);

        s1b::uid_t previous = *begin - 1;
        s1b::uid_t count = 1;
        begin++;

        for ( ; begin != end; begin++, count++)
        {
            s1b::uid_t current = *begin - 1;

            ASSERT_LE(0, current);
            ASSERT_LE(0, previous);
            ASSERT_GT(1000, current);
            ASSERT_GT(1000, previous);

            if (meta_vector[current].x < meta_vector[previous].x)
            {
                FAIL() << "Index is unsorted on x! at " << current << ": "
                    << meta_vector[current].x << " vs "
                    << meta_vector[previous].x;
            }

            if (meta_vector[current].x == meta_vector[previous].x &&
                meta_vector[current].value2 < meta_vector[previous].value2)
            {
                FAIL() << "Index is unsorted on value2! at " << current << ": "
                    << meta_vector[current].value2 << " vs "
                    << meta_vector[previous].value2;
            }

            previous = current;
        }

        ASSERT_EQ(1000, count);
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
