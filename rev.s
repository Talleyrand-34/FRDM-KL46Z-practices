.text
.syntax unified /* Unified Assembly Syntax - UAL */
.thumb /* Thumb Instr. Set */
.cpu cortex-m0plus
.type reverse_int, %function
.global reverse_int
reverse_int:
        movs    r3, r0
        movs    r2, #32
        movs    r0, #0
        push    {r4, lr}
.L2:
        movs    r1, #1
        lsls    r0, r0, #1
        ands    r1, r3
        subs    r2, r2, #1
        orrs    r0, r1
        lsrs    r3, r3, #1
        cmp     r2, #0
        bne     .L2
        pop     {r4, pc}
