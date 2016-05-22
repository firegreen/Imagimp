#ifndef LISTE_H
#define LISTE_H

#include <stdlib.h>
#include <stdbool.h>

#define DECLARER_LISTE(Liste, Element) \
\
typedef struct Liste { \
	Element *element; \
	struct Liste *next; \
} Liste; \
 \
Liste *make_ ## Liste (Element* e);\
bool Liste##_insert(Liste **l, Element *elt);\
void Liste##_remove(Liste **l);\
void Liste##_removeAll(Liste *l);\

#define IMPLEMENTE_LISTE(Liste, Element) \
Liste *make_ ## Liste (Element* e){\
	Liste* liste = malloc(sizeof(Liste));\
	liste->element = e;\
	liste->next = NULL;\
	return liste;\
}\
\
bool Liste##_insert(Liste **l, Element *elt) { \
	Liste *nouvelle = malloc(sizeof(Liste)); \
	if(!nouvelle) \
		return false; \
	nouvelle->element = elt; \
	nouvelle->next = *l; \
	*l = nouvelle; \
	return true; \
} \
void Liste##_remove(Liste **l) { \
	Liste *temp = *l;\
	*l = (*l)->next;\
	free(temp); \
}\
void Liste##_removeAll(Liste *l) { \
	if(l->next!=NULL) \
		Liste##_removeAll(l->next); \
	free(l); \
}

#endif /* LISTE_H */
