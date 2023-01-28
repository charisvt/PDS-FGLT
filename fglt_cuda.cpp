#include <iostream>
#include <fstream>
#include <cassert>
#include <limits>
#include <string.h>

#define n 1000
int main(int argc, char **argv){
  std::string filename = "graph.mtx";
  // ----- retrieve the (non-option) argument:
  if ( (argc <= 1) || (argv[argc-1] == NULL) || (argv[argc-1][0] == '-') ) {
    // there is NO input...
    std::cout << "No filename provided, using 'graph.mtx'." << std::endl;
  }
  else {
    // there is an input...
    filename = argv[argc-1];
    std::cout << "Using graph stored in '" << filename << "'." << std::endl;
  }

  double *t = (double *) calloc(n, sizeof(double));
}

