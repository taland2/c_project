#include "symbol_table.h"
#include "syntax.h"

#define B  0xF000
#define C   0xF00
#define D    0xF0
#define E     0xF
#define B_BITS 12
#define C_BITS  8
#define D_BITS  4

extern char *strdup(const char*);

/* hax_code_print: a function to write the machine image (data + code) on the
   object file */
void print_hex_code(FILE *object, machine_word **code_img,
    				machine_word **data_img, int ic, int dc, int start)
{
    int i;
    long temp;

    for (i = start; i < ic; i++) { /* start with the code image */
        fprintf(object, "%04d A%x-",
        		i, ((unsigned int)(temp = code_img[i]->AER_field)));
        temp = code_img[i]->funct_nd_ops;
        fprintf(object, "B%x-", (unsigned int)((temp & B) >> B_BITS));
        fprintf(object, "C%x-", (unsigned int)((temp & C) >> C_BITS));
        fprintf(object, "D%x-", (unsigned int)((temp & D) >> D_BITS));
        fprintf(object, "E%x\n", (unsigned int)(temp & E));
    }
    for (i = 0; i < dc; i++) { /* now for the data image */
        fprintf(object, "%04d A%x-",  i + ic, (unsigned int)(temp = ABSOLUTE));
        temp = data_img[i]->funct_nd_ops;
        fprintf(object, "B%x-", (unsigned int)((temp & B) >> B_BITS));
        fprintf(object, "C%x-", (unsigned int)((temp & C) >> C_BITS));
        fprintf(object, "D%x-", (unsigned int)((temp & D) >> D_BITS));
        fprintf(object, "E%x\n", (unsigned int)(temp & E));
    }
}

/* make_output: a function to create and edit ane object file, and the externals
   and entries files. for the object file the function uses the "print_hex_code"
   function. the symbols functions located in the symbol table file. */
void make_output(machine_word **code_img, machine_word **data_img, int ic,
				 int dc, char *as_filename)
{
    FILE *object;
    char *temp_name;

    if ((temp_name = strdup(as_filename)) == NULL) {
        printf("error: name file\n");
        return;
    }

    temp_name[strlen(temp_name) -strlen(".as")] = '\0';
    strcat(temp_name,".ob"); /* replace ".as" ending with ".ob" */
    strcat(temp_name, "\0"); /* add the null character to the end of the string */
    object = fopen(temp_name, "w");
    fprintf(object, "%d %d\n", ic-100, dc); /* the title of the object file */
    print_hex_code(object, code_img, data_img, ic, dc, 100);
    /* +100 becuse the code image starts from 100 */

    if ((temp_name = (char *) realloc(temp_name, strlen(temp_name)+4))
    	== NULL) {
        printf("error: out of memory\n");
        return;
    }

    temp_name[strlen(temp_name) - strlen(".ob")] = '\0';
    strcat(temp_name, ".ent"); /* replace with "object" extension */
    print_ent_nd_ext(temp_name); /* now handling the symbols' files */
    free(temp_name);
    fclose(object);
}
