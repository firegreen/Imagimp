#ifndef PPM_H
#define PPM_H

#include <stdbool.h>

unsigned char *readPPM(const char *nom_fichier, unsigned int *width, unsigned int *height);
bool writePPM(const char *nom_fichier, unsigned char *rvb, unsigned w, unsigned h);

#endif /* PPM_H */
