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

#define S1B_VERSION_MAJOR    0
#define S1B_VERSION_MINOR    4
#define S1B_VERSION_REVISION 0

#define S1B_MAGIC0 0x43
#define S1B_MAGIC1 0x42
#define S1B_MAGIC2 0x45
#define S1B_MAGIC3 0x4E
#define S1B_MAGIC4 0x45
#define S1B_MAGIC5 0x53
#define S1B_MAGIC6 0x31
#define S1B_MAGIC7 0x4F

/** Align for 512-bit vector instructions. */
#define S1B_DEFAULT_ALIGNMENT_BYTES 64

/** Use a large push buffer to reduce the number of write operations. */
#if !defined(S1B_PUSH_BUFFER_SIZE)
#define S1B_PUSH_BUFFER_SIZE 10ULL*1024ULL*1024ULL // 10 MiB
#endif

/** Allow developers to specify custom aligments for metadata. */
#if !defined(S1B_META_ALIGNMENT_BYTES)
#define S1B_META_ALIGNMENT_BYTES S1B_DEFAULT_ALIGNMENT_BYTES
#endif

/** Allow developers to specify custom aligments for data. */
#if !defined(S1B_DATA_ALIGNMENT_BYTES)
#define S1B_DATA_ALIGNMENT_BYTES S1B_DEFAULT_ALIGNMENT_BYTES
#endif

#if !defined(S1B_DISABLE_OS_MACROS)
#include "os/macros.hpp"
#endif
