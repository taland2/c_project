#ifndef ASSEMBLER_UTILS_H
#define ASSEMBLER_UTILS_H

#include "Error_Handler.h"

#define FILE_EXT_LEN 3 /* .as */
#define MAX_LINE_LENGTH 80 /* 80 - Using strlen */
#define DIRECTIVE_LEN 4
#define MAX_BUFFER_LENGTH 256

#define FILE_MODE_READ "r"
#define FILE_MODE_WRITE_PLUS "w+"
#define ASSEMBLY_EXT ".as"
#define PREPROCESSOR_EXT  ".am"


extern const char *directives[DIRECTIVE_LEN];

typedef enum {
    DATA = 1,
    STRING,
    ENTRY,
    EXTERN,
    DEFAULT 
} Directive;

typedef struct {
    FILE* file_ptr;
    char* file_name;
    char* file_name_wout_ext;
    int lc; /* Line counter */
    int tc; /* total num of files counter */
    int fc; /* file counter (x out of tc) */
} file_context;


void free_file_context(file_context** context);

size_t get_word_length(char **ptr);
status_error_code copy_string(char** target, const char* source);
status_error_code copy_n_string(char** target, const char* source, size_t count);
file_context* create_file_context(const char* file_name, char* ext, size_t ext_len, char* mode, status_error_code *report);
#endif
