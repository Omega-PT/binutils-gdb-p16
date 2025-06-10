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

/* Instructions with the same mnemonic must be together on the table, 
   since the assembler finds the first mnemonic match, and then tries
   the instructions below if the arguments don't match yet.  */
const inst p16_instruction[] =
{
  {"adc",   INST_LEN, 0x9000, 0xF800, ARITH_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},
  {"add",   INST_LEN, 0xA000, 0xF800, ARITH_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},
  {"add",   INST_LEN, 0x8000, 0xF800, ARITH_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},
  {"and",   INST_LEN, 0xC000, 0xF800, ARITH_BYTE_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},
  {"asr",   INST_LEN, 0xF000, 0xF800, SHIFT_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},

  {"b",     INST_LEN, 0x5800, 0xFC00, BRANCH_INS, {{imm10, 0}}},

  {"bcc",   INST_LEN, 0x4C00, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  {"blo",   INST_LEN, 0x4C00, 0xFC00, BRANCH_INS, {{imm10, 0}}},

  {"bcs",   INST_LEN, 0x4800, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  {"bhs",   INST_LEN, 0x4800, 0xFC00, BRANCH_INS, {{imm10, 0}}},

  {"bge",   INST_LEN, 0x5000, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  {"bl",    INST_LEN, 0x5C00, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  {"blt",   INST_LEN, 0x5400, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  
  {"bzc",   INST_LEN, 0x4400, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  {"bne",   INST_LEN, 0x4400, 0xFC00, BRANCH_INS, {{imm10, 0}}},

  {"bzs",   INST_LEN, 0x4000, 0xFC00, BRANCH_INS, {{imm10, 0}}},
  {"beq",   INST_LEN, 0x4000, 0xFC00, BRANCH_INS, {{imm10, 0}}},

  {"cmp",   INST_LEN, 0xB800, 0xF800, ARITH_BYTE_INS, {{low_regr, 4}, {regr, 7 }}},

  {"eor",   INST_LEN, 0xD000, 0xF800, ARITH_BYTE_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},

  {"ldr",   INST_LEN, 0x0C00, 0xFC00, LD_STOR_INS, {{regr, 0}, {uimm7, 4}}},
  {"ldr",   INST_LEN, 0x0000, 0xFC00, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},
  {"ldr",   INST_LEN, 0x1000, 0xF800, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},

  {"ldrb",  INST_LEN, 0x0800, 0xFC00, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {uimm3, 7}}},
  {"ldrb",  INST_LEN, 0x1800, 0xF800, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {regr, 7 }}},

  {"lsl",   INST_LEN, 0xE000, 0xF800, SHIFT_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},
  {"lsr",   INST_LEN, 0xE800, 0xF800, SHIFT_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},

  {"mov",   INST_LEN, 0x6000, 0xF000, ARITH_INS, {{regr, 0}, {uimm8, 4}}},
  {"mov",   INST_LEN, 0xB000, 0xF870, ARITH_INS, {{regr, 0}, {regr, 7}}},

  {"movs",  INST_LEN, 0XB020, 0xF870, ARITH_INS, {0, 0}},
  {"movt",  INST_LEN, 0x7000, 0xF000, ARITH_INS, {{regr, 0}, {uimm8, 4}}},

  {"mrs",   INST_LEN, 0xB060, 0xF860, ARITH_INS, {{regr, 0}, {p_regr, 4}}},
  {"msr",   INST_LEN, 0xB040, 0xF860, ARITH_INS, {{p_regr, 4}, {regr, 7}}},

  {"mvn",   INST_LEN, 0xB010, 0xF870, ARITH_INS, {{regr, 0}, {regr, 7}}},

  {"orr",   INST_LEN, 0xC800, 0xF800, ARITH_BYTE_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},

  {"pop",   INST_LEN, 0x0400, 0xFC00, NO_TYPE_INS, {{regr, 0}}},
  {"push",  INST_LEN, 0x2400, 0xFC00, NO_TYPE_INS, {{regr, 0}}},

  {"ror",   INST_LEN, 0xF800, 0xF800, SHIFT_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},

  {"rrx",   INST_LEN, 0xD800, 0xF800, SHIFT_INS, {{regr, 0}, {low_regr, 4}}},

  {"sub",   INST_LEN, 0xA800, 0xF800, ARITH_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},
  {"sub",   INST_LEN, 0x8800, 0xF800, ARITH_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},

  {"sbc",   INST_LEN, 0x9800, 0xF800, ARITH_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},

  {"str",   INST_LEN, 0x2000, 0xFC00, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {uimm4, 7}}},
  {"str",   INST_LEN, 0x3000, 0xF800, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {regr, 7}}},

  {"strb",  INST_LEN, 0x2800, 0xFC00, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {uimm3, 7}}},
  {"strb",  INST_LEN, 0x3800, 0xF800, LD_STOR_INS, {{regr, 0}, {low_regr, 4}, {regr, 7 }}},
  {NULL,    0,        0,      0,      0,           {{0, 0}}},
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

/* Build a general purpose register r<N>.  */
#define REG_R(N)    REG(CONCAT2(r,N), N, P16_R_REGTYPE)

const reg_entry p16_regtab[] = {
   REG_R(0), REG_R(1), REG_R(2), REG_R(3),
   REG_R(4), REG_R(5), REG_R(6), REG_R(7),
   REG_R(8), REG_R(9), REG_R(10), REG_R(11),
   REG_R(12), REG_R(13), REG_R(14), REG_R(15),
   REG(sp, 13, P16_R_REGTYPE),
   REG(lr, 14, P16_R_REGTYPE),
   REG(pc, 15, P16_R_REGTYPE),
};
const unsigned int p16_num_regs = ARRAY_SIZE (p16_regtab);

/* Build a processor register.  */
const reg_entry p16_pregtab[] = {
   REG(cpsr, 0, P16_P_REGTYPE),
   REG(spsr, 1, P16_P_REGTYPE),
};
const unsigned int p16_num_pregs =  ARRAY_SIZE (p16_pregtab);

/* P16 operands table. (Incomplete) */
/* Order needs to be the same as the operand_type enum.  */
const operand_entry p16_optab[] = {
    /* Index 0 is dummy, so we can count the instruction's operands.  */
    {0,    nullargs,     0},                        /* dummy */
    {10,   arg_ic,       OP_SIGNED},                /* imm10 */
    {3,    arg_ic,       OP_UNSIGNED},              /* uimm3 */
    {4,    arg_ic,       OP_UNSIGNED},              /* uimm4 */
    {7,    arg_ic,       OP_UNSIGNED},              /* uimm7 */
    {8,    arg_ic,       OP_UNSIGNED},              /* uimm8 */
    {4,    arg_r,        0},                        /* register rd/rm (r0-r15) */
    {3,    arg_r,        0},                        /* register rn (r0-r7) */
    {1,    arg_pr,       0},                        /* processor register (cpsr or spsr) */
};
const unsigned int p16_num_optab = ARRAY_SIZE (p16_optab);

/* P16 traps/interrupts.  */
const trap_entry p16_traps[] =
{
   /* TODO.  */
};
const unsigned int p16_num_traps = ARRAY_SIZE (p16_traps);

