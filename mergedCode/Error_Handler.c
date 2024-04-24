/*=======================================================================================================
Project: Maman 14 - Assembler
Created by:
Edrehy Tal and Liberman Ron Rafail

Date: 18/04/2024
========================================================================================================= */

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include "Error_Handler.h"
#include "Utils.h"
#include "extern_variables.h"

/* status_error_code messages */
const char *msg[MSG_LEN] = {
        "Assembly process for %s.as completed without errors. Output file(s) have been generated.",
        "Assembly terminated with errors. No output file(s) have been generated.",
        "Assembler - Memory allocation error.",
        "Invalid function call - %s.",
        "Assembler - Unable to open file - %s",
        "Assembler - File opened successfully - %s.",
        "Assembler process for %s.as terminated with errors. No output file(s) have been generated.",
        "%s - Extra text on line %d.",
        "%s - Macro length exceeds the maximum limit on line %d. Maximum length is 31 characters.",
        "%s - Missing opening 'mcr' on line %d.",
        "%s - Missing closing 'endmcr' on line %d.",
        "%s - Invalid macro name (%s) on line %d.",
        "Preprocessor (%d/%d) - No output file(s) have been generated - %s.as.",
        "Preprocessor (%d/%d) - Output file(s) have been successfully generated - %s."
};

/**
 * Handles and reports errors during the assembly process.
 *
 * Handles different error codes and formats the error messages accordingly.
 * Additional arguments may be required for specific error messages.
 *
 * @param code      The error code indicating the type of error.
 * @param ...       Additional arguments depending on the error code.
 */
void handle_preprocessor_error(status_error_code code, ...) {
    va_list args;
    file_context *fc = NULL;
    int num, tot;
    char *fncall;

    va_start(args, code);

    if (code == FAILURE || code == ERR_MEM_ALLOC)
        fprintf(stderr, code == ERR_MEM_ALLOC ? "ERROR ->\t%s" : "TERMINATED ->\t%s", msg[code]);
    else if (code == TERMINATE || code == ERR_FOUND_ASSEMBLER) {
        fncall =  va_arg(args, char *);
        fprintf(stderr, code == TERMINATE ? "INTERNAL ERROR ->\t" : "TERMINATED ->\t");
        fprintf(stderr, msg[code], fncall);
    }
    else if (code >= ERR_OPEN_FILE && code <= ERR_MISSING_ENDMCR) {
        fprintf(stderr, "ERROR ->\t");
        fc = va_arg(args, file_context*);
        fprintf(stderr, msg[code], fc->file_name, fc->lc);
    }
    else if (code == ERR_PRE) {
        fprintf(stderr, "ERROR ->\t");
        num = va_arg(args, int);
        tot = va_arg(args, int);
        fncall = va_arg(args, char*);
        fprintf(stderr, msg[code], num, tot, fncall);
    }
    va_end(args);
    fprintf(stderr, "\n");
}

/**
 * Handles and reports progress messages during the assembly process.
 *
 * Handles different progress codes and formats the progress messages accordingly.
 * Additional arguments may be required for specific progress messages.
 *
 * @param code      The progress code indicating the type of progress.
 * @param ...       Additional arguments depending on the progress code.
 */
void handle_preprocessor_progress(status_error_code code, ...) {
    va_list args;
    file_context *fc;
    int num, tot;

    va_start(args, code);
    if (code == NO_ERROR)
        printf(msg[code], va_arg(args, char*));
    else {
        if (code <= OPEN_FILE) {
            fc = va_arg(args, file_context*);
            printf(msg[code], fc->file_name);
        }
        else if (code == PRE_FILE_OK) {
            fc = va_arg(args, file_context*);
            num = va_arg(args, int);
            tot = va_arg(args, int);
            printf(msg[code], num, tot, fc->file_name);
        }
        else
        fprintf(stderr, "INTERNAL ERROR ->\tInvalid function call - handle_preprocessor_progress()");
        va_end(args);
    }
    printf("\n");
}


/* This function receives line number as a parameter and prints a detailed error message
   accordingly to the error global variable */
void write_preprocessor_error(int line_num)
{
    fprintf(stderr, "ERROR (line %d): ", line_num);

    switch (err)
    {
        case SYNTAX_ERR:
            fprintf(stderr, "first non-blank character must be a letter or a dot.\n");

            break;

        case LABEL_ALREADY_EXISTS:
            fprintf(stderr, "label already exists.\n");

            break;

        case LABEL_TOO_LONG:
            fprintf(stderr, "label is too long (LABEL_MAX_LENGTH: %d).\n", LABEL_LENGTH);

            break;

        case LABEL_INVALID_FIRST_CHAR:
            fprintf(stderr, "label must start with an alphanumeric character.\n");

            break;

        case LABEL_ONLY_ALPHANUMERIC:
            fprintf(stderr, "label must only contain alphanumeric characters.\n");

            break;

        case LABEL_CANT_BE_COMMAND:
            fprintf(stderr, "label can't have the same name as a command.\n");

            break;

        case LABEL_CANT_BE_REGISTER:
            fprintf(stderr, "label can't have the same name as a register.\n");

            break;

        case LABEL_ONLY:
            fprintf(stderr, "label must be followed by a command or a directive.\n");

            break;

        case DIRECTIVE_NO_PARAMS:
            fprintf(stderr, "directive must have parameters.\n");

            break;

        case DIRECTIVE_INVALID_NUM_PARAMS:
            fprintf(stderr, "illegal number of parameters for a directive.\n");

            break;

        case DATA_COMMAS_IN_A_ROW:
            fprintf(stderr, "incorrect usage of commas in a .data directive.\n");

            break;

        case DATA_EXPECTED_NUM_OR_CONST:
            fprintf(stderr, ".data expected a numeric parameter or const\n");

            break;

        case DATA_EXPECTED_COMMA_AFTER_NUM:
            fprintf(stderr, ".data expected a comma after a numeric parameter.\n");

            break;

        case DATA_UNEXPECTED_COMMA:
            fprintf(stderr, ".data got an unexpected comma after the last number.\n");

            break;

        case STRING_TOO_MANY_OPERANDS:
            fprintf(stderr, ".string must contain exactly one parameter.\n");

            break;

        case STRING_OPERAND_NOT_VALID:
            fprintf(stderr, ".string operand is invalid.\n");

            break;

        case STRUCT_INVALID_NUM:
            fprintf(stderr, ".struct first parameter must be a number.\n");

            break;

        case STRUCT_EXPECTED_STRING:
            fprintf(stderr, ".struct must have 2 parameters.\n");

            break;

        case STRUCT_INVALID_STRING:
            fprintf(stderr, ".struct second parameter is not a string.\n");

            break;

        case STRUCT_TOO_MANY_OPERANDS:
            fprintf(stderr, ".struct must not have more than 2 operands.\n");

            break;

        case EXPECTED_COMMA_BETWEEN_OPERANDS:
            fprintf(stderr, ".struct must have 2 operands with a comma between them.\n");

            break;

        case EXTERN_NO_LABEL:
            fprintf(stderr, ".extern directive must be followed by a label.\n");

            break;

        case EXTERN_INVALID_LABEL:
            fprintf(stderr, ".extern directive received an invalid label.\n");

            break;

        case EXTERN_TOO_MANY_OPERANDS:
            fprintf(stderr, ".extern must only have one operand that is a label.\n");

            break;

        case COMMAND_NOT_FOUND:
            fprintf(stderr, "invalid command or directive.\n");

            break;

        case COMMAND_UNEXPECTED_CHAR:
            fprintf(stderr, "invalid syntax of a command.\n");

            break;

        case COMMAND_TOO_MANY_OPERANDS:
            fprintf(stderr, "command can't have more than 2 operands.\n");

            break;

        case COMMAND_INVALID_METHOD:
            fprintf(stderr, "operand has invalid addressing method.\n");

            break;
        case COMMAND_INVALID_INDEX:
            fprintf(stderr,"invalid index or array name\n");
            break;

        case COMMAND_INVALID_NUMBER_OF_OPERANDS:
            fprintf(stderr, "number of operands does not match command requirements.\n");

            break;

        case COMMAND_INVALID_OPERANDS_METHODS:
            fprintf(stderr, "operands' addressing methods do not match command requirements.\n");

            break;

        case ENTRY_LABEL_DOES_NOT_EXIST:
            fprintf(stderr, ".entry directive must be followed by an existing label.\n");

            break;

        case ENTRY_CANT_BE_EXTERN:
            fprintf(stderr, ".entry can't apply to a label that was defined as external.\n");

            break;

        case COMMAND_LABEL_DOES_NOT_EXIST:
            fprintf(stderr, "label does not exist.\n");
            break;
        case METHOD_IMMEDIATE_INPUT_INVALID:
            fprintf(stderr, "method immediate is not number or predefined const\n");
            break;

        case CANNOT_OPEN_FILE:
            fprintf(stderr, "there was an error while trying to open the requested file.\n");
            break;

        case DEFINE_MISSING_EQUALS:
            fprintf(stderr, "Define missing =.\n");
            break;
        case DEFINE_INVALID_VALUE:
            fprintf(stderr, "Define invalid values.\n");
            break;
        case DEFINE_INVALID_LABEL:
            fprintf(stderr, "Define invalid LABEL.\n");
            
    }
}
