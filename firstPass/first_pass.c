#include "syntax.h"
#include "data_functions.h"
#include "op_functions.h"
#include "symbol_table.h"
#include "util.h"
#include <ctype.h>

/* copy_alligned_line: copies the next line from the source file to the
   alignedsrc buffer */
int convert_declaration(src_op_line *srcline, machine_word **code_img) {
    /* The size of the maximum label length was chosen as a precaution,
        incase the user's trying to call labels as commands. */
    char cmd[MAXLABEL];
    char first_op[MAXLABEL];
    char second_op[MAXLABEL];
    int address;                       /* word's position inside the code image*/
    char line_size_cmd[MAXLINE] = {0}; /* full line size - to prevent bufferoverflow in get_op() */

    address = srcline->cur_ic;

    if ((code_img[address] =
             (machine_word *)malloc(sizeof(machine_word))) == NULL) {
        printf("failed\n\n"); /* malloc failiure */
        return EXIT_FAILURE;
    }

    get_ops_fields(srcline, first_op, second_op, cmd);
    /* now set "op-code" field in the first machine word */
    strcpy(line_size_cmd, cmd); /* copy the command to a temporary string */
    code_img[address]->funct_nd_ops = opcodes[get_opcode(line_size_cmd)].op_code;
    code_img[address]->AER_field = ABSOLUTE; /* turn 'A' field on by default */
    address++;

    if ((strcmp(cmd, "hlt") == 0) || (strcmp(cmd, "rts") == 0)) {
        return EXIT_SUCCESS;
    } /* if not STOP ot RTS -> create the second data word */

    if ((code_img[address] =
             (machine_word *)malloc(sizeof(machine_word))) == NULL) {
        printf("memory error\n");
        srcline->l = address;
        return EXIT_FAILURE;
    }                                        /* now assemble the second machine word */
    code_img[address]->funct_nd_ops = 0;     /* avoid garbege values */
    code_img[address]->AER_field = ABSOLUTE; /* set A field */
    incode_addressing(code_img, address, first_op, second_op, cmd, srcline);
    address++;
    /* done with the second machine word-> check operands for more data words */
    srcline->l = address - srcline->cur_ic +
                 get_words_num(code_img, address, first_op, second_op, srcline);
    return EXIT_SUCCESS;
}

/* get_first_img: reads the file pointed by the given FILE pointer line by line.
   update the counters, update the Symbols-Table */
int get_first_img(int ic, int dc, machine_word **code_img,
                  machine_word **data_img, FILE *ex_src_fl, char *as_filename,
                  int *error_flag) {
    src_op_line *srcline; /* the source line */
    int pos = 0;          /* line's position indicator */

    if ((srcline = new_linebuff(as_filename, ic, dc)) == NULL) {
        printf("%s:1: error: memmory failure.\n", srcline->as_filename);
        *error_flag = EXIT_FAILURE; /* malloc failure */
    }
    copy_alligned_line(srcline, ex_src_fl); /* read next line from source file */

    while (srcline->alignedsrc[pos] != EOF) {
        identify_lables(srcline);
        pos = (strlen(srcline->label) > 0) ? strlen(srcline->label) + 2 : 0;
        /* true if has a label at line's begining. +1 for ":" +2 for " " */

        if (srcline->alignedsrc[pos] == '.') { /* true if data instruction */
            process_data(srcline, data_img);
        } else {                              /* else -> a command statment */
            if (strlen(srcline->label) > 1) { /* true if strarts with a label */
                install(srcline->label, srcline->cur_ic, ".code", srcline);
            }
            convert_declaration(srcline, code_img);
        }
        get_nl_ready(srcline);
        copy_alligned_line(srcline, ex_src_fl);
    } /* end of file */

    dc = srcline->cur_dc;
    ic = srcline->cur_ic;
    /* now add the Instruction-Counter to symbols with ".data" attributes */
    update_symbols(srcline->cur_ic);
    *error_flag = srcline->error_flag;
    free(srcline);
    return dc; /* end of first pass (return to main file) */
}
