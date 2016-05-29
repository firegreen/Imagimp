#include "dliste.h"
#include "liste.h"
#include "outils.h"
#include "blend.h"

/// ///////////////////////////////////////////////////////////////////////////
/// Histogramme :
/// Structure permettant de donner des infomations sur les pixels d'une image
/// ///////////////////////////////////////////////////////////////////////////
typedef struct {
    unsigned long max;
    unsigned long min;
    unsigned long nbPixelsPerValues[256];
} Histogramme;

/// ///////////////////////////////////////////////////////////////////////////
/// Layer :
/// Structure permettant de traiter une image
/// ///////////////////////////////////////////////////////////////////////////
typedef enum HistogrammeType{ RED_H=0, GREEN_H, BLUE_H, GRAYSCALE_H, NB_H_TYPES } HistogrammeType;

typedef struct Layer {
    unsigned char *rgb;
    unsigned int width, height;
    int x,y;
    BlendMode blendMode;
    float opacity;
	LUTsList* luts;
	Histogramme histogrammes[NB_H_TYPES];
	char histogrammeUpdated;
} Layer;
typedef Layer Layer;

// cree un calque a partir d'un tableau de pixel rgb de largeur width et hauteur height
void makeLayer(Layer* l, const unsigned char* rgb, unsigned int width, unsigned int height,int x, int y);
// cree un calque blanc de largeur width et de hauteur height
void makeEmptyLayer(Layer* l, unsigned int width, unsigned int height,int x, int y);
// remplie le calque l avec les pixels de layerToCopy et lui affecte ses caracteristiques
// les luts reste ceux du calque l
void copyLayer(Layer* l, const Layer* layerToCopy);
// change le mode de melange du calque l
void setBlendMode(Layer* l, BlendMode mode);
// donne la position du calque dans l'espace de la fenetre d'affichage
Bounds layerBoundsInWindow(const Layer *l, unsigned int screen_width, unsigned int screen_height);
// libere un calque
void freeLayer(Layer** l);

// cree l'histogramme definissant le nombre de pixels à partir du calque layer
void makeRedHistogrammeFromLayer(Histogramme* histogramme, const Layer *layer);
void makeGreenHistogrammeFromLayer(Histogramme* histogramme, const Layer *layer);
void makeBlueHistogrammeFromLayer(Histogramme* histogramme, const Layer *layer);
void makeGrayHistogrammeFromLayer(Histogramme* histogramme, const Layer *layer);
//dessine un histogramme dans la zone definis
void drawHistorgramme(const Histogramme* histogramme, const Bounds *b);
// met à jour tout les histogrammes du layer l
void updateHistogramme(Layer* l);

DECLARER_DLISTE(LayersList, Layer)

#include "pile.h"

typedef struct {
    int something;
} Action;

DEFINIR_PILE(Historique, Action)
