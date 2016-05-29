#ifndef PPM_H
#define PPM_H

#include <stdbool.h>

// lit un fichier ppm
// renvoie un tableau 1D de pixels de 3 octet non signe : r,v,b
// renvoie via les parametres les dimensions de l'image charge
unsigned char *readPPM(const char *nom_fichier, unsigned int *width, unsigned int *height);

// ecrit un fichier ppm
bool writePPM(const char *nom_fichier, unsigned char *rvb, unsigned w, unsigned h);

#endif /* PPM_H */
