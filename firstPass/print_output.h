/* make_output: a function to create and edit an object file, and the externals
   and entries files. for the object file the function uses the "print_hex_code"
   function. the symbols functions located in the symbol table file. */
void make_output(machine_word **code_img, machine_word **data_img, 
				 int ic, int dc, char *file_name);
