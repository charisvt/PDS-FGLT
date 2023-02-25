#include <cilk/cilk_api.h>
#include <cilk/cilk.h>
#include <stdio.h>
#include <stdlib.h>
#include "helper.hpp"

#define NGRAPHLET 5

int main(int argc, char **argv) {
  // ~~~~~~~~~~ variable declarations
  mwIndex *row, *col;
  mwSize  n, m, np;
  std::string filename = "graph.mtx";

  // ~~~~~~~~~~ parse inputs

  // ----- retrieve the (non-option) argument:
  if ( (argc <= 1) || (argv[argc-1] == NULL) || (argv[argc-1][0] == '-') ) {
      // there is NO input...
      std::cout << "No filename provided, using 'graph.mtx'." << std::endl;
  }
  else {
      filename = argv[argc-1];
      std::cout << "Using graph stored in '" << filename << "'." << std::endl;
  }
  
  readMTX(&row, &col, &n, &m, filename.c_str());

  std::cout << "n = " << n << " and m = " << m << "\n";


  double **f  = (double **) malloc(NGRAPHLET*sizeof(double *));
  double **fn = (double **) malloc(NGRAPHLET*sizeof(double *));

  for (int igraph = 0; igraph < NGRAPHLET; igraph++){
    f [igraph] = (double *) calloc( n, sizeof(double) );
    fn[igraph] = (double *) calloc( n, sizeof(double) );
  }

  np = getWorkers();

  std::cout << "Initiating fast graphlet transform for '" << filename
    << "' using " << np << " threads." << std::endl;

  compute(f,fn,row,col,n,m,np);

  std::cout << "Computation complete, outputting frequency counts to 'freq_net.csv'" << std::endl;
  
  // output
  std::fstream ofnet("freq_net.csv", std::ios::out );

  if ( ofnet.is_open() ){
    //change f to fn when net freqs are implemented
    output( ofnet, f, n, NGRAPHLET );
  }

  std::cout << "Finished, cleaning up..." << std::endl;
  
  for (int igraph = 0; igraph < NGRAPHLET; igraph++){
      free(f [igraph]);
      free(fn[igraph]);
  }
  
  free( f );
  free( fn );
  free( row );
  free( col );
}