// macro.c
#include "macro.h"

char *save_mcro_content(FILE *fp, fpos_t *pos, int *line_count) {
    int mcro_length;
    char *mcro;
    char str[MAX_LINE_LENGTH];

    /* Set the file pointer to the provided position */
    if (fsetpos(fp, pos) != 0) {
        print_internal_error(ERROR_CODE_11);
        return NULL;
    }
    mcro_length = 0;
    str[0] = '\0';

    /* Read lines from the file until "endmcro" is encountered */
    while (fgets(str, MAX_LINE_LENGTH, fp) && (strcmp(str, "endmcro\n")) != 0) {
        /* checking for a line with endmcro and extra text */
        if ((strstr(str, "endmcro") != NULL) && strlen(str) != strlen("endmcro")) {
            print_internal_error(ERROR_CODE_12);
            return NULL;
        }
        (*line_count)++;
        if (strcmp(str, "endmcro\n") != 0) {
            mcro_length += strlen(str);
        }
    }

    /* Copy the macro content into a dynamically allocated string */
    mcro = copy_text(fp, pos, mcro_length);
    return mcro;
}

int valid_mcro_decl(char *str, char **name, int line_count, char *file_name) {
    /* assumes "mcro " has been encountered right before the function was called */
    char *temp_name, *extra;

    temp_name = strtok(NULL, " \n");
    if (temp_name == NULL) {
        print_internal_error(ERROR_CODE_9);
        return 0;
    }

    /* Check if the macro name clashes with instruction name, opcode, or register name */
    if (is_instr(temp_name) || what_opcode(temp_name) >= 0 || what_reg(temp_name) >= 0) {
        location as_file;
        as_file.file_name = file_name;
        as_file.line_num = line_count;
        print_external_error(ERROR_CODE_17, as_file);
        return 0;
    }
    extra = strtok(NULL, "\n");
    if (extra != NULL) {
        print_internal_error(ERROR_CODE_10);
        return 0;
    }

    /* Copy the extracted macro name into a dynamically allocated string */
    *name = handle_malloc((strlen(temp_name) + 1) * sizeof(char));
    strcpy(*name, temp_name);

    return 1;
}

int add_mcros(char *file_name, node **head) {
    int line_count, success;
    FILE *fp;
    char str[MAX_LINE_LENGTH];
    char *name, *content;
    fpos_t pos;

    /*Lets assume that the process success */
    success = 1;

    /*Open the file and handle it*/
    fp = fopen(file_name, "r");
    if (fp == NULL) {
        print_internal_error(ERROR_CODE_8);
        success = 0;
        return success;
    }
    line_count = 0;
    while (fgets(str, MAX_LINE_LENGTH, fp)) {
        line_count++;

        /* Extract the macro name from the declaration line and validate it */
        if (strcmp(strtok(str, " "), "mcro") == 0) {
            int mcro_line = line_count;
            if (!valid_mcro_decl(str, &name, line_count, file_name)) {
                success = 0;
                continue;
            }

            /* Save the macro content starting from the current file position */
            fgetpos(fp, &pos);
            content = save_mcro_content(fp, &pos, &line_count);
            if (content == NULL) {
                success = 0;
                continue;
            }

            /* going to the end of the macro */
            fsetpos(fp, &pos);
            /* adding the new mcro into the mcro_list */
            add_to_list(head, name, content, mcro_line);
        }
    }

    /*Close the file and return the result*/
    fclose(fp);
    return success;
}

int mcro_call_before_decl(char file_name[], node *head) {
    FILE *fp;
    int line_count, check;
    node *mcro;
    char str[MAX_LINE_LENGTH];

    /* Open the input file for reading */
    fp = fopen(file_name, "r");
    check = 0;
    line_count = 0;
    while (fgets(str, MAX_LINE_LENGTH, fp) != NULL) {
        line_count++;
        /* if line is a mcro declaration then go to next line */
        if (strstr(str, "mcro") != NULL) {
            continue;
        }

        /* Search for the first macro declaration after the current line */
        mcro = head;
        while ((mcro != NULL) && (mcro->line < line_count)) {
            mcro = mcro->next;
        }

        /* If there are no more macro declarations, continue to the next line */
        if (mcro == NULL) {
            continue;
        }

        /* Check for macro calls in the current line */
        while (mcro != NULL) {
            if (strstr(str, mcro->name) != NULL) {
                /* A macro call is found before its declaration */
                print_internal_error(ERROR_CODE_16);
                check = 1;
            }
            mcro = mcro->next;
        }
    }
    fclose(fp);

    /* Return 1 if any macro call is found before its declaration, otherwise return 0 */
    return check;
}
