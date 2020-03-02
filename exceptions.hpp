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

#pragma once

#include "types.hpp"
#include "metadata.hpp"

#include <ex3/exceptions.hpp>

#include <string>

#include <stdint.h>

// TODO make EX3 optional

#define S1B_MAKE_EINFO(Name, Type) \
    typedef boost::error_info< \
        struct tag_##Name##_name, \
        Type \
        > Name##_ei;

#define S1B_MAKE_DERIVED(Name, Base) \
    class Name##_exception : \
        public Base##_exception \
    { \
    };

#define S1B_MAKE_EXCEPTION(Name) \
    S1B_MAKE_DERIVED(Name, s1b)

namespace s1b {

class s1b_exception :
    public ex3::exception_base
{
};

S1B_MAKE_EINFO(maximum_size      , foffset_t      )
S1B_MAKE_EINFO(expected_size     , foffset_t      )
S1B_MAKE_EINFO(actual_size       , foffset_t      )
S1B_MAKE_EINFO(expected_value    , int64_t        )
S1B_MAKE_EINFO(actual_value      , int64_t        )
S1B_MAKE_EINFO(actual_pointer    , const void*    )
S1B_MAKE_EINFO(operation_mode    , os::mode_type  )
S1B_MAKE_EINFO(operation_flags   , os::flags_type )
S1B_MAKE_EINFO(file_descriptor   , os::fd_type    )
S1B_MAKE_EINFO(maximum_attempts  , unsigned int   )
S1B_MAKE_EINFO(offset            , foffset_t      )
S1B_MAKE_EINFO(expected_alignment, foffset_t      )
S1B_MAKE_EINFO(file_size         , foffset_t      )
S1B_MAKE_EINFO(file_position     , foffset_t      )
S1B_MAKE_EINFO(file_name         , std::string    )
S1B_MAKE_EINFO(operation_name    , std::string    )
S1B_MAKE_EINFO(operation_errno   , int            )
S1B_MAKE_EINFO(feature_name      , std::string    )
S1B_MAKE_EINFO(actual_slot_size  , foffset_t      )
S1B_MAKE_EINFO(expected_num_slots, unsigned int   )
S1B_MAKE_EINFO(actual_num_slots  , unsigned int   )
S1B_MAKE_EINFO(maximum_slot      , unsigned int   )
S1B_MAKE_EINFO(requested_slot    , unsigned int   )
S1B_MAKE_EINFO(maximum_uid       , s1b::uid_t     )
S1B_MAKE_EINFO(requested_uid     , s1b::uid_t     )
S1B_MAKE_EINFO(actual_uid        , s1b::uid_t     )
S1B_MAKE_EINFO(index_id          , std::string    )
S1B_MAKE_EINFO(argument_name     , std::string    )
S1B_MAKE_EINFO(expected_count    , int64_t        )
S1B_MAKE_EINFO(actual_count      , int64_t        )

S1B_MAKE_EXCEPTION(empty_mmap                     )
S1B_MAKE_EXCEPTION(empty_index                    )
S1B_MAKE_EXCEPTION(empty_size_increment           )
S1B_MAKE_EXCEPTION(invalid_uid                    )
S1B_MAKE_EXCEPTION(invalid_slot                   )
S1B_MAKE_EXCEPTION(invalid_num_slots              )
S1B_MAKE_EXCEPTION(io                             )
S1B_MAKE_EXCEPTION(format                         )
S1B_MAKE_EXCEPTION(unsupported                    )
S1B_MAKE_EXCEPTION(not_initialized                )
S1B_MAKE_EXCEPTION(null_pointer                   )
S1B_MAKE_EXCEPTION(not_found                      )
S1B_MAKE_EXCEPTION(multiple_results               )

S1B_MAKE_DERIVED(read_only            , io        )
S1B_MAKE_DERIVED(incomplete_read      , io        )
S1B_MAKE_DERIVED(incomplete_write     , io        )
S1B_MAKE_DERIVED(header_mismatch      , format    )
S1B_MAKE_DERIVED(check_data_mismatch  , format    )
S1B_MAKE_DERIVED(file_size_mismatch   , format    )
S1B_MAKE_DERIVED(num_slots_mismatch   , format    )
S1B_MAKE_DERIVED(element_mismatch     , format    )
S1B_MAKE_DERIVED(index_id_mismatch    , format    )
S1B_MAKE_DERIVED(invalid_managed_data , format    )
S1B_MAKE_DERIVED(index_size_too_big   , format    )
S1B_MAKE_DERIVED(file_name_too_long   , format    )
S1B_MAKE_DERIVED(data_offset_overlap  , format    )
S1B_MAKE_DERIVED(misaligned           , format    )
S1B_MAKE_DERIVED(extra_elem_bytes     , misaligned)
S1B_MAKE_DERIVED(extra_slot_bytes     , misaligned)

}
