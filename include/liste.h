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
bool Liste##_insertLast(Liste **l, Element *elt);\
void Liste##_remove(Liste **l);\
void Liste##_removeAt(Liste **l, int i);\
void Liste##_removeLast(Liste** l);\
void Liste##_removeAll(Liste *l);\
Element* Liste##_getLast(const Liste *l);\

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
bool Liste##_insertLast(Liste **l, Element *elt) { \
	Liste *nouvelle = malloc(sizeof(Liste)); \
	if(!nouvelle) \
		return false; \
	nouvelle->element = elt; \
	nouvelle->next = NULL; \
	Liste *current=*l;\
	if(current==NULL){\
		*l = nouvelle;\
	}\
	else{\
		while(current->next!=NULL){\
			current = current->next;\
		}\
		current->next = nouvelle; \
	}\
	return true; \
} \
Element* Liste##_getLast(const Liste* l){\
	if(l==NULL){\
		return NULL;\
	}\
	else{\
		while(l->next!=NULL){\
			l = l->next;\
		}\
		return l->element; \
	}\
}\
void Liste##_remove(Liste **l) { \
	Liste *temp = *l;\
	*l = (*l)->next;\
	free(temp); \
}\
void Liste##_removeAt(Liste** l, int i)\
{\
	if(l==NULL){\
		return;\
	}\
	else{\
		if((*l)->next==NULL){\
			if(i<=0){\
				free((*l));\
				(*l) = NULL;\
			}\
			return;\
		}\
		if(i<=0){\
			Liste* tmp = (*l)->next;\
			free(*l); \
			(*l) = tmp;\
			return;\
		}\
		int j=1;\
		Liste* current = *l;\
		while(current->next!=NULL && j<i){\
			current = current->next;\
		}\
		if(current->next!=NULL){\
			Liste* tmp = current->next->next;\
			free(current->next); \
			current->next = tmp;\
		}\
	}\
}\
void Liste##_removeLast(Liste** l)\
{\
	if(l==NULL){\
		return;\
	}\
	else{\
		if((*l)->next==NULL){\
			free((*l));\
			(*l) = NULL;\
			return;\
		}\
		Liste* current = *l;\
		while(current->next->next!=NULL){\
			current = current->next;\
		}\
		free(current->next); \
		current->next = NULL;\
	}\
}\
void Liste##_removeAll(Liste *l) { \
	if(l->next!=NULL) \
		Liste##_removeAll(l->next); \
	free(l); \
}

#endif /* LISTE_H */
