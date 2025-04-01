//
// #include <stdio.h>
//
// unsigned int reverse_int(unsigned int in) {
//     unsigned int out = 0;
//     // Devolve o inteiro invertido bit a bit
//
//     for (unsigned int i = 0; i < 32; i++) {
//         out = out << 1;
//         out |= in & 1;
//         in = in >> 1;
//     }
//
//     return out;
// }
//
// int main() { // Change void to int
//     unsigned int test = 177; // Use unsigned int since reverse_int expects it
//     unsigned int out = reverse_int(test); // Declare 'out' and fix missing semicolon
//     printf("%u\n", out); // Use %u for unsigned integers
//     return 0; // Return 0 to indicate successful execution
// }
#include <stdio.h>

unsigned int reverse_int(unsigned int in) {
    unsigned int out;
    asm (
        "bswap   %1\n"               // Reverse byte order
        "mov     %0, %1\n"           // Copy input to output
        "and     %0, 252645135\n"    // Mask certain bits
        "shl     %0, 4\n"            // Shift left by 4 bits
        "shr     %1, 4\n"            // Shift input right by 4 bits
        "and     %1, 252645135\n"    // Mask certain bits
        "or      %1, %0\n"           // Combine results
        "mov     %0, %1\n"           // Copy combined result to output
        "and     %0, 858993459\n"    // Mask certain bits
        "shr     %1, 2\n"            // Shift input right by 2 bits
        "and     %1, 858993459\n"    // Mask certain bits
        "lea     %0, [%1 + 4*%0]\n"  // Combine results using LEA
        "mov     %%ecx, %0\n"        // Move output to ECX register
        "and     %%ecx, 1431655765\n"// Mask certain bits in ECX
        "shr     %0\n"               // Shift output right by 1 bit
        "and     %0, 1431655765\n"   // Mask certain bits in output
        "lea     %0, [%0 + 2*%%ecx]\n"// Combine results using LEA
        : "=r"(out)                  // Output operand
        : "r"(in)                    // Input operand
        : "%ecx", "%edi", "%eax", "%rdx"
    );
    return out;
}

int main() {
    unsigned int test = 177;         // Test value (unsigned integer)
    unsigned int out = reverse_int(test); // Call assembly-embedded function
    printf("%u\n", out);             // Print the reversed integer (unsigned)
    return 0;                        // Return success code
}
