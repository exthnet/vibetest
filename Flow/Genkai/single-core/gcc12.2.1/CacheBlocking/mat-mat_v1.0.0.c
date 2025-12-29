#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BLOCK_SIZE 64  // キャッシュブロックサイズ

void matmul(int N, double *a, double *b, double *c)
{
  int i, j, k;
  int ii, jj, kk;
  int i_end, j_end, k_end;

  // キャッシュブロッキング: ブロック単位でループを分割
  for (ii = 0; ii < N; ii += BLOCK_SIZE) {
    i_end = (ii + BLOCK_SIZE < N) ? ii + BLOCK_SIZE : N;
    for (jj = 0; jj < N; jj += BLOCK_SIZE) {
      j_end = (jj + BLOCK_SIZE < N) ? jj + BLOCK_SIZE : N;
      for (kk = 0; kk < N; kk += BLOCK_SIZE) {
        k_end = (kk + BLOCK_SIZE < N) ? kk + BLOCK_SIZE : N;

        // ブロック内の計算
        for (i = ii; i < i_end; i++) {
          for (j = jj; j < j_end; j++) {
            double sum = c[i*N+j];
            for (k = kk; k < k_end; k++) {
              sum += a[i*N+k] * b[k*N+j];
            }
            c[i*N+j] = sum;
          }
        }
      }
    }
  }
}

int main(int argc, char **argv)
{
  int i, j, k;
  int N;
  double *a=NULL, *b=NULL, *c=NULL;
  int out;
  clock_t start, end;
  double elapsed;
  double gflops;

  out = 1;
  if(argc==1){
    N = 10;
  }else{
    N = atoi(argv[1]);
    if(argc==3){
      out = 0;
    }
  }
  printf("N = %d\n", N);
  printf("out = %d\n", out);
  printf("BLOCK_SIZE = %d\n", BLOCK_SIZE);

  // 初期化
  a = (double*)malloc(sizeof(double)*N*N);
  b = (double*)malloc(sizeof(double)*N*N);
  c = (double*)malloc(sizeof(double)*N*N);
  for(i=0; i<N; i++){
    for(j=0; j<N; j++){
      a[i*N+j] = (double)(i+1) + (double)(j+1)*0.01;
      b[i*N+j] = (double)(i+1) + (double)(j+1)*0.01;
      c[i*N+j] = 0.0;
    }
  }

  // メイン計算部（時間計測）
  start = clock();
  matmul(N, a, b, c);
  end = clock();

  elapsed = (double)(end - start) / CLOCKS_PER_SEC;
  gflops = (2.0 * N * N * N) / (elapsed * 1e9);

  printf("Elapsed time: %.6f sec\n", elapsed);
  printf("Performance: %.3f GFLOPS\n", gflops);

  // 結果の確認（必要に応じて）
  if(out==1){
    printf("result:\n");
    for(i=0; i<N; i++){
      for(j=0; j<N; j++){
	printf(" %8.2f", c[i*N+j]);
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
