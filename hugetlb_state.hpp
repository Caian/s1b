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

namespace s1b {

/**
 * @brief Huge pages state of a mapping.
 *
 */
enum hugetlb_state
{
    /** Huge pages are not enabled at compiler level. */
    S1B_HUGETLB_UNSUPPORTED = 0,

    /** The code was compiled with huge table support but they were not
        requested when mapping. */
    S1B_HUGETLB_NOT_USED = 1,

    /** The code was compiled with huge table support but they could not be
        used during runtime. */
    S1B_HUGETLB_FAILED = 2,

    /** The mapping is using huge pages. */
    S1B_HUGETLB_ENABLED = 3,
};

}
