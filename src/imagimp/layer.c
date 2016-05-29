#include <string.h>
#include <math.h>
#include "layer.h"
#include "outils.h"
#define histoRed histogramme[RED_H]->nbPixelsPerValues
#define histoBlue histogramme[BLUE_H]->nbPixelsPerValues
#define histoGreen histogramme[GREEN_H]->nbPixelsPerValues
#define histoGray histogramme[GRAYSCALE_H]->nbPixelsPerValues

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

void makeLayer(Layer* l, const unsigned char* rgb, unsigned int width, unsigned int height,
			   int x, int y)
{
	unsigned long size = height*width*3;
	l->rgb = malloc(size);
	memcpy(l->rgb,rgb,size);
	l->height = height;
	l->width = width;
	l->x = x;
	l->y = y;
	l->blendMode = DEFAULTBLEND;
	l->opacity = 1;
	l->luts = NULL;
	updateHistogramme(l);
}

void copyLayer(Layer *l, const Layer *layerToCopy){
	unsigned size = layerToCopy->height*layerToCopy->width*3;
	if(l->rgb != NULL)
		free(l->rgb);
	l->rgb = malloc(size);
	memcpy(l->rgb,layerToCopy->rgb,size);
	l->height = layerToCopy->height;
	l->width = layerToCopy->width;
	l->x = layerToCopy->x;
	l->y = layerToCopy->y;
	l->blendMode = layerToCopy->blendMode;
	l->opacity = layerToCopy->opacity;
	int i;
	for(i=0;i<256;i++){
		l->histogrammes[RED_H].nbPixelsPerValues[i] = layerToCopy->histogrammes[RED_H].nbPixelsPerValues[i];
		l->histogrammes[BLUE_H].nbPixelsPerValues[i] = layerToCopy->histogrammes[BLUE_H].nbPixelsPerValues[i];
		l->histogrammes[GREEN_H].nbPixelsPerValues[i] = layerToCopy->histogrammes[GREEN_H].nbPixelsPerValues[i];
		l->histogrammes[GRAYSCALE_H].nbPixelsPerValues[i] = layerToCopy->histogrammes[GRAYSCALE_H].nbPixelsPerValues[i];
	}
	l->histogrammeUpdated = 1;
}

void makeEmptyLayer(Layer* l, unsigned int width, unsigned int height, int x, int y){
	unsigned long size = height*width*3;
	if(l->rgb!=NULL) free(l->rgb);
	l->rgb = malloc(height*width*3);
	int i;
	for(i=0;i<size;i++)
		l->rgb[i] = 255;
	l->height = height;
	l->width = width;
	l->x = x;
	l->y = y;
	l->blendMode = DEFAULTBLEND;
	l->opacity = 1;
	l->luts = NULL;
	for(i=0;i<255;i++){
		l->histogrammes[RED_H].nbPixelsPerValues[i] = 0;
		l->histogrammes[BLUE_H].nbPixelsPerValues[i] = 0;
		l->histogrammes[GREEN_H].nbPixelsPerValues[i] = 0;
		l->histogrammes[GRAYSCALE_H].nbPixelsPerValues[i] = 0;
	}
	size /=3;
	l->histogrammes[RED_H].nbPixelsPerValues[i] = size;
	l->histogrammes[BLUE_H].nbPixelsPerValues[i] = size;
	l->histogrammes[GREEN_H].nbPixelsPerValues[i] = size;
	l->histogrammes[GRAYSCALE_H].nbPixelsPerValues[i] = size;
	l->histogrammeUpdated = 1;
}

void setBlendMode(Layer *l, BlendMode mode){
	l->blendMode = mode;
}

void makeGrayHistogrammeFromLayer(Histogramme *histogramme, const Layer *layer){
	unsigned int i;
	memset(histogramme, 0, sizeof(Histogramme));
	histogramme->max=0;
	histogramme->min=(layer->width * layer->height);
	for(i=0 ; i<histogramme->min - 2; i+=3) {
		histogramme->nbPixelsPerValues[(unsigned char)((float)layer->rgb[i] * REDCOEF +
														(float)layer->rgb[i+1] * GREENCOEF +
														(float)layer->rgb[i+2] * BLUECOEF)]++;
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
	for(i=0 ; i<(histogramme->min*3); i+=3) {
		histogramme->nbPixelsPerValues[layer->rgb[i]]++;
	}
	unsigned long somme = 0;
	for(i=0; i<256;i++)
	{
		somme += histogramme->nbPixelsPerValues[i];
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
	for(i=1 ; i<(histogramme->min*3); i+=3) {
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
	for(i=2 ; i<(histogramme->min*3); i+=3) {
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

void updateHistogramme(Layer* l){
	makeRedHistogrammeFromLayer(l->histogrammes + RED_H,
								l);
	makeBlueHistogrammeFromLayer(l->histogrammes + BLUE_H,
								 l);
	makeGreenHistogrammeFromLayer(l->histogrammes + GREEN_H,
								  l);
	makeGrayHistogrammeFromLayer(l->histogrammes + GRAYSCALE_H,
								 l);
	l->histogrammeUpdated = 1;
}

void drawHistorgramme(const Histogramme *histogramme, const Bounds* b){
	int i;
	float lineWidth = b->width/256.;
	float lineHeight = b->height/histogramme->max;
	for(i=0;i<256;i++){
		drawCarre(b->x+(float)i*lineWidth,
				  b->y,b->x+((float)i+1.)*lineWidth,
				  b->y+lineHeight*(float)histogramme->nbPixelsPerValues[i]);
	}
}
