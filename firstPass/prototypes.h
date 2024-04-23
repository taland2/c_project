/*=======================================================================================================
Project: Maman 14 - Assembler
Created by:
Edrehy Tal and Liberman Ron Rafail

Date: 18/04/2024
========================================================================================================= */

#ifndef PROTOTYPES_H

#define PROTOTYPES_H
#include "structs.h"

void first_pass(FILE *fp);
void second_pass(FILE *fp, char *filename);
/*----------------------------------------Function Prototypes Declarations---------------------------------*/

/*------------------ First Pass Functions ------------------
 *Description:
 * In the first cycle: if local label or external labels are found,
 * going to function to save it, and continue to 
 * with needed arguments.
 */

unsigned int build_first_word(int type, int is_first, int is_second, int first_method, int second_method);/* This func constructs the first word for use within the first word value of machine code.*/
int calculate_command_num_additional_words(int is_first, int is_second, int first_method, int second_method); /*This func calculates the number of additional words required for a command.*/
boolean command_accept_methods(int type, int first_method, int second_method);/*This func checks whether the command type accepts the provided addressing methods for its operands, ensuring compatibility based on the specific requirements defined for each command type.*/
boolean command_accept_num_operands(int type, boolean first, boolean second);/*This func determines whether a given command type accepts the provided number of operands, returning TRUE if it does and FALSE otherwise, based on the requirements specified for each command type.*/
int detect_method(char * operand);/*This func detects the addressing method of a given operand in assembly language. the function returns the corresponding method code. If not, it sets an error code and returns "NOT_FOUND".*/
int handle_command(int type, char *line);/*This func handles assembly language commands, validating their syntax, including the number of operands and their addressing methods, and encoding them into our machine code instructions. It reports errors for any issues encountered during parsing or validation, ensuring accurate translation of commands into machine code.*/
int handle_data_directive(char *line);/*This func processes data directives in assembly language by parsing and validating the data elements provided in the directive line. It ensures that the syntax is correct, requiring numbers separated by commas and reporting errors for any issues encountered during parsing. If the data elements are valid, they are encoded into memory for further processing.*/
int handle_directive(int type, char *line);/*This func manages different types of assembly language directives, ensuring their syntax is correct and handling them accordingly.*/
int handle_extern_directive(char *line);/*This func handles the .extern directive in assembly language, extracting and validating the label provided in the directive line, and adding it to the symbol table. If any issues are encountered, such as a missing or invalid label, or too many operands in the directive line, errors are reported. If successful, it returns whether an error occurred during the process.*/
int handle_string_directive(char *line);/*This func processes the .string directive in assembly language. It extracts the string provided in the directive line, validates its format, removes the quotation marks, and encodes the string into memory as ASCII characters. If the string is found to be invalid or if there are too many operands in the directive line, errors are reported. If the operation is successful, it returns without errors.*/
int handle_define_directive(char *line);  /*This function process the constants recived by the define, extract the name and validte it,  errors are reported. If the operation is successful, it returns without errors */
boolean is_label(char *token, int colon);/*This func responsible for validating whether a given token qualifies as a label in assembly language based on various criteria, including length, character composition, and absence of reserved command or register names. If the token meets all requirements, the function returns TRUE; otherwise, it returns FALSE and sets error flags accordingly.*/
int num_words(int method); /*This func determines the number of additional words required for a given addressing method in assembly language instructions. It returns 2 if the method is the index addressing method and 1 otherwise.*/
void read_line(char *line);/*This function reads a line of assembly code, aiming to determine the type of directive or command it represents and whether a label exists in the line. It initializes variables to store the type of directive or command encountered, checks for the presence of a label, and skips any leading whitespace characters. If the line is blank or the first non-whitespace character is not a letter or a dot, a syntax error is reported.*/
void write_num_to_data(int num);/*This function writes a given integer number to the data memory array in the assembly code and advances the data counter to the next memory location.*/
void write_string_to_data(char *str);/*This function writes a string to the data memory array in the assembly code by iterating through each character of the string, converting each character to an unsigned integer, and storing it in consecutive memory locations indicated by the data counter (dc). After writing all characters of the string, it adds a null character ('\0') to terminate the string in the data memory array.*/
/*------------------ Second Pass Functions ------------------
 
 * The second cycle: if exists entry declaration, saving entry labels,
 * and continue to the code parser with needed arguments.
 */
unsigned int build_register_word(boolean is_dest, char *reg);/*This function builds a word representing a register operand in assembly language instructions. It extracts the register number from the given register string, inserts it into the appropriate bits according to whether it's a source or destination operand, and then inserts the Absolute/External/Relocatable (A/R/E) mode to the word before returning it.*/
void check_operands_exist(int type, boolean *is_src, boolean *is_dest);/*This function examines the type of assembly language instruction provided as input and sets boolean flags to indicate whether a source operand and a destination operand are required based on the instruction type. It distinguishes between instructions that require both operands, those that only need a destination operand, and those that don't require any operands at all.*/
int encode_additional_words(char *src, char *dest, boolean is_src, boolean is_dest, int src_method, int dest_method);/*This functio handles the encoding of additional words for assembly language instructions based on the provided source and destination operands, their addressing methods, and whether they are required operands. If both operands are registers and they share the same additional word, a special case is handled where the encoding is performed for both registers together. Otherwise, if either the source or destination operand is present, encoding is performed separately for each operand.*/
void encode_additional_word(boolean is_dest, int method, char *operand);/*This function encodes additional words for assembly language instructions based on various addressing methods like immediate, direct, index, and register, ensuring accurate representation. It handles each addressing mode uniquely, extracting necessary data such as immediate numbers or labels, and encodes them appropriately with the required mode before adding them to the A.R.E set.*/
void encode_label(char *label);/*This function encodes a label into machine code, obtaining its address from the symbols table. If the label exists, it determines whether it's external or relocatable, updates the instruction with the appropriate A.R.E mode, and encodes it into memory. If the label doesn't exist, it increments the instruction counter and sets an error indicating that the label does not exist.*/
int handle_command_second_pass(int type, char *line);/*This function manages the second pass of command encoding, identifying the addressing methods for source and destination operands, extracting operands from the line, associating them with their respective positions, incrementing the instruction counter, and encoding additional words based on the operands' addressing methods.*/
void read_line_second_pass(char *line);/*This function performs the second pass of reading lines, skipping blank lines, labels, and handling directives or commands accordingly. If it encounters a directive, it handles the .entry directive by creating an entry for the symbol. Otherwise, if it identifies a command, it proceeds to encode the command's additional words using the handle_command_second_pass func.*/
void write_output_entry(FILE *fp);/*This function printing the names and addresses of symbols that have an "entry" attribute set to true, separated by a tab character, and then closes the file.*/
void write_output_extern(FILE *fp);/*This function printing the name and address of each symbol to a file, separated by a tab character, and then closes the file.*/
int write_output_files(char *original);/*This function is responsible for generating output files for the assembly program. It first opens the original file to write the object code and writes the object code to it. Then, if there are entry symbols, it opens the original file to write the entry file and writes the entry symbols to it. Similarly, if there are external symbols, it opens the original file to write the extern file and writes the external symbols to it.*/
void write_output_ob(FILE *fp);/*This function writes the output object file (.ob) containing the machine code and data of the assembled program. It first writes the total number of instruction and data words in the program. Then, it iterates through the instructions and data arrays, converting each address and word into base 32 representation and writing them to the file. Finally, it closes the file.*/
int get_number(char* formatted_string);
char* get_string_name(char* formatted_string);

#endif
