#ifndef ASSEMBLER_UTILS_H
#define ASSEMBLER_UTILS_H

#include "Error_Handler.h"

#define FILE_EXT_LEN 3 /* .as */
#define FILE_EXT_LEN_OUT 4 /* .obj */
#define MAX_LINE_LENGTH 80 /* 80 - Using strlen */
#define MAX_LABEL_LENGTH 32 /*  31 + '\0' */
#define DIRECTIVE_LEN 4
#define COMMANDS_LEN 16
#define MAX_BUFFER_LENGTH 256

#define FILE_MODE_READ "r"
#define FILE_MODE_WRITE_PLUS "w+"
#define ASSEMBLY_EXT ".as"
#define PREPROCESSOR_EXT  ".am"
#define OBJECT_EXT  ".ob"
#define ENTRY_EXT  ".ent"
#define EXTERNAL_EXT ".ext"

#define ASSEMBLER_PASSES_H

#define INVALID_ADDRESS (-1)
#define BASE64_CHARS 2
#define A_R_E_BINARY_LEN 2
#define SRC_DEST_OP_BINARY_LEN 3
#define OPCODE_BINARY_LEN 4
#define BINARY_BASE64_BITS 6
#define REGISTER_BINARY_LEN 5
#define ADDRESS_BINARY_LEN 10
#define DEFAULT_DATA_IMAGE_CAP 5
#define BINARY_BITS 12
#define AMT_WORD_4 4
#define AMT_WORD_2 2
#define AMT_WORD_1 1
#define ADDRESS_START 100
#define MAX_MEMORY_SIZE 1024

extern const char *directives[DIRECTIVE_LEN];
extern const char *commands[COMMANDS_LEN];

typedef enum {
    ABSOLUTE,
    EXTERNAL,
    RELOCATABLE
} ARE;

typedef enum {
    LBL,
    NUM,
    STR,
    INV
} Value;

typedef enum {
    DATA = 1,
    STRING,
    ENTRY,
    EXTERN,
    DEFAULT /* for .obj */
} Directive;

typedef enum { 
    IMMEDIATE = 0,
    DIRECT = 1,
    INDEX =2,
    REGISTER = 3,
    INVALID_MD = 4
} Adrs_mod;

typedef enum {
    MOV,
    CMP,
    ADD,
    SUB,
    NOT,
    CLR,
    LEA,
    INC,
    DEC,
    JMP,
    BNE,
    RED,
    PRN,
    JSR,
    RTS,
    HLT,
    INVALID_COMMAND
} Command;

typedef enum {
    SPACE,
    COMMA,
    COLON,
    QUOTE
} Delimiter;

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
Command is_command(const char* src);
Directive is_directive(const char* src);

file_context* create_file_context(const char* file_name, char* ext, size_t ext_len, char* mode, status_error_code *report);
#endif
