#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "data.h"
#include "Error_Handler.h"
#include "Utils.h"

#define get_register_num(reg) ((char) (reg)[2] - '0')

/* "Private" helper functions */
status_error_code concat_default_12bit(data_image *data, char** binary_word);
status_error_code concat_reg_dest(data_image *data, char** binary_word);
status_error_code concat_reg_src(data_image *data, char** binary_word);
status_error_code concat_reg_reg(data_image *data, char** binary_word);
status_error_code concat_address(data_image *data, char** binary_word);

/**
 * Converts a decimal number to a binary string representation.
 *
 * @param decimal The decimal number to be converted to binary.
 * @return A dynamically allocated binary string representation of the decimal number,
 * or NULL if memory allocation fails.
 */
char* decimal_to_binary12(int decimal) {
    int i, carry;
    int is_negative = 0;
    char *binary = malloc((BINARY_BITS + 1) * sizeof(char));
    static const char *lookup_bin[COMMANDS_LEN] = {
            "000000000000",
            "000000000001",
            "000000000010",
            "000000000011",
            "000000000100",
            "000000000101",
            "000000000110",
            "000000000111",
            "000000001000",
            "000000001001",
            "000000001010",
            "000000001011",
            "000000001100",
            "000000001101",
            "000000001110",
            "000000001111"
    };

    if (!binary) {
        handle_error(ERR_MEM_ALLOC);
        return NULL;
    }

    if (decimal >= 0 && decimal <= 15) {
        strcpy(binary, lookup_bin[decimal]);
        return binary;
    }

    if (decimal < 0) {
        decimal = -decimal;
        is_negative = 1;
    }

    for (i = BINARY_BITS - 1; i >= 0; i--) {
        binary[i] = (char)((decimal % 2) + '0');
        decimal /= 2;
    }

    if (is_negative) {
        /* Two's complement for is_negative numbers */
        for (i = 0; i < BINARY_BITS; i++)
            binary[i] = (binary[i] == '0') ? '1' : '0';

        carry = 1;
        for (i = BINARY_BITS - 1; i >= 0; i--) {
            int sum = (binary[i] - '0') + carry;
            binary[i] = (char)((sum % 2) + '0');
            carry = sum / 2;
        }
    }

    binary[BINARY_BITS] = '\0';
    return binary;
}

/**
 * Truncates an input string to the specified length.
 *
 * @param input  The input string to be truncated.
 * @param length The desired length of the truncated string.
 * @return A dynamically allocated truncated string,
 * or NULL if an error occurs or `length` is greater than the input string length.
 */
char* truncate_string(const char *input, int length) {
    unsigned int truncate_len;
    char* truncated = NULL;

    if (!input) {
        if (!(truncated = calloc(length + 1, sizeof(char ))))
            handle_error(ERR_MEM_ALLOC);
        return truncated;
    }

    truncate_len = BINARY_BITS - length;

    if (length >= BINARY_BITS) {
        handle_error(TERMINATE, "truncate_string()");
        return NULL;
    }

    truncated = malloc((length + 1) * sizeof(char));
    if (!truncated) {
        handle_error(ERR_MEM_ALLOC);
        return NULL;
    }

    strncpy(truncated, input + truncate_len, length);
    truncated[length] = '\0';

    return truncated;
}



