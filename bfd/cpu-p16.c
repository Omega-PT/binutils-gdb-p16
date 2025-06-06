/* BFD support for the P16 processor.
   Copyright (C) 2007-2025 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"


const bfd_arch_info_type bfd_p16_arch =
{
  16,			/* Bits in a word.  */
  16,			/* Bits in an address.  */
  8,			/* Bits in a byte.  */
  bfd_arch_p16,	/* Architecture number.  */
  bfd_mach_p16,
  "p16",		/* Arch name.  */
  "p16",		/* Printable name.  */
  1,			/* Section alignment power.  */
  true,			/* Default machine for the architecture (only one) */
  bfd_default_compatible,
  bfd_default_scan,
  bfd_arch_default_fill,
  NULL, /* No next machine (only one) */
  0 /* Maximum offset of a reloc from the start of an insn.  */
};
