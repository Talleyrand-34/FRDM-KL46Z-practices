    .syntax unified
    .thumb
    .thumb_func
    .global reverse2
    .type reverse2, %function

reverse2:
    @ Entrada: r0 (valor a invertir)
    @ Salida : r0 (valor con bits invertidos)
    movs    r3, r0        @ r3 <- input copy
    push    {lr}          @ conservar lr
    movs    r2, #32       @ contador de 32 bits
    movs    r0, #0        @ acumulador resultado = 0
    movs    r1, #0        @ r1 = 0 (operando para ADCS)

.L_loop:
    lsls    r0, r0, #1    @ desplazar acumulador a la izquierda
    lsrs    r3, r3, #1    @ desplazar entrada → LSB a Carry
    adcs    r0, r1        @ r0 = r0 + r1 + Carry  (inserta el bit)
    subs    r2, r2, #1    @ dec contador
    bne     .L_loop

    pop     {pc}          @ volver (lr)
    .size reverse2, .-reverse2
