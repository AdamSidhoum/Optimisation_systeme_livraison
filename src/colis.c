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
    printf("%-6s %-6s %-7s %-9s %-12s\n",
        "ID", "Poids", "Valeur", "Priorite", "Destination");
    printf("--------------------------------------------\n");
}
void colis_afficher(const Colis *c) {
    if(c == NULL) {
        return;
    }
    printf("%-6d %-6d %-7d %-9d %-12s\n",
           c->id, c->poids, c->valeur, c->priorite, c->destination);
}

int colis_cmp_poids(const Colis *a, const Colis *b) {
    return (a->poids > b->poids) - (a->poids < b->poids);
}

int colis_cmp_valeur(const Colis *a, const Colis *b) {
    return (a->valeur > b->valeur) - (a->valeur < b->valeur);
}

int colis_cmp_priorite(const Colis *a, const Colis *b) {
    return (a->priorite > b->priorite) - (a->priorite < b->priorite);

}
