## FGLT

## This is a cuda implemention of the following project:
[FGLT](https://github.com/fcdimitr/fglt) is a C/C++ multi-threading library for
Fast Graphlet Transform of large, sparse, undirected networks/graphs.

compile with
nvcc -arch=sm_61 --compiler-bindir /opt/opencilk/bin/clang -Xcompiler -fopencilk fglt.cpp helper.cpp compute.cpp p2.cu -o fglt_cuda