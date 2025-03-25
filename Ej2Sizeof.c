#include <stdio.h>

int main() {
    // Char / unsigned char
    printf("Char: %zu bytes\n", sizeof(char));
    printf("Unsigned char: %zu bytes\n", sizeof(unsigned char));
    // Short / unsigned short
    printf("Short: %zu bytes\n", sizeof(short));
    printf("Unsigned short: %zu bytes\n", sizeof(unsigned short));
    // Int / unsigned int
    printf("Int: %zu bytes\n", sizeof(int));
    printf("Unsigned int: %zu bytes\n", sizeof(unsigned int));
    // Long / unsigned long
    printf("Long: %zu bytes\n", sizeof(long));
    printf("Unsigned long: %zu bytes\n", sizeof(unsigned long));
    // Long long / unsigned long long
    printf("Long long: %zu bytes\n", sizeof(long long));
    printf("Unsigned long long: %zu bytes\n", sizeof(unsigned long long));
    // Float
    printf("Float: %zu bytes\n", sizeof(float));
    // Double
    printf("Double: %zu bytes\n", sizeof(double));

    return 0;
}