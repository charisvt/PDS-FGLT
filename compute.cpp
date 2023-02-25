#include "helper.hpp"
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

//void spmv(const mwIndex* row, const mwIndex* col, const mwIndex* x, mwIndex* y, const int n);

void spmv(double *f2_i, double *f1, mwIndex i, mwIndex *jStart, mwIndex *ii);

static void p2
(
 double *f4_i,
 double *c3,
 mwIndex i,
 mwIndex *jStart,
 mwIndex *ii,
 double *fl,
 int *pos,
 int *isNgbh,
 mwIndex *isUsed
){

  // setup the count of nonzero columns (j) visited for this row (i)
  mwIndex cnt = 0;

  // --- loop through every nonzero element A(i,k)
  for (mwIndex id_i = jStart[i]; id_i < jStart[i+1]; id_i++){

    // get the column (k)
    mwIndex k = ii[id_i];

    isNgbh[k] = id_i+1;
      
    // --- loop through all nonzero elemnts A(k,j)
    for (mwIndex id_k = jStart[k]; id_k < jStart[k+1]; id_k++){

      // get the column (j)
      mwIndex j = ii[id_k];

      if (i == j) continue;

      // if this column is not visited yet for this row (i), then set it
      if (!isUsed[j]) {
        fl[j]      = 0;  // initialize corresponding element
        isUsed[j]  = 1;  // set column as visited
        pos[cnt++] = j;  // add column position to list of visited
      }

      // increase count of A(i,j)
      fl[j]++;
        
    }

  }

  // --- perform reduction on [cnt] non-empty columns (j) 
  for (mwIndex l=0; l<cnt; l++) {

    // get next column number (j)
    mwIndex j = pos[l];


    if (isNgbh[j]) {
      c3[isNgbh[j]-1]  = fl[j];
        
      f4_i[0]  += fl[j];
    }
      
    // declare it non-used
    isUsed[j] = 0;
  }

  f4_i[0]  /= 2;

}


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

    // --- setup auxilliary vectors (size n)
  double *fl = (double *) calloc( n*np, sizeof(double) );
  int *pos = (int *) calloc( n*np, sizeof(int) );
  mwIndex *isUsed    = (mwIndex *) calloc( n*np, sizeof(mwIndex) );
  
  double *c3 = (double *) calloc( m, sizeof(double) );
  int *isNgbh = (int *) calloc( n*np, sizeof(int) );

  if ( fl == NULL || pos == NULL || isUsed == NULL || c3 == NULL || isNgbh == NULL ){
    printf( "Working memory allocation failed at auxilliary vectors, aborting...\n" );
    return 1;
  }

  cilk_for(mwIndex i=0; i<n;i++) {
    int ip = 0;
    p2( &f[4][i], c3, i, col, row, &fl[ip*n], &pos[ip*n], &isNgbh[ip*n], &isUsed[ip*n] );
  }


/*
  // calculate f4
  // calculate c3 
  c3(row, col, n, f[4]);

  cilk_for(size_t i = 0; i < A->v; i++) {
    f->s2[i] -= 2 * f->s4[i];
    f->s3[i] -= f->s4[i];
  }*/
  free(fl); 
  free(pos);
  free(isUsed);
  free(c3);
  free(isNgbh);

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

