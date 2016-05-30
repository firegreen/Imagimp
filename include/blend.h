#ifndef BLEND_H
#define BLEND_H

#include "liste.h"
struct Layer;
typedef struct Layer Layer;
// coefficients qui permettent de calculer le niveau de gris/la luminance d'un pixel
#define REDCOEF 0.2126
#define GREENCOEF 0.7152
#define BLUECOEF 0.0722

// signature d'une fonction de melange
typedef unsigned char (*BlendPixelFunc)(unsigned char, float, unsigned char, float, void* parameters);

// L'enumeration BlendMode liste des drapeaux permettant de determiner la fonction de melange
typedef enum BLENDMODE { BLEND_ADD=0, BLEND_MOY, BLEND_MULT, BLEND_SUB, BLEND_DIV, BLEND_DARK, BLEND_LIGHT, NBBLEND } BlendMode;
// drapeau definissant le melange par defauts
#define DEFAULTBLEND BLEND_MULT

// operation moyenne de deux pixels
// parameters doit être un pointeur vers la somme des deux opacite
unsigned char moyBlend(unsigned char pixelBelow, float opacityBelow,
				 unsigned char pixelAbove, float opacityAbove,
				 void* parameters);
// operation addition de deux pixels
unsigned char addBlend(unsigned char pixelBelow, float opacityBelow,
			  unsigned char pixelAbove, float opacityAbove,
			  void* parameters);
// operation multiplication de deux pixels
unsigned char multBlend(unsigned char pixelBelow, float opacityBelow,
			  unsigned char pixelAbove, float opacityAbove,
			  void* parameters);
// operation inverse de la moyenne de deux pixels
unsigned char inversedMoyBlend(unsigned char pixelBelow, float opacityBelow,
					unsigned char pixelAbove, float opacityAbove,
					void* parameters);
// operation soustraction de deux pixels
unsigned char subBlend(unsigned char pixelBelow, float opacityBelow,
			  unsigned char pixelAbove, float opacityAbove,
			  void* parameters);
// operation division de deux pixels
unsigned char divBlend(unsigned char pixelBelow, float opacityBelow,
			  unsigned char pixelAbove, float opacityAbove,
			  void* parameters);
// operation sombre de deux pixels
unsigned char darkBlend(unsigned char pixelBelow, float opacityBelow,
			  unsigned char pixelAbove, float opacityAbove,
			  void* parameters);
// operation lumineux de deux pixels
unsigned char lightBlend(unsigned char pixelBelow, float opacityBelow,
			  unsigned char pixelAbove, float opacityAbove,
			  void* parameters);

/// ///////////////////////////////////////////////////////////////////////////
/// LUT :
/// Structure permettant d'affecter des transformation rapidement à une image
/// ///////////////////////////////////////////////////////////////////////////

// L'enumeration Effect liste des drapeaux permettant de generer les LUT correspondant
typedef enum EFFECT {MULTIPLE=-1,CONTRAST=0, BRIGHTNESS, SEPIA, INVERSED, GRAYEFFECT, NBEFFECTS} Effect;

typedef struct {
	Effect LUTEffect;
	float effectAmount;
	char activ;
	char fromGray; // 1 si la source doit être un niveau de gris 0 sinon
	unsigned char values[3][256];	//values[0][...] canal rouge - gray to red si fromGray - red to red sinon
									//values[1][...] canal vert - gray to green si fromGray - green to green sinon
									//values[2][...] canal bleu - gray to blue si fromGray - blue to blue sinon
} LUT;

DECLARER_LISTE(LUTsList, LUT)

//Ajoute la lut lut dans le calque l, il est place à la fin de la liste -> il effectue la derniere transformation
void addLUTToLayer(LUT* lut, Layer* l);
// cree et definis une lut à partir de l'effet demande et du taux de l'effet
LUT* makeLUT(Effect e, float effectAmount);
// definis une lut à partir de l'effet demande et du taux de l'effet
void setLUT(LUT* lut, Effect e, float effectAmount);
// combine deux lut et renvoie le resultat dans values (le lut2 s'applique apres le lut1)
void combineLUT(unsigned char values[3][256], LUT* lut1, LUT* lut2);
// definis out comme la combinaisons de toutes les luts de luts
void combineAllLUT(LUT* out, LUTsList* luts);

/// ///////////////////////////////////////////////////////////////////////////
/// Blend :
/// Structure utilise pour le melange de deux calque
/// Elle permet surtout de passer des parametres d'une fonction à une autre
/// ///////////////////////////////////////////////////////////////////////////

typedef struct {
   unsigned char *pBelow;
   const unsigned char *pAbove;
   unsigned iBelow;
   float opacityBelow, opacityAbove;
   LUT *lut;
   BlendPixelFunc blendFunc;
   void* parameters;
} Blend;

// cree le parametre de melange blend à partir des calques à fusionner et des indices à partir
// desquels on commence le melange
void initBlend(Blend* blend, Layer *lBelow, const Layer *lAbove,
			   unsigned long iStartB, unsigned long iStartA, int inverseAction);
// effectue le melange sur le pixel pointe actuellement par blend, affecte le calque d'en dessous
void processBlendAtCurrentPixel(Blend* blend);
// melange deux calques avec une certaine resolution
// pixelsize : entier plus grand ou egale a 1, il s'agit du nombre de pixel qui prendront la même couleur
void blendTwoLayer(Layer *lBelow, const Layer *lAbove, int inverseAction, int pixelsize);
// melange deux calques avec une certaine resolution dans une zone restreinte
// pixelsize : entier plus grand ou egale a 1, il s'agit du nombre de pixel qui prendront la même couleur
void blendTwoLayerInRect(Layer *lBelow, const Layer *lAbove,
						 int inverseAction, int pixelsize, int x, int y, int width, int height);
// remplie le calque l avec les pixels de layerToCopy dans une zone restreinte
void copyLayerInRect(Layer* l, const Layer* layerToCopy, int x, int y, int width, int height);

#endif
