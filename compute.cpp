#include "helper.hpp"
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cuda_runtime.h>

//void spmv(const mwIndex* row, const mwIndex* col, const mwIndex* x, mwIndex* y, const int n);

void spmv(double *f2_i, double *f1, mwIndex i, mwIndex *jStart, mwIndex *ii);
void p2(double *f4, mwSize n, mwSize m, mwIndex *jStart, mwIndex *ii);

int compute
(
 double ** const f,
 double ** const fn,
 mwIndex *row, //ii
 mwIndex *col, //jStart
 mwSize n,
 mwSize m,
 mwSize np
 ){
  

  // calculate f0 = e 
  cilk_for(mwIndex i = 0; i < n; i++) { 
    f[0][i] = 1; 
  }

  // calculate f1 = p1 where p1 = Ae
  cilk_for(mwIndex i = 0; i < n; i++) {
    f[1][i] = col[i + 1] - col[i];
  }


  // calculate f2 = Ap1 − p1  
  // calculate sparse matrix dense vector multiplication - Ap1 -p1
  cilk_for(mwIndex i = 0; i < n; i++) {   
    spmv( &f[2][i], f[1], i, col, row );
  }

  // calculate f3 = p1 o (p1 -1) / 2
  cilk_for(mwIndex i = 0; i < n; i++) {
    f[3][i] = (f[1][i] * (f[1][i] - 1)) / 2;
  }


  // calculate f4 = A o (A^2) / 2
  p2( f[4], n, m, col, row);
  
  // recalculate f2 f3
  cilk_for(size_t i = 0; i < n; i++) {
    f[2][i] -= 2 * f[4][i];
    f[3][i] -= f[4][i];
  }


  return 1;
}


void spmv(double *f2_i, double *f1, mwIndex i, mwIndex *jStart, mwIndex *ii){
  for (mwIndex id_i = jStart[i]; id_i < jStart[i+1]; id_i++){

    // get the column (k)
    mwIndex k = ii[id_i];
      
    // --- matrix-vector product Ap1
    f2_i[0] += f1[k];
      
  }
  // Ap1 - p1
  f2_i[0]  -= f1[i];
  
}

