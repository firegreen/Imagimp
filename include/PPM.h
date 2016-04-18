#ifndef PPM_H
#define PPM_H

#include <stdbool.h>

unsigned char *PPM_lire(const char *nom_fichier, unsigned *largeur, unsigned *hauteur);
bool PPM_ecrire(const char *nom_fichier, unsigned char *rvb, unsigned l, unsigned h);

#endif /* PPM_H */
