#include <stdio.h>
#include <stdlib.h>
#include "collection.h"

int collection_init(Collection *col, int capacite_initiale) {
    if (col == NULL) {
        return 0;
    }
    if (capacite_initiale < 1) {
        capacite_initiale = 1;
    }
    col->tab = malloc((size_t)capacite_initiale * sizeof(Colis));
    if (col->tab == NULL) {
        col->taille = 0;
        col->capacite = 0;
        return 0;
    }
    col->taille = 0;
    col->capacite = capacite_initiale;
    return 1;
}


int collection_ajouter(Collection *col, Colis c) {
    if (col == NULL) {
        return 0;
    }
    if (col->taille == col->capacite) {
        int nouvelle_cap = (col->capacite > 0) ? col->capacite * 2 : 1;
        Colis *tmp = realloc(col->tab, (size_t)nouvelle_cap * sizeof(Colis));
        if (tmp == NULL) {
            return 0;
        }
        col->tab = tmp;
        col->capacite = nouvelle_cap;
    }
    col->tab[col->taille] = c;
    col->taille++;
    return 1;
}

void collection_afficher(const Collection *col) {
    if (col == NULL) {
        return;
    }
    if (col->taille == 0) {
        printf("Aucun colis.\n");
        return;
    }
    colis_afficher_entete();
    for (int i = 0; i < col->taille; i++) {
        colis_afficher(&col->tab[i]);
    }
}

void collection_liberer(Collection *col) {
    if (col == NULL) {
        return;
    }
    free(col->tab);
    col->tab = NULL;
    col->taille = 0;
    col->capacite = 0;
}
