#ifndef DLISTE_H
#define DLISTE_H

#include <stdlib.h>
#include <stdbool.h>

#define DEFINIR_DLISTE(Liste, Element) \
\
struct Liste { \
    Element *element; \
    struct Liste *precedent, *suivant; \
}; \
typedef struct Liste Liste; \
 \
static bool Liste##_insererAvant(Liste *l, Element *elt) { \
    Liste *nouvelle = malloc(sizeof(Liste)); \
    if(!nouvelle) \
        return false; \
    nouvelle->element = elt; \
    nouvelle->precedent = l->precedent; \
    nouvelle->suivant = l; \
    if(l->precedent) \
        l->precedent->suivant = nouvelle; \
    l->precedent = nouvelle; \
    return true; \
} \
static bool Liste##_insererApres(Liste *l, Element *elt) { \
    Liste *nouvelle = malloc(sizeof(Liste)); \
    if(!nouvelle) \
        return false; \
    nouvelle->element = elt; \
    nouvelle->precedent = l; \
    nouvelle->suivant = l->suivant; \
    if(l->suivant) \
        l->suivant->precedent = nouvelle; \
    l->suivant = nouvelle; \
    return true; \
} \
void Liste##_retirer(Liste *l) { \
    if(l->precedent) \
        l->precedent->suivant = l->suivant; \
    if(l->suivant) \
        l->suivant->precedent = l->precedent; \
    free(l); \
}

#endif /* DLISTE_H */
