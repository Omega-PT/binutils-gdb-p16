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

/* Operand errors.  */
typedef enum
  {
    OP_LEGAL = 0,       /* Legal operand.  */
    OP_OUT_OF_RANGE,    /* Operand not within permitted range.  */
  }
op_err;

/* Instruction mnemonics hash table.  */
static htab_t p16_inst_hash;
/* P16 registers hash table.  */
static htab_t reg_hash;
/* P16 processor registers hash table.  */
static htab_t preg_hash;
/* Current instruction we're assembling. (Instruction table entry)  */
const inst *current_instruction_template;


/* Globals.  */

/* Variable that holds an instruction's encoding (always 2 bytes) */
short global_output_opcode;

/* Nonzero means a relocatable symbol.  */
int global_is_relocatable;

/* A copy of the original instruction (used in error messages).  */
char global_ins_parse[MAX_INST_LEN];

/* The current processed argument number.  */
int global_cur_arg_num;

/* Generic assembler global variables which must be defined by all targets.  */

/* Characters which always start a comment.  */
const char comment_chars[] = ";";

/* Characters which start a comment at the beginning of a line.  */
const char line_comment_chars[] = ";";

/* This array holds machine specific line separator characters.  */
const char line_separator_chars[] = "";

/* Chars that can be used to separate mant from exp in floating point nums.  */
const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant as in 0f12.456  */
const char FLT_CHARS[] = "f'";

/* Target-specific multicharacter options, not const-declared at usage.  */
const char md_shortopts[] = "";
const struct option md_longopts[] = {{NULL, no_argument, NULL, 0}};
const size_t md_longopts_size = sizeof(md_longopts);

/* Return the bit size for a given operand.  */
static int get_opbits(operand_type op) {
    if (op < MAX_OPRD) {
        return p16_optab[op].bit_size;
    }
    return 0;
}

/* Return the argument type of a given operand.  */
static argtype get_optype(operand_type op) {
    if (op < MAX_OPRD) {
        return p16_optab[op].arg_type;
    } else {
        return nullargs;
    }
}

/* Return the flags of a given operand.  */
static int get_opflags(operand_type op) {
    if (op < MAX_OPRD) {
        return p16_optab[op].flags;
    }
    return 0;
}

/* Process machine-dependent command line options.  Called once for
   each option on the command line that the machine-independent part of
   GAS does not understand.  */
int md_parse_option(int c ATTRIBUTE_UNUSED, const char *arg ATTRIBUTE_UNUSED) {
    return 0;
}

/* Initializes a hash table with registers and their entries.  */
static void initialise_reg_hash_table (
    htab_t *hash_table,
	const reg_entry *register_table,
	const unsigned int num_entries
) {
  const reg_entry *rreg;

  *hash_table = str_htab_create();

  for (
    rreg = register_table;
    rreg < (register_table + num_entries);
    rreg++
    ) {
        if (str_hash_insert (*hash_table, rreg->name, rreg, 0) != NULL) {
            as_fatal (_("Duplicate register in hash table %s"), rreg->name);
        }
    }
}


/* This function is called once, at assembler startup time.  This should
   set up all the tables, etc that the MD part of the assembler needs.  */
void md_begin(void) {
    int i = 0;

    /* Set up a hash table for the instructions.  */
    p16_inst_hash = str_htab_create();

    while(p16_instruction[i].mnemonic != NULL) {
        const char *mnemonic = p16_instruction[i].mnemonic;

        if (str_hash_insert(p16_inst_hash, mnemonic, p16_instruction + i, 0)) {
            as_fatal(_("Duplicate mnemonic in hash table %s"), mnemonic);
        }

        /* Only add unique names into the hash table.  */
        do {
            ++i;
        } while (
            p16_instruction[i].mnemonic != NULL &&
            streq(p16_instruction[i].mnemonic, mnemonic)
        );

        /* Initialize reg_hash hash table.  */
        initialise_reg_hash_table(&reg_hash, p16_regtab, NUMREGS);
        /* Initialize preg_hash hash table.  */
        initialise_reg_hash_table(& preg_hash, p16_pregtab, NUMPREGS);
    }
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

/* Reset global variables before parsing a new instruction.  */
static void reset_global_vars(char *op) {
    global_cur_arg_num = 0;
    global_is_relocatable = 0;
    global_output_opcode = 0x0000;

    /* Save a copy of the original OP (used in error messages).  */
    strncpy(global_ins_parse, op, sizeof global_ins_parse - 1);
    global_ins_parse[sizeof global_ins_parse - 1] = 0;
}

/* Process constant values and labels.  */
static int process_labels_and_constants(char *string, assembling_ins *p16_assembling_ins) {
    parsed_argument *cur_arg = p16_assembling_ins->arg + global_cur_arg_num;

    /* Save the input_line_pointer so we can change it to our string, so
       we can parse it as an expression (Then revert the pointer again).  */
    char *input_line_pointer_backup = input_line_pointer;
    input_line_pointer = string;

    expression(&p16_assembling_ins->exp);

    /* Check for leftover garbage data.  */
    if (*input_line_pointer != '\0') {
        /* 'expression' function didn't use all of the operand string, 
           so there's some leftover garbage data it didn't parse.  */
        input_line_pointer = input_line_pointer_backup;
        as_bad(_("Garbage following expression '%s'"), string);
        return 0;
    }

    switch (p16_assembling_ins->exp.X_op) {
        case O_big:
        case O_absent:
            /* A missing or bad expression becomes 0 and throws a warning.  */
            as_bad(_("Missing or invalid expression '%s' taken as 0"), string);
            p16_assembling_ins->exp.X_op = O_constant;
            p16_assembling_ins->exp.X_add_number = 0;
            p16_assembling_ins->exp.X_add_symbol = NULL;
            p16_assembling_ins->exp.X_op_symbol = NULL;
            /* Fallthrough.  */
        
        case O_constant:
            cur_arg->X_op = O_constant;
            cur_arg->constant = p16_assembling_ins->exp.X_add_number;
            break;
        
        case O_symbol:
            cur_arg->X_op = O_symbol;
            cur_arg->constant = p16_assembling_ins->exp.X_add_number;
            p16_assembling_ins->exp.X_add_number = 0;
            p16_assembling_ins->rtype = BFD_RELOC_NONE;
            global_is_relocatable = true;

            /* Since it's a symbol, figure out what relocation it needs.  */
            if (cur_arg->type == arg_c) {
                if (IS_INSN_TYPE(BRANCH_INS)) {
                    p16_assembling_ins->rtype = BFD_RELOC_P16_IMM11_EVEN;
                }
            }

            break;
        
        default:
            cur_arg->X_op = p16_assembling_ins->exp.X_op;
            break;
    }

    input_line_pointer = input_line_pointer_backup;
    return 1;
}

/* Parse some special types of operands.  */
static int set_operand(assembling_ins *p16_assembling_ins, char *operand) {
    char *operandS; /* Pointer to start of sub-operand.  */
    char *operandE; /* Pointer to end of sub-operand.  */
    
    parsed_argument *cur_arg = p16_assembling_ins->arg + global_cur_arg_num;

    /* Initialize pointers.  */
    operandS = operandE = operand;

    switch (cur_arg->type) {
        case arg_ic:    /* Constant value, #0xFF, #123, etc...  */
            operandS++;
            /* Fall through.  */
        case arg_c:     /* Symbol */
            if (!process_labels_and_constants(operandS, p16_assembling_ins)) {
                return 0;
            }
            printf("Processed constant: %d\n", cur_arg->constant);
            break;
    }

    return 1;
}

/* Parses a string and returns its register value
    (or nullregister if it isn't a register).  */
static reg get_register(char *reg_name) {
    const reg_entry *r_entry;

    r_entry = (const reg_entry *)str_hash_find(reg_hash, reg_name);

    if (r_entry != NULL) {
        return r_entry->value.reg_val;
    }
    return nullregister;
}

/* Parses a string and returns its processor register value
    (or nullpregister if it isn't a register).  */
static preg get_pregister(char *preg_name) {
    const reg_entry *r_entry;

    r_entry = (const reg_entry *)str_hash_find(preg_hash, preg_name);

    if (r_entry != NULL) {
        return r_entry->value.preg_val;
    }
    return nullpregister;
}

/* Parses a single operand.  */
static int parse_single_operand(assembling_ins *p16_assembling_ins, char *operand) {
    int return_val;
    parsed_argument *cur_arg = p16_assembling_ins->arg + global_cur_arg_num;

    cur_arg->type = nullargs;

    /* Check if this argument is a general register.  */
    if ((return_val = get_register(operand)) != nullregister) {
        cur_arg->type = arg_r;
        cur_arg->r = return_val;
        cur_arg->X_op = 0;
        return 1;
    }

    /* Check if this argument is a processor register.  */
    if ((return_val = get_pregister(operand)) != nullpregister) {
        cur_arg->type = arg_pr;
        cur_arg->pr = return_val;
        cur_arg->X_op = 0;
        return 1;
    }

    /* Check if this argument is a constant value (preceded by a '#' character).  */
    switch (operand[0]) {
        case '#':
            cur_arg->type = arg_ic;
            goto set_params;
            break;
    }

    /* If not, it could be a symbol, so we set a different arg type.  */
    cur_arg->type = arg_c;

set_params:
    /* Parse an operand according to its type.  */
    cur_arg->constant = 0;
    if (!set_operand(p16_assembling_ins, operand)) {
        return 0;
    }

    return 1;
}

/* Parses the operands, which are saved in p16_assembling_ins.  */
static int parse_operands(assembling_ins *p16_assembling_ins, char *operands) {
    char *operandS;             /* Operands string.  */
    char *operandH, *operandT;  /* Single operand head/tail pointers.  */
    char *operand[MAX_OPERANDS];/* Separating the operands.  */

    int op_num = 0;             /* Current operand number we are parsing.  */
    int bracket_flag = 0;       /* Indicates a bracket '(' was found.  */
    int sq_bracket_flag = 0;    /* Indicates a square bracket '[' was found.  */
  
    /* All pointers point to the start of the operands.  */
    operandS = operandH = operandT = operands;

    while (*operandT != '\0') {
        /* End of operand reached, separate it and save it.  */
        if (*operandT == ',' && bracket_flag != 1 && sq_bracket_flag != 1) {
            *operandT++ = '\0';
            operand[op_num++] = strdup(operandH);
            operandH = operandT;
            continue;
        }
    
        if (*operandT == ' ') {
            as_bad (_("Illegal operands (whitespace): '%s'"), global_ins_parse);
        }

        if (*operandT == '(') {
            bracket_flag = 1;
        } else if (*operandT == '[') {
            sq_bracket_flag = 1;
        }

        if (*operandT == ')') {
            if (bracket_flag) {
                bracket_flag = 0;
            } else {
                as_fatal (_("Missing matching brackets: '%s'"), global_ins_parse);
            }
        } else if (*operandT == ']') {
            if (sq_bracket_flag) {
                sq_bracket_flag = 0;
            } else {
                as_fatal (_("Missing matching brackets: '%s'"), global_ins_parse);
            }
        }  

        operandT++;
    }

    /* Add the last operand.  */
    operand[op_num++] = strdup(operandH);
    p16_assembling_ins->nargs = op_num;

    /* Verify syntax.  */
    if (bracket_flag || sq_bracket_flag) {
        as_fatal (_("Missing matching brackets: '%s'"), global_ins_parse);
    }

    /* Parse operands with square++ brackets: 
       Divides [r0,r1] into argument r0 and argument r1.  */
    char *bracket_arg = operand[op_num - 1];

    if (bracket_arg[0] == '[') {
        char *comma_place = strchr(bracket_arg, ',');
        char *last_bracket_place = strrchr(bracket_arg, ']');

        if (comma_place == NULL) {
            as_fatal (_("Register indexing needs 2 arguments: '%s'"), bracket_arg);
        }

        /* Set string terminators to be able to dupe the strings.  */ 
        *comma_place = '\0';
        *last_bracket_place = '\0';

        char *first_arg = bracket_arg + 1;
        char *second_arg = comma_place + 1;

        /* Set the operands again (overwriting the whole [r0,r1] argument).  */
        operand[op_num - 1] = strdup(first_arg);
        operand[op_num] = strdup(second_arg);

        /* Went from 2 arguments to 3, also update the assembling instruction nargs  */
        op_num++;
        p16_assembling_ins->nargs = op_num;

        free(bracket_arg);
    }

    for (int i = 0; i < op_num; i++) {
        printf("Operand %d: '%s'\n", i, operand[i]);
    }
    
    /* Parse each operand.  */
    for (int i = 0; i < op_num; i++) {
        global_cur_arg_num = i;
        if (!parse_single_operand(p16_assembling_ins, operand[i])) {
            return 0;
        }
        free(operand[i]);
    }

    for (int i = 0; i < p16_assembling_ins->nargs; i++) {
        printf("Arg %d, Type: %d\n", i, p16_assembling_ins->arg[i].type);
    }

    return 1;
}

/* Where the actual instruction parsing begins 
   p16_assembling_ins -> Data structure of the currently assembling instruction
   operands -> String that points to the start of the operands.  */

static int parse_instruction(assembling_ins *p16_assembling_ins, char *operands) {
    /* Currently only calls parse_operands.  */
    if (!parse_operands(p16_assembling_ins, operands)) {
        return 0;
    }
    
    return 1;
}

/* Retrieve the number of operands for the current assembled instruction.  */

static int get_template_num_of_operands(void) {
    int i;

    for (i = 0; current_instruction_template->operands[i].op_type && i < MAX_OPERANDS; i++)
        ;
    return i;
}

/* Prints an operand to global_output_opcode.  */
static void print_operand(int nbits, int shift, parsed_argument *arg) {
    switch (arg->type) {
        case arg_r:
            global_output_opcode |= ((arg->r << shift));
            break;
        
        case arg_ic:
        case arg_c:
            unsigned long mask = (1 << nbits) - 1;
            unsigned long value1 = (arg->constant & mask);
            global_output_opcode |= ((arg->constant & mask) << shift);
            break;
        
        case arg_pr:
            global_output_opcode |= ((arg->pr << shift));
            break;
    }
}

/* Assembles a single isntruction
   Operand types and their values are already parsed and set
   To assemble it, we need a matching template from the isntruction table that:
   1. Has the same number of operands;
   2. Has the same operand types;
   3: The operand size is sufficient for its value.  */

static int assemble_instruction(assembling_ins *p16_assembling_ins, const char *mnemonic) {
    /* Type of each operand in the current template.  */
    argtype cur_template_op_type[MAX_OPERANDS];
    /* Size (in bits) of each operand in the current template.  */
    unsigned int cur_template_op_size[MAX_OPERANDS];
    /* Flags of each operand in the current template.  */
    unsigned int cur_template_op_flags[MAX_OPERANDS];
    /* Instruction type to match.  */
    unsigned int ins_type;
    /* Boolean flag to mark whether a match was found.  */
    int match = 0;
    int i;
    /* Nonzero if an instruction with same number of operands was found.  */
    int found_same_number_of_operands = 0;
    /* Nonzero if an instruction with same argument types was found.  */
    int found_same_argument_types = 0;
    /* Nonzero if a constant was found within the required range.  */
    int found_const_within_range  = 0;
    /* Argument number of an operand with invalid type.  */
    int invalid_optype = -1;
    /* Argument number of an operand with invalid constant value.  */
    int invalid_const = -1;
    /* Operand error (used for issuing various constant error messages).  */
    op_err op_error, const_err = OP_LEGAL;

/* Handy define to get the data of the operands of a given instruction.  */
#define GET_CURRENT_DATA(FUNC, ARRAY) \
    for (i = 0; i < p16_assembling_ins->nargs; i++) \
        ARRAY[i] = FUNC (current_instruction_template->operands[i].op_type)

/* Acquires the informations of the operands and stores them in
cur_type, cur_size and cur_flags.  */
#define GET_CURRENT_TYPE    GET_CURRENT_DATA (get_optype, cur_template_op_type)
#define GET_CURRENT_SIZE    GET_CURRENT_DATA (get_opbits, cur_template_op_size)
#define GET_CURRENT_FLAGS   GET_CURRENT_DATA (get_opflags, cur_template_op_flags)

    /* There are instructions with the same mnemonic, and the variable
       current_instruction_template only has the first one found, so we need
       to check if the types match and, if not, advance the template to check
       again (Since the table has these instructions all next to each other).  */
    ins_type = P16_INS_TYPE(current_instruction_template->flags);

    /* MOVS instruction special check (operands will always be 'pc,lr').  
       Template in instruction table has no operands, but we have read 2.  
       Don't go into the 'while' loop to find a matching one.  */
    if (streq("movs", current_instruction_template->mnemonic)) {
        if (
            p16_assembling_ins->nargs == 2 
            && p16_assembling_ins->arg[0].type == arg_r && p16_assembling_ins->arg[0].r == pc
            && p16_assembling_ins->arg[1].type == arg_r && p16_assembling_ins->arg[1].r == lr
        ) {
            /* Set match to 1 and don't let it print its operands.  */
            match = 1;
            p16_assembling_ins->nargs = 0;
        } else {
            as_bad(_("Invalid MOVS operands (should be movs pc,lr)"));
            return 0;
        }
    }

    while(
        match != 1                                        // Not matched yet
        && current_instruction_template->mnemonic != NULL // Not at the end of the table
        && IS_INSN_MNEMONIC(mnemonic)                     // Still the same mnemonic
        && IS_INSN_TYPE(ins_type)                         // Still the same type
    ) {
        if (get_template_num_of_operands() != p16_assembling_ins->nargs) {
            goto next_instruction;
        }
        found_same_number_of_operands = 1;

        /* Initialize arrays with data of each operand in current template.  */
        GET_CURRENT_TYPE;
        GET_CURRENT_SIZE;
        GET_CURRENT_FLAGS;

        /* Check types.  */
        for (i = 0; i < p16_assembling_ins->nargs; i++) {
            if (cur_template_op_type[i] != p16_assembling_ins->arg[i].type) {
                if (invalid_optype == -1) {
                    invalid_optype = i + 1;
                }
                goto next_instruction;
            }
        }
        found_same_argument_types = 1;

        match = 1;
        break;

    next_instruction:
        current_instruction_template++;
    }

    if (!match) {
        if (!found_same_number_of_operands) {
            as_bad(_("Incorrect number of operands"));
        } else if (!found_same_argument_types) {
            as_bad(_("Illegal type of operand (arg %d)"), invalid_optype);
        }

        return 0;
    } else {
        /* Match successful, save opcode to global_output_opcode.  */
        global_output_opcode = 0;
        global_output_opcode |= current_instruction_template->opcode;
    }

    /* Check for even immediates, where we need to discard the least significant bit.  */
    for (i = 0; i < p16_assembling_ins->nargs; i++) {
        if(cur_template_op_flags[i] & OP_EVEN) {
            parsed_argument *arg = &p16_assembling_ins->arg[i];

            /* Warn if it's not even, since the last bit will be discarded.  */
            if (arg->constant % 2 != 0) {
                as_warn(_("Immediate must be even value: %d"), arg->constant);
            }

            arg->constant >>= 1;
        }
    }

    for (i = 0; i < p16_assembling_ins->nargs; i++) {
        global_cur_arg_num = i;
        print_operand(
            cur_template_op_size[i],
            current_instruction_template->operands[i].shift,
            p16_assembling_ins->arg + i
        );
    }

    return 1;
}

/* Print the instruction. */
static void print_instruction(assembling_ins *p16_assembling_ins) {
    char *this_frag = frag_more(2);

    /* Handle relocations.  */
    bfd_reloc_code_real_type reloc_type = p16_assembling_ins->rtype;

    if ((global_is_relocatable) && reloc_type != BFD_RELOC_NONE) {
        reloc_howto_type *reloc_howto;
        int size;

        reloc_howto = bfd_reloc_type_lookup(stdoutput, reloc_type);

        if (!reloc_howto) abort();

        size = bfd_get_reloc_size(reloc_howto);

        fix_new_exp(
            frag_now,
            this_frag - frag_now->fr_literal,
            size,
            &p16_assembling_ins->exp,
            reloc_howto->pc_relative,
            p16_assembling_ins->rtype
        );
    }

    md_number_to_chars(this_frag, global_output_opcode, 2);
}

/* Actually assemble an instruction.  */
static int p16_assemble(const char *op, char *param) {
    assembling_ins p16_assembling_ins;

    /* Find the instruction in the instruction table.  */
    current_instruction_template = (const inst *)str_hash_find(p16_inst_hash, op);

    if (current_instruction_template == NULL) {
        as_bad(_("Unkown opcode: '%s'"), op);
        return 0;
    }

    printf("Found instruction: %s\n", current_instruction_template->mnemonic);

    if (!parse_instruction(&p16_assembling_ins, param)) {
        return 0;
    }

    if (!assemble_instruction(&p16_assembling_ins, op)) {
        return 0;
    }

    print_instruction(&p16_assembling_ins);

    return 1;
}

/* The function that assembles one assembly instruction 
    and outputs its coresponding machine code*/
void md_assemble(char *op) {
    char *param;

    /* Resets global variables for next instruction.  */
    reset_global_vars(op);

    /* Strips the mnemonic.  */
    for (param = op; *param != 0 && !ISSPACE (*param); param++);
    
    *param++ = '\0';

    printf("op: %s\n", op);
    printf("param: %s\n", param);

    if(!p16_assemble(op, param)) {
        return;
    }

    printf("Successful: %s\n", global_ins_parse);
}