#include <string.h>
#include <math.h>
#include "layer.h"
#include "outils.h"
#define max(a,b) a>b?a:b
#define min(a,b) a<b?a:b

#define histoRed histogramme[RED_H]->nbPixelsPerValues
#define histoBlue histogramme[BLUE_H]->nbPixelsPerValues
#define histoGreen histogramme[GREEN_H]->nbPixelsPerValues
#define histoGray histogramme[GRAYSCALE_H]->nbPixelsPerValues

IMPLEMENTE_DLISTE(LUTsList, LUT)
IMPLEMENTE_DLISTE(LayersList, Layer)


void setRpixel(const Layer *l, unsigned int x, unsigned int y, unsigned long Rvalue) {
    l->rgb[3*(y*l->width + x)] = Rvalue;
}
void setGpixel(const Layer *l, unsigned int x, unsigned int y, unsigned long Gvalue) {
    l->rgb[3*(y*l->width + x)+1] = Gvalue;
}
void setBpixel(const Layer *l, unsigned int x, unsigned int y, unsigned long Bvalue) {
    l->rgb[3*(y*l->width + x)+2] = Bvalue;
}

void makeLayer(Layer* l, const unsigned char* rgb, unsigned int width, unsigned int height, unsigned int x,
               unsigned int y){
    unsigned long size = height*width*3;
    l->rgb = malloc(size*sizeof(unsigned char));
    memcpy(l->rgb,rgb,sizeof(unsigned char)*size);
    l->height = height;
    l->width = width;
    l->x = x;
    l->y = y;
    l->melange = mulBlend;
    l->opacity = 1;
    l->luts = NULL;
    makeRedHistogrammeFromLayer(l->histogrammes + RED_H,
                                l);
    makeBlueHistogrammeFromLayer(l->histogrammes + BLUE_H,
                                l);
    makeGreenHistogrammeFromLayer(l->histogrammes + GREEN_H,
                                l);
    makeGrayHistogrammeFromLayer(l->histogrammes + GRAYSCALE_H,
                                l);
}

void makeEmptyLayer(Layer* l, unsigned int width, unsigned int height, unsigned int x, unsigned int y){
    unsigned long size = height*width*3;
    l->rgb = malloc(height*width*3);
    int i,j,k;
    int nbValuesPerWidth = width*3;
    int block = nbValuesPerWidth*5;
    /*for(i=0;i<size;i++)
        l->rgb[i] = 255;*/
    for(i=0;i<size;i += block){
        if((i/nbValuesPerWidth)%4==0)
            for(k=0;k<block/3;k++){
                int pos = k*3;
                for(j=0;j<3;j++)
                {
                   l->rgb[i+j+pos] = (255 - (40*(i%(5-j))));
                }
            }
        else if((i/nbValuesPerWidth)%4==1)
            for(k=0;k<block/3;k++){
                int pos = k*3;
                for(j=0;j<3;j++)
                {
                   l->rgb[i+j+pos] = (255 - (40*(i%(7-j))));
                }
            }
        else if((i/nbValuesPerWidth)%4==2)
            for(k=0;k<block/3;k++){
                int pos = k*3;
                for(j=0;j<3;j++)
                {
                   l->rgb[i+j+pos] = (255 - (40*(i%(9-j))));
                }
            }
        else
            for(k=0;k<block/3;k++){
                int pos = k*3;
                for(j=0;j<3;j++)
                {
                   l->rgb[i+j+pos] = (255 - (10*(i%(19-j))));
                }
            }

    }
    l->height = height;
    l->width = width;
    l->x = x;
    l->y = y;
    l->melange = mulBlend;
    l->opacity = 1;
    l->luts = NULL;
    for(i=0;i<256;i++){
        l->histogrammes[RED_H].nbPixelsPerValues[i] = size;
        l->histogrammes[BLUE_H].nbPixelsPerValues[i] = size;
        l->histogrammes[GREEN_H].nbPixelsPerValues[i] = size;
        l->histogrammes[GRAYSCALE_H].nbPixelsPerValues[i] = size;
    }

}

void makeGrayHistogrammeFromLayer(Histogramme *histogramme, const Layer *layer){
    unsigned int i;
    memset(histogramme, 0, sizeof(Histogramme));
    histogramme->max=0;
    histogramme->min=(layer->width * layer->height);
    for(i=0 ; i<histogramme->min - 2; i+=3) {
        histogramme->nbPixelsPerValues[(unsigned char)(layer->rgb[i] * 0.299f + layer->rgb[i+1] * 0.587 + layer->rgb[i+2] * 0.114)]++;
    }
    for(i=0; i<256;i++)
    {
        if(histogramme->nbPixelsPerValues[i]>histogramme->max)
            histogramme->max = histogramme->nbPixelsPerValues[i];
        else if(histogramme->nbPixelsPerValues[i]<histogramme->min)
            histogramme->min = histogramme->nbPixelsPerValues[i];
    }
}
void makeRedHistogrammeFromLayer(Histogramme *histogramme, const Layer *layer){
    unsigned int i;
    memset(histogramme, 0, sizeof(Histogramme));
    histogramme->max=0;
    histogramme->min=(layer->width * layer->height);
    for(i=0 ; i<(histogramme->min*3) - 2; i+=3) {
        histogramme->nbPixelsPerValues[layer->rgb[i]]++;
    }
    for(i=0; i<256;i++)
    {
        if(histogramme->nbPixelsPerValues[i]>histogramme->max)
            histogramme->max = histogramme->nbPixelsPerValues[i];
        else if(histogramme->nbPixelsPerValues[i]<histogramme->min)
            histogramme->min = histogramme->nbPixelsPerValues[i];
    }
}
void makeGreenHistogrammeFromLayer(Histogramme *histogramme, const Layer *layer){
    unsigned int i;
    memset(histogramme, 0, sizeof(Histogramme));
    histogramme->max=0;
    histogramme->min=(layer->width * layer->height);
    for(i=1 ; i<(histogramme->min*3) - 2; i+=3) {
        histogramme->nbPixelsPerValues[layer->rgb[i]]++;
    }
    for(i=0; i<256;i++)
    {
        if(histogramme->nbPixelsPerValues[i]>histogramme->max)
            histogramme->max = histogramme->nbPixelsPerValues[i];
        else if(histogramme->nbPixelsPerValues[i]<histogramme->min)
            histogramme->min = histogramme->nbPixelsPerValues[i];
    }
}
void makeBlueHistogrammeFromLayer(Histogramme *histogramme, const Layer *layer){
    unsigned int i;
    memset(histogramme, 0, sizeof(Histogramme));
    histogramme->max=0;
    histogramme->min=(layer->width * layer->height);
    for(i=2 ; i<(histogramme->min*3) - 2; i+=3) {
        histogramme->nbPixelsPerValues[layer->rgb[i]]++;
    }
    for(i=0; i<256;i++)
    {
        if(histogramme->nbPixelsPerValues[i]>histogramme->max)
            histogramme->max = histogramme->nbPixelsPerValues[i];
        else if(histogramme->nbPixelsPerValues[i]<histogramme->min)
            histogramme->min = histogramme->nbPixelsPerValues[i];
    }
}

void drawHistorgramme(const Histogramme *histogramme, HistogrammeType type, const Bounds* b){
    int i;
    switch (type) {
    case RED_H:
        glColor3f(1,0,0);
        break;
    case GREEN_H:
        glColor3f(0,1,0);
        break;
    case BLUE_H:
        glColor3f(0,0,1);
        break;
    default:
        glColor3f(1,1,1);
        break;
    }
    float lineWidth = b->width/256.;
    float lineHeight = b->height/histogramme->max;
    for(i=0;i<256;i++){
        drawCarre(b->x+i*lineWidth,b->y,b->x+(i+1)*lineWidth,b->y+lineHeight*histogramme->nbPixelsPerValues[i]);
    }
}

void addBlend(Layer *out, const Layer *layerbelow, const Layer *layerabove){
    out->height = layerbelow->height;
    out->width = layerbelow->width;
    out->melange = addBlend;
    out->opacity = 1;
    out->luts = NULL;
    unsigned int ibelow,iabove;
    for(ibelow=layerabove->y*layerabove->width+layerabove->x, iabove=0;
        iabove<(layerabove->width * layerabove->height*3); iabove++, ibelow++) {
        out->rgb[iabove] = max(0,(min(255,((layerbelow->rgb[iabove] + layerabove->rgb[iabove] * layerabove->opacity)))));
    }

    makeRedHistogrammeFromLayer(out->histogrammes + RED_H,
                                out);
    makeBlueHistogrammeFromLayer(out->histogrammes + BLUE_H,
                                out);
    makeGreenHistogrammeFromLayer(out->histogrammes + GREEN_H,
                                out);
    makeGrayHistogrammeFromLayer(out->histogrammes + GRAYSCALE_H,
                                out);
}

void normalBlend(Layer *out, const Layer *layerbelow, const Layer *layerabove){
    out->height = layerbelow->height;
    out->width = layerbelow->width;
    out->melange = normalBlend;
    out->opacity = 1;
    out->luts = NULL;
    unsigned int ibelow,iabove;
    for(ibelow=layerabove->y*layerabove->width+layerabove->x, iabove=0;
        iabove<(layerabove->width * layerabove->height*3); iabove++, ibelow++) {
        out->rgb[ibelow] = max(0,(min(255,((layerbelow->rgb[iabove] + layerabove->rgb[iabove] * layerabove->opacity)
                                      /(1.+layerabove->opacity)))));
    }

    makeRedHistogrammeFromLayer(out->histogrammes + RED_H,
                                out);
    makeBlueHistogrammeFromLayer(out->histogrammes + BLUE_H,
                                out);
    makeGreenHistogrammeFromLayer(out->histogrammes + GREEN_H,
                                out);
    makeGrayHistogrammeFromLayer(out->histogrammes + GRAYSCALE_H,
                                out);
}

void mulBlend(Layer *out, const Layer *layerbelow, const Layer *layerabove)
{
    out->height = layerbelow->height;
    out->width = layerbelow->width;
    out->melange = normalBlend;
    out->opacity = 1;
    out->luts = NULL;
    unsigned xStart = max(0,layerabove->x), yStart = max(0,layerabove->y),
            xEnd = (min((layerabove->width*3+layerabove->x),(layerbelow->width*3))),
            yEnd = min((layerabove->height+layerabove->y),(layerbelow->height));
    unsigned int ibelow,iabove,jabove,cptabove,linebegin;
    for(jabove=yStart;jabove<yEnd;jabove++){
        linebegin = jabove*layerbelow->width*3;
        ibelow =  linebegin + layerabove->x;
        cptabove=(jabove-yStart)*layerabove->width*3;
        for(iabove=xStart;iabove<xEnd;iabove++){
                out->rgb[ibelow] = max(0,(min(255,((layerbelow->rgb[ibelow] *
                                            (1.f - layerabove->opacity) + layerabove->rgb[cptabove] * layerabove->opacity)))));
            ibelow++;
            cptabove++;
        }
    }

    makeRedHistogrammeFromLayer(out->histogrammes + RED_H,
                                out);
    makeBlueHistogrammeFromLayer(out->histogrammes + BLUE_H,
                                out);
    makeGreenHistogrammeFromLayer(out->histogrammes + GREEN_H,
                                out);
    makeGrayHistogrammeFromLayer(out->histogrammes + GRAYSCALE_H,
                                out);
}

void makeLUT(LUT* lut, EFFECT e, float amount){
    lut->effectAmount = amount;
    lut->LUTEffect = e;
    int i;
    switch (e) {
    case CONTRASTPLUS:
        lut->values[0] = 0;
        lut->values[127] = 127;
        lut->values[255] = 255;
        for(i=1;i<127;i++)
            lut->values[i] = max(0,i-126*amount);
        for(i=128;i<255;i++)
            lut->values[i] = min(255,i+126*amount);
        break;
    case CONTRASTMINUS:
        lut->values[127] = 127;
        for(i=0;i<127;i++)
            lut->values[i] = min(127,i+126*amount);
        for(i=128;i<256;i++)
            lut->values[i] = max(127,i-126*amount);
        break;
    default:
        for(i=0;i<256;i++)
            lut->values[i] = i;
        break;
    }
}

void combineLUT(LUT *out, LUT *lut1, LUT *lut2){
    out->effectAmount = 1;
    out->LUTEffect = MULTIPLE;
    int i;
    switch (lut2->LUTEffect) {
    case CONTRASTPLUS:
        for(i=1;i<127;i++)
            out->values[i] = max(0,lut1->values[i]-126*lut2->LUTEffect);
        for(i=128;i<255;i++)
            out->values[i] = min(255,lut1->values[i]+126*lut2->LUTEffect);
        break;
    case CONTRASTMINUS:
        for(i=0;i<127;i++)
            out->values[i] = min(127,lut1->values[i]+126*lut2->LUTEffect);
        for(i=128;i<256;i++)
            out->values[i] = max(127,lut1->values[i]-126*lut2->LUTEffect);
        break;
    default:
        for(i=0;i<256;i++)
            out->values[i] = lut1->values[i];
        break;
    }
}

void makeEmptyPicture(Picture* p, unsigned int width, unsigned int height){
    makeEmptyLayer(&p->blank,width,height,0,0);
    p->currentID = 0;
    p->nbLayers = 1;
    p->layers = *make_LayersList(&p->blank);
    p->lastlayer = &p->layers;
    p->current = &p->layers;
    updateCfLayer(p);
}

void addNewEmptyLayer(Picture* p){
    Layer* empty = malloc(sizeof(Layer));
    makeEmptyLayer(empty,p->blank.width,p->blank.height,0,0);
    LayersList_insertAfter(p->lastlayer,empty);
    p->lastlayer = p->lastlayer->next;
    p->current = p->lastlayer;
    p->currentID = p->nbLayers;
    p->nbLayers++;

}

void addNewLayer(Picture* p, unsigned char* rgbSrc, int width, int height){
    Layer* empty = malloc(sizeof(Layer));
    makeLayer(empty,rgbSrc,width,height,0,0);
    LayersList_insertAfter(p->lastlayer,empty);
    p->lastlayer = p->lastlayer->next;
    p->current = p->lastlayer;
    p->currentID = p->nbLayers;
    p->nbLayers++;
}

void makeCfPicture(Layer *lf, LayersList *layers){
    LayersList* current = layers;
    for(current = layers; current!=NULL;current=current->next){
        (*current->element->melange)(lf,lf,current->element);
    }
}

void updateCfLayer(Picture* p){
    makeEmptyLayer(&p->Cf,p->blank.width,p->blank.height,0,0);
    makeCfPicture(&p->Cf,p->layers.next);
}

void removeCurrentLayer(Picture *p){
    LayersList* tmp = NULL;
    if(p->current != &p->layers){
        if(p->current->previous!=NULL){
            p->current->previous->next = p->current->next;
            tmp = p->current->previous;
            p->currentID --;
            if(p->current->next!=NULL)
                p->current->next->previous = p->current->previous;
        }
        if(p->current->next!=NULL){
            p->current->next->previous = p->current->previous;
            tmp = p->current->next;
        }
        freeLayer(&p->current->element);
        free(p->current);
        p->nbLayers--;
        p->current = tmp;
    }
}

Bounds layerBoundsInWindow(const Layer *l, unsigned int screen_width, unsigned int screen_height){
    return makeBounds((float)l->x/(float)screen_width,(float)l->y/(float)screen_height,
                      (float)l->width/(float)screen_width,(float)l->height/(float)screen_height);
}

void freeLayer(Layer **l){
    free((*l)->rgb);
    free(*l);
    *l = NULL;
}
