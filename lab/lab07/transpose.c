#include "transpose.h"
#include "stdio.h"

/* The naive transpose function as a reference. */
void transpose_naive(int n, int blocksize, int *dst, int *src) {
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            dst[y + x * n] = src[x + y * n];
        }
    }
}

// // DEBUG
// void print_matrix(int n, int *A) {
//     for (int i = 0; i < n; i++) {
//         for (int j = 0; j < n; j++) {
//             printf("%d\t", A[i * n + j]);
//         }
//         printf("\n");
//     }
//     printf("\n");
// }

void transpose_blocking_helper(int n, int blocksize, int *dst, int *src) {
    for (int x = 0; x < blocksize; x++) {
        for (int y = 0; y < blocksize; y++) {
            dst[x * n + y] = src[y * n + x];
        }
    }
}

/* Implement cache blocking below. You should NOT assume that n is a
 * multiple of the block size. */
void transpose_blocking(int n, int blocksize, int *dst, int *src) {
    if (n == 1) {
        dst[0] = src[0];
    } else if (n % blocksize != 0) {
        transpose_blocking(n, blocksize - 1, dst, src);
    } else {
        int group = n / blocksize;
        // //DEBUG
        // printf("\nsrc:\n");
        // print_matrix(n, src);
        // //DEBUG
        for (int i = 0; i < group; i++) {
            for (int j = 0; j < group; j++) {
            transpose_blocking_helper(n, blocksize, (dst + i * n * blocksize + j * blocksize), (src + j * n * blocksize + i * blocksize));
            // //DEBUG
            // printf("Block %d\n", i * group + j);
            // printf("dst:\n");
            // print_matrix(n, dst);
            // //DEBUG
            }
        }
    }
    
}
