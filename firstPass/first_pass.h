/* get_first_img: reads the file pointed by the given FILE pointer line by line.
   update the counters, update the Symbols-Table */
int get_first_img(int ic, int dc, machine_word **code_img, machine_word **data_img,
    FILE *ex_src_fl, char *as_filename, int *error_flag);


