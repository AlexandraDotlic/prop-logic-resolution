#include "solver.h"

#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <set>
#include <map>

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
    {
      break;
    }
  }
  /* Proveravamo da smo procitali liniju 'p cnf brPromenljivih brKlauza' */
  if (line[firstNonSpaceIdx] != 'p')
  {
    throw std::runtime_error{"Pogresan format ulaza iz DIMACS stream-a"};
  }
  std::istringstream parser{line.substr(firstNonSpaceIdx+1, std::string::npos)};
  std::string tmp;
  if (!(parser >> tmp) || tmp != "cnf")
  {
    throw std::runtime_error{"Pogresan format ulaza iz DIMACS stream-a"};
  }
  unsigned varCnt, claCnt;
  if (!(parser >> varCnt >> claCnt))
  {
    throw std::runtime_error{"Pogresan format ulaza iz DIMACS stream-a"};
  }
  
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

bool Solver::resolve(const Clause &c1, const Clause &c2, Literal p, Clause &r)
{
    if (std::find(c1.begin(), c1.end(), p) == c1.end())
    {
        throw std::runtime_error{"Literal p ne pripada c1"};
    }

    if (std::find(c2.begin(), c2.end(), -1*p) == c2.end())
    {
        throw std::runtime_error{"Literal ~p ne pripada c2"};
    }

    //dodajemo sve literale iz klauze c1 (osim p) u rezolventu r
    for(auto cIt=c1.begin(); cIt!=c1.end(); ++cIt){
        if(*cIt != p){
            r.push_back(*cIt);
        }
    }
    //U rezolventu r dodajemo sve literale iz klauze c2 koji nisu jednaki ~p i koji nisu sadrzani u rezolventi r
    for(auto cIt=c2.begin(); cIt<c2.end(); cIt++){
        if(*cIt != -p && std::find(r.begin(), r.end(), *cIt) == r.end()){
            r.push_back(*cIt);
        }
    }
   
   //Ukoliko u rezolventi r postoji neki literal koji ima svoju negaciju, dobili smo tautologiju
    for(unsigned i=0; i<r.size(); i++){
        for(unsigned j=i+1; j< r.size(); j++){
            if(r[i]+r[j] == 0){
                return false;
            }
        }
    }
    return true;
}

//Prolazimo kroz celu CNF formulu i za svaki par klauza c1 i c2 koje sadrze p i ~p
//poziva se resolve(c1, c2, p, r)
//ukoliko resolve vrati true brisu se c1 i c2 iz skupa, a dodaje se rezolventa r
//na kraju, ukoliko se dobije prazna klauza rezolucijom, vracamo false u suprotnom true
bool Solver::eliminate(Literal p)
{ 
    std::multimap<Clause, bool> clauseIsResolvedPairs; 
    std::multimap<Clause, bool>::iterator mapIter;
    for(unsigned i = 0; i < m_f.size(); i++)
    {
        clauseIsResolvedPairs.insert(std::pair<Clause, bool>(m_f[i], false));
    }

    for(mapIter=clauseIsResolvedPairs.begin(); mapIter!=clauseIsResolvedPairs.end(); ++mapIter)
    {      
        auto iterP = std::find((mapIter->first).begin(), (mapIter->first).end(), p);
        //ako je p u itoj klauzi koja nije rezolvirana
        if(iterP != (mapIter->first).end() && !mapIter->second)
        {
            //trazimo ~p u nekoj nerezolviranoj
            for(auto mapIter2 = clauseIsResolvedPairs.begin(); mapIter2!=clauseIsResolvedPairs.end(), mapIter != mapIter2; ++mapIter2)
            {
                Clause r;  
                auto iterNotP = std::find((mapIter2->first).begin(), (mapIter2->first).end(), -p);
                if(iterNotP != (mapIter2->first).end() && !mapIter2->second)
                {
                    bool isResolved = resolve(mapIter->first, mapIter2->first, p, r);
                    if(isResolved)
                    {
                        std::remove(m_f.begin(), m_f.end(), mapIter->first);                        
                        std::remove(m_f.begin(), m_f.end(), mapIter2->first);
                        if(r.empty()) //ako je izvedena prazna klauza vracamo false
                        {
                            return false;
                        }
                        m_f.push_back(r);
                        mapIter->second  = true;
                        mapIter2->second = true;
                    }
                }     
            }
        }
    }
    return true;
}


bool Solver::isSatisfiable()
{
    //prolazimo kroz CNF formulu i eliminisemo jedan po jedan literal funkcijom eliminate
    std::set<Literal> literals;
    std::set<Literal>::iterator setIter;
    CNFFormula::iterator cnfIter;
    CNFFormula newFormula;

    //pravimo kopiju vektora sa svim vrednostima >0   
    for (cnfIter=m_f.begin(); cnfIter!=m_f.end(); ++cnfIter)
    {
        for(auto iter=(*cnfIter).begin(); iter!=(*cnfIter).end();++iter){
            literals.insert(std::abs(*iter));
        }
    }
    for(setIter=literals.begin(); setIter!=literals.end(); ++setIter)
    {
        if(!eliminate(*setIter))
        {
            return false;
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
    return out;
}