#include <stdint.h>

#define MATRIX_DIMS_1 5
#define MATRIX_DIMS_2 10

int32_t multiply(int32_t a, int32_t b);
void matrix_multiply(int32_t m, int32_t n, int32_t p, int32_t *A, int32_t *B, int32_t *C);

int32_t _start()
{
    int32_t A[MATRIX_DIMS_1 * MATRIX_DIMS_2]; // 10x50 matrix
    for (int32_t i = 0; i < MATRIX_DIMS_1 * MATRIX_DIMS_2; i++)
    {
        A[i] = i;
    }

    int32_t B[MATRIX_DIMS_1 * MATRIX_DIMS_2]; // 50x10 matrix
    for (int32_t i = 0; i < MATRIX_DIMS_1 * MATRIX_DIMS_2; i++)
    {
        B[i] = i;
    }

    int32_t C[MATRIX_DIMS_1 * MATRIX_DIMS_1]; // 10x10 matrix
    int32_t i, j;

    matrix_multiply(MATRIX_DIMS_1, MATRIX_DIMS_2, MATRIX_DIMS_1, A, B, C);

    return C[0];
}

int32_t multiply(int32_t a, int32_t b)
{
    int32_t result = 0;
    for (uint32_t i = 0; i < b; i++)
    {
        result += a;
    }
    return result;
}

void matrix_multiply(int32_t m, int32_t n, int32_t p, int32_t *A, int32_t *B, int32_t *C)
{
    int32_t i, j, k;

    for (i = 0; i < m; i++)
    {
        for (j = 0; j < p; j++)
        {
            *(C + multiply(i, p) + j) = 0;
            for (k = 0; k < n; k++)
            {
                *(C + multiply(i, p) + j) += multiply(*(A + multiply(i, n) + k), *(B + multiply(k, p) + j));
            }
        }
    }
}

/*
Python NumPy script to generate the expected output for this kernel:

import numpy as np
a = np.array([i for i in range(50)]).reshape(5, 10)
b = np.array([i for i in range(50)]).reshape(10, 5)
a @ b
*/
