#include "syntax.h"
#include "first_pass.h"
#include "print_output.h"
#include "second_pass.h"
#include "symbol_table.h"

#define MAX_CODE_ARR 600 /* maximum array size for code and data images */

extern char *strdup(const char *);



/* free_machine_img: frees the memory allocated for the machine image */
void free_machine_img(machine_word **img, int start, int counter) {
    int i = start;

    for (; i < counter && img[i] != NULL; i++) {
        free(img[i]);
    }
}

/* free_resources: frees all resources allocated during assembly */
void free_resources(char *as_filename, int *error_flag, machine_word **code_img,
                    machine_word **data_img, int ic, int dc) {
    if (as_filename != NULL) {
        free(as_filename);
    }
    if (code_img != NULL) {
        free_machine_img(code_img, 100, ic);
    }

    if (data_img != NULL) {
        free_machine_img(data_img, 0, dc);
    }
    else {
        printf("Error: Null pointer received.\n");
    }
}

/* update_input_filename: updates the input filename to end with ".as" */
char *update_input_filename(char *filename) {
    char *new_filename = malloc(strlen(filename) + 4);
    if (new_filename == NULL) {
        return NULL;
    }
    strcpy(new_filename, filename);
    strcat(new_filename, ".as");
    return new_filename;
}

/* check_error_flag: checks if an error was found during assembly */
int check_error_flag(const int *error_flag) {
    if (error_flag == NULL) {
        printf("Error: Null pointer received.\n");
        return EXIT_FAILURE;
    }

    if (*error_flag == 1) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/* process_assembly_file: processes the assembly file */
void process_assembly_file(char *as_filename, int *ic, int *dc,
                          machine_word **code_img, machine_word **data_img,
                          int *error_flag) {
    FILE *am_file = NULL;
    char *am_filename = expand_macros(as_filename);
    if (!am_filename) {
        *error_flag = EXIT_FAILURE;
        return;
    }

    am_file = fopen(am_filename, "r");
    free(am_filename);
    if (!am_file) {
        printf("Error: Failed to open file %s.\n", am_filename);
        free(am_filename);
        *error_flag = EXIT_FAILURE;
        return;
    }
    /* update the filename to end with ".m" for error messages */
    as_filename[strlen(as_filename) - 1] = 'm';

    *dc = get_first_img(*ic, *dc, code_img, data_img, am_file, as_filename,
                        error_flag);


    fseek(am_file, 0, SEEK_SET); /* reset for second pass */
    *ic = get_second_img(code_img, data_img, am_file, as_filename, error_flag);
    fclose(am_file);
}

/* assemble: assembles the given file and creates the output files */
void assemble(char *argv) {
    machine_word *code_img[MAX_CODE_ARR] = {0};
    machine_word *data_img[MAX_CODE_ARR] = {0};
    int ic = 100, dc = 0;
    int error_flag = EXIT_SUCCESS;
    char *as_filename = update_input_filename(argv);

    if (!as_filename) {
        fprintf(stderr, "Error: Failed to update input filename.\n");
        return;
    }

    process_assembly_file(as_filename, &ic, &dc, code_img, data_img,
                          &error_flag);

    if (check_error_flag(&error_flag) == EXIT_SUCCESS) {
        make_output(code_img, data_img, ic, dc, as_filename);
    }
    else {
        printf("Error: File has errors, no output files created.\n");
        free_symbols();
    }

    free_resources(as_filename, &error_flag, code_img, data_img, ic, dc);
    printf("Finished assembling %s\n", argv);
}

/* main: main function gets the input files and calls the assembler */
int main(int argc, char *argv[]) {
    int i = 0;

    if (argc <= 1) {
        printf("Error: Program needs at least one file to assemble.\n");
        return EXIT_FAILURE;
    }

    for (i = 1; i < argc; i++) {
        assemble(argv[i]);
    }

    return EXIT_SUCCESS;
}
