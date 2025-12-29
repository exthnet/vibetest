/* mat-mat_v2.2.0.c - AVX-512 micro-kernel with 4x16 register blocking and packing
 * Optimization: AVX-512 8-element SIMD (512-bit), 4x16 micro-kernel
 * Compile: gcc -O3 -march=native -mavx512f -mavx512vl -mfma -o mat-mat_v2.2.0 mat-mat_v2.2.0.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <immintrin.h>

#define N 1000

/* Block sizes for cache optimization */
#define BLOCK_I 64
#define BLOCK_K 256
#define BLOCK_J 64

/* Micro-kernel dimensions: 4 rows x 16 columns (2 AVX-512 registers per row) */
#define MR 4
#define NR 16

double a[N][N], b[N][N], c[N][N];

/* Packed buffers with 64-byte alignment for AVX-512 */
static double packed_A[BLOCK_I * BLOCK_K] __attribute__((aligned(64)));
static double packed_B[BLOCK_K * BLOCK_J] __attribute__((aligned(64)));

/* Pack A panel: BLOCK_I x K into MR x K tiles stored contiguously */
static inline void pack_A(int M, int K, double *src, int lda)
{
    double *dst = packed_A;
    for(int i = 0; i < M; i += MR) {
        int mr = (i + MR <= M) ? MR : M - i;
        for(int k = 0; k < K; k++) {
            for(int ii = 0; ii < mr; ii++) {
                dst[ii] = src[(i + ii) * lda + k];
            }
            for(int ii = mr; ii < MR; ii++) {
                dst[ii] = 0.0;  /* Zero padding */
            }
            dst += MR;
        }
    }
}

/* Pack B panel: K x BLOCK_J into K x NR tiles stored contiguously */
static inline void pack_B(int K, int N_size, double *src, int ldb)
{
    double *dst = packed_B;
    for(int j = 0; j < N_size; j += NR) {
        int nr = (j + NR <= N_size) ? NR : N_size - j;
        for(int k = 0; k < K; k++) {
            for(int jj = 0; jj < nr; jj++) {
                dst[jj] = src[k * ldb + j + jj];
            }
            for(int jj = nr; jj < NR; jj++) {
                dst[jj] = 0.0;  /* Zero padding */
            }
            dst += NR;
        }
    }
}

/* 4x16 AVX-512 micro-kernel: computes C[4x16] += A[4xK] * B[Kx16] */
static inline void micro_kernel_4x16_avx512(int K, double *A, double *B, double *C, int ldc)
{
    /* 8 accumulators: 4 rows x 2 AVX-512 registers (8 elements each) */
    __m512d c00 = _mm512_setzero_pd();
    __m512d c01 = _mm512_setzero_pd();
    __m512d c10 = _mm512_setzero_pd();
    __m512d c11 = _mm512_setzero_pd();
    __m512d c20 = _mm512_setzero_pd();
    __m512d c21 = _mm512_setzero_pd();
    __m512d c30 = _mm512_setzero_pd();
    __m512d c31 = _mm512_setzero_pd();

    for(int k = 0; k < K; k++) {
        /* Load 16 elements of B (2 x 8-element vectors) */
        __m512d b0 = _mm512_load_pd(&B[k * NR]);
        __m512d b1 = _mm512_load_pd(&B[k * NR + 8]);

        /* Broadcast each A element and FMA */
        __m512d a0 = _mm512_set1_pd(A[k * MR + 0]);
        c00 = _mm512_fmadd_pd(a0, b0, c00);
        c01 = _mm512_fmadd_pd(a0, b1, c01);

        __m512d a1 = _mm512_set1_pd(A[k * MR + 1]);
        c10 = _mm512_fmadd_pd(a1, b0, c10);
        c11 = _mm512_fmadd_pd(a1, b1, c11);

        __m512d a2 = _mm512_set1_pd(A[k * MR + 2]);
        c20 = _mm512_fmadd_pd(a2, b0, c20);
        c21 = _mm512_fmadd_pd(a2, b1, c21);

        __m512d a3 = _mm512_set1_pd(A[k * MR + 3]);
        c30 = _mm512_fmadd_pd(a3, b0, c30);
        c31 = _mm512_fmadd_pd(a3, b1, c31);
    }

    /* Load existing C values, add results, and store back */
    /* Row 0 */
    __m512d t00 = _mm512_loadu_pd(&C[0 * ldc]);
    __m512d t01 = _mm512_loadu_pd(&C[0 * ldc + 8]);
    _mm512_storeu_pd(&C[0 * ldc], _mm512_add_pd(t00, c00));
    _mm512_storeu_pd(&C[0 * ldc + 8], _mm512_add_pd(t01, c01));

    /* Row 1 */
    __m512d t10 = _mm512_loadu_pd(&C[1 * ldc]);
    __m512d t11 = _mm512_loadu_pd(&C[1 * ldc + 8]);
    _mm512_storeu_pd(&C[1 * ldc], _mm512_add_pd(t10, c10));
    _mm512_storeu_pd(&C[1 * ldc + 8], _mm512_add_pd(t11, c11));

    /* Row 2 */
    __m512d t20 = _mm512_loadu_pd(&C[2 * ldc]);
    __m512d t21 = _mm512_loadu_pd(&C[2 * ldc + 8]);
    _mm512_storeu_pd(&C[2 * ldc], _mm512_add_pd(t20, c20));
    _mm512_storeu_pd(&C[2 * ldc + 8], _mm512_add_pd(t21, c21));

    /* Row 3 */
    __m512d t30 = _mm512_loadu_pd(&C[3 * ldc]);
    __m512d t31 = _mm512_loadu_pd(&C[3 * ldc + 8]);
    _mm512_storeu_pd(&C[3 * ldc], _mm512_add_pd(t30, c30));
    _mm512_storeu_pd(&C[3 * ldc + 8], _mm512_add_pd(t31, c31));
}

void matmul(double a[N][N], double b[N][N], double c[N][N])
{
    /* Initialize C to zero */
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            c[i][j] = 0.0;
        }
    }

    /* Blocked matrix multiplication with packing */
    for(int ii = 0; ii < N; ii += BLOCK_I) {
        int block_i = (ii + BLOCK_I <= N) ? BLOCK_I : N - ii;

        for(int kk = 0; kk < N; kk += BLOCK_K) {
            int block_k = (kk + BLOCK_K <= N) ? BLOCK_K : N - kk;

            /* Pack A block */
            pack_A(block_i, block_k, &a[ii][kk], N);

            for(int jj = 0; jj < N; jj += BLOCK_J) {
                int block_j = (jj + BLOCK_J <= N) ? BLOCK_J : N - jj;

                /* Pack B block */
                pack_B(block_k, block_j, &b[kk][jj], N);

                /* Micro-kernel loop */
                for(int i = 0; i < block_i; i += MR) {
                    int mr = (i + MR <= block_i) ? MR : block_i - i;
                    double *A_ptr = &packed_A[i * block_k];

                    for(int j = 0; j < block_j; j += NR) {
                        int nr = (j + NR <= block_j) ? NR : block_j - j;
                        double *B_ptr = &packed_B[j * block_k];
                        double *C_ptr = &c[ii + i][jj + j];

                        if(mr == MR && nr == NR) {
                            micro_kernel_4x16_avx512(block_k, A_ptr, B_ptr, C_ptr, N);
                        } else {
                            /* Scalar fallback for edge cases */
                            for(int k = 0; k < block_k; k++) {
                                for(int iii = 0; iii < mr; iii++) {
                                    double a_val = A_ptr[k * MR + iii];
                                    for(int jjj = 0; jjj < nr; jjj++) {
                                        C_ptr[iii * N + jjj] += a_val * B_ptr[k * NR + jjj];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

int main()
{
    struct timeval tv1, tv2;
    double t;
    double gflops;

    /* Initialize matrices */
    for(int j = 0; j < N; j++) {
        for(int i = 0; i < N; i++) {
            a[i][j] = (double)(i * N + j);
            b[i][j] = (double)(i * N + j) * 0.01;
            c[i][j] = 0.0;
        }
    }

    /* Timing */
    gettimeofday(&tv1, NULL);
    matmul(a, b, c);
    gettimeofday(&tv2, NULL);

    t = (double)(tv2.tv_sec - tv1.tv_sec) +
        (double)(tv2.tv_usec - tv1.tv_usec) * 1.0e-6;

    /* GFLOPS calculation: 2*N^3 operations */
    gflops = 2.0 * N * N * N / (t * 1.0e9);

    printf("N = %d, Time = %f sec, GFLOPS = %f\n", N, t, gflops);
    printf("c[0][0] = %f, c[N-1][N-1] = %f\n", c[0][0], c[N-1][N-1]);

    return 0;
}
