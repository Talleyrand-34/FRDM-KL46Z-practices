.text
.syntax unified /* Unified Assembly Syntax - UAL */
.thumb /* Thumb Instr. Set */
.cpu cortex-m0plus
.type reverse_int, %function
.global reverse_int

// Resumen optimizaciones
// 1. Eliminar copia R0
// 2. Desarrollar bucle para tener menos comparaciones y saltos

// R0: Entrada (in) - No se copia, lo usamos directamente
// R1: Saliída (out) - Resultado
// R2: Contador de bucle (8 iteracións, desarrollado por 4)
// R3: Registro temporal para operaciones

reverse_int:
        movs    r1, #0          // out = 0 (resultado en r1)
        movs    r2, #8          // Contador = 8 
        push    {r4, lr}        // igual

.L2:
        // Iteración 1
        lsls    r1, r1, #1      // out <<= 1
        movs    r3, #1          // r3 = 1
        ands    r3, r0          // r3 = in & 1
        orrs    r1, r3          // out |= r3
        lsrs    r0, r0, #1      // in >>= 1

        // Iteración 2
        lsls    r1, r1, #1      // out <<= 1
        movs    r3, #1          // r3 = 1
        ands    r3, r0          // r3 = in & 1
        orrs    r1, r3          // out |= r3
        lsrs    r0, r0, #1      // in >>= 1

        // Iteración 3
        lsls    r1, r1, #1      // out <<= 1
        movs    r3, #1          // r3 = 1
        ands    r3, r0          // r3 = in & 1
        orrs    r1, r3          // out |= r3
        lsrs    r0, r0, #1      // in >>= 1

        // Iteración 4
        lsls    r1, r1, #1      // out <<= 1
        movs    r3, #1          // r3 = 1
        ands    r3, r0          // r3 = in & 1
        orrs    r1, r3          // out |= r3
        lsrs    r0, r0, #1      // in >>= 1

        // Control de bucle
        subs    r2, r2, #1      // r2--
        bne     .L2             // if (r2 != 0) goto .L2

        movs    r0, r1          // r0 = out
        pop     {r4, pc}        // Retornar

.end
