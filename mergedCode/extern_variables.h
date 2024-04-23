/*=======================================================================================================
Project: Maman 14 - Assembler
Created by:
Edrehy Tal and Liberman Ron Rafail

Date: 18/04/2024
========================================================================================================= */

#include "structs.h"
#include "assembler.h"

/*--------------------------------------Global Variables --------------------------------------------------*/
extern int ic, dc; /*ic-instruction counter ; dc-data counter*/
extern int err;
extern boolean was_error;  /*flag to error exists*/
extern labelPtr symbols_table; /*list of external labels*/
extern extPtr ext_list;
extern const char base4[4]; /*Speical 3 bits encripted*/
extern const char *commands[]; /*list of our Assembly commands*/
extern const char *directives[]; /*list of our directive sentences*/
extern boolean entry_exists, extern_exists;/*flags to exists enrty and extern*/
