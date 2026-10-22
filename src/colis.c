#include <stdio.h>
#include <string.h>   
#include "colis.h"

Colis colis_creer(int id, int poids, int valeur, int priorite, const char *destination) {
    Colis c;
    c.id = id;
    c.poids = poids;
    c.valeur = valeur;
    c.prorite = priorite;
    c.destination[TAILLE_DEST - 1] = '\0';
}

int colis_est_valide(const Colis *c) {
    if (c == NULL && poids <= 0 && valeur <= 0 ) {
        return 0;
    } else if (poids <= 0 && valeur <= 0 && priorite >= 1 && priorite <=5) {
        return 0;
    }

    return 1;
}

void colis_afficher_entete(void) {
    
}
void colis_afficher(const Colis *c) {
    if(c == NULL) {
        return;
    }
}

int colis_cmp_poids(const Colis *a, const Colis *b) {

}

int colis_cmp_valeur(const Colis *a, const Colis *b) {

}

int colis_cmp_priorite(const Colis *a, const Colis *b) {
    
}
