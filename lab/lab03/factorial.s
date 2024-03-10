.globl factorial

.data
n: .word 8

.text
main:
    la t0, n
    lw a0, 0(t0)
    jal ra, factorial

    addi a1, a0, 0
    addi a0, x0, 1
    ecall # Print Result

    addi a1, x0, '\n'
    addi a0, x0, 11
    ecall # Print newline

    addi a0, x0, 10
    ecall # Exit

factorial:
    addi sp, sp, -4
    sw ra, 0(sp)
    
    add t0, x0, a0
    addi t1, a0, -1
    
    jal ra, loop
    
    add a0, t0, x0
    lw ra, 0(sp)
    addi sp, sp, 4
    
    jr ra
    
loop:
    beq t1, x0, exit
    mul t0, t0, t1
    addi t1, t1, -1
    j loop
    
exit:
    jr ra
    