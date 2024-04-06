#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Error_Handler.h"
#include "Utils.h"

const char *directives[DIRECTIVE_LEN] = {
    "data",
    "string",
    "entry",
    "extern"
};

const char *commands[COMMANDS_LEN] = {
    "mov",
    "cmp",
    "add",
    "sub",
    "not",
    "clr",
    "lea",
    "inc",
    "dec",
    "jmp",
    "bne",
    "red",
    "prn",
    "jsr",
    "rts",
    "hlt"
};

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
    printf("enters create file");
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
    printf("File name to open: %s\n", fc->file_name ? fc->file_name : "NULL");
    if (fc->file_name_wout_ext == NULL) {
        printf("File name to open:2 %s\n", fc->file_name ? fc->file_name : "NULL");
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
        handle_error(ERR_OPEN_FILE, fc);
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
        handle_error(TERMINATE, "copy_string()");
        return TERMINATE;
    }

    if (!*target) *target = NULL;

    if (!*target) free(*target);

    temp = malloc(strlen(source) + 1);
    if (!temp) {
        handle_error(ERR_MEM_ALLOC);
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
        handle_error(TERMINATE, "copy_n_string()");
        return TERMINATE;
    }

    temp = malloc(count + 1);
    if (!temp) {
        handle_error(ERR_MEM_ALLOC);
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
 * Checks if a given string is a valid Directive.
 *
 * @param src The string to check.
 * @return The corresponding Directive index if the string is a valid Directive, otherwise DEFAULT.
 */
Directive is_directive(const char* src) {
    int i;
    if (src)
        for (i = 0; i < DIRECTIVE_LEN; i++)
            if (strncmp(src, directives[i], strlen(directives[i])) == 0)
                return i + 1; /* Corresponding Directive*/
    return 0;
}

/**
 * Checks if a given string is a valid Command.
 *
 * @param src The string to check.
 * @return The corresponding Command index if the string is a valid Command, otherwise 0.
 */
Command is_command(const char* src) {
    int i;
    if (src)
        for (i = 0; i < COMMANDS_LEN; i++)
            if (strcmp(src, commands[i]) == 0)
                return i; /* Corresponding Command*/
    return INVALID_COMMAND;
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