#include "syntax.h"
#include "op_functions.h"

#define TARGT_REGISTER_FIELD 2
#define SRC_OP_ADR_FIELD 6
#define SRC_REGISTER_FIELD 8
#define FUNCT_FIELD 12

typedef enum {
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    INVALID_REG
} register_num;

/* an array of "registers" struct to save the registeres */
struct {
    char *name;
    register_num reg_num;
} registers[] = {
    {"r0", R0},
    {"r1", R1},
    {"r2", R2},
    {"r3", R3},
    {"r4", R4},
    {"r5", R5},
    {"r6", R6},
    {"r7", R7},
    {"invalid", INVALID_REG} /* for errors */
};

/* get_register: gets a register name as a string and return its number, or the
   speciel value for invalid register if not one of r0 - r7 */
int get_register(char source_word[MAXLINE]) {
    int i;
    int invalid_reg;

    invalid_reg = (sizeof(registers) / sizeof(registers[0])) - 1;

    for (i = 0;
         (i < invalid_reg) && (strcmp(source_word, registers[i].name) != 0);
         i++)
        ;

    return i;
}

/* get_addressing: get the addressing type of the operand field */
addressing get_addressing(char *operand_field) {
    int i;

    i = 2; /* +1 becuse the counting will start from 1 */

    if (*operand_field == '#')
        return IMMEDIATE;

    if (*operand_field == 'r') /* maybe valid register direct */
        if (strcmp(registers[get_register(operand_field)].name, "Invalid") != 0)
            return REG_DIRECT;

    while (*operand_field++ != '\0') {
        i++;
        if (*operand_field == '[') { /* maybe valid index addressing */
            return INDEX;
        }
    }
    return DIRECT; /* string without [] */
}

/* get_brackets: return the register number stored inside the brackets [], or
   the a speciel value for invalid register if not one of r0 - r7 */
register_num get_brackets(char *operand) {
    int i;

    while (*operand != '[')
        operand++;
    operand++; /* skip the opening bracket */
    /* now points on the beginning of the operand inside brackets: [->here] */
    for (i = strlen(operand); strlen(operand) - strlen("[rX") < i; i--) {
        if (operand[i] == ']') {
            operand[i] = '\0'; /* cut of the closing bracket ']' */
            break;
        }
    } /* now points on the brackets contents, which ends afterwards with '\0' */
    return registers[get_register(operand)].reg_num;
}

/* incode_addressing: code the given information into the machine word */
void incode_addressing(machine_word **codeword, int address, char *first_op,
                       char *second_op, char *cmd, src_op_line *srcline) {
    addressing mode;
    register_num register_number;

    codeword[address]->funct_nd_ops |= opcodes[get_opcode(cmd)].op_funct
                                       << FUNCT_FIELD; /*set funct field*/

    mode = get_addressing(first_op); /* get first operand addressing */

    if (strlen(second_op) != 0) { /* two operands so first operand is source*/

        codeword[address]->funct_nd_ops |= (mode << SRC_OP_ADR_FIELD);

        if (mode == REG_DIRECT) {

            if ((register_number = registers[get_register(first_op)].reg_num) ==
                INVALID_REG) {

                printf("%s:%d: error: invalid source register '%s'\n",
                       srcline->as_filename, srcline->line_num, first_op);
                srcline->error_flag = EXIT_FAILURE;
            }

            codeword[address]->funct_nd_ops |=
                register_number << SRC_REGISTER_FIELD; /*update machine word*/

        } else if (mode == INDEX) {

            if ((register_number = get_brackets(first_op)) == INVALID_REG) {

                printf("%s:%d: error: invalid source register for index "
                       "addressing mode\n",
                       srcline->as_filename, srcline->line_num);
                srcline->error_flag = EXIT_FAILURE;
            }

            codeword[address]->funct_nd_ops |= register_number
                                               << SRC_REGISTER_FIELD;
        }

        mode = get_addressing(second_op);
        codeword[address]->funct_nd_ops |= mode;
        /* get second operand mode. two ops -> the 2nd is a target operand */
        if (mode == REG_DIRECT) {

            if ((register_number =
                     registers[get_register(second_op)].reg_num) ==
                INVALID_REG) { /* target register error */

                printf("%s:%d: error: invalid target register '%s'\n",
                       srcline->as_filename, srcline->line_num, second_op);
                srcline->error_flag = EXIT_FAILURE;
            }

            codeword[address]->funct_nd_ops |= register_number
                                               << TARGT_REGISTER_FIELD;

        } else if (mode == INDEX) {

            if ((register_number = get_brackets(second_op)) == INVALID_REG) {

                printf("%s:%d: error: invalid target register inside []\n",
                       srcline->as_filename, srcline->line_num);
                srcline->error_flag = EXIT_FAILURE;
            }

            codeword[address]->funct_nd_ops |= register_number
                                               << TARGT_REGISTER_FIELD;
        }
    } else { /* single operand command -> first operand is a target operand*/

        codeword[address]->funct_nd_ops |= mode; /* bits 0-1 */

        if (mode == REG_DIRECT) { /* register direct mode */

            if ((register_number = registers[get_register(first_op)].reg_num) ==
                INVALID_REG) {

                printf("%s:%d: error: invalid target register '%s'\n",
                       srcline->as_filename, srcline->line_num, first_op);
                srcline->error_flag = EXIT_FAILURE;
            }

            codeword[address]->funct_nd_ops |= register_number
                                               << TARGT_REGISTER_FIELD;

        } else if (mode == INDEX) {

            if ((register_number = get_brackets(first_op)) == INVALID_REG) {

                printf("%s:%d: error: invalid target register for index "
                       "addressing mode\n",
                       srcline->as_filename, srcline->line_num);
                srcline->error_flag = EXIT_FAILURE;
            }

            codeword[address]->funct_nd_ops |= register_number
                                               << TARGT_REGISTER_FIELD;
        }
    }
}

/* get_words_num: returns the extra data words required for the operands */
int get_words_num(machine_word **code_img, int address, char *first_op,
                  char *second_op, src_op_line *srcline) {
    addressing mode;
    int i, j, temp, ops_num, extra_words, sign, pos;

    extra_words = 0; /* the extra binary words required for the source line */
    ops_num = 1;     /* number of operands */
    sign = 1;
    pos = 1;
    mode = get_addressing(first_op); /* start with first operands */

    if (!(strlen(second_op) == 0)) /* true if ther's a second operand */
        ops_num++;

    for (j = 1; j <= ops_num; j++) { /* until checking every operand (1 or 2)*/

        if ((mode <= 2) && (mode >= 0)) {
            if ((code_img[address + extra_words] =
                     (machine_word *)malloc(sizeof(machine_word))) == NULL) {
                return -EXIT_FAILURE;
            }
            code_img[address + extra_words]->funct_nd_ops =
                0; /*avoid unwanted values*/
            code_img[address + extra_words]->AER_field =
                ABSOLUTE; /*'A' field */

            if (mode == IMMEDIATE) {
                if (first_op[pos] == '-') {
                    sign = -1;
                    pos++;
                }
                if ((temp = atoi(&first_op[pos])) == 0) { /* missing integer */
                    printf("%s:%d: error: missing a valid number for immediate "
                           "addressing mode\n",
                           srcline->as_filename, srcline->line_num);
                }
                code_img[address + extra_words]->funct_nd_ops = temp * sign;
                extra_words++; /* add one extra data word with the operand value
                                */
            } else if ((mode == INDEX) || (mode == DIRECT)) {
                for (i = 1; i <= 2; i++) {
                    /* avoid double allocation */
                    if (code_img[address + extra_words] == NULL) {
                        if ((code_img[address + extra_words] = (machine_word *)
                                 malloc(sizeof(machine_word))) == NULL) {
                            return -EXIT_FAILURE;
                        }
                    }
                    code_img[address + extra_words]->funct_nd_ops = 0;
                    code_img[address + extra_words]->AER_field = ABSOLUTE;
                    extra_words++;
                }
            }
        }
        if (ops_num ==
            2) { /* if there's a 2nd operands prepere for another round */
            mode = get_addressing(second_op); /* now for the target operand */
            first_op =
                second_op; /*set second operand value to repeat the loop*/
        }
    }
    return extra_words;
}

/* get_opcode: a function to return the "opcode" value of a given input, or a
   speciel value if its undefined */
int get_opcode(char source_word[MAXLINE]) {
    int i;

    i = 0;

    for (i = 0; i < (sizeof(opcodes) / sizeof(opcodes[0])) &&
                (strcmp(source_word, opcodes[i].name) != 0);
         i++)
        ;

    return i;
}
