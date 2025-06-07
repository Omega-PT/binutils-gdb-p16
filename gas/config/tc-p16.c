/* tc-p16.c -- Assembler code for ISEL's P16 processor
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

#include "as.h"
#include "safe-ctype.h"
#include "dwarf2dbg.h"
#include "opcode/p16.h"
#include "elf/p16.h"

#include <limits.h>
#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

/* Word is considered here as a 16-bit unsigned short int.  */
#define WORD_SHIFT  16

/* Register is 2-byte size.  */
#define REG_SIZE   2

/* Maximum size of a single instruction (in words).  */
#define INSN_SIZE   1

/* Generic assembler global variables which must be defined by all targets.  */

/* Characters which always start a comment.  */
const char comment_chars[] = "#";

/* Characters which start a comment at the beginning of a line.  */
const char line_comment_chars[] = "#";

/* This array holds machine specific line separator characters.  */
const char line_separator_chars[] = ";";

/* The function that assembles one assembly instruction 
    and outputs its coresponding machine code*/

void md_assemble(char *op) {
    // Dummy
    char *frag = frag_more(2);
    unsigned short word = 0xABAB;

    md_number_to_chars(frag, word, 2);
}