/*=======================================================================================================
Project: Maman 14 - Assembler
Created by: Edrehy Tal and Liberman Ron Rafail
Date: 18/04/2024
Description: This header file declares the prototypes for all functions used throughout the assembler.
This includes functions for processing assembly instructions and directives, managing symbol tables,
and generating output files.
========================================================================================================= */

#ifndef PROTOTYPES_H
#define PROTOTYPES_H

#include "structs.h"

/* Assembly processing functions for the first and second passes */
void first_pass(FILE *fp); /* Processes the first pass of the assembly input. */
void second_pass(FILE *fp, char *filename); /* Processes the second pass of the assembly input. */

/* Functions for constructing and managing assembly instructions */
unsigned int build_first_word(int type, int is_first, int is_second, int first_method, int second_method); /* Constructs the first word of a command based on type and addressing methods. */
int calculate_command_num_additional_words(int is_first, int is_second, int first_method, int second_method); /* Calculates the number of additional words required for a command. */
boolean command_accept_methods(int type, int first_method, int second_method); /* Checks if command type accepts the provided addressing methods. */
boolean command_accept_num_operands(int type, boolean first, boolean second); /* Determines if the command type accepts the provided number of operands. */
int detect_method(char *operand); /* Identifies the addressing method of an operand. */
int handle_command(int type, char *line); /* Processes an assembly command by parsing and validating its syntax and encoding it into machine code. */
int handle_data_directive(char *line); /* Processes a .data directive, encoding numeric data into memory. */
int handle_directive(int type, char *line); /* Dispatches processing of different assembly directives. */
int handle_extern_directive(char *line); /* Handles the .extern directive by extracting and validating the label. */
int handle_string_directive(char *line); /* Processes a .string directive, encoding a string into memory. */
int handle_define_directive(char *line); /* Processes a .define directive, defining constants. */

/* Label and string manipulation functions */
boolean is_label(char *token, int colon); /* Validates whether a token qualifies as a label. */
int num_words(int method); /* Determines the number of additional words required for an addressing method. */
void analyze_line(char *line); /* Reads and processes a line of assembly code. */
void write_num_to_data(int num); /* Encodes a numeric value into the data memory array. */
void write_string_to_data(char *str); /* Encodes a string into the data memory array. */

/* Functions for the second pass of assembly processing */
unsigned int build_register_word(boolean is_dest, char *reg); /* Builds a word representing a register operand. */
void check_operands_exist(int type, boolean *is_src, boolean *is_dest); /* Determines if operands are required for a command. */
int encode_additional_words(char *src, char *dest, boolean is_src, boolean is_dest, int src_method, int dest_method); /* Handles the encoding of additional words for assembly language instructions. */
void encode_additional_word(boolean is_dest, int method, char *operand); /* Encodes additional words for assembly language instructions. */
void encode_label(char *label); /* Encodes a label into machine code. */
int handle_command_second_pass(int type, char *line); /* Manages command encoding in the second pass. */
void analyze_line_second_pass(char *line); /* Reads and processes lines in the second pass. */

/* Output file generation functions */
void write_output_entry(FILE *fp); /* Writes entry symbols to the .ent output file. */
void write_output_extern(FILE *fp); /* Writes external symbols to the .ext output file. */
int write_output_files(char *original); /* Generates output files for the assembly program. */
void write_output_ob(FILE *fp); /* Writes the assembled output to the .ob file. */

/* Additional utilities for handling formatted strings */
int get_number(char* formatted_string); /* Extracts a number from a formatted string. */
char* get_string_name(char* formatted_string); /* Extracts a string name from a formatted string. */

#endif
