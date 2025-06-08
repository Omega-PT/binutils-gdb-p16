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

/* Chars that can be used to separate mant from exp in floating point nums.  */
const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant as in 0f12.456  */
const char FLT_CHARS[] = "f'";

/* Target-specific multicharacter options, not const-declared at usage.  */
const char md_shortopts[] = "";
const struct option md_longopts[] = {{NULL, no_argument, NULL, 0}};
const size_t md_longopts_size = sizeof(md_longopts);

/* Process machine-dependent command line options.  Called once for
   each option on the command line that the machine-independent part of
   GAS does not understand.  */
int md_parse_option(int c ATTRIBUTE_UNUSED, const char *arg ATTRIBUTE_UNUSED) {
    return 0;
}

/* This function is called once, at assembler startup time.  This should
   set up all the tables, etc that the MD part of the assembler needs.  */
void md_begin(void) {
    return;
}

/* Machine-dependent usage-output.  */
void md_show_usage(FILE *stream ATTRIBUTE_UNUSED) {
    return;
}

/* Function required to be defined, uses the default utility function.  */
const char *md_atof(int type, char *litP, int *sizeP) {
    return ieee_md_atof (type, litP, sizeP, target_big_endian);
}

/* This table describes all the machine specific pseudo-ops
   the assembler has to support.  The fields are:
   *** Pseudo-op name without dot.
   *** Function to call to execute this pseudo-op.
   *** Integer arg to pass to the function.  
   Table ends with an empty pseudo-op.  */
const pseudo_typeS md_pseudo_table[] = {
    {NULL, 0, 0}
};

symbolS *md_undefined_symbol(char *name ATTRIBUTE_UNUSED) {
    return 0;
}

void md_convert_frag(
    bfd *abfd ATTRIBUTE_UNUSED,
    asection *sec ATTRIBUTE_UNUSED,
    fragS *fragP ATTRIBUTE_UNUSED
) {
    return;
}

/* Round up a section size to the appropriate boundary.  */
valueT md_section_align(asection *seg, valueT addr) {
    int align = bfd_section_alignment(seg);
    return ((addr + (1 << align) - 1) & -(1 << align));
}

/* Generate the BFD reloc to be stuck in the object file from the
   fixup used internally in the assembler.  */
arelent *tc_gen_reloc(asection *sec ATTRIBUTE_UNUSED, fixS *fixp) {
    arelent *reloc;

    reloc = notes_alloc (sizeof (arelent));
    reloc->sym_ptr_ptr = notes_alloc (sizeof (asymbol *));
    *reloc->sym_ptr_ptr = symbol_get_bfdsym (fixp->fx_addsy);
    reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
    reloc->addend = fixp->fx_offset;
    reloc->howto = bfd_reloc_type_lookup (stdoutput, fixp->fx_r_type);

    if (!reloc->howto)
    {
        as_bad_where (fixp->fx_file, fixp->fx_line,
            _("Cannot represent %s relocation in object file"),
            bfd_get_reloc_code_name (fixp->fx_r_type));
        return NULL;
    }
    return reloc;
}

/* The location from which a PC relative jump should be calculated,
   given a PC relative reloc.  */
long md_pcrel_from(fixS *fixp) {
    return fixp->fx_frag->fr_address + fixp->fx_where;
}

/* Apply a fixS (fixup of an instruction or data that we didn't have
   enough info to complete immediately) to the data in a frag.  */
void md_apply_fix (fixS *fixP, valueT *valP, segT seg) {
    return;
}

/* Estimate the size of a frag before relaxing.
   Assume everything fits in 2 bytes.  */
int md_estimate_size_before_relax(fragS * fragp, segT segtype ATTRIBUTE_UNUSED) {
    fragp->fr_var = 2;
    return 2;
}

/* Parse an operand that is machine-specific.  */
void md_operand(expressionS *expressionP ATTRIBUTE_UNUSED) {
  return;
}

/* The function that assembles one assembly instruction 
    and outputs its coresponding machine code*/

void md_assemble(char *op) {
    // Dummy
    char *frag = frag_more(2);
    unsigned short word = 0xABAB;

    md_number_to_chars(frag, word, 2);
}