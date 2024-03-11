.globl argmax

.text
# =================================================================
# FUNCTION: Given a int vector, return the index of the largest
#	element. If there are multiple, return the one
#	with the smallest index.
# Arguments:
# 	a0 (int*) is the pointer to the start of the vector
#	a1 (int)  is the # of elements in the vector
# Returns:
#	a0 (int)  is the first index of the largest element
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 77.
# =================================================================
argmax:

    # Prologue
    addi sp, sp, -36
    sw ra, 0(sp)
    sw t0, 4(sp)
    sw t1, 8(sp)
    sw t2, 12(sp)
    sw t3, 16(sp)
    sw t4, 20(sp)
    sw s0, 24(sp)
    sw s1, 28(sp)
    sw s2, 32(sp)


loop_start:
    beq a0, x0, Exceptions  # if pointer is null, raise error

    addi t0, x0, 1
    blt a1, t0, Exceptions  # if the length of the vecotr is less than 1, raise error

    add t0, a0, x0  # load the pointer to the array
    add t1, a1, x0  # t1 is the number of elements in the array

    add t2, x0, x0  # t2 is a counter for loop

    add s0, x0, x0  # record the max index
    lw s1, 0(t0)    # record the max value

loop_continue:
    # calculate the position of target position
    li t3, 4    # stride is 4
    mul t4, t2, t3  # calculate the offset, t4 is the offset
    add t4, t4, t0  # calculate the target address

    lw s2, 0(t4)    # load the current value
    blt s2, s1, less_than   # if current value is less than max value, continue, or update the max value and index

    add s0, t2, x0  # max index = current index
    add s1, s2, x0  # max value = current value

less_than:
    addi t2, t2, 1  # update the counter
    blt t2, t1, loop_continue


loop_end:
    
    add a0, s0, x0  # return max value's index
    
    # Epilogue
    lw ra, 0(sp)
    lw t0, 4(sp)
    lw t1, 8(sp)
    lw t2, 12(sp)
    lw t3, 16(sp)
    lw t4, 20(sp)
    lw s0, 24(sp)
    lw s1, 28(sp)
    lw s2, 32(sp)
    addi sp, sp, 36

    ret

Exceptions:
    li a0, 77   # save error code in a0
    li a7, 93   # 93 means exit
    ecall