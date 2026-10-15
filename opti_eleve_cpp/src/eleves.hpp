#pragma once

#include <string>
#include <vector>

struct Eleve {
    int id = 0;
    std::string nom; // texte libre (peut etre le nom entier)
    int genre = 0;
    int etranger = 0;
    int lecture = 0;
    int math = 0;
    int comportement = 0;
    int separe = 0; // 0 = pas de contrainte, sinon les eleves avec le meme numero doivent etre dans des classes differentes
};

// Lit un CSV separe par ';' avec en-tete.
// Colonnes attendues : Id;Nom;Genre;Etranger;Lecture;Math;Comportement;Separe
// Id et Nom sont optionnels (Id = numero de ligne, Nom = "Eleve <id>" par defaut).
std::vector<Eleve> charger_eleves(const std::string& chemin_csv);
