// In class I said i used 31 instead 32 because i used blp because i thought it has
// better performance, i was worng, it is equivalent, i saw lower number and in my head
// i thought it had more performance

// Optimizaciones
// 1. elimine la copia de la variable r3 y solo use r0 para hacer el calculo
// con mayor eficiencia desde el punto de vista de ciclos y memoria
reverse_int:
        mov     r0, #0          // Initialize output register (reversed bits)
        mov     r2, #32         // Set bit counter (32 for decrementing loop)
.L2:
        ands    r1, r0, #1      //get the less significative bit for the and
        subs    r2, r2, #1      // subs the loop var
        orr     r0, r0, r1, lsl #31 // Append extracted bit to output (shifted into position)
        lsr     r0, r0, #1      // Shift input right by 1 bit
        bne     .L2             // Loop while counter != 0 
        bx      lr              // return
// loop u nroll
// 2. Desenrolle el codigo en una proporcion razonable, de tal forma que tenemos 8 veces menos comprobaciones
// de estado y mas operaciones en menos ciclos
reverse_int:
        mov     r0, #0          // Initialize output register (reversed bits)
        mov     r2, #4          // Set loop counter (4 iterations for unrolled loop)
.L2:
        subs    r2, r2, #1      // Decrement loop counter
        // First bit reversal
        and     r1, r3, #1      // subs the loop var
        orr     r0, r0, r1, lsl #31// #31 #23 #15 #7 // Append extracted bit to output (shifted into position)
        lsr     r0, r0, #1 // Shift input right by 1 bit

        // Second bit reversal
        and     r1, r3, #1
        orr     r0, r0, r1, lsl #31 //#30 #22 #14 #6
        lsr     r0, r0, #1 // Shift input right by 1 bit
        // Third bit reversal
        and     r1, r3, #1
        orr     r0, r0, r1, lsl #31 //#29 #21 #13 #5
        lsr     r0, r0, #1 // Shift input right by 1 bit
        // Forth bit reversal
        and     r1, r3, #1
        orr     r0, r0, r1, lsl #31 //#28 #20 #12 #4
        lsr     r0, r0, #1 // Shift input right by 1 bit
        // Fifth bit reversal
        and     r1, r3, #1
        orr     r0, r0, r1, lsl #31 //#27 #19 #11 #3
        lsr     r0, r0, #1 // Shift input right by 1 bit
        // Sixth bit reversal
        and     r1, r3, #1
        orr     r0, r0, r1, lsl #31 //#26 #18 #10 #2
        lsr     r0, r0, #1 // Shift input right by 1 bit

        // Seventh bit reversal
        and     r1, r3, #1
        orr     r0, r0, r1, lsl #31 //#25 #17 #9 #1
        lsr     r0, r0, #1 // Shift input right by 1 bit
        // Eigth bit reversal
        and     r1, r3, #1
        orr     r0, r0, r1, lsl #31 // #24 #16 #8 #0
        lsr     r0, r0, #1 // Shift input right by 1 bit


        bne     .L2             // Loop while counter != 0
        bx      lr              // return
