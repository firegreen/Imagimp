#include "dliste.h"
#include "outils.h"

typedef enum EFFECT {CONTRASTPLUS, CONTRASTMINUS, MULTIPLE, NBEFFECTS} EFFECT;

typedef struct Layer Layer;
typedef void (*blendFunction)(Layer *out, const Layer *layerbelow, const Layer *layerabove);

void normalBlend(Layer *out, const Layer *layerbelow, const Layer *layerabove);
void addBlend(Layer *out, const Layer *layerbelow, const Layer *layerabove);
void mulBlend(Layer *out, const Layer *layerbelow, const Layer *layerabove);

typedef struct {
    EFFECT LUTEffect;
    float effectAmount;
    unsigned char values[256];
} LUT;

DECLARER_DLISTE(LUTsList, LUT)

typedef struct {
    unsigned long max;
    unsigned long min;
    unsigned long nbPixelsPerValues[256];
} Histogramme;

typedef enum HistogrammeType{ RED_H=0, GREEN_H, BLUE_H, GRAYSCALE_H, NB_H_TYPES } HistogrammeType;

typedef struct Layer {
    unsigned char *rgb;
    unsigned int width, height;
    unsigned int x,y;
    blendFunction melange;
    float opacity;
    LUTsList* luts;
    Histogramme histogrammes[NB_H_TYPES];

} Layer;
typedef Layer Layer;

void makeLayer(Layer* l, const unsigned char* rgb, unsigned int width, unsigned int height,
               unsigned int x, unsigned int y);
void makeEmptyLayer(Layer* l, unsigned int width, unsigned int height,
                    unsigned int x, unsigned int y);
Bounds layerBoundsInWindow(const Layer *l, unsigned int screen_width, unsigned int screen_height);
void freeLayer(Layer** l);

/*static unsigned char getRpixel(const Layer *l, unsigned int x, unsigned int y) {
    return l->rgb[3*(y*l->width + x)];
}
static unsigned char getGpixel(const Layer *l, unsigned int x, unsigned int y) {
    return l->rgb[3*(y*l->width + x) + 1];
}
static unsigned char getBpixel(const Layer *l, unsigned int x, unsigned int y) {
    return l->rgb[3*(y*l->width + x) + 2];
}

static unsigned char getGrayscalepixel(const Layer *l, unsigned int x, unsigned int y) {
    int pos = 3*(y*l->width + x);
    return l->rgb[pos] * 0.299f + l->rgb[pos+1] * 0.587 + l->rgb[pos+2] * 0.114;
}*/

void makeRedHistogrammeFromLayer(Histogramme* histogramme, const Layer *layer);
void makeGreenHistogrammeFromLayer(Histogramme* histogramme, const Layer *layer);
void makeBlueHistogrammeFromLayer(Histogramme* histogramme, const Layer *layer);
void makeGrayHistogrammeFromLayer(Histogramme* histogramme, const Layer *layer);
void drawHistorgramme(const Histogramme* histogramme, HistogrammeType type, const Bounds *b);

void makeLUT(LUT* lut, EFFECT e, float effectAmout);
void combineLUT(LUT* out, LUT* lut1, LUT* lut2);
//void makeLUT(LUT* lut, EFFECT e, float effectAmout);

DECLARER_DLISTE(LayersList, Layer)

typedef struct {
    LayersList layers;
    LayersList* lastlayer;
    Layer Cf;
    LayersList* current;
    Layer blank;
    int nbLayers;
    int currentID;
} Picture;

void makeEmptyPicture(Picture *p, unsigned int width, unsigned int height);
void addNewEmptyLayer(Picture* p);
void addNewLayer(Picture* p, unsigned char* rgbSrc, int width, int height);
void makeCfPicture(Layer* lf, LayersList* layers);
void removeCurrentLayer(Picture* p);
void updateCfLayer(Picture* p);
#include "pile.h"

typedef struct {
    int something;
} Action;

DEFINIR_PILE(Historique, Action)

/* Ajouter un Layer vierge (CAL_1), */
/* Naviguer dans les Layers (CAL_2), */
/* Modfier le paramètre d'opacité d'un Layer (CAL_3), */
/* Modifer la fonction de mélange du Layer (addition/ajout) (CAL_4), */
/* Supprimer le Layer courant (CAL_5). */
/*
 * 1. Ajouter une LUT (LUT_1),
 * 2. Appliquer une LUT a une image (LUT_2),
 * 3. Supprimer la derniere LUT (LUT_3).
 * Les LUT que l'application devra pouvoir ajouter sont listées ci-dessous. Chacune de ces modifications
 * possède un code indiqué entre parentheses.
 * augmentation de luminosité (ADDLUM), dépend d'un paramètre
 * diminution de luminosité (DIMLUM), dépend d'un paramètre
 * augmentation du contraste (ADDCON), dépend d'un paramètre
 * diminution du contraste (DIMLUM), dépend d'un parametre
 * inversion de la couleur (INVERT),
 * effet sepia (SEPIA), peut dépendre d'un ou plusieurs paramètres.
 * L'ajout d'une LUT se fera toujours en fin de liste de LUT pour un Layer donné. L'ajout se fait sur le Layer
 * courant.
 * L'application d'une LUT à une image est detaillee dans le paragraphe 1.5.
 * La suppression d'une LUT est effectuée a la fin de la liste. Elle se fait sur le Layer courant.
 */
