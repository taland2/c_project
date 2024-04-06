#include <ctype.h>
#include "symbol_table.h"
#include "util.h"
#include "syntax.h"

#define MATCH 0

addressing get_addressing(char *operand_field);

/* update_direct_addressing: a function to set the binary value of words using
   direct addressing mode*/
void update_direct_addressing(char *operand, int op_position,
							  machine_word **code_img, src_op_line *srcline)
{
    Symtab_slot *np = NULL;

    if ((np = lookup(operand)) == NULL) {
        printf("%s:%d: error: failed finding symbol '%s' for direct addressing mode\n",
            srcline->as_filename, srcline->line_num, operand);
        srcline->error_flag = EXIT_FAILURE;
        return; /* symbol not found */
    }
    code_img[srcline->cur_ic + op_position]->funct_nd_ops = np->base;

    if (np->is_external == 1) { /* true if operand is an external symbol */
        code_img[srcline->cur_ic + op_position]->AER_field = EXTR;/*turn on'E'*/
        code_img[srcline->cur_ic + op_position]->funct_nd_ops = 0;
        np->base = srcline->cur_ic + op_position; /* set the symbol values */
        np->offset = srcline->cur_ic + op_position + 1;
    } else { /* true if its relocatable */
        code_img[srcline->cur_ic + op_position]->AER_field = RELOCA;/*turn 'R'*/
    }
    op_position++;
    code_img[srcline->cur_ic + op_position]->funct_nd_ops = np->offset;

    if (np->is_external == 1) {
        code_img[srcline->cur_ic + op_position]->AER_field = EXTR;
        code_img[srcline->cur_ic + op_position]->funct_nd_ops = 0;
    } else {
        code_img[srcline->cur_ic + op_position]->AER_field = RELOCA;
    }
}

/* convert_declaration: get the source line and convert it to machine words*/
int update_operands(src_op_line *srcline, machine_word **code_img)
{
    char cmd[MAXLABEL];       /* a variable to place the command field */
    char first_op[MAXLABEL];  /* a variable to place the first operand */
    char second_op[MAXLABEL]; /* a variable to place the second operand */
    int op_position;
    addressing mod;
    int i;

    op_position = 2;
    get_ops_fields(srcline, first_op, second_op, cmd);
    if ((strcmp(cmd, "stop") == MATCH) || (strcmp(cmd, "rts") == MATCH)) {
        return 1;
    }
    mod = get_addressing(first_op);

    if (mod == INDEX) {
        for(i = 0; i < strlen(first_op) && first_op[i] != '['; i++)
            ;
        first_op[i] = '\0';
    } if ((mod == DIRECT) || (mod == INDEX)) {
        update_direct_addressing(first_op, op_position , code_img, srcline);
        op_position = op_position + 2;
    } else if (mod == IMMEDIATE) {
        op_position++;
    }

    if (!(strlen(second_op) == 0)) { /* true if has a second operand */
        mod = get_addressing(second_op);
        if (mod == INDEX) {
            for(i = 0; i < strlen(second_op) && second_op[i] != '['; i++)
                ;
            second_op[i] = '\0';
        } if ((mod == DIRECT) || (mod == INDEX)) {
            update_direct_addressing(second_op, op_position ,code_img, srcline);
            op_position = op_position + 2;
        } else if (mod == IMMEDIATE) {
            op_position++;
        }
    }
    return op_position;
}


/* get_entry: a function to copy the label after ".entry" declaration */
void get_entry(char *maybe_entry, src_op_line *srcline)
{
    int i;
    int j;
    int inside_word;

    j = 0;

    for (i = 0; i < strlen(".entry"); i++) { /* copy the first field */
        if (!isspace(srcline->alignedsrc[i])) {
          *maybe_entry++ = srcline->alignedsrc[i];
          inside_word = 1;
        }
        if ((srcline->alignedsrc[i] == ' ') && (inside_word == 1))
            break;
    }
    for (j = 0; i < MAXLINE && srcline->alignedsrc[i] != '\0'; i++) {/*copy second field */
        if (!isspace(srcline->alignedsrc[i])) {
            srcline->label[j] = srcline->alignedsrc[i];
            j++;
        }
        if (isspace(srcline->alignedsrc[i]) && (j > 0))
            break;
    }
    *maybe_entry = '\0';
    srcline->label[j] = '\0';
    return;
}

/* get_second_img: reads each line input, update the instructions counters,
   update the Symboles-Table for entries */
int get_second_img(machine_word **code_img, machine_word **data_img,
				   FILE *ex_src_fl, char *as_filename, int *error_flag)
{
    src_op_line *srcline = srclinealoc();
    char *data_type;
    /* a variable to return the value after freeing the structure */
    int non_pointer_return;

    srcline->cur_ic = 100; /* get the source line structure ready */
    srcline->cur_dc = 0;
    srcline->l = 0;
    srcline->as_filename = as_filename;
    srcline->line_num = 1;

    copy_alligned_line(srcline, ex_src_fl);

    while (srcline->alignedsrc[0] != EOF) { /* until the end of the input */
        identify_lables(srcline);
        /* first check if the line contains special data '.' sign */
        if (srcline->alignedsrc[0] == '.' ||
            srcline->alignedsrc[strlen(srcline->label) + 2] == '.') {

            data_type = (char*) malloc(strlen(".entry")+1);
            get_entry(data_type, srcline);

            if (strcmp(data_type, ".entry") == MATCH) { /* true if its entry */
                /* update the symbol values in the symbols table */
                if ((install(srcline->label, 0, ".entry", srcline)) == NULL) {
                    free(data_type);
                    return EXIT_FAILURE;
                }
            }
            free(data_type);
        }
        else { /* operation command */
            srcline->cur_ic = srcline->cur_ic+update_operands(srcline, code_img);
        }
        get_nl_ready(srcline);
        copy_alligned_line(srcline, ex_src_fl);
    }
    non_pointer_return = srcline->cur_ic;
    *error_flag = srcline->error_flag;
    free(srcline);
    return non_pointer_return;
}
