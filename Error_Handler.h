#ifndef ASSEMBLER_ERRORS_H
#define ASSEMBLER_ERRORS_H

#define MSG_LEN 14
extern const char *msg[MSG_LEN];

typedef enum {
    NO_ERROR,
    FAILURE,
    ERR_MEM_ALLOC,
    TERMINATE,
    ERR_OPEN_FILE,
    OPEN_FILE,
    ERR_FOUND_ASSEMBLER,
    ERR_EXTRA_TEXT,
    ERR_LINE_TOO_LONG,
    ERR_MISSING_MACRO,
    ERR_MISSING_ENDMACRO,
    ERR_INVAL_MACRO_NAME,
    ERR_PRE,
    PRE_FILE_OK
} status_error_code;

void handle_error(status_error_code code, ...);
void handle_progress(status_error_code code, ...);

#endif
