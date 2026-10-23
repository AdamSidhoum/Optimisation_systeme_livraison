#ifndef BENCH_H
#define BENCH_H

#include "collection.h"
#include "hachage.h"

int systeme_ajouter(Collection *col, TableHachage *t, Colis c);

int bench_generer(Collection *col, TableHachage *t, int n);

int recherche_lineaire(const Collection *col, int id);

void bench_lancer(int avec_insertion_100000);

#endif
