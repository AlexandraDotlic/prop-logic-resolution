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
         * @brief Funkcija primenjuje pravilo rezolucije za neki par klauza c1 i c2 i popunjava rezolventu r
         * @return Vraca false ukoliko je dobijena rezolventa tautologija ili vec postoji u skupu klauza,
         * u suprotnom vraca true
        */
        bool resolve(const Clause &c1, const Clause &c2, Clause &r);
         /**
         * @brief Funkcija utvrdjuje zadovoljivost formule primenom metoda rezolucije
         * @return Vraca false ukoliko je u skup dodata prazna klauza u suprotnom vraca true
        */
        bool isSatisfiable();

        CNFFormula getFormula();
    private:
        CNFFormula m_f;
        /**
         * @brief Funkcija koja primenjuje pravilo rezolucije na klauze c1 i c2
         * po literalu p i popunjava rezolventu r
         * @return Vraca true ukoliko dobijena rezolventa nije tautologija i ne postoji u skupu, u suprotnom false
        */
        bool resolve(const Clause &c1, const Clause &c2, Literal p, Clause &r);

};
std::ostream& operator<<(std::ostream &out, const CNFFormula &f);
std::ostream& operator<<(std::ostream &out, const Clause &c);

#endif //SOLVER_H