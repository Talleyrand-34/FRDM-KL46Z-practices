#include <stdio.h>

int My_Div2(int a, int b) {
    int c = 0;
    asm(
        ".syntax unified\n\t"
        "udiv %[COCIENTE], %[DIVIDENDO], %[DIVISOR]\n\t"  // Perform unsigned division
        : [DIVIDENDO] "+l" (a), [COCIENTE] "+l" (c)       // Output operands
        : [DIVISOR] "l" (b)                               // Input operands
    );
    return c;
}

void main() {
    int dividend = 10;
    int divisor = 2;
    int result = My_Div2(dividend, divisor);

    printf("Result of %d / %d is %d\n", dividend, divisor, result);
}
