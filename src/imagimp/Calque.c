#include <string.h>
#include "Calque.h"

void Calque_histogrammeRVB(const Calque *calque, Histogramme histogramme) {
    size_t x, y;
    unsigned long somme;
    memset(histogramme, 0, sizeof(Histogramme));
    for(y=0 ; y<calque->h ; ++y) {
        for(x=0 ; x<calque->l ; ++x) {
            somme = Calque_lireR(calque, x, y)
                  + Calque_lireV(calque, x, y)
                  + Calque_lireB(calque, x, y);
            ++histogramme[(unsigned char)(somme/3ul)];
        }
    }
}
void Calque_histogrammeR(const Calque *calque, Histogramme histogramme) {
    size_t x, y;
    memset(histogramme, 0, sizeof(Histogramme));
    for(y=0 ; y<calque->h ; ++y)
        for(x=0 ; x<calque->l ; ++x)
            ++histogramme[Calque_lireR(calque, x, y)];
}
void Calque_histogrammeV(const Calque *calque, Histogramme histogramme) {
    size_t x, y;
    memset(histogramme, 0, sizeof(Histogramme));
    for(y=0 ; y<calque->h ; ++y)
        for(x=0 ; x<calque->l ; ++x)
            ++histogramme[Calque_lireV(calque, x, y)];
}
void Calque_histogrammeB(const Calque *calque, Histogramme histogramme) {
    size_t x, y;
    memset(histogramme, 0, sizeof(Histogramme));
    for(y=0 ; y<calque->h ; ++y)
        for(x=0 ; x<calque->l ; ++x)
            ++histogramme[Calque_lireB(calque, x, y)];
}
