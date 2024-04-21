#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include "Error_Handler.h"
#include "Utils.h"

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
void handle_error(status_error_code code, ...) {
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
    else if (code >= ERR_OPEN_FILE && code <= ERR_MISSING_ENDMACRO) {
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
void handle_progress(status_error_code code, ...) {
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
        fprintf(stderr, "INTERNAL ERROR ->\tInvalid function call - handle_progress()");
        va_end(args);
    }
    printf("\n");
}
