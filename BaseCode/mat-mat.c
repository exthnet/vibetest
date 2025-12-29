#include <stdio.h>
#include <stdlib.h>

void matmul(int N, double *a, double *b, double *c)
{
  int i, j, k;
  for(i=0; i<N; i++){
    for(j=0; j<N; j++){
      for(k=0; k<N; k++){
	c[i*N+j] += a[i*N+k] * b[k*N+j];
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

  // メイン計算部
  matmul(N, a, b, c);

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
