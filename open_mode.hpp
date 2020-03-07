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
 * @brief File open modes.
 *
 */
enum open_mode
{
    /** Open an existing file for read. */
    S1B_OPEN_DEFAULT = 0,

    /** Allow creating files if they do not exist and writing to them. */
    S1B_OPEN_WRITE = 1,

    /** Set the file size to zero if it already exists. */
    S1B_OPEN_TRUNC = 2,

    /** Write + Trunc, create an empty file existing allowing writing. */
    S1B_OPEN_NEW   = S1B_OPEN_TRUNC | S1B_OPEN_WRITE,
};

}
