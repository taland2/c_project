#if !defined(syntax)
#define syntax

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 80
#define MAXLABEL 31
#define ABSOLUTE 4
#define RELOCA 2
#define EXTR 1

typedef enum addressing { IMMEDIATE,
                          DIRECT,
                          INDEX,
                          REG_DIRECT,
                          NO_AD = 0 } addressing;

/* a Bit-fields structure for binary code representation of a word */
typedef struct machine_word {
    unsigned int AER_field      : 4; /* bits 16-10 */
    unsigned int funct_nd_ops   :16; /* bits 0-15 */
} machine_word;

/* a stucture to temporarly save details about the current source line  */
typedef struct src_op_line {
    char alignedsrc[MAXLINE]; /* member to store source statment aligned */
    char label[MAXLABEL]; /* member to store label (if exists) */
    char *as_filename; /* input file name */
    int line_num; /* line number */
    int l; /* lenght of the source line in binary words */
    int error_flag;
    int cur_dc;
    int cur_ic;
} src_op_line;
#endif
