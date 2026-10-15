import numpy as np
import pandas as pd
from copy import copy

class Genetique:
    def __init__(self):
        self.Nb_classes=4
        self.Nb_select = 5

        # math, francais, attitude 
        self.w_notes = np.array([2, 2, 4])
        # genre, racisé
        self.w_caracteristiques = np.array([10, 5])
        self.w_ecart_type_notes = 0.5

        self.eleves = pd.read_csv("eleves.csv", sep=";")
        self.Nb_eleves = self.eleves.shape[0]
        self.eleve_par_classe = int(self.Nb_eleves/self.Nb_classes)

        self.notes = np.array([self.eleves["Math"], self.eleves["Lecture"], self.eleves["Comportement"]]).T
        
        self.moyennes_notes = np.mean(self.notes, axis=0)

        self.caracteristiques_eleves = np.array([self.eleves["Genre"], self.eleves["Etranger"]]).T
        self.moyenne_caracteristique = np.mean(self.caracteristiques_eleves, axis=0)

    def fct_cout(self, classes):
        cout_max = -np.inf
        for classe in classes:
            moyenne_notes_classe = np.mean(self.notes[classe], axis=0)
            moyenne_caracteristique_classe = np.mean(self.caracteristiques_eleves[classe], axis=0)

            cout = np.sum(abs(self.moyenne_caracteristique - moyenne_caracteristique_classe) * self.w_caracteristiques)
            cout+= np.sum(abs(self.moyennes_notes - moyenne_notes_classe) * self.w_notes) 
            cout+= -np.std(self.notes[classe])*self.w_ecart_type_notes
            cout_max = max(cout, cout_max)
        return cout_max
    
    def genere_classe_alea(self):
        classes_l = np.random.choice(self.Nb_eleves, size=self.Nb_eleves, replace=False)
        classes = self.separer_en_classes(classes_l)
        return classes
    
    def separer_en_classes(self, classes_l):
        classes = []
        eleve_par_classe = self.eleve_par_classe
        nb_eleves = self.Nb_eleves
        for i in range(self.Nb_classes):
            reste = (nb_eleves )%(self.Nb_classes-(i))

            eleve_par_classe = self.eleve_par_classe + reste
            it = self.Nb_eleves-nb_eleves
            classes.append(classes_l[it:it +eleve_par_classe])
            nb_eleves = nb_eleves - eleve_par_classe
        return classes


    def mutate(self, classes, nb_modif=2):
        # le but ici est de legerement modifier les classes aléatoirement
        classes_l = np.concat(classes)
        classes_l_target = copy(classes_l)
        indx = np.random.choice(classes_l.shape[0], size=(nb_modif, 2), replace=False)
        for i in range(nb_modif):
            classes_l_target[indx[i, 0]] = classes_l[indx[i, 1]]
            classes_l_target[indx[i, 1]] = classes_l[indx[i, 0]]
        # classes_l[indx[0]], classes_l[indx[1]] = classes_l[indx[1]],classes_l[indx[0]]
        # print(indx)
        classes = self.separer_en_classes(classes_l_target)
        return classes
    
    def algo(self):
        self.nb_generations = 30
        self.nb_selection_par_generations = 20
        self.pop_par_gen = 500
        pop_classes = []
        # on genere la premiere classes 
        for i_generation in range(self.pop_par_gen):
            pop_classes.append(self.genere_classe_alea())

        for i_generation in range(self.nb_generations):
            couts = []
            for classes in pop_classes:
                couts.append(self.fct_cout(classes))
            indx = np.argsort(couts)[:self.nb_selection_par_generations]

            nouvelle_classes = []
            for it in range(self.pop_par_gen):
                i = indx[it%self.nb_selection_par_generations]
                nouvelle_classes.append(self.mutate(pop_classes[i]))
            pop_classes = copy(nouvelle_classes)
            
            print(i_generation, np.min(couts))
        # print(pop_classes[indx[0]])
        return pop_classes[indx[0]]

    def afficher_classes(self, classes):
        for i in range(self.Nb_classes):
            print(f'classe {i+1}:')
            print(self.eleves.iloc[classes[i]])


if __name__=="__main__":
    gen = Genetique()
    # classes = gen.genere_classe_alea()
    # print(classes)
    # print(gen.mutate(classes))
    classes = gen.algo()
    print(classes)
    gen.afficher_classes(classes)


    

        
        

