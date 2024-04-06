#include "syntax.h"

/* a structure to create linked "symbol type" */
typedef struct Symtab_slot { /* the saved symbol in the table */
    char *name; /* the label */
    int value;  /* value */
    int base;
    int offset;
    /* attribute: */
    unsigned int is_code;
    unsigned int is_data;
    unsigned int is_entry;
    unsigned int is_external;
    struct Symtab_slot *next; /* next symbol in the line with same hash value */
} Symtab_slot;

/* lookup: look for a label in the symbols-table */
Symtab_slot *lookup(char *label);

/* install: gets a symbol name and details and saves them into a "symbol slot"
	in symbols hash table. returns a pointer to the "symbol-slot" if succeded,
	or NULL if failed */
Symtab_slot *install(char *name, int value, char *artbt, src_op_line *srcline);

/* update_symbols: sets the base and offset value of the saved symbols and also
    gets the instruction counter to update the values of ".data" symbols */
void update_symbols(int icf);

/* print_ent_nd_ext: a function to search for symbols with "entry" or "external"
	attribures in the symbols-table, and to print enties on a new ".ent" file
	and	externals on a new ".ext" file, both starts whith the given file name.*/
void print_ent_nd_ext(char *ent_file_name);

/* print_table: a function to print the symbol table on the commad line
	(for debuuging - not used by the assembler) */
void print_table(void);

void free_symbols(void);