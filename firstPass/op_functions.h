#if !defined(OP_HEADER)
#define OP_HEADER
#include "syntax.h"

#define _IMMEDIATE  1 /* 0001 */
#define _DIRECT     2 /* 0010 */
#define _INDEX      4 /* 0100 */
#define _REG_DIRECT 8 /* 1000 */

typedef enum {
    MOV_F = 0,  CMP_F = 0,  ADD_F = 10, SUB_F,      LEA_F = 0, CLR_F = 10,
    NOT_F,      INC_F,      DEC_F,      JMP_F = 10, BNE_F,     JSR_F,     
    RED_F = 0,  PRN_F = 0,  RTS_F = 0,  HLT_F = 0  
} funct;

typedef enum { 
    MOV_OP  =      1,
    CMP_OP  =      2,
    ADD_OP  =      4,
    SUB_OP  =      4,
    LEA_OP  =   0x10,
    CLR_OP  =   0x20,
    NOT_OP  =   0x20,
    INC_OP  =   0x20,
    DEC_OP  =   0x20,
    JMP_OP  =  0x200,
    BNE_OP  =  0x200,
    JSR_OP  =  0x200,
    RED_OP  = 0x1000,
    PRN_OP  = 0x2000,
    RTS_OP  = 0x4000,
    HLT_OP = 0x8000,
    INVALID =     -1
} opcode;

/* an array of "instruction" struct to save the values (name, opcodes, funct) 
   for every command. It's mainly used by the functions of the "utility" file
   to check for syntax errors, by comparing the source line fields with the
   valid matching operands number and addressing modes. */
static struct {
    char *name;
    unsigned int op_code;
    unsigned int op_funct;
    int required_ops;
    unsigned int valid_src_mod : 4;
    unsigned int valid_trgt_mod : 4;
} opcodes[] = {
    {"mov", MOV_OP, MOV_F, 2,
    (_IMMEDIATE + _DIRECT + _INDEX + _REG_DIRECT), /* 1111 in binary */
    (_DIRECT + _INDEX + _REG_DIRECT)},  /* 1110 in binary */

    {"cmp", CMP_OP, CMP_F, 2,
    (_IMMEDIATE + _DIRECT + _INDEX + _REG_DIRECT),
    (_IMMEDIATE + _DIRECT + _INDEX + _REG_DIRECT)},
    
    {"add", ADD_OP, ADD_F, 2,
    (_IMMEDIATE + _DIRECT + _INDEX + _REG_DIRECT),
    (_DIRECT + _INDEX + _REG_DIRECT)},
    
    {"sub", SUB_OP, SUB_F, 2,
    (_IMMEDIATE + _DIRECT + _INDEX + _REG_DIRECT),
    (_DIRECT + _INDEX + _REG_DIRECT)},

    {"lea", LEA_OP, LEA_F, 2,
    (_DIRECT + _INDEX),
    (_DIRECT + _INDEX + _REG_DIRECT)},

    {"clr", CLR_OP, CLR_F, 1,
    (0),
    (_DIRECT + _INDEX + _REG_DIRECT)},

    {"not", NOT_OP, NOT_F, 1,
    (0),
    (_DIRECT + _INDEX + _REG_DIRECT)},

    {"inc", INC_OP, INC_F, 1,
    (0),
    (_DIRECT + _INDEX + _REG_DIRECT)},
    
    {"dec", DEC_OP, DEC_F, 1,
    (0),
    (_DIRECT + _INDEX + _REG_DIRECT)},

    {"jmp", JMP_OP, JMP_F, 1,
    (0),
    (_DIRECT + _INDEX)},

    {"bne", BNE_OP, BNE_F, 1,
    (0),
    (_DIRECT + _INDEX)},

    {"jsr", JSR_OP, JSR_F, 1,
    (0),
    (_DIRECT + _INDEX)},

    {"red", RED_OP, RED_F, 1,
    (0),
    (_DIRECT + _INDEX + _REG_DIRECT)},

    {"prn", PRN_OP, PRN_F, 1,
    (0),
    (_IMMEDIATE + _DIRECT + _INDEX + _REG_DIRECT)},

    {"rts", RTS_OP, RTS_F, 0, 0, 0},
  
    {"hlt", HLT_OP, HLT_F, 0, 0, 0},

    {"invalid", NOT_OP, NOT_F, -1, 0, 0}
};

/* get_opcode: a function to return the "opcode" value of a given input, or a
   speciel value if its undefined */
int get_opcode(char source_word[MAXLINE]);

/* get_addressing: get the addressing type of the operand field */
addressing get_addressing(char *operand_field);

/* incode_adressing: code the addressing info into the machine word */
void incode_addressing(machine_word **codeword, int address, char *first_op,
    				   char *second_op, char *cmd, src_op_line *srcline);

/* get_words_num: returns the extras data words required for the operands */
int get_words_num(machine_word **code_img, int address, char *first_op,
				  char *second_op, src_op_line *srcline);

#endif
