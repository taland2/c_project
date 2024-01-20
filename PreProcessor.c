//This function will gets text file of c program and will create new file with the c code expanded#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void expandCode(const char* inputFilePath, const char* outputFilePath) {
    FILE* inputFile = fopen(inputFilePath, "r");
    FILE* outputFile = fopen(outputFilePath, "w");

    if (inputFile == NULL || outputFile == NULL) {
        printf("Failed to open files.\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), inputFile)) {
        if (line[0] == '#') {
            // Handle preprocessor directive
            // Expand the code and write to the output file
            // ...
        } else {
            // Write the line as-is to the output file
            fputs(line, outputFile);
        }
    }

    fclose(inputFile);
    fclose(outputFile);
}

int main() {
    const char* inputFilePath = "/c:/Users/Ye/Desktop/טל/c_project/input.c";
    const char* outputFilePath = "/c:/Users/Ye/Desktop/טל/c_project/output.c";

    expandCode(inputFilePath, outputFilePath);

    return 0;
}
