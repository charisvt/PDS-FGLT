## FGLT

## This is a cuda implemention of the following project:
[FGLT](https://github.com/fcdimitr/fglt) 

Only net frequencies $f_0 - f_4$ have been implemented.

compile with
`nvcc -arch=sm_61 --compiler-bindir /opt/opencilk/bin/clang -Xcompiler -fopencilk fglt.cpp helper.cpp compute.cpp p2.cu -o fglt_cuda`

## Requirements
CUDA runtime, compute >= 6.0, *cilk