/* p16.h -- Header file for P16 opcode and register tables.

   This file is part of GAS, GDB and the GNU binutils.

   GAS, GDB, and GNU binutils is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GAS, GDB, and GNU binutils are distributed in the hope that they will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef _P16_H_
#define _P16_H_

/* P16 Core Registers: */
typedef enum
{
  /* 16-bit general purpose registers.  */
  r0, r1, r2, r3, 
  r4, r5, r6, r7, 
  r8, r9, r10, r11, 
  r12, r13, r14, r15,

  /* Alternate register names */
  sp = 13, lr = 14, pc = 15,

  /* Not a register.  */
  nullregister,
  MAX_REG
}
reg;

/* P16 Processor Registers: */
typedef enum
{
  /* Processor registers.  */
  cpsr, spsr,

  /* Not a processor register.  */
  nullpregister,
  MAX_PREG
}
preg;

/* P16 Register types. */
typedef enum
{
  P16_R_REGTYPE,    /* r0 <-> r15 */
  P16_RLOW_REGTYPE, /* r0 <-> r7 */
  P16_P_REGTYPE     /* Processor register */
}
reg_type;


/* TODO - List P16 argument types */

/* P16 argument types :
   The argument types correspond to instructions operands

   Argument types :
   r - register
   ic - immediate
   pr - processor register.  */

typedef enum
{
  arg_r,
  arg_pr,
  arg_ic,

  /* Not an argument.  */
  nullargs
}
argtype;


/* P16 operand types:
    The operand types correspond to instructions operands. */
typedef enum
{
  dummy,
  /* N-bit signed immediate, only used for branches. */
  imm10,
  /* N-bit unsigned immediate. */
  uimm3, uimm4, uimm7, uimm8,

  /* TODO - Add register-relative operand types. */

  /* Register (r0 <-> r15).  */
  regr, 
  /* Low register (r0 <-> r7).  */
  low_regr,
  /* processor register.  */
  p_regr, 

  /* Not an operand.  */
  nulloperand,
  /* Maximum supported operand.  */
  MAX_OPRD
}
operand_type;

/* P16 instruction types.  */
/* TODO - List all the instruction types */
#define NO_TYPE_INS       0
#define ARITH_INS         1
#define LD_STOR_INS       2
#define BRANCH_INS        3
#define ARITH_BYTE_INS    4
#define SHIFT_INS         5
#define BRANCH_NEQ_INS    6
#define LD_STOR_INS_INC   7
#define STOR_IMM_INS      8
#define CSTBIT_INS        9

/* Maximum value supported for instruction types.  */
#define P16_INS_MAX        (1 << 4)
/* Mask to record an instruction type.  */
#define P16_INS_MASK       (P16_INS_MAX - 1)
/* Return instruction type, given instruction's attributes.  */
#define P16_INS_TYPE(attr) ((attr) & P16_INS_MASK)

/* Maximum operands per instruction.  */
#define MAX_OPERANDS     3
/* Maximum register name length. */
#define MAX_REGNAME_LEN  10
/* Maximum instruction length. */
#define MAX_INST_LEN     256

/* Operand must be an unsigned number.  */
#define OP_UNSIGNED   (1 << 0)
/* Operand must be a signed number.  */
#define OP_SIGNED     (1 << 1)

/* Single operand description.  */

typedef struct
{
  /* Operand type.  */
  operand_type op_type;
  /* Operand location within the opcode.  */
  unsigned int shift;
}
operand_desc;

/* Instruction data structure used in instruction table.  */

typedef struct
{
  /* Name.  */
  const char *mnemonic;
  /* Size (in words).  */
  unsigned int size;
  /* 16-bit opcode (Bits with arguments are set to 0)*/
  unsigned short opcode;
  /* 16-bit opcode mask (Where the opcode is constant) */
  unsigned short opcode_mask;
  /* Attributes.  */
  unsigned int flags;
  /* Operands (always last, so unreferenced operands are initialized).  */
  operand_desc operands[MAX_OPERANDS];
}
inst;

/* Instruction length. (In words)*/
#define INST_LEN 1

/* TODO - Update */
/* Data structure for a single instruction's arguments (Operands).  */
typedef struct
{
  /* Register or base register.  */
  reg r;
  /* Index register.  */
  reg i_r;
  /* Processor register.  */
  preg pr;
  /* Constant/immediate/absolute value.  */
  long constant;
  /* Argument type.  */
  argtype type;
  /* Size of the argument (in bits) required to represent.  */
  int size;
  /* The type of the expression.  */
  unsigned char X_op;
}
parsed_argument;

/* Internal structure to hold the various entities
   corresponding to the current assembling instruction.  */
typedef struct
{
  /* Number of arguments.  */
  int nargs;
  /* The argument data structure for storing args (operands).  */
  parsed_argument arg[MAX_OPERANDS];

  expressionS exp;
  bfd_reloc_code_real_type rtype;
  
  /* Instruction size (in bytes).  */
  int size;
}
assembling_ins;

/* Structure to hold information about predefined operands.  */

typedef struct
{
  /* Size (in bits).  */
  unsigned int bit_size;
  /* Argument type.  */
  argtype arg_type;
  /* One bit syntax flags.  */
  int flags;
}
operand_entry;

/* Structure to hold trap handler information.  */

typedef struct
{
  /* Trap name.  */
  char *name;
  /* Index in dispatch table.  */
  unsigned int entry;
}
trap_entry;

/* Structure to hold information about predefined registers.  */

typedef struct
{
  /* Name (string representation).  */
  char *name;
  /* Value (enum representation).  */
  union
  {
    /* Register.  */
    reg reg_val;
    /* processor register.  */
    preg preg_val;
  } value;
  /* Register image.  */
  int image;
  /* Register type.  */
  reg_type type;
}
reg_entry;

/* P16 opcode table.  */
extern const inst p16_instruction[];
extern const unsigned int p16_num_opcodes;
#define NUMOPCODES cr16_num_opcodes

/* P16 operands table.  */
extern const operand_entry p16_optab[];
extern const unsigned int p16_num_optab;

/* P16 registers table.  */
extern const reg_entry p16_regtab[];
extern const unsigned int p16_num_regs;
#define NUMREGS p16_num_regs

/* P16 processor registers table.  */
extern const reg_entry p16_pregtab[];
extern const unsigned int p16_num_pregs;
#define NUMPREGS p16_num_pregs

/* P16 trap/interrupt table.  */
extern const trap_entry p16_traps[];
extern const unsigned int p16_num_traps;
#define NUMTRAPS p16_num_traps

/* A macro for representing the instruction "constant" opcode, that is,
   the FIXED part of the instruction. The "constant" opcode is represented
   as a 32-bit unsigned long, where OPC is expanded (by a left SHIFT)
   over that range.  */
#define BIN(OPC,SHIFT)        (OPC << SHIFT)

/* Is the current instruction type is TYPE ?  */
#define IS_INSN_TYPE(TYPE)              \
  (P16_INS_TYPE (current_instruction_template->flags) == TYPE)

/* Is the current instruction mnemonic is MNEMONIC ?  */
#define IS_INSN_MNEMONIC(MNEMONIC)    \
  (strcmp (current_instruction_template->mnemonic, MNEMONIC) == 0)

/* Does the current instruction has register list ?  */
#define INST_HAS_REG_LIST              \
  (current_instruction_template->flags & REG_LIST)


/* Utility macros for string comparison.  */
#define streq(a, b)           (strcmp (a, b) == 0)

/* Long long type handling.  */
/* Replace all appearances of 'long long int' with LONGLONG.  */
typedef long long int LONGLONG;
typedef unsigned long long ULONGLONG;

/* Data types for opcode handling.  */
typedef unsigned long dwordU;
typedef unsigned short wordU;

/* Prototypes for function in cr16-dis.c.  */
//extern void cr16_make_instruction (void);
//extern int  cr16_match_opcode (void);

#endif /* _P16_H_ */
