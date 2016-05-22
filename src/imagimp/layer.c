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

IMPLEMENTE_LISTE(LUTsList, LUT)
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
               int x, int y){
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
        makeRedHistogrammeFromLayer(l->histogrammes + RED_H,
                                    l);
        makeBlueHistogrammeFromLayer(l->histogrammes + BLUE_H,
                                    l);
        makeGreenHistogrammeFromLayer(l->histogrammes + GREEN_H,
                                    l);
        makeGrayHistogrammeFromLayer(l->histogrammes + GRAYSCALE_H,
                                    l);
    }    unsigned long size = height*width*3;
    l->rgb = malloc(size);
    memcpy(l->rgb,rgb,size);
    l->height = height;
    l->width = width;
    l->x = x;
    l->y = y;
    l->blendMode = DEFAULTBLEND;
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
    l->luts = layerToCopy->luts;
    int i;
    for(i=0;i<256;i++){
        l->histogrammes[RED_H].nbPixelsPerValues[i] = layerToCopy->histogrammes[RED_H].nbPixelsPerValues[i];
        l->histogrammes[BLUE_H].nbPixelsPerValues[i] = layerToCopy->histogrammes[BLUE_H].nbPixelsPerValues[i];
        l->histogrammes[GREEN_H].nbPixelsPerValues[i] = layerToCopy->histogrammes[GREEN_H].nbPixelsPerValues[i];
        l->histogrammes[GRAYSCALE_H].nbPixelsPerValues[i] = layerToCopy->histogrammes[GRAYSCALE_H].nbPixelsPerValues[i];
    }
}

void copyLayerInRect(Layer *l, const Layer *layerToCopy, int x, int y,
                     int width, int height){
    unsigned int   labovePpW = layerToCopy->width*3,
                    lbelowPpW = l->width*3;
    unsigned int   xStartB, xStartA,
                    yStartB, yStartA,
                    xEndB, yEndB;
    if(layerToCopy->x<0){
        xStartB = max(0,(x * 3));
        xStartA = layerToCopy->x * -3+xStartB;
    }
    else{
        xStartB = max((layerToCopy->x * 3),(x * 3));
        xStartA = xStartB-layerToCopy->x * 3;
    }
    xEndB = (min((layerToCopy->x * 3+labovePpW), x*3+width*3));
    if(layerToCopy->y<0){
        yStartB = max(0,y);
        yStartA = -layerToCopy->y+yStartB;
    }
    else{
        yStartB = max(layerToCopy->y,y);
        yStartA = yStartB-layerToCopy->y;
    }
    yEndB = min((layerToCopy->y+layerToCopy->height), y+height);

    unsigned int ibelow,lineIncA,lineIncB,iabove,endLine,offsetX,offsetY;
    ibelow  = yStartB*lbelowPpW + xStartB;
    iabove  = yStartA*labovePpW + xStartA;
    offsetX = 1;
    offsetY = lbelowPpW;
    lineIncA= labovePpW - (xStartA + (xEndB - xStartB)) + xStartA;
    lineIncB= offsetY - xEndB + xStartB;
    endLine = ibelow + (xEndB-xStartB)- offsetX;
    while(endLine<yEndB*lbelowPpW){
        while(ibelow<endLine){
            l->rgb[ibelow] = layerToCopy->rgb[iabove];
            iabove ++;
            ibelow ++;
        }

        iabove  += lineIncA;
        ibelow  += lineIncB;
        endLine += offsetY;
    }
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

void setBlendMode(Layer *l, BlendMode mode){
    l->blendMode = mode;
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

void updateHistogramme(Layer* l){
    makeRedHistogrammeFromLayer(l->histogrammes + RED_H,
                                l);
    makeBlueHistogrammeFromLayer(l->histogrammes + BLUE_H,
                                l);
    makeGreenHistogrammeFromLayer(l->histogrammes + GREEN_H,
                                l);
    makeGrayHistogrammeFromLayer(l->histogrammes + GRAYSCALE_H,
                                l);
}

typedef unsigned char (*BlendPixelFunc)(unsigned char, float, unsigned char, float, void* parameters);

BlendPixelFunc functionFromBlendMode(BlendMode bm, int inverseAction){
    if(inverseAction){
        switch (bm) {
        case BLEND_ADD:
            return subBlend;
        case BLEND_SUB:
            return addBlend;
        case BLEND_MULT:
            return divBlend;
        case BLEND_DIV:
            return multBlend;
        case BLEND_MOY:
            return inversedMoyBlend;
        default:
            break;
        }
    }
    else{
        switch (bm) {
        case BLEND_ADD:
            return addBlend;
        case BLEND_SUB:
            return subBlend;
        case BLEND_MULT:
            return multBlend;
        case BLEND_DIV:
            return divBlend;
        case BLEND_MOY:
            return moyBlend;
        default:
            break;
        }
    }
    return multBlend;
}


void blendTwoLayer(Layer *lBelow, const Layer *lAbove, int inverseAction, int pixelsize){

    unsigned int   labovePpW = lAbove->width*3,
                    lbelowPpW = lBelow->width*3;
    unsigned int   xStartB, xStartA,
                    yStartB, yStartA,
                    xEndB, yEndB;

    if(lAbove->x<0){
        xStartB = 0;
        xStartA = lAbove->x * -3;
    }
    else{
        xStartB = lAbove->x * 3;
        xStartA = 0;
    }
    xEndB = (min((lAbove->x * 3+labovePpW), lbelowPpW));
    if(lAbove->y<0){
        yStartB = 0;
        yStartA = -lAbove->y;
    }
    else{
        yStartB = lAbove->y;
        yStartA = 0;
    }
    yEndB = min((lAbove->y+lAbove->height), lBelow->height);

    unsigned int ibelow,lineIncA,lineIncB,iabove,cpt,endLine,offsetX,offsetY;
    ibelow  = yStartB*lbelowPpW + xStartB;
    iabove  = yStartA*labovePpW + xStartA;
    offsetX = pixelsize*3;
    offsetY = pixelsize*lbelowPpW;
    lineIncA= labovePpW*pixelsize - (xStartA + (xEndB - xStartB)) + xStartA;
    lineIncB= offsetY - xEndB + xStartB;
    endLine = ibelow + (xEndB-xStartB) - offsetX;

    BlendPixelFunc blendFunc = functionFromBlendMode(lAbove->blendMode,inverseAction);
    void* parameters = NULL;
    if(lAbove->blendMode==BLEND_MOY){
        parameters = malloc(sizeof(float));
        *((float*)parameters) =  lBelow->opacity+lAbove->opacity;
    }

	LUT* finalLUT = malloc(sizeof(LUT));
	combineAllLUT(finalLUT,lAbove->luts);


    unsigned int i,j;
    unsigned char p;
    while(endLine<=(yEndB-pixelsize)*lbelowPpW){
        while(ibelow<endLine){
            for(cpt=0;cpt<3;cpt++){
                p =(*blendFunc)(lBelow->rgb[ibelow+cpt],lBelow->opacity,
											  finalLUT->values[lAbove->rgb[iabove+cpt]],lAbove->opacity,
                                              parameters);
                for(i=ibelow+cpt;i<ibelow+offsetX;i+=3)
                    for(j=i;j<i+offsetY;j+=lbelowPpW)
                        lBelow->rgb[j] = p;
            }
            iabove +=offsetX;
            ibelow +=offsetX;
        }

        while(ibelow<endLine+offsetX){
            lBelow->rgb[ibelow] = (*blendFunc)(lBelow->rgb[ibelow],lBelow->opacity,
                                                   lAbove->rgb[iabove],lAbove->opacity,
                                                   parameters);
            iabove ++;
            ibelow ++;
        }

        iabove  += lineIncA;
        ibelow  += lineIncB;
        endLine += offsetY;
    }
    /*while(ibelow<yEndB*lbelowPpW){
        lBelow->rgb[ibelow] = (*blendFunc)(lBelow->rgb[ibelow],lBelow->opacity,
                                               lAbove->rgb[iabove],lAbove->opacity,
                                               parameters);
        iabove ++;
        ibelow ++;
    }*/
	free(finalLUT);

}

void blendTwoLayerInRect(Layer *lBelow, const Layer *lAbove, int inverseAction, int pixelsize, int x, int y, int width, int height)
{
    unsigned int   labovePpW = lAbove->width*3,
                    lbelowPpW = lBelow->width*3;
    unsigned int   xStartB, xStartA,
                    yStartB, yStartA,
                    xEndB, yEndB;
	if(x < 0)
		x = 0;
	if(x+width>lBelow->width)
		width = lBelow->width-x;
	if(y<0)
		y = 0;
	if(y+height>lBelow->height)
		height = lBelow->height-y;

    if(lAbove->x<0){
        xStartB = max(0,(x*3));
        xStartA = lAbove->x * -3+xStartB;
    }
    else{
        xStartB = max((lAbove->x * 3),(x*3));
        xStartA = xStartB-lAbove->x * 3;
    }
    xEndB = (min((lAbove->x * 3+labovePpW), x*3+width*3));
    if(lAbove->y<0){
        yStartB = max(0,y);
        yStartA = -lAbove->y+yStartB;
    }
    else{
        yStartB = max(lAbove->y,y);
        yStartA = yStartB-lAbove->y;
    }
    yEndB = min((lAbove->y+lAbove->height), y+height);

    unsigned int ibelow,lineIncA,lineIncB,iabove,cpt,endLine,offsetX,offsetY;
    ibelow  = yStartB*lbelowPpW + xStartB;
    printf("ibelow:%u\n",ibelow);
    iabove  = yStartA*labovePpW + xStartA;
    offsetX = pixelsize*3;
    offsetY = pixelsize*lbelowPpW;
    lineIncA= labovePpW*pixelsize - (xStartA + (xEndB - xStartB)) + xStartA;
    lineIncB= offsetY - xEndB + xStartB;
    endLine = ibelow + (xEndB-xStartB) - offsetX;

    BlendPixelFunc blendFunc = functionFromBlendMode(lAbove->blendMode,inverseAction);
    void* parameters = NULL;
    if(lAbove->blendMode==BLEND_MOY){
        parameters = malloc(sizeof(float));
        *((float*)parameters) =  lBelow->opacity+lAbove->opacity;
    }

	LUT* finalLUT = malloc(sizeof(LUT));
	combineAllLUT(finalLUT,lAbove->luts);

    unsigned i,j;
    while(endLine<=(yEndB-pixelsize)*lbelowPpW){
        while(ibelow<endLine){
            for(cpt=0;cpt<3;cpt++){
                unsigned char p =(*blendFunc)(lBelow->rgb[ibelow+cpt],lBelow->opacity,
											  finalLUT->values[lAbove->rgb[iabove+cpt]],lAbove->opacity,
                                              parameters);
                for(i=ibelow+cpt;i<ibelow+offsetX;i+=3)
                    for(j=i;j<i+offsetY;j+=lbelowPpW)
                        lBelow->rgb[j] = p;

            }
            iabove +=offsetX;
            ibelow +=offsetX;
        }
        while(ibelow<endLine+offsetX){
                lBelow->rgb[ibelow] = (*blendFunc)(lBelow->rgb[ibelow],lBelow->opacity,
                                                   lAbove->rgb[iabove],lAbove->opacity,
                                                   parameters);
            iabove ++;
            ibelow ++;
        }

        iabove  += lineIncA;
        ibelow  += lineIncB;
        endLine += offsetY;
    }
	free(finalLUT);
}

unsigned char addBlend(unsigned char pixelBelow, float opacityBelow,
              unsigned char pixelAbove, float opacityAbove, void* parameters){
    return min(255,(pixelBelow + pixelAbove*opacityAbove));
}

unsigned char moyBlend(unsigned char pixelBelow, float opacityBelow,
                 unsigned char pixelAbove, float opacityAbove, void* parameters){
    return (pixelBelow*opacityBelow + pixelAbove*opacityAbove)/(*((float*)parameters));
}

unsigned char multBlend(unsigned char pixelBelow, float opacityBelow,
                       unsigned char pixelAbove, float opacityAbove, void* parameters)
{
    return pixelBelow + opacityAbove*(pixelAbove - pixelBelow);//pixelBelow*(1.-opacityAbove) + pixelAbove*opacityAbove;
}

unsigned char inversedMoyBlend(unsigned char pixelBelow, float opacityBelow,
                                   unsigned char pixelAbove, float opacityAbove, void* parameters){
    return (pixelBelow*(*((float*)parameters)) - pixelAbove*opacityAbove)/opacityBelow;
}

unsigned char subBlend(unsigned char pixelBelow, float opacityBelow,
                        unsigned char pixelAbove, float opacityAbove, void* parameters){
    if(pixelBelow<=pixelAbove*opacityAbove) return 0;
    else return pixelBelow - pixelAbove*opacityAbove;
}

unsigned char divBlend(unsigned char pixelBelow, float opacityBelow,
                        unsigned char pixelAbove, float opacityAbove,void* parameters){
    if(pixelBelow<=pixelAbove*opacityAbove) return 0;
    else return (pixelBelow-pixelAbove*opacityAbove)/(1.-opacityAbove);
}

void addLUTToLayer(LUT *lut, Layer *l){
	LUTsList_insert(&l->luts,lut);
}

void setLUT(LUT *lut, Effect e, float amount){
	lut->effectAmount = amount;
	lut->LUTEffect = e;
	lut->activ = 1;
	int i;
	switch (e) {
	case CONTRAST:
		lut->values[127] = 127;
		for(i=0;i<127;i++)
			lut->values[i] = min((max(0,(i-126*amount))),127);
		for(i=128;i<256;i++)
			lut->values[i] = max((min(255,(i+126*amount))),127);
		break;

	/*case BRIGHTNESSPLUS:
		for(i=0;i<256;i++)
		lut->values[i] = i+amount;
		break;

	case BRIGHTNESSMINUS:
		for(i=0;i<256;i++)
		lut->values[i] = i- amount;
		break;

	case SATURATIONPLUS:




	case SATURATIONMINUS:*/




	default:
		for(i=0;i<256;i++)
			lut->values[i] = i;
		break;
	}
}

LUT *makeLUT(Effect e, float amount){
	LUT* lut = malloc(sizeof(LUT));
	setLUT(lut,e,amount);
	return lut;
}

void combineLUT(unsigned char values[256], LUT *lut1, LUT *lut2){
	if(lut2->activ){
		int i;
		for(i=0;i<256;i++)
			values[i] = lut2->values[lut1->values[i]];
	}
}

void combineAllLUT(LUT* out, LUTsList* luts){
	int i = 0;
	for(;i<256;i++)
		out->values[i] = i;
	out->activ = 1;
	out->effectAmount = 1;
	out->LUTEffect = MULTIPLE;
	while(luts!=NULL){
		combineLUT(out->values, out, luts->element);
		luts = luts->next;
	}
}

void makeEmptyPicture(Picture* p, unsigned int width, unsigned int height){
    p->blank.rgb = p->beforeCf.rgb = p->Cf.rgb = NULL;
    makeEmptyLayer(&p->blank,width,height,0,0);
    copyLayer(&p->beforeCf,&p->blank);
    p->currentID = 0;
    p->nbLayers = 1;
    p->layers = *make_LayersList(&p->blank);
    p->lastlayer = &p->layers;
    p->current = &p->layers;
    updateCfLayer(p,1);
}

void addNewEmptyLayer(Picture* p){
    Layer* empty = malloc(sizeof(Layer));
    makeEmptyLayer(empty,p->blank.width,p->blank.height,0,0);
    LayersList_insertAfter(p->lastlayer,empty);
    p->lastlayer = p->lastlayer->next;
    p->nbLayers++;
    changeCurrentTo(p,p->nbLayers-1);
}



void addNewLayer(Picture* p, unsigned char* rgbSrc, int width, int height){
    Layer* empty = malloc(sizeof(Layer));
    makeLayer(empty,rgbSrc,width,height,0,0);
    LayersList_insertAfter(p->lastlayer,empty);
    p->lastlayer = p->lastlayer->next;
    p->nbLayers++;
    changeCurrentTo(p,p->nbLayers-1);
}

void makeCfPicture(Layer *lf, LayersList *layers, int pixelsize){
    LayersList* current = layers;
    for(current = layers; current!=NULL;current=current->next){
        blendTwoLayer(lf,current->element,0, pixelsize);
    }
}

void updateCfLayer(Picture* p, int pixelsize){
    copyLayer(&p->Cf,&p->beforeCf);
    makeCfPicture(&p->Cf,p->current, pixelsize);
}

void putCurrentLayerFront(Picture *p){
    if(p->current->previous!=NULL)
    {
        if(p->current->next!=NULL){
            int x = min(p->current->element->x,p->current->next->element->x),
                y = min(p->current->element->y,p->current->next->element->y),
                x2 = max((p->current->element->x + p->current->element->width),
                         (p->current->next->element->x + p->current->next->element->width)),
                y2 = max((p->current->element->y + p->current->element->height),
                         (p->current->next->element->y + p->current->next->element->height));
            int width = x2 - x, height = y2 -y;
			LayersList* next = NULL;
            if(p->current->next!=NULL){
				next = p->current->next->next;
                p->current->next->previous = p->current->previous;
				if(p->current->next->next){
					p->current->next->next->previous = p->current;
				}
				p->current->next->next = p->current;
            }
            if(p->current->previous!=NULL){
                p->current->previous->next = p->current->next;
            }
            p->current->previous = p->current->next;
			p->current->next = next;
            p->currentID++;
            copyLayerInRect(&p->beforeCf,&p->blank,x,y,width,height);

            int i = 1;
            LayersList* l;
            for(l = p->layers.next;i<p->currentID;l=l->next,i++){
                blendTwoLayerInRect(&p->beforeCf,l->element,0,1,x,y,width,height);
            }
        }
    }
}

void putCurrentLayerBehind(Picture *p){
    if(p->current->previous!=NULL)
    {
        if(p->current->previous->previous!=NULL){
            int  x = min(p->current->element->x,p->current->previous->element->x),
                 y = min(p->current->element->y,p->current->previous->element->y),
                 x2 = max((p->current->element->x + p->current->element->width),
                         (p->current->previous->element->x + p->current->previous->element->width)),
                 y2 = max((p->current->element->y + p->current->element->height),
                         (p->current->previous->element->y + p->current->previous->element->height));
            int width = x2 - x, height = y2 -y;
			LayersList* previous = NULL;
			if(p->current->previous!=NULL){
				previous = p->current->previous->previous;
                p->current->previous->next = p->current->next;
				if(p->current->previous->previous!=NULL){
					p->current->previous->previous->next = p->current;
				}
				p->current->previous->previous = p->current;
            }
            if(p->current->next!=NULL){
                p->current->next->previous = p->current->previous;
            }
			p->current->next = p->current->previous;
			p->current->previous = previous;
            p->currentID--;
            copyLayerInRect(&p->beforeCf,&p->blank,x,y,width,height);

            int i = 1;
            LayersList* l;
            for(l = p->layers.next;i<p->currentID;l=l->next,i++){
                blendTwoLayerInRect(&p->beforeCf,l->element,0,1,x,y,width,height);
            }
        }
    }
}

void removeCurrentLayer(Picture *p){
    if(p->current->previous!=NULL){
        LayersList* tmp = p->current;
        p->current->previous->next = p->current->next;
        if(p->current->next)
            p->current->next->previous = p->current->previous;
        if(p->current == p->lastlayer)
            p->lastlayer = p->current->previous;
        changeCurrentToBelowLayer(p);
        freeLayer(&tmp->element);
        free(tmp);
        p->nbLayers--;
    }
}

void changeCurrentTo(Picture* p, int indice){
    if(indice<=0 || indice>=p->nbLayers || p->currentID==indice) return;
    if(indice==0)
    {
        p->current = &p->layers;
        p->currentID = 0;
        copyLayer(&p->beforeCf,&p->blank);
    }
    else if(indice==1){
        p->current = p->layers.next;
        p->currentID = 1;
        copyLayer(&p->beforeCf,&p->blank);
    }
    else if(p->currentID<indice){
        for(;p->currentID<indice;p->currentID++){
            blendTwoLayer(&p->beforeCf,p->current->element,0,1);
            p->current = p->current->next;
        }
    }
    else{
        for(;p->currentID>indice;p->currentID--){
            blendTwoLayer(&p->beforeCf,p->current->element,1,1);
            p->current = p->current->previous;
        }
    }
}

void changeCurrentToAboveLayer(Picture* p){
    if(p->current->next!=NULL){
        blendTwoLayer(&p->beforeCf,p->current->element,0,1);
        p->current = p->current->next;
        p->currentID++;
    }
}

void changeCurrentToBelowLayer(Picture* p){
    if(p->current->previous!=NULL){
        p->current = p->current->previous;
        p->currentID--;
        copyLayerInRect(&p->beforeCf,&p->blank, p->current->element->x,
                                                p->current->element->y,
                                                p->current->element->width,
                                                p->current->element->height);
        LayersList* l = p->layers.next;
        int i=1;
        for(;i<p->currentID;i++,l=l->next){
            blendTwoLayerInRect(&p->beforeCf,l->element,0,1,p->current->element->x,
                                                            p->current->element->y,
                                                            p->current->element->width,
                                                            p->current->element->height);
        }
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

void translateCurrentLayer(Picture *p, int tx, int ty){
    if(p->current->previous!=NULL){
        p->current->element->x += tx;
        p->current->element->y += ty;
    }
}

int pictureIsEmpty(const Picture* p){
    return p->current->previous==NULL;
}
