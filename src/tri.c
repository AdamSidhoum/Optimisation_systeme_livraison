#include <stdlib.h>
#include "tri.h"

int *tri_indices_init(int n) {
    int *ind = malloc((size_t)(n > 0 ? n : 1) * sizeof(int));
    if (ind == NULL) return NULL;
    for (int i = 0; i < n; i++) ind[i] = i;
    return ind;
}

static void fusionner(const Colis *tab, int *indices, int *tmp,
                      int g, int m, int d, CmpColis cmp, int ordre) {
    int i = g, j = m + 1, k = g;
    while (i <= m && j <= d) {
        if (ordre * cmp(&tab[indices[i]], &tab[indices[j]]) <= 0) {
            tmp[k++] = indices[i++];
        } else {
            tmp[k++] = indices[j++];
        }
    }
    while (i <= m) tmp[k++] = indices[i++];
    while (j <= d) tmp[k++] = indices[j++];
    for (k = g; k <= d; k++) indices[k] = tmp[k];
}

static void fusion_rec(const Colis *tab, int *indices, int *tmp,
                       int g, int d, CmpColis cmp, int ordre) {
    if (g >= d) return;
    int m = g + (d - g) / 2;
    fusion_rec(tab, indices, tmp, g, m, cmp, ordre);
    fusion_rec(tab, indices, tmp, m + 1, d, cmp, ordre);
    fusionner(tab, indices, tmp, g, m, d, cmp, ordre);
}

int tri_fusion(const Colis *tab, int *indices, int n, CmpColis cmp, int ordre) {
    if (n < 2) return 1;
    int *tmp = malloc((size_t)n * sizeof(int));
    if (tmp == NULL) return 0;
    fusion_rec(tab, indices, tmp, 0, n - 1, cmp, ordre);
    free(tmp);
    return 1;
}

void tri_insertion(const Colis *tab, int *indices, int n, CmpColis cmp, int ordre) {
    for (int i = 1; i < n; i++) {
        int cle = indices[i];
        int j = i - 1;
        while (j >= 0 && ordre * cmp(&tab[indices[j]], &tab[cle]) > 0) {
            indices[j + 1] = indices[j];
            j--;
        }
        indices[j + 1] = cle;
    }
}
