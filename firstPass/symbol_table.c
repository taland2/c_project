#include "symbol_table.h"
#include "syntax.h"

#define HASHSIZE 1


extern char *strdup(const char*);

/* the symbol-table impleted by hash-table */
static Symtab_slot *hashtab[HASHSIZE];

/* hash: form hash value for string s */
unsigned hash(char *s)
{
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}

/* free_symbols: a function to free memory allocated to symbols before the next
	source file assembling */
void free_symbols(void)
{
    Symtab_slot * ptr;
    int i;

    for (i = 0; i< HASHSIZE; i++) {
        while (hashtab[i] != NULL) {
            ptr = hashtab[i]->next;
            free(hashtab[i]->name);
            free(hashtab[i]);
            hashtab[i] = ptr;
        }
    }
}

/* lookup: look for a label in the hashtab */
Symtab_slot *lookup(char *label)
{
    Symtab_slot *np;

    for (np = hashtab[hash(label)]; np != NULL; np = np->next) {
        if (strcmp(label, np->name) == 0) {
            return np; /* found */
        }
    }
    return NULL; /* incase not found */
}

static char *saved_words[] = {
	".string", ".data", ".entry", ".external",
	"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
	"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne",
	"red", "prn", "jsr", "rts", "hlt"
};

/* install: gets a symbol name and details and saves them into a "symbol slot"
	in symbols hash table. returns a pointer to the "symbol-slot" if succeded,
	or NULL if failed */
Symtab_slot *install(char *name, int value, char *artbt, src_op_line *srcline)
{
    Symtab_slot *np;
    unsigned hashval;
    int i;

	/* first check if the label isn't a saved word */
    for (i = 0; i < sizeof(saved_words)/sizeof(char*); i++) {
    	if ((strcmp(name, saved_words[i])) == 0) {
    		printf("%s:%d: error: word '%s' can't be a saved word\n",
            	srcline->as_filename, srcline->line_num, name);
            srcline->error_flag = EXIT_FAILURE;
            return NULL;
          }
	}

    if ((np = lookup(name)) == NULL) { /* not found */
        np = calloc(1, sizeof(Symtab_slot));
        if (np == NULL || (np->name = strdup(name)) == NULL) {
            printf("%s:%d: error: failed duplicating symbol %s\n",
            	srcline->as_filename, srcline->line_num, name);
            srcline->error_flag = EXIT_FAILURE;
            return NULL;
        }
        hashval = hash(name); /* get the hash-value from the hash function */
        np->next = hashtab[hashval];/* move forward what's in the linked-list */
        np->value = value;
        hashtab[hashval] = np; /* take its place at the begining of the list */

        /* now update the symbol's attributes using the given char pointer */
        if (strcmp(artbt, ".code") == 0) {
            np->is_code = 1;
        }
        if (strcmp(artbt, ".data") == 0) {
            np->is_data = 1;
        }
        if (strcmp(artbt, ".extern") == 0) {
            np->is_external = 1;
        }
        return np;
    }
    else { /* symbol's already in the table */
        if ((strcmp(artbt, ".entry") == 0) && (np->is_entry != 1)) {
            np->is_entry = 1;
            return np;
        } else {
            printf("%s:%d: error: redefinition of saved symbol %s\n",
                srcline->as_filename, srcline->line_num, name);
            printf("artbt is %s and in enrty = %d\n\n", artbt, np->is_entry);
            srcline->error_flag = EXIT_FAILURE;
            return NULL;
        }
    } /* symbol's unfound */
    printf("%s:%d: error: failed finding symbol %s\n",
    	srcline->as_filename, srcline->line_num, name);
    srcline->error_flag = EXIT_FAILURE;
    return NULL;
}

/* get_base: gets a place on the image ("value") and returns its "base" number*/
int get_base(int value)
{
    if (value < 0) {
        printf("Error - value must be non negative.\n");
    }
    while (value > 0) {
        value = value -1;
        if(!(value%16)) /* true if modulo 16 = 0 */
            return value;
    }
    return 0; /* also if value = 0 from the first place */
}

/* update_symbols: sets the base and offset value of the saved symbols and also
    gets the instruction counter to update the values of ".data" symbols */
void update_symbols(int icf)
{
    Symtab_slot *ptr;
    int i;

    i = 0;

    for (i = 0; i< HASHSIZE; i++) {
        ptr = hashtab[i];
        while (ptr != NULL) {

            if (ptr->is_data == 1) { /* true if symbol has "data" attribute */
                ptr->value = ptr->value + icf;
            }
            ptr->base = get_base(ptr->value);
            ptr->offset = ptr->value - ptr->base;
            ptr = ptr->next;
        }
    }
}

/* print_table: a function to print the symbol table on the commad line
	(for debuuging - not used by the assembler) */
void print_table(void)
{
    Symtab_slot *ptr;
    int i, j;
    i = 0;
    j = 0;

    for (i = 0; i< HASHSIZE; i++) {

        ptr = hashtab[i];

        while (ptr != NULL) {
            printf("\nhashtab in %d place is: ", i);
            while (ptr->name[j] != '\0') {
                putchar(ptr->name[j]);
                j++;
                if (ptr->name[j] == ' ') {
                    break;
                }
            }
            putchar('\n');
            printf("\t\tvalue  = [%d]\n", ptr->value);
            printf("\t\tbase   = [%d]\n", ptr->base);
            printf("\t\toffset = [%d]\n", ptr->offset);
            printf("\t\tartbts:  ");
            if (ptr->is_code == 1)
                printf("[.code] ");
            if (ptr->is_data == 1)
                printf("[.data] ");
            if (ptr->is_entry == 1)
                printf("[.entry] ");
            if (ptr->is_external == 1)
                printf("[.external]");
            putchar('\n');
            j=0;
            ptr = ptr->next;
        }
    }
}

/* print_ent_nd_ext: a function to search for symbols with "entry" or "external"
	attribures in the symbols-table, and to print enties on a new ".ent" file
	and	externals on a new ".ext" file, both starts whith the given file name.*/
void print_ent_nd_ext(char *ent_file_name)
{
    FILE *ent; /* for entries */
    FILE *ext; /* for externals */
    char *ext_file_name;
    Symtab_slot *ptr;
    int i;
    int wrote_ent;
    int wrote_ext;

    wrote_ent = 0;
    wrote_ext = 0;

    ext_file_name = strdup(ent_file_name);
    /* now cut the "ent" extension and replace with "object" extension */
    ext_file_name[strlen(ent_file_name) - strlen("ent")] = '\0';
    strcat(ext_file_name, "ext");
    /* now creates files for entries and externals */
    ent = fopen(ent_file_name, "w");
    ext = fopen(ext_file_name, "w");

    for (i = 0; i< HASHSIZE; i++) {

        ptr = hashtab[i];

        while (ptr != NULL) {

            if (ptr->is_entry == 1) {/* true if symbol has an entry attribute */
                fprintf(ent,"%s,%04d,%04d\n",
                	ptr->name, ptr->base, ptr->offset);
                wrote_ent = 1;
            }

            if (ptr->is_external == 1) {
                fprintf(ext,"%s BASE %04d\n%s OFFSET %04d\n\n",
                    ptr->name, ptr->base, ptr->name, ptr->offset);
                wrote_ext = 1;
            }
            ptr = ptr->next;
        }
    }
    fclose(ent);
    fclose(ext);

    if (wrote_ent == 0) {
        remove(ent_file_name);
    }
    if (wrote_ext == 0) {
        remove(ext_file_name);
    }
    free(ext_file_name);
    free_symbols();
}
