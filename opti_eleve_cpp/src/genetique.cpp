#include "genetique.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <stdexcept>

Genetique::Genetique(const Parametres& params, const std::vector<Eleve>& liste_eleves)
    : p(params), eleves(liste_eleves), nb_eleves(static_cast<int>(eleves.size())),
      eleve_par_classe(nb_eleves / p.nb_classes) {
    if (nb_eleves < p.nb_classes)
        throw std::runtime_error("moins d'eleves que de classes");

    notes.reserve(nb_eleves);
    caracteristiques.reserve(nb_eleves);
    for (const Eleve& e : eleves) {
        notes.push_back({static_cast<double>(e.math), static_cast<double>(e.lecture),
                         static_cast<double>(e.comportement)});
        caracteristiques.push_back({static_cast<double>(e.genre), static_cast<double>(e.etranger)});
    }
    for (int i = 0; i < nb_eleves; ++i)
        for (int m = 0; m < 3; ++m) moyennes_notes[m] += notes[i][m] / nb_eleves;
    for (int i = 0; i < nb_eleves; ++i)
        for (int c = 0; c < 2; ++c) moyennes_caracteristiques[c] += caracteristiques[i][c] / nb_eleves;

    w_notes = {p.w_math, p.w_lecture, p.w_comportement};
    w_caracteristiques = {p.w_genre, p.w_etranger};

    // Verification de la faisabilite des contraintes de separation
    std::map<int, int> taille_groupe;
    for (const Eleve& e : eleves)
        if (e.separe != 0) ++taille_groupe[e.separe];
    for (const auto& [groupe, taille] : taille_groupe)
        if (taille > p.nb_classes)
            std::cerr << "Attention : " << taille << " eleves ont Separe=" << groupe
                      << " mais il n'y a que " << p.nb_classes
                      << " classes, la contrainte ne pourra pas etre respectee.\n";

    if (p.graine != 0)
        rng.seed(p.graine);
    else
        rng.seed(std::random_device{}());
}

double Genetique::fct_cout(const Classes& classes) const {
    double cout_max = -std::numeric_limits<double>::infinity();
    for (const std::vector<int>& classe : classes) {
        std::array<double, 3> moy_notes{};
        std::array<double, 2> moy_car{};
        for (int i : classe) {
            for (int m = 0; m < 3; ++m) moy_notes[m] += notes[i][m];
            for (int c = 0; c < 2; ++c) moy_car[c] += caracteristiques[i][c];
        }
        double n = static_cast<double>(classe.size());
        for (double& v : moy_notes) v /= n;
        for (double& v : moy_car) v /= n;

        double cout = 0.0;
        for (int c = 0; c < 2; ++c)
            cout += std::fabs(moyennes_caracteristiques[c] - moy_car[c]) * w_caracteristiques[c];
        for (int m = 0; m < 3; ++m)
            cout += std::fabs(moyennes_notes[m] - moy_notes[m]) * w_notes[m];

        // ecart-type de toutes les notes de la classe (population, comme np.std)
        double somme = 0.0;
        for (int i : classe)
            for (int m = 0; m < 3; ++m) somme += notes[i][m];
        double moyenne = somme / (3.0 * n);
        double var = 0.0;
        for (int i : classe)
            for (int m = 0; m < 3; ++m) var += (notes[i][m] - moyenne) * (notes[i][m] - moyenne);
        var /= 3.0 * n;
        cout -= std::sqrt(var) * p.w_ecart_type_notes;

        // penalite : eleves avec le meme numero Separe (non nul) dans la meme classe
        std::map<int, int> groupes;
        for (int i : classe)
            if (eleves[i].separe != 0) ++groupes[eleves[i].separe];
        for (const auto& [groupe, k] : groupes)
            if (k > 1) cout += p.w_separe * (k * (k - 1)) / 2.0;

        cout_max = std::max(cout, cout_max);
    }
    return cout_max;
}

Classes Genetique::separer_en_classes(const std::vector<int>& liste) const {
    Classes classes;
    int restants = nb_eleves;
    size_t it = 0;
    for (int i = 0; i < p.nb_classes; ++i) {
        int reste = restants % (p.nb_classes - i);
        int taille = eleve_par_classe + reste;
        classes.emplace_back(liste.begin() + it, liste.begin() + it + taille);
        it += taille;
        restants -= taille;
    }
    return classes;
}

Classes Genetique::genere_classe_alea() {
    std::vector<int> liste(nb_eleves);
    std::iota(liste.begin(), liste.end(), 0);
    std::shuffle(liste.begin(), liste.end(), rng);
    return separer_en_classes(liste);
}

Classes Genetique::mutate(const Classes& classes) {
    // aplatit la repartition, echange nb_modif_mutation paires d'eleves, redecoupe
    std::vector<int> liste;
    liste.reserve(nb_eleves);
    for (const std::vector<int>& classe : classes)
        liste.insert(liste.end(), classe.begin(), classe.end());

    int nb_echanges = std::min(p.nb_modif_mutation, nb_eleves / 2);
    std::vector<int> indices(nb_eleves);
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), rng);
    for (int k = 0; k < nb_echanges; ++k)
        std::swap(liste[indices[2 * k]], liste[indices[2 * k + 1]]);

    return separer_en_classes(liste);
}

Classes Genetique::algo() {
    std::vector<Classes> pop;
    pop.reserve(p.pop_par_gen);
    for (int i = 0; i < p.pop_par_gen; ++i) pop.push_back(genere_classe_alea());

    Classes meilleure_solution;
    double meilleur_cout = std::numeric_limits<double>::infinity();

    for (int generation = 0; generation < p.nb_generations; ++generation) {
        std::vector<double> couts(pop.size());
        for (size_t i = 0; i < pop.size(); ++i) couts[i] = fct_cout(pop[i]);

        std::vector<size_t> ordre(pop.size());
        std::iota(ordre.begin(), ordre.end(), 0);
        std::sort(ordre.begin(), ordre.end(), [&](size_t a, size_t b) { return couts[a] < couts[b]; });

        double min_cout = couts[ordre[0]];
        if (min_cout < meilleur_cout) {
            meilleur_cout = min_cout;
            meilleure_solution = pop[ordre[0]];
        }
        std::cout << "generation " << generation << " : cout min = " << min_cout << '\n';

        std::vector<Classes> nouvelle_pop;
        nouvelle_pop.reserve(p.pop_par_gen);
        for (int it = 0; it < p.pop_par_gen; ++it) {
            size_t parent = ordre[it % p.nb_selection_par_generation];
            nouvelle_pop.push_back(mutate(pop[parent]));
        }
        pop = std::move(nouvelle_pop);
    }
    std::cout << "meilleur cout trouve : " << meilleur_cout << '\n';
    return meilleure_solution;
}

void Genetique::ecrire_classes_csv(const Classes& classes) const {
    for (size_t c = 0; c < classes.size(); ++c) {
        std::string nom_fichier = p.prefixe_sortie + "_" + std::to_string(c + 1) + ".csv";
        std::ofstream f(nom_fichier);
        if (!f) throw std::runtime_error("impossible d'ecrire " + nom_fichier);
        f << "Id;Nom;Genre;Etranger;Lecture;Math;Comportement;Separe\n";
        for (int i : classes[c]) {
            const Eleve& e = eleves[i];
            f << e.id << ';' << e.nom << ';' << e.genre << ';' << e.etranger << ';' << e.lecture << ';'
              << e.math << ';' << e.comportement << ';' << e.separe << '\n';
        }
        std::cout << "ecrit : " << nom_fichier << " (" << classes[c].size() << " eleves)\n";
    }
}

void Genetique::afficher_resume(const Classes& classes) const {
    for (size_t c = 0; c < classes.size(); ++c) {
        std::array<double, 3> moy_notes{};
        std::array<double, 2> moy_car{};
        for (int i : classes[c]) {
            for (int m = 0; m < 3; ++m) moy_notes[m] += notes[i][m];
            for (int k = 0; k < 2; ++k) moy_car[k] += caracteristiques[i][k];
        }
        double n = static_cast<double>(classes[c].size());
        std::cout << "classe " << c + 1 << " : " << classes[c].size() << " eleves"
                  << " | genre=" << moy_car[0] / n << " etranger=" << moy_car[1] / n
                  << " | math=" << moy_notes[0] / n << " lecture=" << moy_notes[1] / n
                  << " comportement=" << moy_notes[2] / n << '\n';
    }
}
