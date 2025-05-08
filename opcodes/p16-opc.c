/* p16-opc.c -- Table of opcodes for the P16 processor.
  
   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <stdio.h>
#include "libiberty.h"
#include "symcat.h"
#include "opcode/p16.h"

const inst p16_instruction[] =
{
  {"ldr",   INST_LEN, 0x0C00, 0xFC00, LD_STOR_INS, {{regr, 0}, {uimm7, 4}}},
  {"pop",   INST_LEN, 0x0400, 0xFC00, NO_TYPE_INS, {{regr, 0}}},
  {"push",  INST_LEN, 0x2400, 0xFC00, NO_TYPE_INS, {{regr, 0}}},

  {"ldr",   INST_LEN, 0x0000, 0xFC00, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},
  {"ldrb",  INST_LEN, 0x0800, 0xFC00, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {uimm3, 7}}},
  {"ldr",   INST_LEN, 0x1000, 0xF800, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},
  {"ldrb",  INST_LEN, 0x1800, 0xF800, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {regr, 7 }}},

  {"str",   INST_LEN, 0x2000, 0xFC00, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},
  {"strb",  INST_LEN, 0x2800, 0xFC00, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {uimm3, 7}}},
  {"str",   INST_LEN, 0x3000, 0xF800, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},
  {"strb",  INST_LEN, 0x3800, 0xF800, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {regr, 7 }}},

  {"add",   INST_LEN, 0x8000, 0xF800, ARITH_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},
  {"sub",   INST_LEN, 0x8800, 0xF800, ARITH_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},
  {"adc",   INST_LEN, 0x9000, 0xF800, ARITH_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},
  {"sbc",   INST_LEN, 0x9800, 0xF800, ARITH_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},
  {"add",   INST_LEN, 0xA000, 0xF800, ARITH_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},
  {"sub",   INST_LEN, 0xA800, 0xF800, ARITH_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},

  {"cmp",   INST_LEN, 0xB800, 0xF800, ARITH_BYTE_INS, {{low_regr, 4}, {regr, 7 }}},

  {"and",   INST_LEN, 0xC000, 0xF800, ARITH_BYTE_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},
  {"orr",   INST_LEN, 0xC800, 0xF800, ARITH_BYTE_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},
  {"eor",   INST_LEN, 0xD000, 0xF800, ARITH_BYTE_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},
  {"rrx",   INST_LEN, 0xD800, 0xF800, SHIFT_INS, {{regr, 0}, {low_regr, 4}}},

  {"lsl",   INST_LEN, 0xE000, 0xF800, SHIFT_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},
  {"lsr",   INST_LEN, 0xE800, 0xF800, SHIFT_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},
  {"asr",   INST_LEN, 0xF000, 0xF800, SHIFT_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},
  {"ror",   INST_LEN, 0xF800, 0xF800, SHIFT_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},

  {"mov",   INST_LEN, 0xB000, 0xF870, ARITH_INS, {{regr, 0}, {regr, 7}}},
  {"mvn",   INST_LEN, 0xB010, 0xF870, ARITH_INS, {{regr, 0}, {regr, 7}}},

  {"movs"},
  // TODO - Figure out how to differentiate the instructions
  {"msr",   INST_LEN, 0xB040, 0xF860, ARITH_INS, {{p_regr, 4}, {regr, 7}}},
  {"mrs",   INST_LEN, 0xB060, 0xF860, ARITH_INS, {{regr, 0}}, {p_regr, 4}},

  {"mov",   INST_LEN, 0x6000, 0xF000, ARITH_INS, {{regr, 0}, {uimm8, 4}}},
  {"movt",  INST_LEN, 0x7000, 0xF000, ARITH_INS, {{regr, 0}, {uimm8, 4}}},

  // Branches
  {"beq",   INST_LEN, 0x4000, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  {"bzs",   INST_LEN, 0x4000, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  
  {"bne",   INST_LEN, 0x4400, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  {"bzc",   INST_LEN, 0x4400, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  
  {"bcs",   INST_LEN, 0x4800, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  {"blo",   INST_LEN, 0x4800, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  
  {"bcc",   INST_LEN, 0x4C00, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  {"bhs",   INST_LEN, 0x4C00, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  
  {"bge",   INST_LEN, 0x5000, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  {"blt",   INST_LEN, 0x5400, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  {"b",     INST_LEN, 0x5800, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  {"bl",    INST_LEN, 0x5C00, 0xFC00, BRANCH_INS, {{imm10, 0}}},
};

const unsigned int p16_num_opcodes = ARRAY_SIZE (p16_instruction);

/* Macro to build a reg_entry, which have an opcode image :
   For example :
      REG(u4, 0x84, CR16_U_REGTYPE)
   is interpreted as :
      {"u4",  u4, 0x84, CR16_U_REGTYPE}
   The union initializer (second member) always refers to the first
   member of the union, so cast NAME to that type to avoid possible
   compiler warnings when used for CR16_P_REGTYPE cases.  */
#define REG(NAME, N, TYPE)    {STRINGX(NAME), {(reg) NAME}, N, TYPE}

#define REGP(NAME, BNAME, N, TYPE)    {STRINGX(NAME), {BNAME}, N, TYPE}

const reg_entry p16_regtab[] =
{ /* Build a general purpose register r<N>.  */
#define REG_R(N)    REG(CONCAT2(r,N), N, CR16_R_REGTYPE)

};

const unsigned int p16_num_regs = ARRAY_SIZE (p16_regtab);

const reg_entry p16_pregtab[] =
{
/* Build a processor register.  */

};

const unsigned int p16_num_pregs =  ARRAY_SIZE (p16_pregtab);

const char *p16_b_cond_tab[]=
{

};

const unsigned int p16_num_cc =  ARRAY_SIZE (p16_b_cond_tab);

/* P16 operands table.  */
const operand_entry p16_optab[] =
{
  
};

const unsigned int p16_num_optab = ARRAY_SIZE (p16_optab);

/* P16 traps/interrupts.  */
const trap_entry p16_traps[] =
{

};

const unsigned int p16_num_traps = ARRAY_SIZE (p16_traps);

