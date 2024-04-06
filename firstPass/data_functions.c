#include "symbol_table.h"
#include "syntax.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef enum data_type { DATA, STRING, ENTRY, EXTERN, INVALID_DATA } data_type;

/* max: returns the maximum value between two integers */
int max(int a, int b) { return a > b ? a : b; }

/* get_label: returns the label of the source line */
char *get_label(src_op_line *srcline) { return srcline->label; }

/* get_label_length: returns the length of the label */
size_t get_label_length(src_op_line *srcline) { return strlen(srcline->label); }

/* copy_datatype: returns the data type as a string */
char *copy_datatype(const char *aligned_instruction) {
    size_t start = 0, end = 0;
    char *temp_string = NULL;

    start = strcspn(aligned_instruction, ".");
    if (start == strlen(aligned_instruction)) {
        return NULL;
    }

    end = start + strcspn(aligned_instruction + start, " \n");

    temp_string = (char *)malloc(end - start + 1);
    if (!temp_string) {
        perror("Memory allocation failed for datatype string");
        return NULL;
    }

    strncpy(temp_string, aligned_instruction + start, end - start);
    temp_string[end - start] = '\0';

    return temp_string;
}

/* get_datatype: returns the data type of the source line */
data_type get_datatype(const char *aligned_instruction) {
    int i;
    data_type type = INVALID_DATA;
    const char *data_instructions[] = {".data", ".string", ".entry", ".extern",
                                       ".error"};
    char *temp_string = copy_datatype(aligned_instruction);
    if (!temp_string) {
        return INVALID_DATA;
    }

    for (i = 0; i < sizeof(data_instructions) / sizeof(data_instructions[0]);
         i++) {
        if (strcmp(temp_string, data_instructions[i]) == 0) {
            type = (data_type)i;
            break;
        }
    }
    free(temp_string);
    return type;
}

/* install_entry: the function only prints warnning. entries are handled
    separatly from the second pass file */
void install_entry(src_op_line *srcline, machine_word **data_img) {
    if (strlen(srcline->label) > 0)
        printf("%s:%d: warrning: label '%s' before an enrty\n",
               srcline->as_filename, srcline->line_num, srcline->label);
}

/* get_string_start: returns the index of the first character in the string */
int get_string_start(src_op_line *srcline) {
    int start = strlen("  .string \"");

    if (strlen(srcline->label) > 0) { /* true if ther's a label to install */
        if (install(get_label(srcline), srcline->cur_dc + 1, ".data",
                    srcline) == NULL) {
            printf("%s:%d: error: symbol '%s' aleady defined\n",
                   srcline->as_filename, srcline->line_num, srcline->label);
        }
        start += strlen(srcline->label);
    }

    return start;
}

/* check_quotes: prints an error message if the string doesn't end with quotes
 */
void check_quotes(src_op_line *srcline, int start, int ending) {
    if (srcline->alignedsrc[ending] != '"') { /* miss quotes at string's end */
        printf("%s:%d: error: missing '\"' at the end of sting '%.*s'\n",
               srcline->as_filename, srcline->line_num, ending - start + 1,
               &srcline->alignedsrc[start]); /* print only the string content */
        srcline->error_flag = EXIT_FAILURE;
    }
}

/* data_img_malloc: allocates memory for the data image */
int data_img_malloc(machine_word **data_img, src_op_line *srcline) {
    if ((data_img[srcline->cur_dc] =
             (machine_word *)malloc(sizeof(machine_word))) == NULL) {
        printf("%s:%d: error: data image assress '%d' unavailable\n",
               srcline->as_filename, srcline->line_num, srcline->cur_dc);
        return 0;
    }
    return 1;
}

/* install_string: creates a binary-word representation for every character
   in the string after the special word ".string". The function also updates
   the sign of the number and handles errors regarding invalid numbers. */
void install_string(src_op_line *srcline, machine_word **data_img) {
    int ending, start;

    ending = 0; /* a variable to represent the end of a string (closin "") */
    start = 0;  /* same for the beginning of the string */

    if (!data_img_malloc(data_img, srcline)) {
        return;
    }

    start = get_string_start(srcline);
    ending = strlen(srcline->alignedsrc) - 2; /* -1 for '\"' and -2 for '\n' */

    check_quotes(srcline, start, ending);

    for (; start < ending; start++) {
        data_img[srcline->cur_dc]->funct_nd_ops =
            (int)srcline->alignedsrc[start];
        srcline->cur_dc++;

        if ((data_img[srcline->cur_dc] =
                 (machine_word *)malloc(sizeof(machine_word))) == NULL) {
            printf("%s:%d: error: data image assress '%d' unavailable\n",
                   srcline->as_filename, srcline->line_num, srcline->cur_dc);
            return;
        } /* allocate memory to the data image for the next character */
    }     /* end of string to install */

    data_img[srcline->cur_dc]->funct_nd_ops = (int)'\0';
    srcline->cur_dc++;
    return;
}

/* is_valid_number_end: returns true if the character at the given position
   is a valid character to end a number */
int is_valid_number_end(src_op_line *srcline, int pos) {
    return (srcline->alignedsrc[pos] == ',') ||
           (srcline->alignedsrc[pos] == '\n') ||
           (srcline->alignedsrc[pos] == ' ');
}

/* is_sign: returns true if the character at the given position is a sign */
int is_sign(src_op_line *srcline, int pos) {
    return (srcline->alignedsrc[pos] == '+') ||
           (srcline->alignedsrc[pos] == '-');
}

/* get_sign: returns the sign of the number at the given position */
int get_sign(src_op_line *srcline, int pos) {
    return (srcline->alignedsrc[pos] == '-') ? -1 : 1;
}

/* handle_sign: updates the sign of the number at the given position */
void handle_sign(src_op_line *srcline, int *pos, int *sign) {
    if (is_sign(srcline, *pos)) {
        *sign = get_sign(srcline, *pos);
        (*pos)++;
    }
}

/* has_data_img_changed: prints an error message if the data image hasn't
   changed */
void has_data_img_changed(machine_word **data_img, src_op_line *srcline,
                          int old_dc) {
    if (!(srcline->cur_dc - old_dc)) { /* if data image hasn't changed */
        printf("%s:%d: error: data instruction must contains at least one "
               "number\n",
               srcline->as_filename, srcline->line_num);
        srcline->error_flag = EXIT_FAILURE;
    }
}

/* check_missing_number: prints an error message if the data instruction
   contains a missing number */
void check_missing_number(src_op_line *srcline, int after_comma,
                          int was_number) {
    if ((after_comma == 1) && (was_number == 0)) { /*missing number*/
        printf("%s:%d: error: missing a number after ','\n",
               srcline->as_filename, srcline->line_num);
        srcline->error_flag = EXIT_FAILURE;
    }
}

/* skip_spaces_until_comma: skips spaces until a comma is found */
int skip_spaces_until_comma(src_op_line *srcline, int *pos) {
    int after_comma = 0;
    while ((*pos < strlen(srcline->alignedsrc)) &&
           (srcline->alignedsrc[*pos] != ',')) {
        (*pos)++;
    }
    after_comma = (srcline->alignedsrc[*pos] == ',') ? 1 : 0;
    if (srcline->alignedsrc[*pos] == ' ')
        (*pos)++;
    return after_comma;
}

/* invalid_number: prints an error message for an invalid number */
void invalid_number(src_op_line *srcline, int pos) {
    /* make sure its not end of line */
    if (srcline->alignedsrc[pos] == '\n') {
        printf("%s:%d: error: missing a number after ','\n",
               srcline->as_filename, srcline->line_num);
        srcline->error_flag = EXIT_FAILURE;
        return;
    } else {
        printf("%s:%d: error: invalid digit '%c'\n", srcline->as_filename,
               srcline->line_num, srcline->alignedsrc[pos]);
        srcline->error_flag = EXIT_FAILURE;
    }
}

/* parse_number: parses a number from the source line */
int parse_number(src_op_line *srcline, int *pos, int *temp_num) {
    int sign = 1;
    if (srcline->alignedsrc[*pos] == ' ') {
        (*pos)++;
    }
    handle_sign(srcline, pos, &sign);
    if (!isdigit(srcline->alignedsrc[*pos])) {
        invalid_number(srcline, *pos);
        return 0;
    }
    while (isdigit(srcline->alignedsrc[*pos])) {
        *temp_num = *temp_num * 10 + (srcline->alignedsrc[*pos] - '0');
        (*pos)++;
    }
    *temp_num *= sign;
    return 1;
}

/* install_numbers: creates a binary-word representation for every number
   after the special word ".data" */
void install_numbers(src_op_line *srcline, machine_word **data_img) {
    int pos = strlen(".data "); /* set position on the first operand */
    int temp_num = 0;
    int was_number = 0; /* flags to deal with the last number */
    int after_comma = 0;
    const int old_dc = srcline->cur_dc;
    size_t label_length = get_label_length(srcline);

    if (!data_img_malloc(data_img, srcline)) {
        return;
    }

    if (strlen(srcline->label) > 0) { /* if ther's a label before the data */
        if (install(get_label(srcline), srcline->cur_dc + 1, ".data",
                    srcline) == NULL) {
            printf("error: symbol aleady defined...\n");
        }
        pos += label_length + 2; /* +1 for ':', +2 for anather space */
    }

    for (; pos < strlen(srcline->alignedsrc); pos++) { /* until end of line */
        was_number = parse_number(srcline, &pos, &temp_num);
        check_missing_number(srcline, after_comma, was_number);

        if (was_number) {
            if (is_valid_number_end(srcline, pos)) {
                data_img[srcline->cur_dc]->funct_nd_ops = temp_num;
                srcline->cur_dc++; /*done intall data, update data counter*/
                if (!data_img_malloc(data_img, srcline))
                    return;
            }
        }
        after_comma = skip_spaces_until_comma(srcline, &pos);
    }
    /* finished reading the numbers and updatind the Data-Counter */
    has_data_img_changed(data_img, srcline, old_dc);
    free(data_img[srcline->cur_dc]); /* free the last allocated space */
}

/* install_extern: prints a warning if finds a label before an exteranl data
   saves the external sympol in the symbol table with external attributes. */
void install_extern(src_op_line *srcline, machine_word **data_img) {
    int i;

    i = 0;

    if (strlen(srcline->label) > 0) { /* a label before an external data */
        printf("%s:%d: warrning: label '%s' before an external declaration\n",
               srcline->as_filename, srcline->line_num, srcline->label);
    }

    for (i = strlen(".extern "); srcline->alignedsrc[i] != '\0'; i++) {
        /* copy the symbol after ".extern" */
        srcline->label[i - strlen(".extern ")] = srcline->alignedsrc[i];
    }
    srcline->label[i - strlen(".extern ") - 1] = '\0';

    if (install(srcline->label, 0, ".extern", srcline) == NULL) {
        printf("exit from external symbol statment\n");
    }
}

/* handle_unknown_data: prints an error message for an unknown data instruction
 */
void handle_unknown_data(src_op_line *srcline) {
    /* Find the start of the instruction (after the '.') */
    const char *start = strchr(srcline->alignedsrc, '.') + 1;

    /* If no '.' found, or it's the last character, print a default error
     * message */
    if (!start || *start == '\0') {
        fprintf(stderr, "%s:%d: error: undefined data instruction\n",
                srcline->as_filename, srcline->line_num);
    } else {
        size_t len = strcspn(start, " \t\n");

        fprintf(stderr, "%s:%d: error: undefined data instruction: '.%.*s'\n",
                srcline->as_filename, srcline->line_num, (int)len, start);
    }
    srcline->error_flag = EXIT_FAILURE;
}

/* process_data: identifies the data type of the source line
   and calls the appropriate function to handle it. */
void process_data(src_op_line *srcline, machine_word **data_img) {
    data_type type = get_datatype(srcline->alignedsrc);

    switch (type) {
    case DATA:
        install_numbers(srcline, data_img);
        break;
    case STRING:
        install_string(srcline, data_img);
        break;
    case ENTRY:
        install_entry(srcline, data_img);
        break;
    case EXTERN:
        install_extern(srcline, data_img);
        break;
    case INVALID_DATA:
        handle_unknown_data(srcline);
        break;
    }
}
