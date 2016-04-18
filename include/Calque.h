#include <stddef.h>
#include <stdint.h>
#include "dliste.h"

typedef struct Calque Calque;
typedef void (*FonctionMelange)(Calque *resultat, const Calque *dessous, const Calque *dessus);

void Melange_add(Calque *resultat, const Calque *dessous, const Calque *dessus);
void Melange_mul(Calque *resultat, const Calque *dessous, const Calque *dessus);

typedef struct {
    unsigned char table[256];
} LUT;

DEFINIR_DLISTE(ListeLUTs, LUT)

struct Calque {
    unsigned char *rvb;
    size_t l, h;
    FonctionMelange melange;
    float opacite;
    ListeLUTs luts;
    unsigned char *rvb_rendu;
};

static unsigned char Calque_lireR(const Calque *c, size_t x, size_t y) {
    return c->rvb[3*(y*c->l + x)];
}
static unsigned char Calque_lireV(const Calque *c, size_t x, size_t y) {
    return c->rvb[3*(y*c->l + x) + 1];
}
static unsigned char Calque_lireB(const Calque *c, size_t x, size_t y) {
    return c->rvb[3*(y*c->l + x) + 1];
}

typedef uint32_t Histogramme[256];

void Calque_histogrammeRVB(const Calque *calque, Histogramme histogramme);
void Calque_histogrammeR(const Calque *calque, Histogramme histogramme);
void Calque_histogrammeV(const Calque *calque, Histogramme histogramme);
void Calque_histogrammeB(const Calque *calque, Histogramme histogramme);

DEFINIR_DLISTE(ListeCalques, Calque);

typedef struct {
    ListeCalques calques;
    Calque rendu;
    Calque calque_virtuel;
} Image;

#include "pile.h"

typedef struct {
    int something;
} Action;

DEFINIR_PILE(Historique, Action)


void Image_importerPPM();
void Image_exporterPPM();
void Image_ajouterCalqueVierge();
/* Ajouter un calque vierge (CAL_1), */
/* Naviguer dans les calques (CAL_2), */
/* Modfier le paramètre d'opacité d'un calque (CAL_3), */
/* Modifer la fonction de mélange du calque (addition/ajout) (CAL_4), */
/* Supprimer le calque courant (CAL_5). */
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
 * L'ajout d'une LUT se fera toujours en fin de liste de LUT pour un calque donné. L'ajout se fait sur le calque
 * courant.
 * L'application d'une LUT à une image est detaillee dans le paragraphe 1.5.
 * La suppression d'une LUT est effectuée a la fin de la liste. Elle se fait sur le calque courant.
 */
