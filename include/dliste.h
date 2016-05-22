#ifndef DLISTE_H
#define DLISTE_H

#include <stdlib.h>
#include <stdbool.h>

#define DECLARER_DLISTE(Liste, Element) \
\
typedef struct Liste { \
    Element *element; \
    struct Liste *previous, *next; \
} Liste; \
 \
Liste *make_ ## Liste (Element* e);\
bool Liste##_insertBefore(Liste *l, Element *elt);\
bool Liste##_insertAfter(Liste *l, Element *elt);\
void Liste##_remove(Liste *l);\
void Liste##_removeAll(Liste *l);\


#define IMPLEMENTE_DLISTE(Liste, Element) \
Liste *make_ ## Liste (Element* e){\
    Liste* liste = malloc(sizeof(Liste));\
    liste->element = e;\
    liste->previous = liste->next = NULL;\
    return liste;\
}\
\
bool Liste##_insertBefore(Liste *l, Element *elt) { \
    Liste *nouvelle = malloc(sizeof(Liste)); \
    if(!nouvelle) \
        return false; \
    nouvelle->element = elt; \
    nouvelle->previous = l->previous; \
    nouvelle->next = l; \
    if(l->previous) \
        l->previous->next = nouvelle; \
    l->previous = nouvelle; \
    return true; \
} \
bool Liste##_insertAfter(Liste *l, Element *elt) { \
    Liste *nouvelle = malloc(sizeof(Liste)); \
    if(!nouvelle) \
        return false; \
    nouvelle->element = elt; \
    nouvelle->previous = l; \
    nouvelle->next = l->next; \
    if(l->next) \
        l->next->previous = nouvelle; \
    l->next = nouvelle; \
    return true; \
} \
void Liste##_remove(Liste *l) { \
    if(l->previous) \
        l->previous->next = l->next; \
    if(l->next) \
        l->next->previous = l->previous; \
    free(l); \
}\
void Liste##_removeAll(Liste *l) { \
	if(l->next!=NULL) \
		Liste##_removeAll(l->next); \
	l->previous->next = NULL; \
	free(l); \
}

#endif /* DLISTE_H */
