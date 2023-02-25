#ifndef HELPER_H_
#define HELPER_H_

#include <stdio.h>
#include <cstdlib>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <cassert>
#include <limits>

// type definitions
typedef size_t    mwSize;         /* unsigned pointer-width integer */
typedef size_t    mwIndex;        /* unsigned pointer-width integer */

#define NGRAPHLET 5

void readMTX
  (
   mwIndex       **       row,
   mwIndex       **       col,
   mwSize         * const n,
   mwSize         * const m,
   char    const  * const filename
);

int compute
  (
   double ** const f,
   double ** const fn,
   mwIndex *ii,
   mwIndex *jStart,
   mwSize n,
   mwSize m,
   mwSize np
   );

int getWorkers();

std::ostream& output(std::ostream& outfile, double** arr, int rows, int cols);

#endif