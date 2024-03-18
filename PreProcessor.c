#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Header Files/PreProcessor.h"
#include "../Header Files/util.h"
#include "../Header Files/globals.h"
#include "../Header Files/handle_text.h"
#include "Errors.h"
#include "macro.h"

int remove_extra_spaces_and_add_mcros(char *file_name, node **head);
int check_mcro_call_before_decl(char *file_name, node *head);
int remove_mcros_decl_and_replace(char *file_name, node *head, char **final_file);
int mcro_exec(char file_name[]);

int remove_extra_spaces_and_add_mcros(char *file_name, node **head) {
    char *new_file = remove_extra_spaces_file(file_name);
    if (new_file == NULL) {
        return 0;
    }

    if (!add_mcros(new_file, head)) {
        free(new_file);
        return 0;
    }

    return 1;
}

int check_mcro_call_before_decl(char *file_name, node *head) {
    if (mcro_call_before_decl(file_name, head)) {
        return 0;
    }

    return 1;
}

int remove_mcros_decl_and_replace(char *file_name, node *head, char **final_file) {
    char *new_file = remove_mcros_decl(file_name);
    if (new_file == NULL) {
        return 0;
    }

    *final_file = replace_all_mcros(new_file, head);
    if (*final_file == NULL) {
        free(new_file);
        return 0;
    }

    free(new_file);
    return 1;
}

int mcro_exec(char file_name[]) {
    node *head = NULL;
    char *final_file;

    if (!remove_extra_spaces_and_add_mcros(file_name, &head)) {
        return 0;
    }

    if (!check_mcro_call_before_decl(file_name, head)) {
        return 0;
    }

    if (!remove_mcros_decl_and_replace(file_name, head, &final_file)) {
        return 0;
    }

    free_list(head);
    free(final_file);

    return 1;
}

// Other code
