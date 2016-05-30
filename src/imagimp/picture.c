#include "picture.h"

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

void resizePicture(Picture* p, unsigned int width, unsigned int height){
	makeEmptyLayer(&p->blank,width,height,0,0);
	copyLayer(&p->beforeCf,&p->blank);
	LayersList* l;
	int i = 1;
	for(l = p->layers.next;i<p->currentID;l=l->next,i++){
		blendTwoLayer(&p->beforeCf,l->element,0,1);
	}
	p->Cf.histogrammeUpdated = 0;
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
	p->Cf.histogrammeUpdated = 0;
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
			p->Cf.histogrammeUpdated = 0;
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
			p->Cf.histogrammeUpdated = 0;
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
		p->Cf.histogrammeUpdated = 0;
	}
}

void changeCurrentTo(Picture* p, int i){
	if(i<=0 || i>=p->nbLayers || p->currentID==i) return;
	if(i==0)
	{
		p->current = &p->layers;
		p->currentID = 0;
		copyLayer(&p->beforeCf,&p->blank);
	}
	else if(i==1){
		p->current = p->layers.next;
		p->currentID = 1;
		copyLayer(&p->beforeCf,&p->blank);
	}
	else if(p->currentID<i){
		for(;p->currentID<i;p->currentID++){
			blendTwoLayer(&p->beforeCf,p->current->element,0,1);
			p->current = p->current->next;
		}
	}
	else{
		for(;p->currentID>i;p->currentID--){
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

void freeAllLayers(LayersList **l){
	if(*l!=NULL){
		LayersList* current = *l;
		while(current->next!=NULL){
			LayersList* next = current->next;
			freeLayer(&current->element);
			free(current);
			current = next;
		}
		free(*l);
		*l = NULL;
	}
}

void translateCurrentLayer(Picture *p, int tx, int ty){
	if(p->current->previous!=NULL){
		p->current->element->x += tx;
		p->current->element->y += ty;
		p->Cf.histogrammeUpdated = 0;
	}
}

int pictureIsEmpty(const Picture* p){
	return p->current->previous==NULL;
}

void freePicture(Picture* p){
	freeAllLayers(&p->layers.next);
	free(p->layers.element->rgb);
}
