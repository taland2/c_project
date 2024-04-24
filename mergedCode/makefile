assembler: main.o first_pass.o Labels.o struct_ext.o second_pass.o utils.o PreProcessor.o Error_Handler.o
	gcc -g -ansi -Wall -pedantic main.o first_pass.o struct_ext.o second_pass.o utils.o Labels.o PreProcessor.o Error_Handler.o -lm -o assembler

main.o: main.c prototypes.h assembler.h extern_variables.h structs.h utils.h
	gcc -c -ansi -Wall -pedantic main.c -o main.o

first_pass.o: first_pass.c prototypes.h assembler.h extern_variables.h structs.h utils.h
	gcc -c -ansi -Wall -pedantic first_pass.c -o first_pass.o

Labels.o: Labels.c prototypes.h assembler.h extern_variables.h structs.h utils.h
	gcc -c -ansi -Wall -pedantic Labels.c -o Labels.o

utils.o: utils.c prototypes.h assembler.h extern_variables.h structs.h utils.h
	gcc -c -ansi -Wall -pedantic utils.c -o utils.o

second_pass.o: second_pass.c prototypes.h assembler.h extern_variables.h structs.h
	gcc -c -ansi -Wall -pedantic second_pass.c -o second_pass.o

struct_ext.o: struct_ext.c prototypes.h assembler.h extern_variables.h structs.h
	gcc -c -ansi -Wall -pedantic struct_ext.c -o struct_ext.o

Error_Handler.o: Error_Handler.c Error_Handler.h Utils.h
	gcc -ansi -pedantic -Wall -c Error_Handler.c

PreProcessor.o: PreProcessor.c PreProcessor.h utils.h Error_Handler.h
	gcc -ansi -pedantic -Wall -c PreProcessor.c

.PHONY: clean

clean:
	rm -f *.o *.am *.ent *.ext *.ob *.exe