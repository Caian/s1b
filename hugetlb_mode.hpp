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

#pragma once

namespace s1b {

/**
 * @brief Huge pages reservation mode.
 *
 */
enum hugetlb_mode
{
    /** Do not try to allocate huge pages. */
    S1B_HUGETLB_OFF = 0,

    /** Try to allocate huge pages but fall back to normal pages if not
        available. */
    S1B_HUGETLB_TRY = 1,

    /** Only perform the mapping using huge pages. */
    S1B_HUGETLB_FORCE = 2,
};

}
