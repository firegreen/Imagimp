// #include "Imagimp.h"
// #include "glimagimp/interface.h"
//
// Imagimp imagimp;
//
// void sur_clavier(unsigned char ascii, int x, int y) {
//     imagimp.fonction_clavier(&imagimp, ascii, x, y);
// }
// void sur_clavier_special(int touche, int x, int y) {
//     imagimp.fonction_clavier_special(&imagimp, touche, x, y);
// }
// void sur_souris(int bouton, int appuye, int x, int y) {
//     imagimp.fonction_souris(&imagimp, bouton, appuye, x, y);
// }
// void sur_dessin(void) {
//     imagimp.fonction_dessin(&imagimp);
// }
//
// int main(int argc, char *argv[]) {
//     fixeFonctionClavier(sur_clavier);
//     fixeFonctionClavierSpecial(sur_clavier_special);
//     fixeFonctionClicSouris(sur_souris);
//     fixeFonctionDessin(sur_dessin);
//     Imagimp_lancer(&imagimp, argc, argv);
//     return 0;
// }

/***************************************************************************
 *   Copyright (C) 2008 by Venceslas BIRI                                  *
 *   biri@univ-mlv.fr                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <string.h>
#include <stdio.h>
#include "outils.h"
#include "interface.h"
#include "Imagimp.h"

/// ///////////////////////////////////////////////////////////////////////////
/// Tableau representant une image...
unsigned char* image_base = NULL;
unsigned char* image_switch = NULL;
int switch_image = 0;

unsigned char* createImage(unsigned int* w_im,unsigned int* h_im) {
    unsigned int i,j,k;
    *w_im = 800;
    *h_im = 600;

    unsigned char* tabRVB =
            (unsigned char*) malloc((*w_im)*(*h_im)*3*sizeof(unsigned char));
    if (tabRVB == NULL) {
        printf("Erreur d'allocation dans createImage\n");
        exit(1);
    }
    // Pour chaque ligne
    k = 0;
    for(i = 0;i<(*h_im);i++) {
        for(j = 0;j<(*w_im);j++) {
            tabRVB[k++] = (unsigned char)(int)(255*j/(float)(*w_im));
            tabRVB[k++] = (unsigned char)(int)(255*j/(float)(*w_im));
            tabRVB[k++] = (unsigned char)(int)(255*j/(float)(*w_im));
        }
    }
    return tabRVB;
}

int main(int argc, char* argv[]) {
    Imagimp_launch(argc,argv);
    return 0;
}
