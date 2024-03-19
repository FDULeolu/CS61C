#include "transpose.h"

/* The naive transpose function as a reference. */
void transpose_naive(int n, int blocksize, int *dst, int *src) {
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            dst[y + x * n] = src[x + y * n];
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
        for (int i = 0; i < group; i++) {
            for (int j = 0; j < group; j++) {
            transpose_naive(blocksize, 0, (dst + i * n * blocksize + j * blocksize), (src + j * n * blocksize + i * blocksize));
            }
        }
    }
    
}
