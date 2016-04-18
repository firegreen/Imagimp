#ifndef PILE_H
#define PILE_H

#include <stdlib.h>
#include <assert.h>

#define DEFINIR_PILE(Pile, Element) \
/*
\
struct Pile { \
    Element element; \
    struct Pile *suivant; \
}; \
typedef struct Pile Pile; \
 \
static inline bool Pile##_is_empty(const Pile## *p) { \
    return !!(p->top);\
} \
 \
static inline bool Pile##_push(Pile *p, Element val) { \
    Pile *old_top = p->top; \
    p->top = malloc(sizeof(Pile##_linkedLINKEDNODE)); \
    if(!p->top) \
        return false;\
    p->top->val = val; \
    p->top->next = old_top; \
    return true; \
} \
 \
static inline Pile Pile##_pop(Pile## *p) { \
    assert(p->top); \
    Pile##_linkedLINKEDNODE *new_top = p->top->next; \
    Pile val = p->top->val; \
    free(p->top); \
    p->top = new_top; \
    return val; \
} \
 \
static inline Pile Pile##_top(const Pile## *p) { \
    assert(p->top); \
    return p->top->val; \
} \
 \
static void Pile##_walk(Pile## *p, \
                       void (*func)(size_t index, Pile *valptr)) { \
    size_t i; \
    Pile##_linkedLINKEDNODE *cur; \
    for(i=0, cur=p->top ; cur ; cur=cur->next, ++i) \
         func(i, &(cur->val));\
}
*/
#endif /* PILE_H */
