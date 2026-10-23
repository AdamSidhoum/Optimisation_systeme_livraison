#ifndef COLLECTION_H
#define COLLECTION_H

#include "colis.h"

typedef struct Collection {
    Colis *tab;
    int taille;
    int capacite;
} Collection;

int  collection_init(Collection *col, int capacite_initiale);
int  collection_ajouter(Collection *col, Colis c);
void collection_afficher(const Collection *col);
void collection_liberer(Collection *col);

#endif
