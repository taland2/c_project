#ifndef ASSEMBLER_PREPROCESSOR_H
#define ASSEMBLER_PREPROCESSOR_H

#include <stdio.h>
#include "Utils.h"
#include "Error_Handler.h"

#define MAX_MACRO_NAME_LENGTH 32 /* 31 + '\0' */
#define MACRO_START "mcr"
#define MACRO_END "endmcr"
#define SKIP_MCRO 3 /* mcr length */
#define SKIP_MCR0_END 7 /* endmcr length */


typedef struct node{
    char* name;
    char* body;
    struct node* next;
} node;


status_error_code assembler_preprocessor(file_context *src, file_context *dest);

status_error_code handle_macro_start(file_context *src, char *line, int *found_macro, char **macro_name, char **macro_body);
status_error_code handle_macro_body(char *line, int found_macro, char **macro_body);
status_error_code handle_macro_end(char *line, int *found_macro, char **macro_name, char **macro_body);
status_error_code write_to_file(file_context *src, file_context *dest, char *line, int found_macro, int found_error);
status_error_code add_macro(char* name, char* body);

node* is_macro_exists(char* name);

void free_macros();

#endif
