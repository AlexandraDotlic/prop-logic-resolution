#include "solver.h"

#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <set>
#include <map>
#include <numeric>

Solver::Solver(const CNFFormula &f)
  : m_f(f)
{
}

Solver::Solver(std::istream &dimacsStream)
{
  /* Citamo uvodne komentare, preskacemo prazne linije */
  std::string line;
  std::size_t firstNonSpaceIdx;
  while (std::getline(dimacsStream, line))
  {
    firstNonSpaceIdx = line.find_first_not_of(" \t\r\n");
    if (firstNonSpaceIdx != std::string::npos && line[firstNonSpaceIdx] != 'c')
      break;
  }
  /* Proveravamo da smo procitali liniju 'p cnf brPromenljivih brKlauza' */
  if (line[firstNonSpaceIdx] != 'p')
    throw std::runtime_error{"Pogresan format ulaza iz DIMACS stream-a"};

  std::istringstream parser{line.substr(firstNonSpaceIdx+1, std::string::npos)};
  std::string tmp;
  if (!(parser >> tmp) || tmp != "cnf")
    throw std::runtime_error{"Pogresan format ulaza iz DIMACS stream-a"};

  unsigned varCnt, claCnt;
  if (!(parser >> varCnt >> claCnt))
    throw std::runtime_error{"Pogresan format ulaza iz DIMACS stream-a"};
  
  /* Citamo klauze linije po liniju preskacuci komentare i prazne linije */
  m_f.resize(claCnt);
  int clauseIdx = 0;
  while (std::getline(dimacsStream, line))
  {
    firstNonSpaceIdx = line.find_first_not_of(" \t\r\n");
    if (firstNonSpaceIdx != std::string::npos && line[firstNonSpaceIdx] != 'c')
    {
      parser.clear();
      parser.str(line);
      std::copy(std::istream_iterator<int>{parser}, {}, std::back_inserter(m_f[clauseIdx]));
      m_f[clauseIdx++].pop_back(); /* izbacujemo nulu sa kraja linije */
    }
  }
}

bool Solver::resolve(const Clause &c1, const Clause &c2, Clause &r)
{
    for(unsigned i = 0; i<c1.size(); i++)
    {
        for(unsigned j=0; j< c2.size(); j++)
        {
            if(c1[i]+c2[j]==0)
                return resolve(c1, c2, c1[i], r);
        }
    }
    return false;
}

bool Solver::resolve(const Clause &c1, const Clause &c2, Literal p, Clause &r)
{       
    if (std::find(c1.begin(), c1.end(), p) == c1.end())
        throw std::runtime_error{"Literal p ne pripada c1"};

    if (std::find(c2.begin(), c2.end(), -1*p) == c2.end())
        throw std::runtime_error{"Literal ~p ne pripada c2"};

    //dodajemo sve literale iz klauze c1 (osim p) u rezolventu r
    for(auto cIt=c1.begin(); cIt!=c1.end(); ++cIt){
        if(*cIt == p )
           continue;
         r.push_back(*cIt);
    }
    //U rezolventu r dodajemo sve literale iz klauze c2 koji nisu jednaki ~p i koji nisu sadrzani u rezolventi r
    for(auto cIt=c2.begin(); cIt<c2.end(); ++cIt){
        if(*cIt == -p || std::find(r.begin(), r.end(), *cIt) != r.end())
            continue;       
        r.push_back(*cIt);
    }

   //Ukoliko u rezolventi r postoji neki literal koji ima svoju negaciju, dobili smo tautologiju
    for(unsigned i=0; i<r.size(); i++){
        for(unsigned j=i+1; j< r.size(); j++){
            if(r[i]+r[j] == 0)
                return false;
        }
    }
    //Ukoliko u skupu vec postoji ista klauza vracamo false
    std::set<int> rs(r.begin(), r.end());
    for(unsigned i = 0; i<m_f.size();i++)
    {
        std::set<int> s(m_f[i].begin(), m_f[i].end());
        if(s==rs)
            return false;           
    }
    return true;
}

bool Solver::isSatisfiable()
{ 
    //za svake dve klauze iz skupa primenjujemo pravilo rezolucije dok god nije dodata prazna klauza
    for(unsigned i=0;i<m_f.size();i++)
    {
        for(unsigned j=0;j<m_f.size();j++)
        {
            if(i == j)
                continue;
            Clause r;
            bool isResolved = resolve(m_f[i],m_f[j], r);
            if(isResolved)
            {
                m_f.push_back(r);
                std::cout<<"Resolving " <<m_f[i]<<" and "<< m_f[j]<<" added -> "<<r<<std::endl;
                //Ukoliko je dodata prazna klauza
                if(r.empty())
                    return false;                
            }
        }
    }
    return true;
}


CNFFormula Solver::getFormula()
{
    return m_f;
}

std::ostream& operator<<(std::ostream &out, const CNFFormula &f)
{
    for(unsigned i = 0; i < f.size() - 1; i++)
    {
        out<<f[i]<<",";
    }
    out<<f[f.size()-1];
    return out;
}

std::ostream& operator<<(std::ostream &out, const Clause &c)
{
    if(c.empty()) 
        out<<"{}";
    else{

        out<<"{";
        for(unsigned i = 0; i < c.size()-1; i++)
        {
            if(c[i]<0){
                out<<"~p"<<std::abs(c[i])<<",";
            }
            else{
                out<<"p"<<c[i]<<",";
            }

        }
        if(c[c.size()-1]<0){
            out<<"~p"<<std::abs(c[c.size()-1]);
        }
        else{
            out<<"p"<<c[c.size()-1];
        }
        out<<"}";
    }
    
    return out;
}


