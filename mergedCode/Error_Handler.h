/*=======================================================================================================
Project: Maman 14 - Assembler
Created by:
Edrehy Tal and Liberman Ron Rafail

Date: 18/04/2024
========================================================================================================= */

#ifndef ASSEMBLER_ERRORS_H
#define ASSEMBLER_ERRORS_H

#define MSG_LEN 14
extern const char *msg[MSG_LEN];

typedef enum {
    NO_ERROR=0,
    FAILURE,
    ERR_MEM_ALLOC,
    TERMINATE,
    ERR_OPEN_FILE,
    OPEN_FILE,
    ERR_FOUND_ASSEMBLER,
    ERR_EXTRA_TEXT,
    ERR_LINE_TOO_LONG,
    ERR_MISSING_MCR,
    ERR_MISSING_ENDMCR,
    ERR_INVAL_MACRO_NAME,
    ERR_PRE,
    PRE_FILE_OK
} status_error_code;

void handle_preprocessor_error(status_error_code code, ...);
void handle_preprocessor_progress(status_error_code code, ...);
void write_preprocessor_error(int line_num); /* This function is called when an error output is needed */


#endif
