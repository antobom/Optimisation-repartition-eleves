#include "eleves.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>

namespace {

std::string trim(const std::string& s) {
    size_t debut = s.find_first_not_of(" \t\r\n");
    if (debut == std::string::npos) return "";
    size_t fin = s.find_last_not_of(" \t\r\n");
    return s.substr(debut, fin - debut + 1);
}

std::string minuscules(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::vector<std::string> decouper(const std::string& ligne, char sep) {
    std::vector<std::string> champs;
    std::string courant;
    for (char c : ligne) {
        if (c == sep) {
            champs.push_back(trim(courant));
            courant.clear();
        } else {
            courant += c;
        }
    }
    champs.push_back(trim(courant));
    return champs;
}

int colonne(const std::vector<std::string>& entetes, const std::string& nom) {
    for (size_t i = 0; i < entetes.size(); ++i)
        if (minuscules(entetes[i]) == nom) return static_cast<int>(i);
    return -1;
}

int lire_entier(const std::vector<std::string>& champs, int col, const std::string& nom_col, int num_ligne) {
    if (col < 0 || static_cast<size_t>(col) >= champs.size() || champs[col].empty())
        throw std::runtime_error("ligne " + std::to_string(num_ligne) + " : colonne '" + nom_col + "' manquante ou vide");
    try {
        return std::stoi(champs[col]);
    } catch (...) {
        throw std::runtime_error("ligne " + std::to_string(num_ligne) + " : valeur entiere invalide pour '" + nom_col + "' : " + champs[col]);
    }
}

} // namespace

std::vector<Eleve> charger_eleves(const std::string& chemin_csv) {
    std::ifstream f(chemin_csv);
    if (!f) throw std::runtime_error("impossible d'ouvrir le fichier des eleves : " + chemin_csv);

    std::string ligne;
    if (!std::getline(f, ligne)) throw std::runtime_error(chemin_csv + " est vide");
    std::vector<std::string> entetes = decouper(ligne, ';');

    int c_id = colonne(entetes, "id");
    int c_nom = colonne(entetes, "nom");
    int c_genre = colonne(entetes, "genre");
    int c_etranger = colonne(entetes, "etranger");
    int c_lecture = colonne(entetes, "lecture");
    int c_math = colonne(entetes, "math");
    int c_comportement = colonne(entetes, "comportement");
    int c_separe = colonne(entetes, "separe");

    if (c_genre < 0 || c_etranger < 0 || c_lecture < 0 || c_math < 0 || c_comportement < 0 || c_separe < 0)
        throw std::runtime_error(chemin_csv + " : en-tete incomplet, colonnes attendues : Id;Nom;Genre;Etranger;Lecture;Math;Comportement;Separe");

    std::vector<Eleve> eleves;
    int num_ligne = 1;
    while (std::getline(f, ligne)) {
        ++num_ligne;
        if (trim(ligne).empty()) continue;
        std::vector<std::string> champs = decouper(ligne, ';');

        Eleve e;
        e.id = (c_id >= 0 && static_cast<size_t>(c_id) < champs.size() && !champs[c_id].empty())
                   ? lire_entier(champs, c_id, "Id", num_ligne)
                   : static_cast<int>(eleves.size()) + 1;
        e.nom = (c_nom >= 0 && static_cast<size_t>(c_nom) < champs.size() && !champs[c_nom].empty())
                    ? champs[c_nom]
                    : "Eleve " + std::to_string(e.id);
        e.genre = lire_entier(champs, c_genre, "Genre", num_ligne);
        e.etranger = lire_entier(champs, c_etranger, "Etranger", num_ligne);
        e.lecture = lire_entier(champs, c_lecture, "Lecture", num_ligne);
        e.math = lire_entier(champs, c_math, "Math", num_ligne);
        e.comportement = lire_entier(champs, c_comportement, "Comportement", num_ligne);
        e.separe = lire_entier(champs, c_separe, "Separe", num_ligne);
        eleves.push_back(e);
    }
    if (eleves.empty()) throw std::runtime_error(chemin_csv + " : aucun eleve");
    return eleves;
}
