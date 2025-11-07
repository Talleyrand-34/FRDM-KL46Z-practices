// main-shared.c
// Implementación enlazando con función ensamblador externa (rev.s)

// Declaración da función en ensamblador (definida en rev.s)
extern unsigned int reverse_int(unsigned int in);

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
    
    volatile unsigned int result_asm, result_c;
    
    // Probar ambas implementacións
    for (int i = 0; i < 6; i++) {
        result_asm = reverse_int(test_values[i]);
        result_c = reverse_int_c(test_values[i]);
        
        // Os resultados deberían coincidir
        if (result_asm != result_c) {
            // Erro: resultados non coinciden
            while(1);
        }
    }
    
    // Bucle infinito
    while(1) {
        result_asm = reverse_int(0x12345678);
    }
    
    return 0;
}
