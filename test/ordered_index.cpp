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

#define S1B_TEST_NAME OrderedIndex

#include "s1b_test.hpp"
#include "index_test.hpp"

#include <s1b/queries/at.hpp>
#include <s1b/traits/indexed_type.hpp>
#include <s1b/traits/walk_iterator.hpp>
#include <s1b/traits/query_iterator.hpp>
#include <s1b/indexing/ordered_index.hpp>
#include <s1b/rwp_metadata.hpp>
#include <s1b/mapped_metadata.hpp>

namespace {

// TODO unlink
// TODO query

typedef s1b::rwp_metadata<test_adapter> test_rwp_metadata;
typedef s1b::mapped_metadata<test_adapter> test_mapped_metadata;
typedef s1b::indexing::ordered_index<test_key> test_ordered_index;
typedef s1b::managed_index<test_ordered_index> test_index;
typedef s1b::traits::walk_iterator<test_index>::type test_walk_iterator;
typedef s1b::traits::query_iterator<test_index>::type test_query_iterator;

static const size_t idxsz = sizeof(s1b::traits::indexed_type<
    test_index
    >::type);

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
        idxsz*1024, 1, idxsz*1024);
}

S1B_TEST(OpenExistingRWP)

    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        idxsz*1024, 1, idxsz*1024);
    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        idxsz*1024, 1, idxsz*1024);
}

S1B_TEST(ConstructNoRetriesRWP)

    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        1024 + idxsz*1024, 0, 0);
}

S1B_TEST(ConstructInvalidRetriesRWP)

    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        1024 + idxsz*1024, 0, 1024);
    _Open<test_rwp_metadata, s1b::empty_size_increment_exception>(
        s1b_file_name, 1024 + idxsz*1024, 1, 0);
}

S1B_TEST(ConstructWithRetryRWP)

    _Open<test_rwp_metadata, NoException>(s1b_file_name,
        1024, 1, idxsz*1024);
}

S1B_TEST(ConstructNotEnoughRetriesRWP)

    _Open<test_rwp_metadata, s1b::index_size_too_big_exception>(
        s1b_file_name, 1024, 2, idxsz*1024 / 4);
}

S1B_TEST(OpenNewNonExistingMapped)

    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        idxsz*1024, 1, idxsz*1024);
}

S1B_TEST(OpenExistingMapped)

    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        idxsz*1024, 1, idxsz*1024);
    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        idxsz*1024, 1, idxsz*1024);
}

S1B_TEST(ConstructNoRetriesMapped)

    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        1024 + idxsz*1024, 0, 0);
}

S1B_TEST(ConstructInvalidRetriesMapped)

    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        1024 + idxsz*1024, 0, 1024);
    _Open<test_mapped_metadata, s1b::empty_size_increment_exception>(
        s1b_file_name, 1024 + idxsz*1024, 1, 0);
}

S1B_TEST(ConstructWithRetryMapped)

    _Open<test_mapped_metadata, NoException>(s1b_file_name,
        1024, 1, idxsz*1024);
}

S1B_TEST(ConstructNotEnoughRetriesMapped)

    _Open<test_mapped_metadata, s1b::index_size_too_big_exception>(
        s1b_file_name, 1024, 2, idxsz*1024 / 4);
}

S1B_TEST(Filename)

    static const size_t initial_size = 1024;
    static const size_t size_increment = idxsz*1024;

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
    static const size_t size_increment = idxsz*1024;

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

        ASSERT_STREQ("s1b::managed_index/s1b::indexing::ordered_index/"
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
    static const size_t size_increment = 16048;

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

        s1b::uid_t previous = begin->second - 1;
        s1b::uid_t count = 1;
        begin++;

        for ( ; begin != end; begin++, count++)
        {
            s1b::uid_t current = begin->second - 1;

            ASSERT_LE(0, current);
            ASSERT_LE(0, previous);
            ASSERT_GT(1000, current);
            ASSERT_GT(1000, previous);
            float x = begin->first.x;
            char value2 = begin->first.value2;
            ASSERT_EQ(meta_vector[current].x, x);
            ASSERT_EQ(meta_vector[current].value2, value2);

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

S1B_TEST(EmptyRange)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 8024;

    std::vector<test_metadata> meta_vector;

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        s1b::traits::indexed_type<test_index>::type index_min, index_max;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        ASSERT_EQ(0, index.get_index()->size());
        ASSERT_THROW(index.get_index()->get_range(),
            s1b::empty_index_exception);
        ASSERT_THROW(index.get_index()->get_range(index_min, index_max),
            s1b::empty_index_exception);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(Range)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    test_key key_min, key_max;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);

        test_key key(meta);

        if (i == 1)
        {
            key_min = key;
            key_max = key;
        }
        else
        {
            if (key < key_min)
                key_min = key;

            if (key_max < key)
                key_max = key;
        }
    }

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        s1b::traits::indexed_type<test_index>::type index_min, index_max;
        std::pair<
            s1b::traits::indexed_type<test_index>::type,
            s1b::traits::indexed_type<test_index>::type
            > index_pair;

        s1b::uid_t idx_min, idx_max;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        ASSERT_EQ(1000, index.get_index()->size());
        ASSERT_NO_THROW(index_pair = index.get_index()->get_range());
        idx_min = index_pair.first.second - 1;
        idx_max = index_pair.second.second - 1;
        ASSERT_EQ(key_min.x, index_pair.first.first.x);
        ASSERT_EQ(key_min.value2, index_pair.first.first.value2);
        ASSERT_EQ(key_max.x, index_pair.second.first.x);
        ASSERT_EQ(key_max.value2, index_pair.second.first.value2);
        ASSERT_LE(0, idx_min);
        ASSERT_LE(0, idx_max);
        ASSERT_GT(1000, idx_min);
        ASSERT_GT(1000, idx_max);
        ASSERT_EQ(meta_vector[idx_min].x, index_pair.first.first.x);
        ASSERT_EQ(meta_vector[idx_min].value2, index_pair.first.first.value2);
        ASSERT_EQ(meta_vector[idx_max].x, index_pair.second.first.x);
        ASSERT_EQ(meta_vector[idx_max].value2, index_pair.second.first.value2);
        ASSERT_NO_THROW(index.get_index()->get_range(index_min, index_max));
        idx_min = index_min.second - 1;
        idx_max = index_max.second - 1;
        ASSERT_EQ(key_min.x, index_min.first.x);
        ASSERT_EQ(key_min.value2, index_min.first.value2);
        ASSERT_EQ(key_max.x, index_max.first.x);
        ASSERT_EQ(key_max.value2, index_max.first.value2);
        ASSERT_LE(0, idx_min);
        ASSERT_LE(0, idx_max);
        ASSERT_GT(1000, idx_min);
        ASSERT_GT(1000, idx_max);
        ASSERT_EQ(meta_vector[idx_min].x, index_min.first.x);
        ASSERT_EQ(meta_vector[idx_min].value2, index_min.first.value2);
        ASSERT_EQ(meta_vector[idx_max].x, index_max.first.x);
        ASSERT_EQ(meta_vector[idx_max].value2, index_max.first.value2);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(QueryEmpty)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_query_iterator iterator_min, iterator_max;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        test_key key_min, key_max;
        key_min.x = 10;
        key_max.x = 20;

        ASSERT_EQ(0, index.get_index()->size());
        ASSERT_NO_THROW(index.get_index()->query(
            s1b::queries::make_closed_interval(key_min, key_max),
            iterator_min,
            iterator_max
        ));
        ASSERT_TRUE(iterator_min == iterator_max);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(QueryAll)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    test_key key_min, key_max;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);

        test_key key(meta);

        if (i == 1)
        {
            key_min = key;
            key_max = key;
        }
        else
        {
            if (key < key_min)
                key_min = key;

            if (key_max < key)
                key_max = key;
        }
    }

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_query_iterator iterator_min, iterator_max;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        ASSERT_EQ(1000, index.get_index()->size());
        ASSERT_NO_THROW(index.get_index()->query(
            s1b::queries::make_closed_interval(key_min, key_max),
            iterator_min,
            iterator_max
        ));
        size_t count = 0;
        for ( ; iterator_min != iterator_max; iterator_min++, count++)
        {
            s1b::uid_t idx_min = iterator_min->second - 1;
            ASSERT_LE(0, idx_min);
            ASSERT_GT(1000, idx_min);
            ASSERT_FALSE(iterator_min->first < key_min);
            ASSERT_FALSE(key_max < iterator_min->first);
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

S1B_TEST(QueryEmptyLowerDistinct)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    test_key key_min;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);

        test_key key(meta);

        if (i == 1)
        {
            key_min = key;
        }
        else
        {
            if (key < key_min)
                key_min = key;
        }
    }

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_query_iterator iterator_min, iterator_max;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        test_key lower_key = key_min;
        lower_key.x -= 1000;

        test_key upper_key = key_min;
        upper_key.x -= 990;

        ASSERT_EQ(1000, index.get_index()->size());
        ASSERT_NO_THROW(index.get_index()->query(
            s1b::queries::make_closed_interval(lower_key, upper_key),
            iterator_min,
            iterator_max
        ));
        ASSERT_TRUE(iterator_min == iterator_max);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(QueryEmptyUpperDistinct)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    test_key key_max;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);

        test_key key(meta);

        if (i == 1)
        {
            key_max = key;
        }
        else
        {
            if (key_max < key)
                key_max = key;
        }
    }

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_query_iterator iterator_min, iterator_max;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        test_key lower_key = key_max;
        lower_key.x += 1000;

        test_key upper_key = key_max;
        upper_key.x += 1010;

        ASSERT_EQ(1000, index.get_index()->size());
        ASSERT_NO_THROW(index.get_index()->query(
            s1b::queries::make_closed_interval(lower_key, upper_key),
            iterator_min,
            iterator_max
        ));
        ASSERT_TRUE(iterator_min == iterator_max);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(QueryEmptyLowerEqual)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    test_key key_min;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);

        test_key key(meta);

        if (i == 1)
        {
            key_min = key;
        }
        else
        {
            if (key < key_min)
                key_min = key;
        }
    }

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_query_iterator iterator_min, iterator_max;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        test_key lower_key = key_min;
        lower_key.x -= 1000;

        ASSERT_EQ(1000, index.get_index()->size());
        ASSERT_NO_THROW(index.get_index()->query(
            s1b::queries::make_closed_interval(lower_key, lower_key),
            iterator_min,
            iterator_max
        ));
        ASSERT_TRUE(iterator_min == iterator_max);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(QueryEmptyUpperEqual)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    test_key key_max;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);

        test_key key(meta);

        if (i == 1)
        {
            key_max = key;
        }
        else
        {
            if (key_max < key)
                key_max = key;
        }
    }

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_query_iterator iterator_min, iterator_max;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        test_key lower_key = key_max;
        lower_key.x += 1000;

        ASSERT_EQ(1000, index.get_index()->size());
        ASSERT_NO_THROW(index.get_index()->query(
            s1b::queries::make_closed_interval(lower_key, lower_key),
            iterator_min,
            iterator_max
        ));
        ASSERT_TRUE(iterator_min == iterator_max);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(QueryOneFirst)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    test_key key_min;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);

        if (i == 1)
            meta.x -= 1230000;

        meta_vector.push_back(meta);

        test_key key(meta);

        if (i == 1)
        {
            key_min = key;
        }
        else
        {
            if (key < key_min)
                key_min = key;
        }
    }

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_query_iterator iterator_min, iterator_max;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        test_key lower_key = key_min;

        ASSERT_EQ(1000, index.get_index()->size());
        ASSERT_NO_THROW(index.get_index()->query(
            s1b::queries::make_closed_interval(lower_key, lower_key),
            iterator_min,
            iterator_max
        ));
        size_t count = 0;
        for ( ; iterator_min != iterator_max; iterator_min++, count++)
        {
            s1b::uid_t idx_min = iterator_min->second - 1;
            ASSERT_LE(0, idx_min);
            ASSERT_GT(1000, idx_min);
            ASSERT_FALSE(iterator_min->first < lower_key);
            ASSERT_FALSE(lower_key < iterator_min->first);
        }
        ASSERT_EQ(1, count);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(QueryOneLast)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    test_key key_max;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);

        if (i == 1000)
            meta.x += 1230000;

        meta_vector.push_back(meta);

        test_key key(meta);

        if (i == 1)
        {
            key_max = key;
        }
        else
        {
            if (key_max < key)
                key_max = key;
        }
    }

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_query_iterator iterator_min, iterator_max;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        test_key lower_key = key_max;

        ASSERT_EQ(1000, index.get_index()->size());
        ASSERT_NO_THROW(index.get_index()->query(
            s1b::queries::make_closed_interval(lower_key, lower_key),
            iterator_min,
            iterator_max
        ));
        size_t count = 0;
        for ( ; iterator_min != iterator_max; iterator_min++, count++)
        {
            s1b::uid_t idx_min = iterator_min->second - 1;
            ASSERT_LE(0, idx_min);
            ASSERT_GT(1000, idx_min);
            ASSERT_FALSE(iterator_min->first < lower_key);
            ASSERT_FALSE(lower_key < iterator_min->first);
        }
        ASSERT_EQ(1, count);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(QueryOneMiddle)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    int i_middle = 123;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);

        if (i-1 == i_middle)
            meta.x += 0.123;

        meta_vector.push_back(meta);
    }

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_query_iterator iterator_min, iterator_max;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        test_key lower_key(meta_vector[i_middle]);

        ASSERT_EQ(1000, index.get_index()->size());
        ASSERT_NO_THROW(index.get_index()->query(
            s1b::queries::make_closed_interval(lower_key, lower_key),
            iterator_min,
            iterator_max
        ));
        size_t count = 0;
        for ( ; iterator_min != iterator_max; iterator_min++, count++)
        {
            s1b::uid_t idx_min = iterator_min->second - 1;
            ASSERT_LE(0, idx_min);
            ASSERT_GT(1000, idx_min);
            ASSERT_FALSE(iterator_min->first < lower_key);
            ASSERT_FALSE(lower_key < iterator_min->first);
        }
        ASSERT_EQ(1, count);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(QuerySomeLeft)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    test_key key_min, key_max;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);

        test_key key(meta);

        if (i == 1)
        {
            key_min = key;
            key_max = key;
        }
        else
        {
            if (key < key_min)
                key_min = key;

            if (key_max < key)
                key_max = key;
        }
    }

    float k = (key_max.x - key_min.x) / 3;
    key_min.x -= k;
    key_max.x -= k;

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_query_iterator iterator_min, iterator_max;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        ASSERT_EQ(1000, index.get_index()->size());
        ASSERT_NO_THROW(index.get_index()->query(
            s1b::queries::make_closed_interval(key_min, key_max),
            iterator_min,
            iterator_max
        ));
        for ( ; iterator_min != iterator_max; iterator_min++)
        {
            s1b::uid_t idx_min = iterator_min->second - 1;
            ASSERT_LE(0, idx_min);
            ASSERT_GT(1000, idx_min);
            ASSERT_FALSE(iterator_min->first < key_min);
            ASSERT_FALSE(key_max < iterator_min->first);
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

S1B_TEST(QuerySomeRight)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    test_key key_min, key_max;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);

        test_key key(meta);

        if (i == 1)
        {
            key_min = key;
            key_max = key;
        }
        else
        {
            if (key < key_min)
                key_min = key;

            if (key_max < key)
                key_max = key;
        }
    }

    float k = (key_max.x - key_min.x) / 3;
    key_min.x += k;
    key_max.x += k;

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_query_iterator iterator_min, iterator_max;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        ASSERT_EQ(1000, index.get_index()->size());
        ASSERT_NO_THROW(index.get_index()->query(
            s1b::queries::make_closed_interval(key_min, key_max),
            iterator_min,
            iterator_max
        ));
        for ( ; iterator_min != iterator_max; iterator_min++)
        {
            s1b::uid_t idx_min = iterator_min->second - 1;
            ASSERT_LE(0, idx_min);
            ASSERT_GT(1000, idx_min);
            ASSERT_FALSE(iterator_min->first < key_min);
            ASSERT_FALSE(key_max < iterator_min->first);
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

S1B_TEST(QuerySomeMiddle)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    test_key key_min, key_max;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);

        test_key key(meta);

        if (i == 1)
        {
            key_min = key;
            key_max = key;
        }
        else
        {
            if (key < key_min)
                key_min = key;

            if (key_max < key)
                key_max = key;
        }
    }

    float k = (key_max.x - key_min.x) / 3;
    key_min.x += k;
    key_max.x -= k;

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_query_iterator iterator_min, iterator_max;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        ASSERT_EQ(1000, index.get_index()->size());
        ASSERT_NO_THROW(index.get_index()->query(
            s1b::queries::make_closed_interval(key_min, key_max),
            iterator_min,
            iterator_max
        ));
        for ( ; iterator_min != iterator_max; iterator_min++)
        {
            s1b::uid_t idx_min = iterator_min->second - 1;
            ASSERT_LE(0, idx_min);
            ASSERT_GT(1000, idx_min);
            ASSERT_FALSE(iterator_min->first < key_min);
            ASSERT_FALSE(key_max < iterator_min->first);
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

S1B_TEST(AtEmpty)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        test_key key;
        key.x = 10;

        ASSERT_EQ(0, index.get_index()->size());
        ASSERT_THROW(s1b::queries::at(*index.get_index(), key),
            s1b::not_found_exception);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(AtOneMiddle)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    int i_middle = 123;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);

        if (i-1 == i_middle)
            meta.x += 0.123;

        meta_vector.push_back(meta);
    }

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_query_iterator iterator;

        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        test_key lower_key(meta_vector[i_middle]);

        ASSERT_EQ(1000, index.get_index()->size());
        ASSERT_NO_THROW(iterator = s1b::queries::at(*index.get_index(),
            lower_key));
        s1b::uid_t idx_min = iterator->second - 1;
        ASSERT_LE(0, idx_min);
        ASSERT_GT(1000, idx_min);
        ASSERT_FALSE(iterator->first < lower_key);
        ASSERT_FALSE(lower_key < iterator->first);
    }
    catch (const std::exception& e)
    {
        std::cerr
            << boost::current_exception_diagnostic_information()
            << std::endl;
        FAIL();
    }
}

S1B_TEST(AtSomeMiddle)

    static const size_t initial_size = 1024;
    static const size_t size_increment = 16048;

    std::vector<test_metadata> meta_vector;

    for (int i = 1; i <= 1000; i++)
    {
        test_metadata meta;
        initialize_metadata::small_i(meta, i);
        meta_vector.push_back(meta);
    }

    test_key key(meta_vector[300]);

    std::string meta_filename(s1b_file_name);
    meta_filename += "_metadata";

    try
    {
        test_mapped_metadata metadata(meta_filename, meta_vector.begin(),
            meta_vector.end());

        test_index index(s1b_file_name, metadata,
            initial_size, 1, size_increment);

        ASSERT_EQ(1000, index.get_index()->size());
        ASSERT_THROW(s1b::queries::at(*index.get_index(), key),
            s1b::multiple_results_exception);
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
