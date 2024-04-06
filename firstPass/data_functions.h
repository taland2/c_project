/* proccess_data: install the given input from the source line into 
   the data image as binary words. If it finds a relavant label also
   saves it in the Symbols-Table. The function also handles locally
   data-regarding input errors (undefind use of speciel "." symbol /
   missing string or numburs etc.) and update the error flag saved
   inside the source line structure accordingly. */
void process_data(src_op_line *srcline, machine_word **data_img);
