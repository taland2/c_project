#include <ctype.h>
#include <string.h>
#include "syntax.h"
#include "op_functions.h"

#define ALIGNED (srcline->alignedsrc)

enum { FALSE, TRUE };

/* srclinealoc: get a space for a source line structure */
src_op_line *srclinealoc(void)
{
    return (src_op_line *) calloc(1, sizeof(src_op_line));
}

/* identify_lables: check if the line begins wiht a label and if so copy it
	to the label member inside the source line structure. */
void identify_lables(src_op_line *srcline)
{
    char temp_string[MAXLABEL];
    int is_valid;
    int i;

    is_valid = isalpha(srcline->alignedsrc[0]) ? TRUE : FALSE;

    for (i = 0; i<MAXLABEL && srcline->alignedsrc[i] != ' '
            && srcline->alignedsrc[i] != ':' ; i++) {
        /* copy the first source line field to the temporary string */
        temp_string[i] = srcline->alignedsrc[i];

        if (!isalnum(temp_string[i])) {
            is_valid = FALSE; /* label must contain only letters and numbers */
        }
    }

    temp_string[i] = '\0';

    if (srcline->alignedsrc[i] == ':') { /* maybe valid label */
        strncpy(srcline->label, temp_string, i);

        if (!is_valid) {
            printf("%s:%d: error: invalid label '%s'\n",
                srcline->as_filename, srcline->line_num, temp_string);
            srcline->error_flag = EXIT_FAILURE;
        }
    } else {
        srcline->label[0] = '\0'; /*doesn't have a label or has an invalid one*/
    }
}


/* get_nl_ready: get next source line ready before converted to machine words */
void get_nl_ready(src_op_line *srcline)
{
    memset(srcline->alignedsrc, '\0', MAXLINE);
    memset(srcline->label, '\0', MAXLABEL);
    srcline->line_num++;
    srcline->cur_ic = srcline->cur_ic + srcline->l;
    srcline->l = 0;
}

/* skip_spaces: a function to skip backtab and regular spaces. */
char skip_spaces(char c, FILE *ex_src_fl)
{
    while (c == '\t' || c == ' '){
        c = getc(ex_src_fl);
    }
    return c;
}

/* copy_alligned_line: copy the current source line from the file into the
    source line structure, while skiping extra spaces (including) tabs,
    comments and empty lines. update the line number as well.*/
void copy_alligned_line(src_op_line *srcline, FILE *ex_src_fl)
{
    int i;
    char c;
    c = getc(ex_src_fl);

    while (isspace(c)) {
    	if (c == '\n') /* an empty line */
    		srcline->line_num++;
        c = getc(ex_src_fl);
    }

    if(c == EOF) { /* true if reached end of file */
        srcline->alignedsrc[0] = EOF;
        return;
    }

    while (c == ';') { /* a comment line */

        while (c != '\n') {
            c = getc(ex_src_fl);
        }

        c = getc(ex_src_fl);
        c = skip_spaces(c, ex_src_fl);
        srcline->line_num++; /* update the line number */
    }

    for (i = 0; i < MAXLINE && c != '\n'; i++) {
        while (isspace(c) && c != '\n')
            c = skip_spaces(c, ex_src_fl);
        if (c == '\n')
            break;
        srcline->alignedsrc[i] = c;
        c = getc(ex_src_fl);
        if (c == '\t' || c == ' ')
            srcline->alignedsrc[++i] = ' ';
    }
    srcline->alignedsrc[i] = '\n';
    srcline->alignedsrc[i+1] = '\0';
}


/* get_operand_field: copy the next word in the aligned instruction to the
   "next field" string */
void get_operand_field (char *next_field, char *aligned_instruction)
{
    while ( *aligned_instruction  != '\0' &&
            *aligned_instruction  != '\n' &&
            *aligned_instruction  != ',') {
        *next_field++ = *aligned_instruction++;
    }
    *next_field++ = '\0';
}

/* verify_op_syntax: the function examines the numer of operands for each
	command, and ther's addressing mode. It compers each operand mode to the
	command's ligal addressing modes stored in the "op_function" header, and
	prints errors accordingly. */
void verify_op_syntax(src_op_line *srcline, char *first_op,
					  char *second_op, char *cmd)
{
    int i;
    int total_ops; /* the number of operands (one two or none) */
    int invalid_reg;
    addressing mod;

    total_ops = (strlen(first_op) > 0) ? (strlen(second_op) > 0 ? 2 : 1) :
    									 (strlen(second_op) > 0 ? 1 : 0);

    invalid_reg = (sizeof(opcodes)/sizeof(opcodes[0])) -1;
    for (i = 0; (i < invalid_reg) && (strcmp(cmd, opcodes[i].name) != 0); i++)
        ;

    if (!strcmp(opcodes[i].name, "invalid")) { /* an undefined operatin */

        printf("%s:%d: error: undefined operation '%s'\n",
            srcline->as_filename, srcline->line_num, cmd);
        srcline->error_flag = EXIT_FAILURE;
        return;

    } else if (opcodes[i].required_ops > total_ops) { /* not enough operands */

        printf("%s:%d: error: missing %d operand for command '%s'\n",
            srcline->as_filename, srcline->line_num,
            (opcodes[i].required_ops - total_ops), cmd);
        srcline->error_flag = EXIT_FAILURE;
        return;

    } else if (opcodes[i].required_ops < total_ops) { /* too many operands */

        printf("%s:%d: error: passing %d extra operands than required for command '%s'\n",
            srcline->as_filename, srcline->line_num,
            (total_ops - opcodes[i].required_ops), cmd);
        srcline->error_flag = EXIT_FAILURE;
        return;
    }

    if (total_ops == 2) { /* first operand is source, second is destination */

        mod = get_addressing(first_op); /* start with the source */
        mod = mod < 2  ? mod+1 : ((mod & 2) << (mod -1));
        /* set on only the bit representing the addressing mode number, meaning:
        	0->0001		1->0010		2->0100		3->1000 */

        if (!(mod &= opcodes[i].valid_src_mod)) { /* source addresing error */

            printf("%s:%d: error: invaling addressing mode for source operand '%s'\n",
                srcline->as_filename, srcline->line_num, first_op);
            srcline->error_flag = EXIT_FAILURE;
        }

        mod = get_addressing(second_op); /* now for the target opernad */
        mod = mod < 2  ? mod+1 : ((mod & 2) << (mod -1));

        if (!(mod &= opcodes[i].valid_trgt_mod)) { /* target addressing error */

            printf("%s:%d: error: invaling addressing mode for target operand '%s'\n",
                srcline->as_filename, srcline->line_num, second_op);
            srcline->error_flag = EXIT_FAILURE;
        }

    } else if (total_ops == 1) { /* line contains only a destination operand */

        mod = get_addressing(first_op);
        mod = mod < 2  ? mod+1 : ((mod & 2) << (mod -1));

        if (!(mod &= opcodes[i].valid_trgt_mod)) { /* target addressing error */

            printf("%s:%d: error: invaling addressing mode for target operand '%s'\n",
                srcline->as_filename, srcline->line_num, first_op);
            srcline->error_flag = EXIT_FAILURE;
        }
    }
}

/* get_ops_fields: a function to copy the command and operands fields from the
	source line into the space pointed by the given pointers */
void get_ops_fields(src_op_line *srcline, char *first_op,
					char *second_op, char *cmd)
{
    int pos;
    int label_length;
    int temp;

    label_length = strlen(srcline->label);
    pos = (label_length > 0) ? label_length + 2 : 0; /* +1 for ':' +2 for ' ' */

    for (temp = 0; temp < MAXLINE && (srcline->alignedsrc[temp + pos] != ' ') &&
                (srcline->alignedsrc[temp + pos] != '\n'); temp++) {

        cmd[temp] = srcline->alignedsrc[temp + pos]; /* copy the command */
    }

    cmd[temp] = '\0';
    pos += strlen(cmd) + 1;
    get_operand_field(first_op, &(srcline->alignedsrc[pos]));/* copy first op */
    pos = label_length + strlen(cmd) + strlen(first_op) + 3;
    /* get the position of the second operand */

    if (isspace(srcline->alignedsrc[pos])) {
        pos++; /* +1 to avoid ' ' */
    } else if (srcline->alignedsrc[pos] == ',') {
        pos++; /* +1 to avoid ',' */
        if (isspace(srcline->alignedsrc[pos]) == 1)
            pos++; /* +1 to avoid ' ' */
    } else if (srcline->alignedsrc[pos-2] == ',' &&
    		   srcline->alignedsrc[pos-1] != ' ') {
        pos--;
    }
    /* copy second operand (or just '\0' if doesn't exist) */
    get_operand_field(second_op, &srcline->alignedsrc[pos]);
    verify_op_syntax(srcline, first_op, second_op, cmd); /* check for errors */
}

/* binary_code_print: a function to print a binary representation of the machine
	code on the screen (it mainly used for debbuging the program). */
void binary_code_print(machine_word **code_img, int ic, int start)
{
    int i;
    int j;
    int temp;

    for (i = start; i < ic; i++) {
        temp = code_img[i]->AER_field;
        putchar('|');
        for (j = 3; j >= 0; j--) {
            putchar(temp & (1u << j) ? '1' : '0');
        }
        putchar('|');

        temp = code_img[i]->funct_nd_ops;
        for (j = 13; j >= 0; j--) {
            putchar(temp & (1u << j) ? '1' : '0');
        if (j%4 == 0)
            putchar('|');
        }
        putchar('\n');
    }
}

/* binary_word_print: a function to print a single binary machine word
	(it mainly used to debbug the program).  */
void binary_word_print(machine_word *codeword)
{
    int j;
    int temp;

    temp = codeword->AER_field;
    putchar('|');
    for (j = 3; j >= 0; j--) {
        putchar(temp & (1u << j) ? '1' : '0');
    }
    putchar('|');

    temp = codeword->funct_nd_ops;
    for (j = 15; j >= 0; j--) {
        putchar(temp & (1u << j) ? '1' : '0');
    if (j%4 == 0)
        putchar('|');
    }
    putchar('\n');
}

/* new_linebuff: gets a pointer that points to a new source line structure */
src_op_line *new_linebuff(char *as_filename, int ic, int dc)
{
    src_op_line *srcline;
    if ((srcline = srclinealoc()) == NULL) {
        printf("error: not enough memmory\n");
        return NULL;
    }
    srcline->as_filename = as_filename;
    srcline->cur_dc = dc;
    srcline->cur_ic = ic;
    srcline->line_num = 1;
    srcline->error_flag = 1;
    srcline->error_flag = EXIT_SUCCESS;
    srcline->l = 0;
    return srcline;
}
