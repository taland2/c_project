#include "syntax.h"

/* srclinealoc: get a space for a source line structure */
src_op_line *srclinealoc(void);

/* identify_lables: check if the line begins with a label and if so copy it
	to the label member inside the source line structure. */
void identify_lables(src_op_line * srcline);

/* get_nl_ready: get next source line ready before converted to machine words */
void get_nl_ready(src_op_line * srcline);

/* copy_alligned_line: copy the current source line from the file into the
	source line structure, while skiping extra spaces (including) tabs, 
	comments and empty lines. update the line number as well.*/
void copy_alligned_line(src_op_line *srcline, FILE *ex_src_fl);

/* get_ops_fields: a function to copy the command and operands fields from the
	source line into the space pointed by the given pointers */
void get_ops_fields(src_op_line *srcline, char *first_op,
					char *second_op, char *cmd);

/* binary_code_print: a function to print a binary representation of the machine
	code on the screen (it mainly used for debbuging the program). */
void binary_code_print(machine_word **code_img, int ic, int start);

/* binary_word_print: a function to print a single binary machine word (it
	mainly used to debbug the program). */
void binary_word_print(machine_word *codeword);

/* new_linebuff: a function to get a pointer that's pointing to a new source
	line structure */ 
src_op_line *new_linebuff(char *as_filename, int ic, int dc);
