// main-standalone.c
// Implementación con código ensamblador embebido (inline ASM)

// Función con código ensamblador embebido (desenrolado 4x)
unsigned int reverse_int(unsigned int in)
{
    unsigned int out = 0;
    unsigned int counter = 8;  // 32/4 = 8 iteracións
    
    asm(
        ".syntax unified\n\t"
        "movs   %[OUT], #0\n\t"           // out = 0
        "movs   %[COUNTER], #8\n\t"       // counter = 8
        "1:\n\t"                          // Etiqueta de bucle local
        
        // Iteración 1
        "lsls   %[OUT], %[OUT], #1\n\t"   // out <<= 1
        "movs   r3, #1\n\t"               // r3 = 1
        "ands   r3, %[IN]\n\t"            // r3 = in & 1
        "orrs   %[OUT], r3\n\t"           // out |= r3
        "lsrs   %[IN], %[IN], #1\n\t"     // in >>= 1
        
        // Iteración 2
        "lsls   %[OUT], %[OUT], #1\n\t"   // out <<= 1
        "movs   r3, #1\n\t"               // r3 = 1
        "ands   r3, %[IN]\n\t"            // r3 = in & 1
        "orrs   %[OUT], r3\n\t"           // out |= r3
        "lsrs   %[IN], %[IN], #1\n\t"     // in >>= 1
        
        // Iteración 3
        "lsls   %[OUT], %[OUT], #1\n\t"   // out <<= 1
        "movs   r3, #1\n\t"               // r3 = 1
        "ands   r3, %[IN]\n\t"            // r3 = in & 1
        "orrs   %[OUT], r3\n\t"           // out |= r3
        "lsrs   %[IN], %[IN], #1\n\t"     // in >>= 1
        
        // Iteración 4
        "lsls   %[OUT], %[OUT], #1\n\t"   // out <<= 1
        "movs   r3, #1\n\t"               // r3 = 1
        "ands   r3, %[IN]\n\t"            // r3 = in & 1
        "orrs   %[OUT], r3\n\t"           // out |= r3
        "lsrs   %[IN], %[IN], #1\n\t"     // in >>= 1
        
        // Control de bucle
        "subs   %[COUNTER], #1\n\t"       // counter--
        "bne    1b\n\t"                   // if (counter != 0) goto 1
        
        : [OUT] "+l" (out), [IN] "+l" (in), [COUNTER] "+l" (counter)
        :
        : "r3", "cc"
    );
    
    return out;
}

// Función de referencia en C puro
unsigned int reverse_int_c(unsigned int in)
{
    unsigned int out = 0;
    for (unsigned int i=0; i<32; i++) {
        out = out << 1;
        out |= in & 1;
        in = in >> 1;
    }
    return out;
}

int main(void)
{
    unsigned int test_values[] = {
        0x00000000,
        0xFFFFFFFF,
        0x12345678,
        0xAAAAAAAA,
        0x55555555,
        0x80000001
    };
    
    volatile unsigned int result_inline, result_c;
    
    // Probar ambas implementacións
    for (int i = 0; i < 6; i++) {
        result_inline = reverse_int(test_values[i]);
        result_c = reverse_int_c(test_values[i]);
        
        // Os resultados deberían coincidir
        if (result_inline != result_c) {
            // Erro: resultados non coinciden
            while(1);
        }
    }
    
    // Bucle infinito
    while(1) {
        result_inline = reverse_int(0x12345678);
    }
    
    return 0;
}
