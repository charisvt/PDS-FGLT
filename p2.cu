#include "helper.hpp"
#include <cuda_runtime.h>
#include <chrono>


// cudaCalloc macro with error checking
#define cudaCalloc(A, B, C) \
  do { \
    cudaError_t __cudaCalloc_err = cudaMalloc(A, B*C); \
    if (__cudaCalloc_err == cudaSuccess) cudaMemset(*A, 0, B*C); \
  } while (0)

__global__ void p2_kernel(double *f4_i,
 double *c3,
 mwIndex *jStart,
 mwIndex *ii,
 double *fl,
 int *isNgbh,
 mwIndex *isUsed)
{
    // Calculate the starting and ending indices of the nonzero elements in the current row
    mwIndex i = blockIdx.x * blockDim.x + threadIdx.x;
    mwIndex start = jStart[i];
    mwIndex end = jStart[i + 1];
    mwIndex count = end - start;

    // Allocate shared memory for the pos array
    extern __shared__ int pos[];

    // Initialize cnt to 0
    mwIndex cnt = 0;

    // Loop through every nonzero element A(i,k)
    for (mwIndex id_i = start + threadIdx.x; id_i < end; id_i += blockDim.x){

        // get the column (k)
        mwIndex k = ii[id_i];

        isNgbh[k] = id_i+1;

        // loop through all nonzero elements A(k,j)
        for (mwIndex id_k = jStart[k]; id_k < jStart[k+1]; id_k++){

            // get the column (j)
            mwIndex j = ii[id_k];

            if (i == j) continue;

            // if this column is not visited yet for this row (i), then set it
            if (!isUsed[j]) {
                fl[j] = 0.0;  // initialize corresponding element
                isUsed[j] = 1;  // set column as visited
                pos[cnt++] = j;  // add column position to list of visited
            }

            // increase count of A(i,j)
            fl[j]++;

        }

    }

    // perform reduction on [cnt] non-empty columns (j)
    for (mwIndex l=0; l<cnt; l++) {

        // get next column number (j)
        int j = pos[l];

        if (isNgbh[j]) {
            c3[isNgbh[j]-1]  = fl[j];

            // atomic add to f4_i[0] to avoid race conditions
            atomicAdd(&f4_i[0], fl[j]);
        }

        // declare it non-used
        isUsed[j] = 0;
    }

    // divide by 2 only once in the first thread of each block
    if (threadIdx.x == 0) {
        f4_i[0] /= 2.0;
    }
}

void p2(double *f4, mwSize n, mwSize m, mwIndex *jStart, mwIndex *ii) {

    // start time measurement
    auto start = std::chrono::high_resolution_clock::now();

    // allocate device memory
    double *d_f4, *d_c3, *fl;
    mwIndex *d_jStart, *d_ii, *isUsed;
    int *isNgbh;
        
    cudaMalloc(&d_jStart, (n + 1) * sizeof(int));
    cudaMalloc(&d_ii, m * sizeof(int));

    cudaCalloc(&d_f4, n , sizeof(double) );
    cudaCalloc(&fl, n, sizeof(double) );
    cudaCalloc(&isNgbh, n, sizeof(int) );
    cudaCalloc(&d_c3, m, sizeof(double) );
    cudaCalloc(&isUsed, n, sizeof(mwIndex) );

    // copy input data to device memory
    cudaMemcpy(d_ii, ii, (n + 1) * sizeof(mwIndex), cudaMemcpyHostToDevice);
    cudaMemcpy(d_jStart, jStart, m * sizeof(mwIndex), cudaMemcpyHostToDevice);

    // set up grid and block dimensions
    dim3 grid_dim(1,1);
    dim3 block_dim(n);
    if(n > 1024){
        grid_dim.x = (n + 1024 - 1) / 1024;
        block_dim.x = 1024;
    }

    std::cout << "Running cuda kernels\n";
    // launch the CUDA kernel
    p2_kernel<<<grid_dim, block_dim>>>(d_f4, d_c3, d_jStart, d_ii, fl, isNgbh, isUsed);

    // synchronisation
    cudaDeviceSynchronize();

    // copy output data back to host memory
    cudaMemcpy(f4, d_f4, sizeof(double), cudaMemcpyDeviceToHost);

    // free device memory
    cudaFree(d_f4);
    cudaFree(d_c3);
    cudaFree(d_jStart);
    cudaFree(d_ii);
    
    // stop time measurement
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Time taken by p2: " << duration << " milliseconds" << std::endl;
}