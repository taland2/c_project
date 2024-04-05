#include <stdio.h>
#include <stdlib.h>
#include "Error_Handler.h"
#include "Utils.h"
#include "preprocessor.h"


#define HANDLE_STATUS(file, code) if ((code) == ERR_MEM_ALLOC) { \
    handle_error(code, (file)); \
    if (file) free_file_context(&(file)); \
    return ERR_MEM_ALLOC; \
    }

#define CHECK_ERROR_CONTINUE(report, file) \
    if ((report) != NO_ERROR) {      \
    handle_error(ERR_FOUND_ASSEMBLER, (file));\
        continue;                    \
    }

/**
 * Processes the input source file for assembler preprocessing.
 *
 * Reads the source file and process it accordingly by the assembler passes and the preprocessor.

 *
 * @param file_name     The name of the input source file to process.
 * @param dest          Pointer to the destination file_context struct.
 * @param index         The index of the file being processed.
 * @param file_number           The total number of files to be processed.
 *
 * @return The status of the file processing.
 * @return NO_ERROR if successful, or FAILURE if an error occurred.
 */
status_error_code preprocess_file(const char* file_name, file_context** dest , int index, int file_number) {
    file_context *src = NULL;
    status_error_code code = NO_ERROR;

    src = create_file_context(file_name, ASSEMBLY_EXT, FILE_EXT_LEN, FILE_MODE_READ, &code);
    HANDLE_STATUS(src, code);

    handle_progress(OPEN_FILE, src);

    *dest = create_file_context(file_name, PREPROCESSOR_EXT, FILE_EXT_LEN, FILE_MODE_WRITE_PLUS, &code);
    HANDLE_STATUS(*dest, code);
    (*dest)->tc = file_number;
    (*dest)->fc = index;

    code = assembler_preprocessor(src, *dest);

    if (src) free_file_context(&src);

    if (code != NO_ERROR) {
        handle_error(ERR_PRE, index, file_number, file_name);
        free_file_context(dest);
        return FAILURE;
    } else {
        handle_progress(PRE_FILE_OK, *dest, index, file_number);
        return NO_ERROR;
    }
}
status_error_code preprocess_file(const char* file_name, file_context** dest , int index, int file_number);


int main(int argc, char *argv[]) {
    int i;
    status_error_code report;
    file_context *dest_am = NULL;
    if (argc == 1) {
        handle_error(FAILURE);
        exit(FAILURE);
    }

    for (i = 1; i < argc; i++) {
        report = preprocess_file(argv[i], &dest_am, i, argc - 1);
        CHECK_ERROR_CONTINUE(report, argv[i]);
    }
    return 0;
}

