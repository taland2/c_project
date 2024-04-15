/*=======================================================================================================
Project: Maman 14 - Assembler
Created by:
Edrehy Tal and Liberman Ron Rafail

Date: 18/04/2024
========================================================================================================= */

#ifndef ASSEMBLER_H

#define ASSEMBLER_H

/*--------------------------------------Constant Variables -----------------------------------------------*/

#define MEMORY_START 100 /*Start Ram memory address*/
#define EMPTY_WORD 0 /*Legnth empty world*/
#define EXTERNAL_DEFAULT_ADDRESS 0 /*External memory address*/
#define NOT_FOUND -1 /*func return when not found*/
#define NO_ERROR 0 /*func return when pass*/
#define ERROR 1 /*func return when failed*/


#define LINE_LENGTH 80 /*max length of line in the input file*/
#define MINIMUM_LABEL_LENGTH_WITH_COLON 2
#define MINIMUM_LABEL_LENGTH_WITHOUT_COLON 1
#define LABEL_LENGTH 31 /* maximum characters per label */

#define MAX_COMMAND_LENGTH 4 /* maximum number of characters in a command */
#define MIN_COMMAND_LENGTH 3 /* minimum number of characters in a command */

#define REGISTER_LENGTH 2 /* a register's name contains 2 characters */
#define MIN_REGISTER 0 /* r0 is the first CPU register */
#define MAX_REGISTER 7 /* r7 is the last CPU register */
#define MAX_OP_LENGTH 20 /* minimum label length*/

#define MAX_EXTENSION_LENGTH 5

#define BASE32_SEQUENCE_LENGTH 3 /* A base32 sequence of a word consists of 2 digits (and '\0' ending) */


#define NUM_DIRECTIVES 5 /* number of existing directives*/
#define NUM_COMMANDS 16 /* number of existing commands */

#define FIRST_STRUCT_FIELD 1 /* Index of first struct field */
#define SECOND_STRUCT_FIELD 2 /* Index of second struct field */

#define MAX_INDEX_LENGTH 12 /*maximum index could be not more than the machine code bits there for 2^12=4096*/

/* Bit-related info */
#define BITS_IN_WORD 14 /* 14 bit on each c*/
#define BITS_IN_OPCODE 4 /*4 opconde bits*/
#define BITS_IN_METHOD 2 /*Addressing method; bits number 2-3 in command*/
#define BITS_IN_ARE 2 /*A.R.E command bits */
#define BITS_IN_REGISTER 4 /*register bits lenght */
#define BITS_IN_ADDRESS 8 /*Adreess bits lenght */
#define BITS_UNUSED 4 /*Not inused bits; last 4 bits in a command*/


/* Addressing methods bits location in the first word of a command */
#define SRC_METHOD_START_POS 4
#define SRC_METHOD_END_POS 5
#define DEST_METHOD_START_POS 2
#define DEST_METHOD_END_POS 3

#define MACHINE_RAM 4096 /*Maximum Ram capacity*/

/**************************************** Enums ****************************************/

/* Directives types */
enum directives {DATA, STRING, ENTRY, EXTERN, UNKNOWN_TYPE}; 

/* Enum of commands ordered by their opcode */
enum commands {MOV, CMP, ADD, SUB, NOT, CLR, LEA, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, HLT, UNKNOWN_COMMAND};

/* Enum of all errors that can be outputted */
enum errors {
    SYNTAX_ERR = 1, LABEL_ALREADY_EXISTS, LABEL_TOO_LONG, LABEL_INVALID_FIRST_CHAR, LABEL_ONLY_ALPHANUMERIC,
    LABEL_CANT_BE_COMMAND, LABEL_ONLY, LABEL_CANT_BE_REGISTER,
    DIRECTIVE_NO_PARAMS, DIRECTIVE_INVALID_NUM_PARAMS, DATA_COMMAS_IN_A_ROW, DATA_EXPECTED_NUM,
    DATA_EXPECTED_COMMA_AFTER_NUM, DATA_UNEXPECTED_COMMA,
    STRING_TOO_MANY_OPERANDS, STRING_OPERAND_NOT_VALID, STRUCT_EXPECTED_STRING,
    STRUCT_INVALID_STRING, EXPECTED_COMMA_BETWEEN_OPERANDS, STRUCT_INVALID_NUM, STRUCT_TOO_MANY_OPERANDS,
    EXTERN_NO_LABEL, EXTERN_INVALID_LABEL, EXTERN_TOO_MANY_OPERANDS,
    COMMAND_NOT_FOUND, COMMAND_UNEXPECTED_CHAR, COMMAND_TOO_MANY_OPERANDS,
    COMMAND_INVALID_METHOD, COMMAND_INVALID_NUMBER_OF_OPERANDS, COMMAND_INVALID_OPERANDS_METHODS,
    ENTRY_LABEL_DOES_NOT_EXIST, ENTRY_CANT_BE_EXTERN, COMMAND_LABEL_DOES_NOT_EXIST,
    CANNOT_OPEN_FILE,COMMAND_INVALID_INDEX
};

/* When we need to specify if label should contain a colon or not */
enum {NO_COLON, COLON};

/* Addressing methods ordered by their code */
enum methods {METHOD_IMMEDIATE, METHOD_DIRECT, METHOD_INDEX, METHOD_REGISTER, METHOD_UNKNOWN};

/* A/R/E modes ordered by their numerical value */
enum ARE {ABSOLUTE, EXTERNAL, RELOCATABLE};

/* Types of files that indicate what is the desirable file extension */
enum filetypes {FILE_INPUT, FILE_AM, FILE_OBJECT, FILE_ENTRY, FILE_EXTERN};

#endif
