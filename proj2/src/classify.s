.globl classify

.text
classify:
    # =====================================
    # COMMAND LINE ARGUMENTS
    # =====================================
    # Args:
    #   a0 (int)    argc
    #   a1 (char**) argv
    #   a2 (int)    print_classification, if this is zero, 
    #               you should print the classification. Otherwise,
    #               this function should not print ANYTHING.
    # Returns:
    #   a0 (int)    Classification
    # Exceptions:
    # - If there are an incorrect number of command line args,
    #   this function terminates the program with exit code 89.
    # - If malloc fails, this function terminats the program with exit code 88.
    #
    # Usage:
    #   main.s <M0_PATH> <M1_PATH> <INPUT_PATH> <OUTPUT_PATH>

    # Check the number of arguments
    li t0, 5
    bne a0, t0, exit_wrong_argc

    # prologue
    addi sp, sp, -48
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)
    sw s6, 28(sp)
    sw s7, 32(sp)
    sw s8, 36(sp)
    sw s9, 40(sp)
    sw s10, 44(sp)

    mv s0, a1   # load argv
    mv s1, a2   # load print flag

	# =====================================
    # LOAD MATRICES
    # =====================================

    # Load pretrained m0
    li a0, 8        # load rows and columns

    jal malloc
    beq a0, x0, exit_malloc_error

    mv s2, a0       # s2 stores rows and columns for m0
    lw a0, 4(s0)    # load m0 path
    addi a1, s2, 0  # row
    addi a2, s2, 4  # column

    jal read_matrix

    mv s3, a0       # s3 is a pointer to m0

    # Load pretrained m1
    li a0, 8        # load rows and columns

    jal malloc
    beq a0, x0, exit_malloc_error

    mv s4, a0       # s4 stores rows and columns for m1
    lw a0, 8(s0)    # load m1 path
    addi a1, s4, 0  # row
    addi a2, s4, 4  # column

    jal read_matrix

    mv s5, a0       # s5 is a pointer to m1

    # Load input matrix
    li a0, 8        # load rows and columns

    jal malloc
    beq a0, x0, exit_malloc_error

    mv s6, a0       # s6 stores rows and columns for m1
    lw a0, 12(s0)    # load input matrix path
    addi a1, s6, 0  # row
    addi a2, s6, 4  # column

    jal read_matrix

    mv s7, a0       # s7 is a pointer to input matrix

    # =====================================
    # RUN LAYERS
    # =====================================
    # 1. LINEAR LAYER:    m0 * input
    # 2. NONLINEAR LAYER: ReLU(m0 * input)
    # 3. LINEAR LAYER:    m1 * ReLU(m0 * input)

    lw t0, 0(s2)
    lw t1, 4(s6)
    mul a0, t0, t1  # m0 * number of input's elements
    slli a0, a0, 2  # plus 4

    jal malloc
    beq a0, x0, exit_malloc_error

    mv s8, a0       # s8 is a pointer to hidden layer matrix

    mv a0, s3
    lw a1, 0(s2)
    lw a2, 4(s2)
    mv a3, s7
    lw a4, 0(s6)
    lw a5, 4(s6)
    mv a6, s8       # load params

    jal matmul

    # Put in ReLU
    lw t0, 0(s2)
    lw t1, 4(s6)
    mul a1, t0, t1  # number of elements in hidden layer matrix
    mv a0, s8       # load the pointer to hidden layer matrix

    jal relu

    li a0, 40       

    jal malloc
    beq a0, x0, exit_malloc_error

    mv s9, a0       # s9 is a pointer to score matrix

    mv a0, s5       
    lw a1, 0(s4)
    lw a2, 4(s4)
    mv a3, s8
    lw a4, 0(s2)
    lw a5, 4(s6)
    mv a6, s9       # load params

    jal matmul


    # =====================================
    # WRITE OUTPUT
    # =====================================
    # Write output matrix
    lw a0, 16(s0)   # output path
    mv a1, s9
    lw a2, 0(s4)
    lw a3, 4(s6)    # load params

    jal write_matrix

    # =====================================
    # CALCULATE CLASSIFICATION/LABEL
    # =====================================
    # Call argmax
    mv a0, s9
    li a1, 10       # load params

    jal argmax

    mv s10, a0      # store the result of classifation

    bne s1, x0, no_print

    # Print classification
    mv a1, s10
    jal print_int   # print out the classifation result

    # Print newline afterwards for clarity
    li a1, '\n'
    jal print_char  # print a new line

no_print:
    # Free the space
    mv a0, s2
    jal free

    mv a0, s3
    jal free

    mv a0, s4
    jal free

    mv a0, s5
    jal free

    mv a0, s6
    jal free

    mv a0, s7
    jal free

    mv a0, s8
    jal free

    mv a0, s9
    jal free

    mv a0, s10      # return the result

    # epilogue
    lw ra, 0(sp)
    lw s0, 4(sp)
    lw s1, 8(sp)
    lw s2, 12(sp)
    lw s3, 16(sp)
    lw s4, 20(sp)
    lw s5, 24(sp)
    lw s6, 28(sp)
    lw s7, 32(sp)
    lw s8, 36(sp)
    lw s9, 40(sp)
    lw s10, 44(sp)
    addi sp, sp, 48

    ret

exit_wrong_argc:
    li a1, 89
    j exit2

exit_malloc_error:
    li a1, 88
    j exit2
