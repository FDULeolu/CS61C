.globl relu

.text
# ==============================================================================
# FUNCTION: Performs an inplace element-wise ReLU on an array of ints
# Arguments:
# 	a0 (int*) is the pointer to the array
#	a1 (int)  is the # of elements in the array
# Returns:
#	None
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 78.
# ==============================================================================
relu:
    # Prologue
    addi sp, sp, -28
    sw ra, 0(sp)
    sw t0, 4(sp)
    sw t1, 8(sp)
    sw t2, 12(sp)
    sw t3, 16(sp)
    sw t4, 20(sp)
    sw s0, 24(sp)

loop_start:
    beq a0, x0, Exceptions  # if pointer is null, raise error

    addi t0, x0, 1
    blt a1, t0, Exceptions  # if the length of the vecotr is less than 1, raise error

    add t0, a0, x0  # load the pointer to the array
    add t1, a1, x0  # t1 is the number of elements in the array

    add t2, x0, x0  # t2 is a counter for loop

loop_continue:
    # calculate the position of target position
    li t3, 4    # stride is 4
    mul t4, t2, t3  # calculate the offset, t4 is the offset
    add t4, t4, t0  # calculate the target address

    lw s0, 0(t4)    # load the value

    bge s0, x0, greater_than_zero    # if value is less than 0, change the value to zero
    sw x0, 0(t4)

greater_than_zero:
    addi t2, t2, 1  # update the counter
    blt t2, t1, loop_continue


loop_end:


    # Epilogue
    lw ra, 0(sp)
    lw t0, 4(sp)
    lw t1, 8(sp)
    lw t2, 12(sp)
    lw t3, 16(sp)
    lw t4, 20(sp)
    lw s0, 24(sp)
    addi sp, sp, 28

	ret

Exceptions:
    li a1, 78   # save error code in a1
    exit2