#include "blend.h"
#include "outils.h"
#include "layer.h"

IMPLEMENTE_LISTE(LUTsList, LUT)

typedef struct BlendFrame{
	unsigned long	labovePpW, lbelowPpW;
	unsigned long	lineIncA,lineIncB,
					endLine,
					offsetX,offsetY;
	unsigned long iAboveStart, iBelowStart;
	unsigned long iBelowEnd;
} BlendFrame;

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
	if(pixelBelow*(1.-opacityAbove)<= pixelAbove*opacityAbove) return 0;
	return pixelBelow*(1.-opacityAbove) - pixelAbove*opacityAbove;
}

unsigned char darkBlend(unsigned char pixelBelow, float opacityBelow,
			  unsigned char pixelAbove, float opacityAbove,
			  void* parameters){
	return min(pixelBelow,pixelAbove*opacityAbove);
}

unsigned char lightBlend(unsigned char pixelBelow, float opacityBelow,
			  unsigned char pixelAbove, float opacityAbove,
			  void* parameters){
	return max(pixelBelow,pixelAbove*opacityAbove);
}

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
		case BLEND_DARK:
			return lightBlend;
		case BLEND_LIGHT:
			return darkBlend;
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
		case BLEND_DARK:
			return darkBlend;
		case BLEND_LIGHT:
			return lightBlend;
		default:
			break;
		}
	}
	return multBlend;
}

void processBlendAtCurrentBloc(Blend* blend, const BlendFrame* frame){
	int cpt;
	unsigned char p;
	unsigned char gray;
	unsigned char *pIBelow, *pJBelow;
	if(blend->lut->fromGray){
		gray = (float)(*blend->pAbove) * REDCOEF + (float)(*blend->pAbove+1) * GREENCOEF + (float)(*blend->pAbove+2) * BLUECOEF;
		for(cpt=0;cpt<3;cpt++){
			p =(*blend->blendFunc)(*(blend->pBelow+cpt),blend->opacityBelow,
								blend->lut->values[cpt][gray],blend->opacityAbove,blend->parameters);
			for(pIBelow = blend->pBelow+cpt;pIBelow<blend->pBelow + frame->offsetX;pIBelow+=3)
				for(pJBelow=pIBelow;pJBelow<pIBelow+frame->offsetY;pJBelow+=frame->lbelowPpW)
					(*pJBelow) = p;

		}
	}
	else{
		for(cpt=0;cpt<3;cpt++){
			p =(*blend->blendFunc)(*(blend->pBelow+cpt),blend->opacityBelow,
								blend->lut->values[cpt][*(blend->pAbove+cpt)],blend->opacityAbove,blend->parameters);
			for(pIBelow = blend->pBelow+cpt;pIBelow<blend->pBelow + frame->offsetX;pIBelow+=3)
				for(pJBelow=pIBelow;pJBelow<pIBelow+frame->offsetY;pJBelow+=frame->lbelowPpW)
					(*pJBelow) = p;

		}
	}
}

void processBlendAtCurrentPixel(Blend *blend){
	int cpt;
	unsigned char gray;
	if(blend->lut->fromGray){
		gray = (float)(*blend->pAbove) * REDCOEF + (float)(*blend->pAbove+1) * GREENCOEF + (float)(*blend->pAbove+2) * BLUECOEF;
		for(cpt=0;cpt<3;cpt++){
			(*(blend->pBelow+cpt)) =(*blend->blendFunc)(*(blend->pBelow+cpt),blend->opacityBelow,
								blend->lut->values[cpt][gray],blend->opacityAbove,blend->parameters);
		}
	}
	else{
		for(cpt=0;cpt<3;cpt++){
			(*(blend->pBelow+cpt))  =(*blend->blendFunc)(*(blend->pBelow+cpt),blend->opacityBelow,
								blend->lut->values[cpt][*(blend->pAbove+cpt)],blend->opacityAbove,blend->parameters);
		}
	}
}

void initBlendFrame(BlendFrame* blendF, Layer *lBelow, const Layer *lAbove, int pixelsize){
	unsigned long	xStartB, xStartA,
					yStartB, yStartA,
					xEndB, yEndB;
	blendF->labovePpW = lAbove->width*3;
	blendF->lbelowPpW = lBelow->width*3;
	if(lAbove->x<0){
		xStartB = 0;
		xStartA = lAbove->x * -3;
	}
	else{
		xStartB = lAbove->x * 3;
		xStartA = 0;
	}
	xEndB = (min((lAbove->x * 3+blendF->labovePpW), blendF->lbelowPpW));
	if(lAbove->y<0){
		yStartB = 0;
		yStartA = -lAbove->y;
	}
	else{
		yStartB = lAbove->y;
		yStartA = 0;
	}
	yEndB = min((lAbove->y+lAbove->height), lBelow->height);

	blendF->iBelowStart  = yStartB*blendF->lbelowPpW + xStartB;
	blendF->iAboveStart  = yStartA*blendF->labovePpW + xStartA;
	blendF->offsetX = pixelsize*3;
	blendF->offsetY = pixelsize*blendF->lbelowPpW;
	blendF->lineIncA= blendF->labovePpW*pixelsize - (xStartA + (xEndB - xStartB)) + xStartA;
	blendF->lineIncB= blendF->offsetY - xEndB + xStartB;
	blendF->endLine = blendF->iBelowStart + (xEndB-xStartB) - blendF->offsetX;
	blendF->iBelowEnd = (yEndB - (pixelsize)) * blendF->lbelowPpW;
}

void initBlendFrameRect(BlendFrame* blendF, Layer *lBelow, const Layer *lAbove,
						int x, int y, int width, int height, int pixelsize){
	unsigned long	xStartB, xStartA,
					yStartB, yStartA,
					xEndB, yEndB;
	blendF->labovePpW = lAbove->width*3;
	blendF->lbelowPpW = lBelow->width*3;
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
	xEndB = (min((lAbove->x * 3+blendF->labovePpW), x*3+width*3));
	if(lAbove->y<0){
		yStartB = max(0,y);
		yStartA = -lAbove->y+yStartB;
	}
	else{
		yStartB = max(lAbove->y,y);
		yStartA = yStartB-lAbove->y;
	}
	yEndB = min((lAbove->y+lAbove->height), y+height);
	yEndB = min((lAbove->y+lAbove->height), lBelow->height);

	blendF->iBelowStart  = yStartB*blendF->lbelowPpW + xStartB;
	blendF->iAboveStart  = yStartA*blendF->labovePpW + xStartA;
	blendF->offsetX = pixelsize*3;
	blendF->offsetY = pixelsize*blendF->lbelowPpW;
	blendF->lineIncA= blendF->labovePpW*pixelsize - (xStartA + (xEndB - xStartB)) + xStartA;
	blendF->lineIncB= blendF->offsetY - xEndB + xStartB;
	blendF->endLine = blendF->iBelowStart + (xEndB-xStartB) - blendF->offsetX;
	blendF->iBelowEnd = (yEndB - (pixelsize)) * blendF->lbelowPpW;
}

void initBlend(Blend* blend, Layer *lBelow, const Layer *lAbove, unsigned long iStartB, unsigned long iStartA,
			   int inverseAction){
	blend->blendFunc = functionFromBlendMode(lAbove->blendMode,inverseAction);
	if(lAbove->blendMode==BLEND_MOY){
		blend->parameters = malloc(sizeof(float));
		*((float*)blend->parameters) =  lBelow->opacity+lAbove->opacity;
	}

	blend->lut = malloc(sizeof(LUT));
	combineAllLUT(blend->lut,lAbove->luts);

	blend->iBelow = iStartB;
	blend->pBelow = lBelow->rgb + iStartB;
	blend->pAbove = lAbove->rgb + iStartA;
	blend->opacityAbove = lAbove->opacity;
	blend->opacityBelow = lBelow->opacity;
}

char blendNotEnded(const Blend *blend, const BlendFrame* frame){
	return blend->iBelow < frame->iBelowEnd;
}

void nextLine(Blend *blend, BlendFrame* frame){
	blend->pAbove  += frame->lineIncA;
	blend->iBelow  += frame->lineIncB;
	blend->pBelow  += frame->lineIncB;
	frame->endLine += frame->offsetY;
}

void nextPixelBloc(Blend *blend, const BlendFrame* frame){
	blend->pAbove += frame->offsetX;
	blend->iBelow += frame->offsetX;
	blend->pBelow += frame->offsetX;
}
void nextPixel(Blend *blend){
	blend->pAbove ++;
	blend->iBelow ++;
	blend->pBelow ++;
}

void blendTwoLayer(Layer *lBelow, const Layer *lAbove, int inverseAction, int pixelsize){

	BlendFrame frame;
	Blend blend;

	initBlendFrame(&frame,lBelow,lAbove,pixelsize);
	initBlend(&blend,lBelow,lAbove,frame.iBelowStart,frame.iAboveStart,inverseAction);

	while(blendNotEnded(&blend,&frame)){
		while(blend.iBelow<frame.endLine){
			processBlendAtCurrentBloc(&blend,&frame);
			nextPixelBloc(&blend,&frame);
		}
		while(blend.iBelow<frame.endLine+frame.offsetX){
			processBlendAtCurrentPixel(&blend);
			nextPixel(&blend);
		}
		nextLine(&blend,&frame);
	}
	free(blend.lut);
	lBelow->histogrammeUpdated = 0;
}

void blendTwoLayerInRect(Layer *lBelow, const Layer *lAbove, int inverseAction, int pixelsize,
						 int x, int y, int width, int height)
{
	BlendFrame frame;
	Blend blend;

	initBlendFrameRect(&frame,lBelow,lAbove,x,y,width,height,pixelsize);
	initBlend(&blend,lBelow,lAbove,frame.iBelowStart,frame.iAboveStart,inverseAction);

	while(blendNotEnded(&blend,&frame)){
		while(blend.iBelow<frame.endLine){
			processBlendAtCurrentBloc(&blend,&frame);
			nextPixelBloc(&blend,&frame);
		}

		while(blend.iBelow<frame.endLine+frame.offsetX){
			processBlendAtCurrentPixel(&blend);
			nextPixel(&blend);
		}
		nextLine(&blend,&frame);
	}
	free(blend.lut);
	lBelow->histogrammeUpdated = 0;
}

void addLUTToLayer(LUT *lut, Layer *l){
	LUTsList_insertLast(&l->luts,lut);
}

void setLUT(LUT *lut, Effect e, float amount){
	lut->effectAmount = amount;
	lut->LUTEffect = e;
	lut->activ = 1;
	int i,cpt;
	switch (e) {
	case CONTRAST:
		for(cpt=0;cpt<3;cpt++){
			lut->values[cpt][127] = 127;
			for(i=0;i<127;i++)
				lut->values[cpt][i] = min(max(0,i-126.*amount),127);
			for(i=128;i<256;i++)
				lut->values[cpt][i] = max(min(255,i+126.*amount),127);
		}
		lut->fromGray = 0;
		break;

	case BRIGHTNESS:
		for(i=0;i<256;i++)
			lut->values[0][i] = max(min(255,i+255.*amount),0);
		for(i=0;i<256;i++)
			lut->values[1][i] = max(min(255,i+255.*amount),0);
		for(i=0;i<256;i++)
			lut->values[2][i] = max(min(255,i+255.*amount),0);
		lut->fromGray = 0;
		break;
	case SEPIA:
		for(i=0;i<256;i++)
			lut->values[0][i] = max(min(255,i*(1.1*(1+amount))),0);
		for(i=0;i<256;i++)
			lut->values[1][i] = max(min(255,i*(0.8*(1+amount))),0);
		for(i=0;i<256;i++)
			lut->values[2][i] = max(min(255,i*(0.5*(1+amount))),0);
		lut->fromGray = 1;
		break;
	case INVERSED:
		for(cpt=0;cpt<3;cpt++)
			for(i=0;i<256;i++)
				lut->values[cpt][i] = 255-i;
		lut->fromGray = 0;
		break;
	case GRAYEFFECT:
		for(cpt=0;cpt<3;cpt++)
			for(i=0;i<256;i++)
				lut->values[cpt][i] = i;
		lut->fromGray = 1;
	/*case SATURATIONPLUS:




	case SATURATIONMINUS:*/




	default:
		for(cpt=0;cpt<3;cpt++)
			for(i=0;i<256;i++)
				lut->values[cpt][i] = i;
		break;
	}
}

LUT *makeLUT(Effect e, float amount){
	LUT* lut = malloc(sizeof(LUT));
	setLUT(lut,e,amount);
	return lut;
}

void combineLUT(unsigned char values[3][256], LUT *lut1, LUT *lut2){
	if(lut2->activ){
		int i,cpt;
		if(lut2->fromGray){
			unsigned char gray;
			for(i=0;i<256;i++){
				gray =	(float)lut1->values[0][i] * REDCOEF +
						(float)lut1->values[1][i] * GREENCOEF +
						(float)lut1->values[2][i] * BLUECOEF;
				for(cpt=0;cpt<3;cpt++)
					values[cpt][i] = lut2->values[cpt][gray];
			}
		}
		else{
			for(cpt=0;cpt<3;cpt++)
				for(i=0;i<256;i++){
					values[cpt][i] = lut2->values[cpt][lut1->values[cpt][i]];
				}
		}
	}
}

void combineAllLUT(LUT* out, LUTsList* luts){
	int i,cpt;
	for(cpt=0;cpt<3;cpt++)
		for(i=0;i<256;i++)
			out->values[cpt][i] = i;
	out->activ = 1;
	out->effectAmount = 1;
	out->LUTEffect = MULTIPLE;
	out->fromGray = 0;
	if(luts){
		out->fromGray = luts->element->fromGray;
		do{
			combineLUT(out->values, out, luts->element);
			luts = luts->next;
		}while(luts!=NULL);
	}
}

void copyLayerInRect(Layer *l, const Layer *layerToCopy, int x, int y,
					 int width, int height){
	BlendFrame frame;
	Blend blend;

	initBlendFrameRect(&frame,l,layerToCopy,x,y,width,height,1);
	initBlend(&blend,l,layerToCopy,frame.iBelowStart,frame.iAboveStart,0);

	while(blendNotEnded(&blend,&frame)){
		while(blend.iBelow<frame.endLine){
			*blend.pBelow = *blend.pAbove;
			nextPixel(&blend);
		}
		nextLine(&blend,&frame);
	}
	free(blend.lut);
	l->histogrammeUpdated = 0;
}
