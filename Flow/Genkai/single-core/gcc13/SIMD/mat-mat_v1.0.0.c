/*
 * mat-mat_v1.0.0.c - Matrix Multiplication with Auto-Vectorization
 * PG1.3 - GCC 13.3.1 SIMD Optimization
 *
 * v1.0.0: Loop reordering (i-k-j) for better cache utilization
 *         + compiler auto-vectorization hints
 *
 * Compile: gcc -O3 -march=native -mavx512f -mfma -funroll-loops -ftree-vectorize mat-mat.c -o a.out
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void matmul(int N, double * __restrict__ a, double * __restrict__ b, double * __restrict__ c)
{
    int i, j, k;

    /* Loop reordering: i-k-j for better memory access pattern
     * This allows vectorization of the innermost j-loop
     * and improves cache locality by accessing b[k*N+j] sequentially
     */
    for (i = 0; i < N; i++) {
        for (k = 0; k < N; k++) {
            double aik = a[i*N + k];
            #pragma GCC ivdep
            for (j = 0; j < N; j++) {
                c[i*N + j] += aik * b[k*N + j];
            }
        }
    }
}

double get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

int main(int argc, char **argv)
{
    int i, j;
    int N;
    double *a = NULL, *b = NULL, *c = NULL;
    int out;
    double start, end, elapsed;
    double gflops;

    out = 1;
    if (argc == 1) {
        N = 10;
    } else {
        N = atoi(argv[1]);
        if (argc == 3) {
            out = 0;
        }
    }
    printf("N = %d\n", N);
    printf("out = %d\n", out);

    /* Aligned memory allocation for SIMD */
    a = (double*)aligned_alloc(64, sizeof(double) * N * N);
    b = (double*)aligned_alloc(64, sizeof(double) * N * N);
    c = (double*)aligned_alloc(64, sizeof(double) * N * N);

    if (!a || !b || !c) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    /* Initialize matrices */
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            a[i*N + j] = (double)(i + 1) + (double)(j + 1) * 0.01;
            b[i*N + j] = (double)(i + 1) + (double)(j + 1) * 0.01;
            c[i*N + j] = 0.0;
        }
    }

    /* Main computation with timing */
    start = get_time();
    matmul(N, a, b, c);
    end = get_time();

    elapsed = end - start;
    gflops = (2.0 * (double)N * (double)N * (double)N) / (elapsed * 1e9);

    printf("Elapsed time: %.6f sec\n", elapsed);
    printf("Performance: %.1f GFLOPS\n", gflops);

    /* Result output (optional) */
    if (out == 1) {
        printf("result:\n");
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                printf(" %8.2f", c[i*N + j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    free(c);
    free(b);
    free(a);

    return 0;
}
