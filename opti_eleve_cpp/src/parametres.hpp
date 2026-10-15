#pragma once

#include <string>

struct Parametres {
    // Classes
    int nb_classes = 4;

    // Algorithme genetique
    int nb_generations = 30;
    int pop_par_gen = 500;
    int nb_selection_par_generation = 20;
    int nb_modif_mutation = 2;
    unsigned int graine = 0; // 0 = graine aleatoire

    // Ponderations de la fonction de cout
    double w_math = 2.0;
    double w_lecture = 2.0;
    double w_comportement = 4.0;
    double w_genre = 10.0;
    double w_etranger = 5.0;
    double w_ecart_type_notes = 0.5;
    double w_separe = 1000.0; // penalite si deux eleves a separer sont ensemble

    // Fichiers
    std::string fichier_eleves = "eleves.csv";
    std::string prefixe_sortie = "classe";
};

// Lit un fichier .ini. Les cles absentes gardent leur valeur par defaut.
// Si le fichier n'existe pas, les valeurs par defaut sont retournees.
Parametres charger_parametres(const std::string& chemin_ini);
