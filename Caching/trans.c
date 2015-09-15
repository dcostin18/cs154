/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

void trans_one(int M, int N, int A[N][M], int B[M][N]);
void trans_simp(int M, int N, int A[N][M], int B[M][N]);
void trans_simper(int M, int N, int A[N][M], int B[M][N]);
void trans_top(int M, int N, int A[N][M], int B[M][N]);
void trans_bottom(int M, int N, int A[N][M], int B[M][N]);
void trans_rect(int M, int N, int A[M][N], int B[M][N]);

// 32 * 64
char trans_rect_desc[] = "32x64";
void trans_rect(int M, int N, int A[N][M], int B[M][N])
{
    trans_top(M, N, A, B);
    trans_bottom(M, N, A, B);
    return;
}




// 32 * 32
char trans_one_desc[] = "First approach (32x32)";
void trans_one(int M, int N, int A[N][M], int B[M][N])
{
  int mr, mc, row;
  for(mr=0; mr<N; mr+=8)
    {
      for(mc=0; mc<M; mc+=8)
        {         
	  for(row=0; row<8; row++)
	    {
	      int e1 = A[mr+row][mc+0];
	      int e2 = A[mr+row][mc+1];
	      int e3 = A[mr+row][mc+2];
	      int e4 = A[mr+row][mc+3];
	      int e5 = A[mr+row][mc+4];
	      int e6 = A[mr+row][mc+5];
	      int e7 = A[mr+row][mc+6];
	      int e8 = A[mr+row][mc+7];                    
	      
	      B[mc+0][mr+row] = e1;
	      B[mc+1][mr+row] = e2;
	      B[mc+2][mr+row] = e3;
	      B[mc+3][mr+row] = e4;
	      B[mc+4][mr+row] = e5;
	      B[mc+5][mr+row] = e6;
	      B[mc+6][mr+row] = e7;
	      B[mc+7][mr+row] = e8;                           
            }
        }
    }
  
  return;
  
  
}

void trans_top(int M, int N, int A[N][M], int B[M][N])
{
  int mr, mc, row, col;
  for(mr=0; mr<N/2; mr+=8)
    {
      for(mc=0; mc<M; mc+=8)
        {
	  if(mr!=mc)
            {
	      for(row=0; row<8; row++)
                {
		  for(col=0; col<8; col++)
                    {
		      B[mc+col][mr+row] = A[mr+row][mc+col];
                    }
                }

            }
	  else
            {
	      for(row=0; row<8; row++)
                {
		  int e1 = A[mr+row][mc+0];
		  int e2 = A[mr+row][mc+1];
		  int e3 = A[mr+row][mc+2];
		  int e4 = A[mr+row][mc+3];
		  int e5 = A[mr+row][mc+4];
		  int e6 = A[mr+row][mc+5];
		  int e7 = A[mr+row][mc+6];
		  int e8 = A[mr+row][mc+7];

		  B[mc+0][mr+row] = e1;
		  B[mc+1][mr+row] = e2;
		  B[mc+2][mr+row] = e3;
		  B[mc+3][mr+row] = e4;
		  B[mc+4][mr+row] = e5;
		  B[mc+5][mr+row] = e6;
		  B[mc+6][mr+row] = e7;
		  B[mc+7][mr+row] = e8;
                }
            }
        }
    }

  return;


}

void trans_bottom(int M, int N, int A[N][M], int B[M][N])
{
  int mr, mc, row;
  for(mr=32; mr<N; mr+=8)
    {
      for(mc=0; mc<M; mc+=8)
        {
          /*    for(row=0; row<8; row++)
                {
                  for(col=0; col<8; col++)
                    {
                      B[mc+col][mr+row] = A[mr+row][mc+col];
                    }
                }

            }
	    else
            {*/
              for(row=0; row<8; row++)
                {
                  int e1 = A[mr+row][mc+0];
                  int e2 = A[mr+row][mc+1];
                  int e3 = A[mr+row][mc+2];
                  int e4 = A[mr+row][mc+3];
                  int e5 = A[mr+row][mc+4];
                  int e6 = A[mr+row][mc+5];
                  int e7 = A[mr+row][mc+6];
                  int e8 = A[mr+row][mc+7];

                  B[mc+0][mr+row] = e1;
		  B[mc+1][mr+row] = e2;
                  B[mc+2][mr+row] = e3;
                  B[mc+3][mr+row] = e4;
                  B[mc+4][mr+row] = e5;
                  B[mc+5][mr+row] = e6;
                  B[mc+6][mr+row] = e7;
                  B[mc+7][mr+row] = e8;
                }
		}
        }
    

  return;


}



/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
  if(M == 32 && N == 32)
    trans_one(M, N, A, B);
  else if(M == 61 && N == 67)
    trans_simper(M, N, A, B);
  else
    trans_simp(M, N, A, B);
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}

char trans_simp_desc[] = "Simple row-wise scan transpose per 8X8";
void trans_simp(int M, int N, int A[N][M], int B[M][N])
{
  int i, j, tmp, mc, mr;
  for(mr = 0; mr < N; mr+=8){
    for(mc = 0; mc < M; mc+=8){
      for (i = 0; i < 8; i++) {
	for (j = 0; j < 8; j++) {
	  tmp = A[j+mr][i+mc];
	  B[i+mc][j+mr] = tmp;
	}
      }
    }
  }
}

char trans_simper_desc[] = "Simple row-wise scan transpose per 16X16";
void trans_simper(int M, int N, int A[N][M], int B[M][N])
{
  int i, j, tmp, mc, mr;
  for(mr = 0; mr < N; mr+=16){
    for(mc = 0; mc < M; mc+=16){
      for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
	  if(i+mc<M && j+mr < N){
	    tmp = A[j+mr][i+mc];
	    B[i+mc][j+mr] = tmp;
	  }
        }
      }
    }
  }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
    registerTransFunction(trans_one, trans_one_desc);
    registerTransFunction(trans_rect, trans_rect_desc);
    // registerTransFunction(manip_bot, manip_bot_desc);
    registerTransFunction(trans_simp, trans_simp_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

// version red
