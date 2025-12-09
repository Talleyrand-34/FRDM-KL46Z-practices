.syntax unified
    .thumb
    .thumb_func
    .global reverse3
    .type reverse3, %function
reverse3:
       push    {r7, lr}
        sub     sp, sp, #16
        add     r7, sp, #0
        str     r0, [r7, #4]
        movs    r3, #0
        str     r3, [r7, #12]
        movs    r3, #0
        str     r3, [r7, #8]
        b       .L2
.L3:
        ldr     r3, [r7, #12]
        lsls    r2, r3, #1
        ldr     r3, [r7, #4]
        movs    r1, #1
        ands    r3, r1
        orrs    r3, r2
        str     r3, [r7, #12]
        ldr     r3, [r7, #4]
        asrs    r3, r3, #1
        str     r3, [r7, #4]
        ldr     r3, [r7, #8]
        adds    r3, r3, #1
        str     r3, [r7, #8]
.L2:
        ldr     r3, [r7, #8]
        cmp     r3, #31
        ble     .L3
        ldr     r3, [r7, #12]
        lsls    r3, r3, #1
        ldr     r2, .L5
        ands    r2, r3
        ldr     r3, [r7, #12]
        asrs    r3, r3, #1
        ldr     r1, .L5+4
        ands    r3, r1
        orrs    r3, r2
        str     r3, [r7, #12]
        ldr     r3, [r7, #12]
        lsls    r3, r3, #2
        ldr     r2, .L5+8
        ands    r2, r3
        ldr     r3, [r7, #12]
        asrs    r3, r3, #2
        ldr     r1, .L5+12
        ands    r3, r1
        orrs    r3, r2
        str     r3, [r7, #12]
        ldr     r3, [r7, #12]
        lsls    r3, r3, #4
        ldr     r2, .L5+16
        ands    r2, r3
        ldr     r3, [r7, #12]
        asrs    r3, r3, #4
        ldr     r1, .L5+20
        ands    r3, r1
        orrs    r3, r2
        str     r3, [r7, #12]
        ldr     r3, [r7, #12]
        lsls    r2, r3, #24
        ldr     r3, [r7, #12]
        lsls    r1, r3, #8
        movs    r3, #255
        lsls    r3, r3, #16
        ands    r3, r1
        orrs    r2, r3
        ldr     r3, [r7, #12]
        asrs    r1, r3, #8
        movs    r3, #255
        lsls    r3, r3, #8
        ands    r3, r1
        orrs    r3, r2
        ldr     r2, [r7, #12]
        lsrs    r2, r2, #24
        orrs    r3, r2
        str     r3, [r7, #12]
        ldr     r3, [r7, #12]
        movs    r0, r3
        mov     sp, r7
        add     sp, sp, #16
        pop     {r7, pc}
.L5:
        .word   -1431655766
        .word   1431655765
        .word   -858993460
        .word   858993459
        .word   -252645136
        .word   252645135
