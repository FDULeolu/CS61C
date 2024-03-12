.globl matmul

.text
# =======================================================
# FUNCTION: Matrix Multiplication of 2 integer matrices
# 	d = matmul(m0, m1)
# Arguments:
# 	a0 (int*)  is the pointer to the start of m0 
#	a1 (int)   is the # of rows (height) of m0
#	a2 (int)   is the # of columns (width) of m0
#	a3 (int*)  is the pointer to the start of m1
# 	a4 (int)   is the # of rows (height) of m1
#	a5 (int)   is the # of columns (width) of m1
#	a6 (int*)  is the pointer to the the start of d
# Returns:
#	None (void), sets d = matmul(m0, m1)
# Exceptions:
#   Make sure to check in top to bottom order!
#   - If the dimensions of m0 do not make sense,
#     this function terminates the program with exit code 72.
#   - If the dimensions of m1 do not make sense,
#     this function terminates the program with exit code 73.
#   - If the dimensions of m0 and m1 don't match,
#     this function terminates the program with exit code 74.
# =======================================================
matmul:

    # Error checks
    ble a1, x0, exception_72
    ble a2, x0, exception_72
    ble a4, x0, exception_73
    ble a5, x0, exception_73
    bne a2, a4, exception_74
    # Prologue
    addi sp, sp, -56
    sw s2, 0(sp)
    sw s3, 4(sp)
    sw s4, 8(sp)
    sw t3, 12(sp)
    sw s0, 16(sp)
    sw s1, 20(sp)
    sw s5, 24(sp)
    sw s6, 28(sp)
    sw s7, 32(sp)
    sw s8, 36(sp)
    sw s9, 40(sp)
    sw s10, 44(sp)
    sw s11, 48(sp)
    sw ra, 52(sp)


    mv s5, a0
    mv s6, a1
    mv s7, a2
    mv s8, a3
    mv s9, a4
    mv s10, a5
    mv s11, a6  # store params

    li s2, 0  # outer loop counter
    li s4, 4    # s4 is always 4

outer_loop_start:
    mul t3, s4, s2
    mul t3, t3, s7  # calculate offset
    add s0, s5, t3  # calculate the address of t0th row

    li s3, 0  # inner loop counter

inner_loop_start:
    mul t3, s4, s3  # calculate offset
    add s1, t3, s8  # calculate the address of t1th column

    mv a0, s0
    mv a1, s1
    mv a2, s7
    addi a3, x0, 1
    mv a4, s10   # load params for dot.s

    jal dot # call dot


    # DEBUG
    mul t3, s2, s10 
    add t3, t3, s3  
    mul t3, t3, s4  # calculate d's offset
    add t3, s11, t3 # calculate the address of target position in d
    # DEBUG

    sw a0, 0(t3)

inner_loop_end:
    addi s3, s3, 1
    blt s3, s10, inner_loop_start


outer_loop_end:
    addi s2, s2, 1
    blt s2, s7, outer_loop_start

    # Epilogue
    lw s2, 0(sp)
    lw s3, 4(sp)
    lw s4, 8(sp)
    lw t3, 12(sp)
    lw s0, 16(sp)
    lw s1, 20(sp)
    lw s5, 24(sp)
    lw s6, 28(sp)
    lw s7, 32(sp)
    lw s8, 36(sp)
    lw s9, 40(sp)
    lw s10, 44(sp)
    lw s11, 48(sp)
    lw ra, 52(sp)
    addi sp, sp, 56
    ret


exception_72:
    li a1, 72   # save error code in a1
    j exit2
exception_73:
    li a1, 73   # save error code in a1
    j exit2
exception_74:
    li a1, 74   # save error code in a1
    j exit2