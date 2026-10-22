#ifndef COLIS_H
#define COLIS_H

#define TAILLE_DEST 32

typedef struct Colis {
    int id;
    int poids;
    int valeur;
    int priorite;
    char destination[TAILLE_DEST];
} Colis;

#endif