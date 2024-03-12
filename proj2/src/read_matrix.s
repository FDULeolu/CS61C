.globl read_matrix

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
# Exceptions:
# - If malloc returns an error,
#   this function terminates the program with error code 88.
# - If you receive an fopen error or eof, 
#   this function terminates the program with error code 90.
# - If you receive an fread error or eof,
#   this function terminates the program with error code 91.
# - If you receive an fclose error or eof,
#   this function terminates the program with error code 92.
# ==============================================================================
read_matrix:

    # Prologue
    addi sp, sp, -52
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw t0, 24(sp)
    sw t1, 28(sp)
    sw t2, 32(sp)
    sw t3, 36(sp)
    sw s5, 40(sp)
    sw s6, 44(sp)
    sw s7, 48(sp)

    mv s0, a0                   # s0 is a pointer to filename
    mv s1, a1                   # s1 is a pointer to row
    mv s2, a2                   # s2 is a pointer to column

    # Open the file
	mv a1, s0
    li a2, 0                    # load params for fopen

    jal fopen                   # open the file, now we have a0 as file descriptor

    li t0, -1
    beq a0, t0, exception_90    # if file descriptor is -1, raise error code 90
    mv s3, a0                   # s3 is the file descriptor
    
    # Read the matrix
    # Read the row number
    mv a1, s3                   # load file descriptor
    mv a2, s1                   # load the pointer to the row
    li a3, 4                    # row is 4 bytes

    jal fread                   # read the row
    li t0, 4
    bne a0, t0, exception_91    # meet an error when reading file

    # Read the column number 
    mv a1, s3                   # load file descriptor
    mv a2, s2                   # load the pointer to the column
    li a3, 4                    # column is 4 bytes

    jal fread                   # read the column
    li t0, 4
    bne a0, t0, exception_91    # meet an error when reading file

    # Calculate the bytes matrix need
    lw t0, 0(s1)                # row number
    lw t1, 0(s2)                # column number

    mul t2, t0, t1              # number of elements in matrix
    li t3, 4                    # every elemet is 4 bytes
    mul s5, t2, t3              # t2 is the number of bytes 
    mv s6, t2                   # s6 store number of elements in matrix

    # Malloc space in memory
    mv a0, s5                   # load the number of bytes

    jal malloc
    beq a0, x0, exception_88    # meet an error when mallocing space

    mv s4, a0                   # save the pointer to matrix in s4

    # Read the matrix
    mv s7, x0                   # loop counter, start from zero
loop_start:

    li t0, 4
    mul t1, t0, s7              # calculate the offset
    add t1, t1, s4              # calculate the address to read in number

    mv a1, s3                   # load file descriptor
    mv a2, t1                   # load the pointer to the start address
    li a3, 4                    # read 4 bytes every time

    jal fread                   # read the matrix
    li t0, 4
    bne a0, t0, exception_91    # meet an error when reading matrix

    addi s7, s7, 1              # update the counter
    bne s7, s6, loop_start      # loop

    # Finish read, close the file
    mv a1, s3                   # load file descriptor

    jal fclose                  # close the file
    bne a0, x0, exception_92    # meet an error when closing 

    mv a0, s4                   # return matrix address

    # Epilogue
    lw ra, 0(sp)
    lw s0, 4(sp)
    lw s1, 8(sp)
    lw s2, 12(sp)
    lw s3, 16(sp)
    lw s4, 20(sp)
    lw t0, 24(sp)
    lw t1, 28(sp)
    lw t2, 32(sp)
    lw t3, 36(sp)
    lw s5, 40(sp)
    lw s6, 44(sp)
    lw s7, 48(sp)
    addi sp, sp, 52

    ret

exception_88:
    li a1, 88
    j exit2

exception_90:
    li a1, 90
    j exit2

exception_91:
    li a1, 91
    j exit2

exception_92:
    li a1, 92
    j exit2
