.globl dot

.text
# =======================================================
# FUNCTION: Dot product of 2 int vectors
# Arguments:
#   a0 (int*) is the pointer to the start of v0
#   a1 (int*) is the pointer to the start of v1
#   a2 (int)  is the length of the vectors
#   a3 (int)  is the stride of v0
#   a4 (int)  is the stride of v1
# Returns:
#   a0 (int)  is the dot product of v0 and v1
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 75.
# - If the stride of either vector is less than 1,
#   this function terminates the program with error code 76.
# =======================================================
dot:

    # Prologue
    addi sp, sp, -36
    sw ra, 0(sp)
    sw t0, 4(sp)
    sw t1, 8(sp)
    sw t2, 12(sp)
    sw t3, 16(sp)
    sw s0, 20(sp)
    sw s1, 24(sp)
    sw s2, 28(sp)
    sw t4, 32(sp)

loop_start:
    addi t0, x0, 1
    blt a2, t0, vector_length_error # if the length of the vector is less than 1, raise error
    blt a3, t0, stride_error    # if stride is less than 1, raise error
    blt a4, t0, stride_error    # if stride is less than 1, raise error

    add t0, x0, x0  # t0 is a counter
    add t1, x0, x0  # t1 is the sum

loop_continue:
    addi t4, x0, 4
    mul t2, t4, a3  
    mul t2, t0, t2  # calculate the first vector offset
    add t3, t2, a0  # get the first value's address
    lw s0, 0(t3)    # get the first value

    mul t2, t4, a4 
    mul t2, t0, t2  # calculate the second vector offset
    add t3, t2, a1  # get the second value's address
    lw s1, 0(t3)    # get the second value

    mul s2, s0, s1  # calculate the product
    add t1, s2, t1  # sum the result

    addi t0, t0, 1  # update the counter
    blt t0, a2, loop_continue   # loop

loop_end:
    add a0, t1, x0  # save return value

    # Epilogue
    lw ra, 0(sp)
    lw t0, 4(sp)
    lw t1, 8(sp)
    lw t2, 12(sp)
    lw t3, 16(sp)
    lw s0, 20(sp)
    lw s1, 24(sp)
    lw s2, 28(sp)
    lw t4, 32(sp)
    addi sp, sp, 36
    
    ret

vector_length_error:
    li a1, 75   # save error code in a1
    j exit2

stride_error:
    li a1, 76   # save error code in a1
    j exit2