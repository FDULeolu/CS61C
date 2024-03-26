#include "matrix.h"
#include <cstdio>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include<math.h>

// Include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#include <x86intrin.h>
#endif

/* Below are some intel intrinsics that might be useful
 * void _mm256_storeu_pd (double * mem_addr, __m256d a)
 * __m256d _mm256_set1_pd (double a)
 * __m256d _mm256_set_pd (double e3, double e2, double e1, double e0)
 * __m256d _mm256_loadu_pd (double const * mem_addr)
 * __m256d _mm256_add_pd (__m256d a, __m256d b)
 * __m256d _mm256_sub_pd (__m256d a, __m256d b)
 * __m256d _mm256_fmadd_pd (__m256d a, __m256d b, __m256d c)
 * __m256d _mm256_mul_pd (__m256d a, __m256d b)
 * __m256d _mm256_cmp_pd (__m256d a, __m256d b, const int imm8)
 * __m256d _mm256_and_pd (__m256d a, __m256d b)
 * __m256d _mm256_max_pd (__m256d a, __m256d b)
*/

/*
 * Generates a random double between `low` and `high`.
 */
double rand_double(double low, double high) {
    double range = (high - low);
    double div = RAND_MAX / range;
    return low + (rand() / div);
}

/*
 * Generates a random matrix with `seed`.
 */
void rand_matrix(matrix *result, unsigned int seed, double low, double high) {
    srand(seed);
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, rand_double(low, high));
        }
    }
}

/*
 * Allocate space for a matrix struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data array
 * and initialize all entries to be zeros. Remember to set all fieds of the matrix struct.
 * `parent` should be set to NULL to indicate that this matrix is not a slice.
 * You should return -1 if either `rows` or `cols` or both have invalid values, or if any
 * call to allocate memory in this function fails. If you don't set python error messages here upon
 * failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix(matrix **mat, int rows, int cols) {
    if (rows < 1 || cols < 1) {
        fprintf(stderr, "invalid input");
        return -1;  // invalid input
    }

    *mat = (matrix*) malloc(sizeof(matrix));
    if (*mat == NULL) {
        fprintf(stderr, "allocation failed");
        return -1;  // allocation failed
    }

    (*mat)->data = (double **) malloc(rows * sizeof(double *));
    if ((*mat)->data == NULL) {
        free(*mat);
        fprintf(stderr, "allocation failed");
        return -1;  // allocation failed
    }

    for (int i = 0; i < rows; i++) {
        (*mat)->data[i] = (double *) calloc(cols, sizeof(double));
        if ((*mat)->data[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free((*mat)->data[j]);
            }
            free((*mat)->data);
            free(*mat);
            fprintf(stderr, "allocation failed");
            return -1;
        }
    }

    (*mat)->cols = cols;
    (*mat)->rows = rows;
    (*mat)->parent = NULL;
    (*mat)->ref_cnt = 0;
    if (rows == 1 | cols == 1) {
        (*mat)->is_1d = 1;
    } else {
        (*mat)->is_1d = 0;
    }

    return 0;
}

/*
 * Allocate space for a matrix struct pointed to by `mat` with `rows` rows and `cols` columns.
 * This is equivalent to setting the new matrix to be
 * from[row_offset:row_offset + rows, col_offset:col_offset + cols]
 * If you don't set python error messages here upon failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix_ref(matrix **mat, matrix *from, int row_offset, int col_offset,
                        int rows, int cols) {
    if (rows < 1 || cols < 1) {
        fprintf(stderr, "invalid input");
        return -1;  // invalid input
    }

    *mat = (matrix*) malloc(sizeof(matrix));
    if (*mat == NULL) {
        fprintf(stderr, "allocation failed");
        return -1;  // allocation failed
    }

    (*mat)->data = (double **) malloc(rows * sizeof(double *));
    if ((*mat)->data == NULL) {
        free(*mat);
        fprintf(stderr, "allocation failed");
        return -1;  // allocation failed
    }

    for (int i = 0; i < rows; i++) {
        (*mat)->data[i] = from->data[row_offset + i];    
    }

    (*mat)->cols = cols;
    (*mat)->rows = rows;
    (*mat)->parent = from;
    (*mat)->ref_cnt = 0;
    if (rows == 1 | cols == 1) {
        (*mat)->is_1d = 1;
    } else {
        (*mat)->is_1d = 0;
    }

    from->ref_cnt++;

    return 0;
}

/*
 * This function will be called automatically by Python when a numc matrix loses all of its
 * reference pointers.
 * You need to make sure that you only free `mat->data` if no other existing matrices are also
 * referring this data array.
 * See the spec for more information.
 */
void deallocate_matrix(matrix *mat) {
    if (mat->parent == NULL) {
        for (int i = 0; i < mat->rows; i++) {
            free(mat->data[i]);
        }
    } else {
        mat->parent->ref_cnt--;
    }
    free(mat->data);
    free(mat);
}

/*
 * Return the double value of the matrix at the given row and column.
 * You may assume `row` and `col` are valid.
 */
double get(matrix *mat, int row, int col) {
    return mat->data[row][col];
}

/*
 * Set the value at the given row and column to val. You may assume `row` and
 * `col` are valid
 */
void set(matrix *mat, int row, int col, double val) {
    mat->data[row][col] = val;
}

/*
 * Set all entries in mat to val
 */
void fill_matrix(matrix *mat, double val) {
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            mat->data[i][j] = val;
        }
    }
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    if (result == NULL) {
        int flag = allocate_matrix(&result, mat1->rows, mat1->cols);
        if (flag != 0) {
            return -1;
        }
    }
    if (mat1->cols != mat2->cols || mat1->rows != mat2->rows) {
        fprintf(stderr, "Can't add two matrics with different shape");
        return -1;
    }
    for (int i = 0; i < mat1->rows; i++) {
        for (int j = 0; j < mat1->cols; j++) {
            result->data[i][j] = mat1->data[i][j] + mat2->data[i][j];
        }
    }
    return 0;
}

/*
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    if (result == NULL) {
        int flag = allocate_matrix(&result, mat1->rows, mat1->cols);
        if (flag != 0) {
            return -1;
        }
    }
    if (mat1->cols != mat2->cols || mat1->rows != mat2->rows) {
        fprintf(stderr, "Can't sub matrics with different shape");
        return -1;
    }
    for (int i = 0; i < mat1->rows; i++) {
        for (int j = 0; j < mat1->cols; j++) {
            result->data[i][j] = mat1->data[i][j] - mat2->data[i][j];
        }
    }
    return 0;
}

/*
 * Store the result of multiplying mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that matrix multiplication is not the same as multiplying individual elements.
 */
int mul_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    if (result == NULL) {
        int flag = allocate_matrix(&result, mat1->rows, mat2->cols);
        if (flag != 0) {
            return -1;
        }
    }
    if (mat1->cols != mat2->rows) {
        fprintf(stderr, "the number of mat1's cols should be equal to the number of mat2's rows");
        return -1;
    }
    for (int i = 0; i < mat1->rows; i++) {
        for (int j = 0; j < mat2->cols; j++) {
            for (int k = 0;  k < mat1->cols; k++) {
                result->data[i][j] += mat1->data[i][k] * mat2->data[k][j];
            }
        }
    }
    return 0;
}

/*
 * Store the result of raising mat to the (pow)th power to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that pow is defined with matrix multiplication, not element-wise multiplication.
 */
int pow_matrix(matrix *result, matrix *mat, int pow) {
    if (mat->cols != mat->rows) {
        fprintf(stderr, "mat should be a n x n matrix");
        return -1;
    }
    if (result == NULL) {
        int flag_result = allocate_matrix(&result, mat->rows, mat->cols);
        if (flag_result != 0) {
            return -1;
        }
    }
    matrix *tmp;
    int flag_tmp = allocate_matrix(&tmp, mat->rows, mat->cols);
    if (flag_tmp != 0) {
        return -1;
    }
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            result->data[i][j] = mat->data[i][j];
        }
    }
    while (pow > 0) {
        for (int i = 0; i < mat->rows; i++) {
            for (int j = 0; j < mat->cols; j++) {
                tmp->data[i][j] = result->data[i][j];
            }
        }
        mul_matrix(result, tmp, mat);
        pow--;
    }
    for (int i = 0; i < mat->rows; i++) {
        free(tmp->data[i]);
    }
    free(tmp->data);
    free(tmp);
    return 0;
    
}

/*
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int neg_matrix(matrix *result, matrix *mat) {
    int flag = allocate_matrix(&result, mat->rows, mat->cols);
    if (flag != 0) {
        return -1;
    }
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            result->data[i][j] = - mat->data[i][j];
        }
    }
    return 0;
}

/*
 * Store the result of taking the absolute value element-wise to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int abs_matrix(matrix *result, matrix *mat) {
    int flag = allocate_matrix(&result, mat->rows, mat->cols);
    if (flag != 0) {
        return -1;
    }
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            result->data[i][j] = fabs(mat->data[i][j]);
        }
    }
    return 0;
}
