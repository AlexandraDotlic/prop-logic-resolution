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
  std::cout<<"Formula: "<<formula<<std::endl;
  std::cout<<std::endl;
  bool isSat = s.isSatisfiable();
  std::cout<<std::endl;

  if (isSat)
    std::cout << "Formula is satisfiable." << std::endl;
  else
    std::cout << "Formula is unsatisfiable." << std::endl;
  return 0;
}
