#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "PPM.h"

/* D'après le standard http://netpbm.sourceforge.net/doc/ppm.html */
unsigned char *PPM_lire(const char *nom_fichier, unsigned *largeur, unsigned *hauteur) {

    FILE *fichier = fopen(nom_fichier, "rb");
    if(!fichier) {
        fprintf(stderr, "%s: N'a pas pu ouvrir le fichier.\n", nom_fichier);
        return NULL;
    }

    char car;
    unsigned variante, valeur_max;
    unsigned etape;
    for(etape=0 ; etape < 3 ; )  {
        car = fgetc(fichier);
        if(car == EOF)
            return NULL;
        if(car == '#') {
            do car = fgetc(fichier); while(car != '\n' && car!=EOF);
            if(car == EOF)
                return NULL;
            continue;
        }
        if(!isdigit(car))
            continue;
        fseek(fichier, -1, SEEK_CUR);
        switch(etape) {
        case 0: if(fscanf(fichier, "%u", &variante)   <= 0) return NULL; ++etape; break;
        case 1: if(fscanf(fichier, "%u", largeur)     <= 0) return NULL; ++etape; break;
        case 2: if(fscanf(fichier, "%u", hauteur)     <= 0) return NULL; ++etape; break;
        case 3: if(fscanf(fichier, "%u", &valeur_max) <= 0) return NULL; fgetc(fichier); break;
        }
    }

    unsigned char *gris, *rvb = malloc(3*(*largeur)*(*hauteur));
    if(!rvb) {
        fprintf(stderr, "%s: Pas de place pour allouer les pixels.\n", nom_fichier);
        return NULL;
    }
    size_t i;
    switch(variante) {
    case 6: fread(rvb, 1, 3*(*largeur)*(*hauteur), fichier); break;
    case 5:
        gris = malloc((*largeur)*(*hauteur));
        if(!gris) {
            fprintf(stderr, "%s: Pas de place pour allouer le gris.\n", nom_fichier);
            free(rvb);
            return NULL;
        }
        fread(gris, 1, (*largeur)*(*hauteur), fichier);
        for(i=0 ; i<(*largeur)*(*hauteur) ; ++i)
            rvb[3*i] = rvb[3*i+1] = rvb[3*i+2] = gris[i];
        free(gris);
        break;
    }
    fclose(fichier);
    return rvb;
}

bool PPM_ecrire(const char *nom_fichier, unsigned char *rvb, unsigned l, unsigned h) {
    FILE *fichier = fopen(nom_fichier, "wb");
    if(!fichier) {
        fprintf(stderr, "%s: Ne peut pas ouvrir le fichier en écriture.\n", nom_fichier);
        return false;
    }
    fprintf(fichier, "P6\n%u %u\n255\n", l, h);
    fwrite(rvb, 1, 3*l*h, fichier);
    fclose(fichier);
    return true;
}
