#ifndef TRI_H
#define TRI_H

#include "colis.h"



int  *tri_indices_init(int n);

int  tri_fusion(const Colis *tab, int *indices, int n, CmpColis cmp, int ordre);

/* Tri par insertion : O(n^2), uniquement pour comparer dans les mesures. */
void tri_insertion(const Colis *tab, int *indices, int n, CmpColis cmp, int ordre);

#endif
