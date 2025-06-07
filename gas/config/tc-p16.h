/* tc-p16.h -- Header file for tc-p16.c, the P16 GAS port.
   Copyright (C) 2007-2025 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the
   Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#ifndef TC_P16_H
#define TC_P16_H

#define TC_P16 1

#define TARGET_BYTES_BIG_ENDIAN 0

#define GLOBAL_OFFSET_TABLE_NAME "_GLOBAL_OFFSET_TABLE_"

#define TARGET_FORMAT "elf32-p16"
#define TARGET_ARCH   bfd_arch_p16

#define WORKING_DOT_WORD
#define LOCAL_LABEL_PREFIX '.'

#define md_number_to_chars      number_to_chars_littleendian

#endif /* TC_P16_H */
