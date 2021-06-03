#include "solver.h"
#include <fstream>
#include <stdexcept>

int main(int argc, char **argv)
{
  if (2 != argc)
    throw std::runtime_error{"Usage: ./3 dimacs.cnf"};
  
  std::ifstream dimacsStream{argv[1]};
  if (!dimacsStream)
    throw std::runtime_error{"Bad path to dimacs file"};
    
  Solver s{dimacsStream};

  CNFFormula formula = s.getFormula();
  std::cout<<"Formula: "<<formula<<" is ";
  bool isSat = s.isSatisfiable();
  if (isSat)
    std::cout << "satisfiable." << std::endl;
  else
    std::cout << "unsatisfiable." << std::endl;
  return 0;
}
