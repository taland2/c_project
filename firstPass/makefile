assembler: main.o  first_pass.o second_pass.o print_output.o op_functions.o symbol_table.o data_functions.o util.o
	gcc -g -ansi -Wall -pedantic main.o  first_pass.o second_pass.o print_output.o op_functions.o symbol_table.o data_functions.o util.o -o assembler

main.o: main.c syntax.h first_pass.h
	gcc -g -c -ansi -Wall -pedantic main.c  -o main.o

first_pass.o:  first_pass.c op_functions.h symbol_table.h data_functions.h util.h
	gcc -g -c -ansi -Wall -pedantic first_pass.c -o first_pass.o

op_functions.o: syntax.h
	gcc -g -c -ansi -Wall -pedantic op_functions.c -o op_functions.o

symbol_table.o: symbol_table.c symbol_table.h
	gcc -g -c -ansi -Wall -pedantic symbol_table.c -o symbol_table.o

data_functions.o: data_functions.c syntax.h symbol_table.h data_functions.h
	gcc -g -c -ansi -Wall -pedantic data_functions.c -o data_functions.o

util.o: util.c syntax.h
	gcc -g -c -ansi -Wall -pedantic util.c -o util.o

second_pass.o: second_pass.c syntax.h symbol_table.h util.h data_functions.h
	gcc -g -c -ansi -Wall -pedantic second_pass.c -o second_pass.o

print_output.o:
	gcc -g -c -ansi -Wall -pedantic print_output.c -o print_output.o

clean:
	rm -f *.o assembler *.ob *.ent *.ext *.am