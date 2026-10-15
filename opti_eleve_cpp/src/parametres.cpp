#include "parametres.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
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

using Ini = std::map<std::string, std::map<std::string, std::string>>;

Ini lire_ini(const std::string& chemin) {
    Ini ini;
    std::ifstream f(chemin);
    if (!f) throw std::runtime_error("impossible d'ouvrir le fichier de configuration : " + chemin);

    std::string section;
    std::string ligne;
    int num_ligne = 0;
    while (std::getline(f, ligne)) {
        ++num_ligne;
        ligne = trim(ligne);
        if (ligne.empty() || ligne[0] == ';' || ligne[0] == '#') continue;
        if (ligne.front() == '[' && ligne.back() == ']') {
            section = minuscules(trim(ligne.substr(1, ligne.size() - 2)));
            continue;
        }
        size_t egal = ligne.find('=');
        if (egal == std::string::npos)
            throw std::runtime_error(chemin + ":" + std::to_string(num_ligne) + " : ligne invalide (cle = valeur attendu)");
        std::string cle = minuscules(trim(ligne.substr(0, egal)));
        std::string valeur = trim(ligne.substr(egal + 1));
        ini[section][cle] = valeur;
    }
    return ini;
}

int get_int(const Ini& ini, const std::string& section, const std::string& cle, int defaut) {
    auto s = ini.find(section);
    if (s == ini.end()) return defaut;
    auto v = s->second.find(cle);
    if (v == s->second.end() || v->second.empty()) return defaut;
    return std::stoi(v->second);
}

double get_double(const Ini& ini, const std::string& section, const std::string& cle, double defaut) {
    auto s = ini.find(section);
    if (s == ini.end()) return defaut;
    auto v = s->second.find(cle);
    if (v == s->second.end() || v->second.empty()) return defaut;
    return std::stod(v->second);
}

std::string get_string(const Ini& ini, const std::string& section, const std::string& cle, const std::string& defaut) {
    auto s = ini.find(section);
    if (s == ini.end()) return defaut;
    auto v = s->second.find(cle);
    if (v == s->second.end() || v->second.empty()) return defaut;
    return v->second;
}

} // namespace

Parametres charger_parametres(const std::string& chemin_ini) {
    Parametres p;
    std::ifstream test(chemin_ini);
    if (!test) {
        std::cerr << "Attention : " << chemin_ini << " introuvable, valeurs par defaut utilisees.\n";
        return p;
    }
    test.close();

    Ini ini = lire_ini(chemin_ini);

    p.nb_classes = get_int(ini, "classes", "nb_classes", p.nb_classes);

    p.nb_generations = get_int(ini, "algorithme", "nb_generations", p.nb_generations);
    p.pop_par_gen = get_int(ini, "algorithme", "pop_par_gen", p.pop_par_gen);
    p.nb_selection_par_generation = get_int(ini, "algorithme", "nb_selection_par_generation", p.nb_selection_par_generation);
    p.nb_modif_mutation = get_int(ini, "algorithme", "nb_modif_mutation", p.nb_modif_mutation);
    p.graine = static_cast<unsigned int>(get_int(ini, "algorithme", "graine", static_cast<int>(p.graine)));

    p.w_math = get_double(ini, "ponderations", "w_math", p.w_math);
    p.w_lecture = get_double(ini, "ponderations", "w_lecture", p.w_lecture);
    p.w_comportement = get_double(ini, "ponderations", "w_comportement", p.w_comportement);
    p.w_genre = get_double(ini, "ponderations", "w_genre", p.w_genre);
    p.w_etranger = get_double(ini, "ponderations", "w_etranger", p.w_etranger);
    p.w_ecart_type_notes = get_double(ini, "ponderations", "w_ecart_type_notes", p.w_ecart_type_notes);
    p.w_separe = get_double(ini, "ponderations", "w_separe", p.w_separe);

    p.fichier_eleves = get_string(ini, "fichiers", "fichier_eleves", p.fichier_eleves);
    p.prefixe_sortie = get_string(ini, "fichiers", "prefixe_sortie", p.prefixe_sortie);

    if (p.nb_classes < 1) throw std::runtime_error("nb_classes doit etre >= 1");
    if (p.nb_selection_par_generation < 1) throw std::runtime_error("nb_selection_par_generation doit etre >= 1");
    if (p.pop_par_gen < 1) throw std::runtime_error("pop_par_gen doit etre >= 1");

    return p;
}
