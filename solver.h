#ifndef SOLVER_H
#define SOLVER_H

#include<iostream>
#include<vector>

using Literal = int;
using Clause = std::vector<Literal>;
using CNFFormula = std::vector<Clause>;

class Solver{
    public:
        /**
         * @brief Konstruktor koji prima CNF formulu
         * @param f - formula za koju se ispituje zadovoljivost
        */
        Solver(const CNFFormula &f);
        /**
         * @brief Konstruktor koji prima C++ stream iz koga se cita CNF u DIMACS formatu
         * @param dimacsStream - ulazni stream
        */
        Solver(std::istream &dimacsStream);
        /**
         * @brief Funkcija koja primenjuje iskaznu rezoluciju na klauze c1 i c2 za 
         * po literalu p i popunjava rezolventu r
         * @return Vraca true ukoliko dobijena rezolventa nije tautologija, u suprotnom false
        */
        bool resolve(const Clause &c1, const Clause &c2, Literal p, Clause &r);
        /**
         * @brief Funkcija eliminise literal p iz cnf formule na koju se primenjuje
         * tako sto primenjuje pravilo iskazne rezolucije.
         * @return Vraca false ukoliko se rezolucijom dobije prazna klauza, 
         * u suprotnom vraca true
        */
        bool eliminate(Literal p);
        /**
         * @brief Funkcija proverava da li je data formula zadovoljiva
        */
        bool isSatisfiable();

        CNFFormula getFormula();
    private:
        CNFFormula m_f;

};
/**
 * @brief operator << sluzi za ispis u C++ stream
 * @param out - stream u koji se pise
 * @param f - formula koja se pise u stream
 * @return  referenca na stream kako bi pozivi '<<' mogli da se ulancaju
 */
std::ostream& operator<<(std::ostream &out, const CNFFormula &f);
std::ostream& operator<<(std::ostream &out, const Clause &c);

#endif //SOLVER_H