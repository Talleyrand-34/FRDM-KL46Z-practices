// arm gcc 14.2.0
reverse_int:
        mov     r3, r0 // safe copy
        mov     r2, #32 // loop variable initialization
        mov     r0, #0 // out initialization
.L2:
        and     r1, r3, #1 // get bit of the last position of init
        subs    r2, r2, #1 // subs the loop var
        orr     r0, r1, r0, lsl #1 // left shift
        lsr     r3, r3, #1 // right shift copy
        bne     .L2 // jump condition
        bx      lr // return
