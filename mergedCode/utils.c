/*=======================================================================================================
Project: Maman 14 - Assembler
Created by:
Edrehy Tal and Liberman Ron Rafail

Date: 18/04/2024
========================================================================================================= */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#include "prototypes.h"
#include "assembler.h"
#include "extern_variables.h"
#include "utils.h"

const char base4[4] = {
        '*','#','%','!'};

/* This function extracts bits, given start and end positions of the bit-sequence (0 is LSB) */
unsigned int extract_bits(unsigned int word, int start, int end)
{
    unsigned int result;
    int length = end - start + 1; /* Length of bit-sequence */
    unsigned int mask = (int) pow(2, length) - 1; /* Creating a '111...1' mask with above line's length */

    mask <<= start; /* Moving mask to place of extraction */
    result = word & mask; /* The bits are now in their original position, and the rest is 0's */
    result >>= start; /* Moving the sequence to LSB */
    return result;
}

/* Converting a word to 2 digits in base 4 (as a string) */
char *convert_to_base_4(unsigned int num)
{
    char *base4_seq = (char *) malloc(base4_SEQUENCE_LENGTH);

    /* To convert from binary to base 4 we simply takes 2 bits for each digit */
    base4_seq[0] = base4[extract_bits(num, 12, 13)]; /* MSB*/
    base4_seq[1] = base4[extract_bits(num, 10, 11)];
    base4_seq[2] = base4[extract_bits(num, 8, 9)];
    base4_seq[3] = base4[extract_bits(num, 6, 7)];
    base4_seq[4] = base4[extract_bits(num, 4, 5)];
    base4_seq[5] = base4[extract_bits(num, 2, 3)];
    base4_seq[6] = base4[extract_bits(num, 0, 1)];/*LSB*/
    base4_seq[7] = '\0';

    return base4_seq;
}

/* This function checks if a string is a number (all digits) */
boolean is_number(char *seq)
{
    if(end_of_line(seq)) return FALSE;
    if(*seq == '+' || *seq == '-') /* a number can contain a plus or minus sign */
    {
        seq++;
        if(!isdigit(*seq++)) return FALSE; /* but not only a sign */
    }

    /* Check that the rest of the token is made of digits */
    while(!end_of_line(seq))
    {
        if(!isdigit(*seq++)) return FALSE;
    }
    return TRUE;
}

/* This function checks if a given sequence is a valid string (wrapped with "") */
boolean is_string(char *string)
{
    if(string == NULL) return FALSE;

    if (*string == '"') /* starts with " */
        string++;
    else
        return FALSE;

    while (*string && *string != '"') { /* Goes until end of string */
        string++;
    }

    if (*string != '"') /* a string must end with " */
        return FALSE;

    string++;
    if (*string != '\0') /* string token must end after the ending " */
        return FALSE;

    return TRUE;
}

/* This function inserts given A/R/E 2 bits into given info bit-sequence (the info is being shifted left) */
unsigned int insert_are(unsigned int info, int are)
{
    return (info << BITS_IN_ARE) | are; /* OR operand allows insertion of the 2 bits because 1 + 0 = 1 */
}

/* This function creates a file name by appending suitable extension (by type) to the original string */
char *create_file_name(char *original, int type)
{
    char *modified = (char *) malloc(strlen(original) + MAX_EXTENSION_LENGTH);
    if(modified == NULL)
    {
        fprintf(stderr, "Dynamic allocation error.");
        exit(ERROR);
    }

    strcpy(modified, original); /* Copying original filename to the bigger string */

    /* Concatenating the required file extension */

    switch (type)
    {
        case FILE_INPUT:
            strcat(modified, ".as");
            break;
        case FILE_AM:
            strcat(modified, ".am");
            break;

        case FILE_OBJECT:
            strcat(modified, ".ob");
            break;

        case FILE_ENTRY:
            strcat(modified, ".ent");
            break;

        case FILE_EXTERN:
            strcat(modified, ".ext");

    }
    return modified;
}

/* This function inserts a given word to instructions memory */
void encode_to_instructions(unsigned int word)
{
    instructions[ic++] = word;
}

/* This functions returns 1 if there's an error (AKA: global variable err has changed) */
int is_error()
{
    return err != NO_ERROR;
}


/* This function copies the next token of a list (comma separated e.x. 1, "abc", 4) to a dest array.
 * Returns a pointer to the first character after the token
 */
char *next_list_token(char *dest, char *line)
{
    char *temp = dest;

    if(end_of_line(line)) /* If the given line is empty, copy an empty token */
    {
        dest[0] = '\0';
        return NULL;
    }

    if(isspace(*line)) /* If there are spaces in the beginning of the token, skip them */
        line = skip_spaces(line);

    if(*line == ',') /* A comma deserves a separate, single-character token */
    {
        strcpy(dest, ",");
        return ++line;
    }

    /* Manually copying token until a ',', whitespace or end of line */
    while(!end_of_line(line) && *line != ',' && !isspace(*line))
    {
        *temp = *line;
        temp++;
        line++;
    }
    *temp = '\0';

    return line;
}

/* This function copies supposedly next string into dest array and returning a pointer to the
 * first character after it
 */
char *next_token_string(char *dest, char *line)
{
    char temp[LINE_LENGTH];
    line = next_list_token(dest, line);
    if(*dest != '"') return line;
    while(!end_of_line(line) && dest[strlen(dest) - 1] != '"')
    {
        line = next_list_token(temp, line);
        if(line) strcat(dest, temp);
    }
    return line;
}

/* Checking for the end of line/given token in the character that char* points to */
int end_of_line(char *line)
{
    return line == NULL || *line == '\0' || *line == '\n';
}

/* This function returns a pointer to the start of next token in the line */
char *next_token(char *seq)
{
    if(seq == NULL) return NULL;
    while(!isspace(*seq) && !end_of_line(seq)) seq++; /* Skip rest of characters in the current token (until a space) */
    seq = skip_spaces(seq); /* Skip spaces */
    if(end_of_line(seq)) return NULL;
    return seq;
}

/* This function copies the next token (until a space or end of line) to a destination string */
void extract_token(char *dest, char *line)
{
    int i = 0;
    if(dest == NULL || line == NULL) return;

    while(i < LINE_LENGTH && !isspace(line[i]) && line[i] != '\0') /* Copying token until its end to *dest */
    {
        dest[i] = line[i];
        i++;
    }
    dest[i] = '\0';
}

/* This function finds an index of a string in an array of strings */
int find_index(char *token, const char *arr[], int n)
{
    int i;
    for(i = 0; i < n; i++)
        if (strcmp(token, arr[i]) == 0)
            return i;
    return NOT_FOUND;
}

/* Check if a token matches a register name */
boolean is_register(char *token)
{
    /* A register must have 2 characters, the first is 'r' and the second is a number between 0-7 */
    return strlen(token) == REGISTER_LENGTH && token[0] == 'r' &&
            token[1] >= '0' &&
            token[1] <= '7';
}

/* Check if a token matches a directive name */
int find_directive(char *token)
{
    if(token == NULL || *token != '.') return NOT_FOUND;
    return find_index(token, directives, NUM_DIRECTIVES);
}

/* Check if a token matches a command name */
int find_command(char *token)
{
    int token_len = strlen(token);
    if(token_len > MAX_COMMAND_LENGTH || token_len < MIN_COMMAND_LENGTH)
        return NOT_FOUND;
    return find_index(token, commands, NUM_COMMANDS);
}

/* This function skips spaces of a string and returns a pointer to the first non-blank character */
char *skip_spaces(char *ch)
{
    if(ch == NULL) return NULL;
    while (isspace(*ch)) /* Continue the loop if the character is a whitespace */
        ch++;
    return ch;
}

/* Function that ignores a line if it's blank/a comment */
int ignore(char *line)
{
    line = skip_spaces(line);
    return *line == ';' || *line == '\0' || *line == '\n';
}





/**
 * Creates a file context object, add extension to file name,
 * and opens the file in the specified mode.
 *
 * @param file_name The name of the file.
 * @param ext The extension to append to the file name.
 * @param ext_len The length of the extension.
 * @param mode The file mode for opening the file (e.g., "r" for read, "w" for write).
 * @param report Pointer to the status_error_code report variable.
 * @return A pointer to the created file context object if successful, NULL otherwise.
 */
file_context* create_file_context(const char* file_name, char* ext, size_t ext_len, char* mode, status_error_code *report) {
     file_context* fc = NULL;
    FILE* file = NULL;
    char *file_name_w_ext = NULL;
    size_t len;
    /* Allocate memory for the file_context structure */
    fc = (file_context*)malloc(sizeof(file_context));
    if (fc == NULL) {
        *report = ERR_MEM_ALLOC;
        return NULL; /* Directly return NULL to indicate failure */
    }

    /* Calculate length needed for the file name with extension, including space for null terminator */
    len = strlen(file_name) + ext_len + 1;
    file_name_w_ext = (char*)malloc(len * sizeof(char));
    if (file_name_w_ext == NULL) {
        *report = ERR_MEM_ALLOC;
        free(fc); /* Free the allocated file_context structure */
        return NULL; /* Early return if memory allocation fails */
    }

    /* Construct the full file name with its extension */
    strcpy(file_name_w_ext, file_name);
    strcat(file_name_w_ext, ext);

    /* Manually duplicate the file_name to file_name_wout_ext for keeping a version without extension */
    fc->file_name_wout_ext = (char*)malloc(strlen(file_name) + 1);
    if (fc->file_name_wout_ext == NULL) {
        *report = ERR_MEM_ALLOC;
        free(file_name_w_ext); /* Free the constructed file name string */
        free(fc); /* Free the allocated file_context structure */
        return NULL; /* Early return if memory allocation fails */
    }
    strcpy(fc->file_name_wout_ext, file_name);

    /* Duplicate the constructed file name with extension to fc->file_name */
    fc->file_name = (char*)malloc(strlen(file_name_w_ext) + 1);
    if (fc->file_name == NULL) {
        *report = ERR_MEM_ALLOC;
        free(fc->file_name_wout_ext); /* Free the version without extension */
        free(file_name_w_ext); /* Free the constructed file name string */
        free(fc); /* Free the allocated file_context structure */
        return NULL; /* Early return if memory allocation fails */
    }
    strcpy(fc->file_name, file_name_w_ext);

    /* Attempt to open the file with the constructed file name */
    file = fopen(fc->file_name, mode);
    if (file == NULL) {
        handle_preprocessor_error(ERR_OPEN_FILE, fc);
        *report = ERR_OPEN_FILE;
        free(fc->file_name); /* Free the file name string */
        free(fc->file_name_wout_ext); /* Free the version without extension */
        free(fc); /* Free the file_context structure */
        free(file_name_w_ext); /* Free the constructed file name string */
        return NULL; /* Early return if file opening fails */
    }

    /* Set the remaining fields of the file_context structure */
    fc->file_ptr = file;
    fc->lc = 1; /* Initialize line count */
    *report = NO_ERROR;

    /* Cleanup the temporary file name string */
    free(file_name_w_ext);

    return fc; /* Return the constructed file_context structure */
}

/**
 * Finds the length of the consecutive characters in a word, skipping leading white spaces.
 * Updates the pointer to point to the start of the word.
 *
 * @param ptr Pointer to the input string. Updated to point to the start of the word.
 * @return Length of the word.
 */
size_t get_word_length(char **ptr) {
    char *start;
    size_t length = 0;

    if (!**ptr)
        return 0;

    while (**ptr && isspace((int)**ptr))
        (*ptr)++;

    start = *ptr;
    while (**ptr && !isspace((int)**ptr)) {
        (*ptr)++;
        length++;
    }

    *ptr = start;
    return length;
}


/**
 * Copy a string from source to target and allocate memory for the target
 * according to the source.
 *
 * @param target  Pointer to the target string.
 * @param source  Pointer to the source string.
 * @return        Status: NO_ERROR if successful, otherwise the error status_error_code.
 */
status_error_code copy_string(char** target, const char* source) {
    char* temp = NULL;
    if (!source) {
        handle_preprocessor_error(TERMINATE, "copy_string()");
        return TERMINATE;
    }

    if (!*target) *target = NULL;

    if (!*target) free(*target);

    temp = malloc(strlen(source) + 1);
    if (!temp) {
        handle_preprocessor_error(ERR_MEM_ALLOC);
        return ERR_MEM_ALLOC;
    }

    strcpy(temp, source);
    temp[strlen(source)] = '\0';

    *target = temp;
    return NO_ERROR;
}

/**
 * Copy the first count characters from a source string to target string,
 * and allocate memory to target according to source.
 *
 * @param target  Target string.
 * @param source  Source string.
 * @param count number of characters to copy
 *
 * @return status_error_code, NO_ERROR in case of no error otherwise else the error status_error_code.
 */
status_error_code copy_n_string(char** target, const char* source, size_t count) {
    char* temp = NULL;
    if (!source) {
        handle_preprocessor_error(TERMINATE, "copy_n_string()");
        return TERMINATE;
    }

    temp = malloc(count + 1);
    if (!temp) {
        handle_preprocessor_error(ERR_MEM_ALLOC);
        return ERR_MEM_ALLOC;
    }
    strncpy(temp, source, count);
    temp[count] = '\0';

    if (!*target)*target = NULL;

    if (!*target) free(*target);

    *target = temp;
    return NO_ERROR;
}

/**
 * Frees the memory occupied by a file_context structure.
 * Closes the file pointer if it's open and frees the dynamically allocated file name.
 *
 * @param context The file_context structure to be freed.
 */
void free_file_context(file_context** context) {
    if (*context != NULL) {
        if ((*context)->file_ptr != NULL)
            fclose((*context)->file_ptr);

        if ((*context)->file_name != NULL)
            free((*context)->file_name);

        if ((*context)->file_name_wout_ext != NULL)
            free((*context)->file_name_wout_ext);

        free(*context);
        *context = NULL;
    }
}