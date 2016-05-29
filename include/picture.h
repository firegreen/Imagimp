#ifndef PICTURE_H
#define PICTURE_H
/// ///////////////////////////////////////////////////////////////////////////
/// Picture :
/// Structure permettant de créé un rendu à partir d'une liste de calque
/// ///////////////////////////////////////////////////////////////////////////

#include "layer.h"

typedef struct {
	LayersList layers;
	int nbLayers;
	LayersList* lastlayer;
	LayersList* current;
	int currentID;
	Layer Cf;
	Layer beforeCf;
	Layer blank;
} Picture;


// Crée une image avec un calque blanc de largeur width et d'hauteur height
void makeEmptyPicture(Picture *p, unsigned int width, unsigned int height);
// Ajoute un calque vide à l'image, le calque est ajoute au dessus des calques existants
void addNewEmptyLayer(Picture* p);
// Redimensionne le calque de base de l'image
void resizePicture(Picture *p, unsigned int width, unsigned int height);
// Ajoute un un calque rempli par rgbSrc de largeur width et d'hauteur height
void addNewLayer(Picture* p, unsigned char* rgbSrc, int width, int height);
// Renvoie vrai si l'image ne contient que le calque de base
int pictureIsEmpty(const Picture* p);
// Supprime le calque courant, le calque courant devient celui d'en-dessous
void removeCurrentLayer(Picture* p);
// fait avancer le calque courant
void putCurrentLayerFront(Picture* p);
// fait reculer le calque courant
void putCurrentLayerBehind(Picture* p);
// Met à jour le rendu final avec une certaine resolution
// pixelsize : entier plus grand ou egale a 1, il s'agit du nombre de pixel qui prendront la même couleur
void updateCfLayer(Picture* p, int pixelsize);
// change le calque courant, le calque courant devient le ième calque au-dessus du calque de base
void changeCurrentTo(Picture *p, int i);
// change le calque courant, le calque courant devient celui d'au-dessus
void changeCurrentToAboveLayer(Picture* p);
// change le calque courant, le calque courant devient celui d'en-dessous
void changeCurrentToBelowLayer(Picture* p);
// effectue une translation du calque courant
void translateCurrentLayer(Picture* p, int tx, int ty);

// Rend une image à partir de la liste de calque layers et rempli le calque lf avec une certaine resolution
// pixelsize : entier plus grand ou egale a 1, il s'agit du nombre de pixel qui prendront la même couleur
void makeCfPicture(Layer* lf, LayersList* layers, int pixelsize);

#endif
