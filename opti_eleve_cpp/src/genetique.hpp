#pragma once

#include "eleves.hpp"
#include "parametres.hpp"

#include <array>
#include <random>
#include <vector>

// Une solution : nb_classes listes d'indices d'eleves (indices dans le vecteur eleves).
using Classes = std::vector<std::vector<int>>;

class Genetique {
public:
    Genetique(const Parametres& params, const std::vector<Eleve>& eleves);

    // Lance l'algorithme genetique et retourne la meilleure repartition trouvee.
    Classes algo();

    // Ecrit un fichier CSV par classe : <prefixe_sortie>_1.csv, _2.csv, ...
    void ecrire_classes_csv(const Classes& classes) const;

    // Affiche un resume des effectifs et moyennes de chaque classe.
    void afficher_resume(const Classes& classes) const;

    double fct_cout(const Classes& classes) const;

private:
    Classes genere_classe_alea();
    Classes separer_en_classes(const std::vector<int>& liste) const;
    Classes mutate(const Classes& classes);

    Parametres p;
    std::vector<Eleve> eleves;
    int nb_eleves;
    int eleve_par_classe;

    // notes[i] = {math, lecture, comportement}, caracteristiques[i] = {genre, etranger}
    std::vector<std::array<double, 3>> notes;
    std::vector<std::array<double, 2>> caracteristiques;
    std::array<double, 3> moyennes_notes{};
    std::array<double, 2> moyennes_caracteristiques{};
    std::array<double, 3> w_notes{};
    std::array<double, 2> w_caracteristiques{};

    std::mt19937 rng;
};
