#include "eleves.hpp"
#include "genetique.hpp"
#include "parametres.hpp"

#include <exception>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    std::string chemin_config = "config.ini";
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "-h" || arg == "--help") {
            std::cout << "Usage : opti_eleve [config.ini]\n"
                         "Repartit les eleves de eleves.csv en classes equilibrees\n"
                         "et ecrit un fichier CSV par classe (<prefixe_sortie>_N.csv).\n";
            return 0;
        }
        chemin_config = arg;
    }

    try {
        Parametres params = charger_parametres(chemin_config);
        std::vector<Eleve> eleves = charger_eleves(params.fichier_eleves);
        std::cout << eleves.size() << " eleves charges depuis " << params.fichier_eleves << '\n';

        Genetique gen(params, eleves);
        Classes classes = gen.algo();
        gen.afficher_resume(classes);
        gen.ecrire_classes_csv(classes);
    } catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << '\n';
        return 1;
    }
    return 0;
}
