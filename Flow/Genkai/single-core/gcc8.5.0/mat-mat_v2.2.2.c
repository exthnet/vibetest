/* mat-mat_v2.2.2.c - AVX-512 version based on v2.1.0 (MR=4, NR=8) */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <immintrin.h>

#define N 1000
#define BLOCK_I 64
#define BLOCK_K 256
#define BLOCK_J 64
#define MR 4
#define NR 8

double a[N][N], b[N][N], c[N][N];

static double packed_A[BLOCK_I * BLOCK_K] __attribute__((aligned(64)));
static double packed_B[BLOCK_K * BLOCK_J] __attribute__((aligned(64)));

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
                dst[ii] = 0.0;
            }
            dst += MR;
        }
    }
}

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
                dst[jj] = 0.0;
            }
            dst += NR;
        }
    }
}

/* 4x8 AVX-512 micro-kernel: uses single 512-bit register for 8 B elements */
static inline void micro_kernel_4x8_avx512(int K, double *A, double *B, double *C, int ldc)
{
    __m512d c0 = _mm512_setzero_pd();
    __m512d c1 = _mm512_setzero_pd();
    __m512d c2 = _mm512_setzero_pd();
    __m512d c3 = _mm512_setzero_pd();

    for(int k = 0; k < K; k++) {
        __m512d b0 = _mm512_load_pd(&B[k * NR]);

        __m512d a0 = _mm512_set1_pd(A[k * MR + 0]);
        __m512d a1 = _mm512_set1_pd(A[k * MR + 1]);
        __m512d a2 = _mm512_set1_pd(A[k * MR + 2]);
        __m512d a3 = _mm512_set1_pd(A[k * MR + 3]);

        c0 = _mm512_fmadd_pd(a0, b0, c0);
        c1 = _mm512_fmadd_pd(a1, b0, c1);
        c2 = _mm512_fmadd_pd(a2, b0, c2);
        c3 = _mm512_fmadd_pd(a3, b0, c3);
    }

    __m512d t0 = _mm512_loadu_pd(&C[0 * ldc]);
    __m512d t1 = _mm512_loadu_pd(&C[1 * ldc]);
    __m512d t2 = _mm512_loadu_pd(&C[2 * ldc]);
    __m512d t3 = _mm512_loadu_pd(&C[3 * ldc]);

    _mm512_storeu_pd(&C[0 * ldc], _mm512_add_pd(t0, c0));
    _mm512_storeu_pd(&C[1 * ldc], _mm512_add_pd(t1, c1));
    _mm512_storeu_pd(&C[2 * ldc], _mm512_add_pd(t2, c2));
    _mm512_storeu_pd(&C[3 * ldc], _mm512_add_pd(t3, c3));
}

void matmul(double a[N][N], double b[N][N], double c[N][N])
{
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            c[i][j] = 0.0;
        }
    }

    for(int ii = 0; ii < N; ii += BLOCK_I) {
        int block_i = (ii + BLOCK_I <= N) ? BLOCK_I : N - ii;
        for(int kk = 0; kk < N; kk += BLOCK_K) {
            int block_k = (kk + BLOCK_K <= N) ? BLOCK_K : N - kk;
            pack_A(block_i, block_k, &a[ii][kk], N);
            for(int jj = 0; jj < N; jj += BLOCK_J) {
                int block_j = (jj + BLOCK_J <= N) ? BLOCK_J : N - jj;
                pack_B(block_k, block_j, &b[kk][jj], N);
                for(int i = 0; i < block_i; i += MR) {
                    int mr = (i + MR <= block_i) ? MR : block_i - i;
                    double *A_ptr = &packed_A[i * block_k];
                    for(int j = 0; j < block_j; j += NR) {
                        int nr = (j + NR <= block_j) ? NR : block_j - j;
                        double *B_ptr = &packed_B[j * block_k];
                        double *C_ptr = &c[ii + i][jj + j];
                        if(mr == MR && nr == NR) {
                            micro_kernel_4x8_avx512(block_k, A_ptr, B_ptr, C_ptr, N);
                        } else {
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
    double t, gflops;

    for(int j = 0; j < N; j++) {
        for(int i = 0; i < N; i++) {
            a[i][j] = (double)(i * N + j);
            b[i][j] = (double)(i * N + j) * 0.01;
            c[i][j] = 0.0;
        }
    }

    gettimeofday(&tv1, NULL);
    matmul(a, b, c);
    gettimeofday(&tv2, NULL);

    t = (double)(tv2.tv_sec - tv1.tv_sec) + (double)(tv2.tv_usec - tv1.tv_usec) * 1.0e-6;
    gflops = 2.0 * N * N * N / (t * 1.0e9);

    printf("N = %d, Time = %f sec, GFLOPS = %f\n", N, t, gflops);
    printf("c[0][0] = %f, c[N-1][N-1] = %f\n", c[0][0], c[N-1][N-1]);

    return 0;
}
