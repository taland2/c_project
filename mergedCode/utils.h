/*=======================================================================================================
Project: Maman 14 - Assembler
Created by:
Edrehy Tal and Liberman Ron Rafail

Date: 18/04/2024
========================================================================================================= */
#ifndef ASSEMBLER_UTILS_H
#define ASSEMBLER_UTILS_H
#include "structs.h"
#include "Error_Handler.h"


#define FILE_EXT_LEN 3 /* .as */
#define MAX_LINE_LENGTH 80 /* 80 - Using strlen */
#define MAX_BUFFER_LENGTH 256

#define FILE_MODE_READ "r"
#define FILE_MODE_WRITE_PLUS "w+"
#define ASSEMBLY_EXT ".as"
#define PREPROCESSOR_EXT  ".am"

/* Helper functions that are used for parsing tokens and navigating through them */
char *next_token_string(char *dest, char *line);
char *next_list_token(char *dest, char *line);
char *next_token(char *seq);
char *skip_spaces(char *ch);
void extract_token(char *dest, char *line);
int end_of_line(char *line);
int ignore(char *line);

/* Helper functions that are used to determine types of tokens */
int find_index(char *token, const char *arr[], int n);
int find_command(char *token);
int find_directive(char *token);
boolean is_string(char *string);
boolean is_number(char *seq);
boolean is_register(char *token);

/* Helper functions that are used for creating files and assigning required extensions to them */
char *create_file_name(char *original, int type);
FILE *open_file(char *filename, int type);
char *convert_to_base_4(unsigned int num);

/* Functions of external labels positions' linked list */
extPtr add_ext(extPtr *hptr, char *name, unsigned int reference);
void free_ext(extPtr *hptr);
void print_ext(extPtr h);

/* Functions of symbols table */
labelPtr add_label(labelPtr *hptr, char *name, unsigned int address, char *property,boolean external, ...);
int delete_label(labelPtr *hptr, char *name);
void free_labels(labelPtr *hptr);
void offset_addresses(labelPtr label, int num, boolean is_data);
unsigned int get_label_address(labelPtr h, char *name);
labelPtr get_label(labelPtr h, char *name);
boolean is_existing_label(labelPtr h, char *name);
boolean is_external_label(labelPtr h, char *name);
int make_entry(labelPtr h, char *name);
void print_labels(labelPtr h);

/* Functions that handle errors */
int is_error();

/* Helper functions for encoding and building words */
unsigned int extract_bits(unsigned int word, int start, int end);
void encode_to_instructions(unsigned int word);
unsigned int insert_are(unsigned int info, int are);



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