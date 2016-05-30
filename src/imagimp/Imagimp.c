#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include "interface.h"
#include "picture.h"
#include "imagimp.h"
#include "PPM.h"
#include "outils.h"
#include "imagimp_dialog.h"


const Color IMAGIMPBACK = {0.05,0.01,0.07,1};
const Color IMAGIMPFORE = {0.3,0.6,0.4,1};
float initial_opacity;
int initial_Blend;
char inputStr[256];

void Imagimp_updateButton();

/// ///////////////////////////////////////////////////////////////////////////
/// Handler
/// ///////////////////////////////////////////////////////////////////////////

void opacitySliderHandle(const void* opacity){
    Imagimp_setOpacityToCurrentLayer(*((float*)opacity));
}

void Imagimp_putCurrentLayerFront(){
	putCurrentLayerFront(&Imagimp.picture);
	Imagimp_updateButton();
	Imagimp_refresh(1);
}

void Imagimp_putCurrentLayerBehind(){
	putCurrentLayerBehind(&Imagimp.picture);
	Imagimp_updateButton();
	Imagimp_refresh(1);
}

void Imagimp_switchHistogramme(){

	Imagimp.histogrammeMode = ((Imagimp.histogrammeMode+2)%(NB_H_TYPES+2))-1;
	switch (Imagimp.histogrammeMode) {
	case RED_H:
		setButtonText(Imagimp.mainButtons + BTN_MAINHISTOGRAMME,"Histogramme Vert");
		break;
	case GREEN_H:
		setButtonText(Imagimp.mainButtons + BTN_MAINHISTOGRAMME,"Histogramme Bleu");
		break;
	case BLUE_H:
		setButtonText(Imagimp.mainButtons + BTN_MAINHISTOGRAMME,"Histogramme Luminosite");
		break;
	case GRAYSCALE_H:
		setButtonText(Imagimp.mainButtons + BTN_MAINHISTOGRAMME,"Tous les Histogrammes");
		break;
	case NB_H_TYPES:
		setButtonText(Imagimp.mainButtons + BTN_MAINHISTOGRAMME,"Cacher Histogramme");
		break;
	default:
		setButtonText(Imagimp.mainButtons + BTN_MAINHISTOGRAMME,"Histogramme Rouge");
		break;
	}

}

void Imagimp_switchDisplay(){
	if(Imagimp.displayMode){
		Imagimp.displayMode = 0;
		actualiseImage(Imagimp.picture.current->element->rgb,
					   Imagimp.picture.current->element->width,Imagimp.picture.current->element->height);
		setButtonText(Imagimp.mainButtons + BTN_DISPLAYMODE,"Affiche Rendu");
	}
	else{
		Imagimp.displayMode = 1;
		updateCfLayer(&Imagimp.picture,1);
		actualiseImage(Imagimp.picture.Cf.rgb, Imagimp.picture.Cf.width,Imagimp.picture.Cf.height);
		setButtonText(Imagimp.mainButtons + BTN_DISPLAYMODE,"Affiche Calque Original");
	}
}

void putInversion(){
	addLUTToLayer(makeLUT(INVERSED,0),Imagimp.picture.current->element);
	desactiveDialog();
	Imagimp_refresh(1);
}
void putGrayEffect(){
	addLUTToLayer(makeLUT(GRAYEFFECT,0),Imagimp.picture.current->element);
	desactiveDialog();
	Imagimp_refresh(1);
}

////////////////////////////////////////////////////
/// handleDialog
//////////////////////////////////////////////////

void handleOpacityDialog(DIALOGBTNS answer){
    switch (answer) {
    case BTN_CANCEL:
        Imagimp_setOpacityToCurrentLayer(initial_opacity);
        break;
    default:
        break;
    }
}

void handlePPMImportDialog(DIALOGBTNS answer){
    unsigned int width, height;
    unsigned char* rgb;
    switch (answer) {
    case BTN_OK:
        rgb = readPPM(convertString(Dialog.input),&width,&height);
        if(rgb==NULL){
            activeDialog("Une erreur est servenu lors de la lecture du fichier", FLAGS_OK,NULL);
            return;
        }
        addNewLayer(&Imagimp.picture,rgb,width,height);
        Imagimp_updateButton();
        setComponentInactiv(Imagimp.mainButtons + BTN_DELETELAYER,0);
        Imagimp_refresh(1);
        break;
    default:
        break;
    }
}

void handlePPMExportDialog(DIALOGBTNS answer){
    switch (answer) {
    case BTN_OK:
        updateCfLayer(&Imagimp.picture,1);
        if(!writePPM(convertString(Dialog.input),Imagimp.picture.Cf.rgb,
					 Imagimp.picture.Cf.width,Imagimp.picture.Cf.height))
			activeDialog("Une erreur est servenu lors de l'écriture du fichier", FLAGS_OK,NULL);
		else
			activeDialog("L'image a bien été enregistré :)", FLAGS_OK,NULL);
		break;
	default:
		break;
	}
}

void handleResizeDialog(DIALOGBTNS answer){
	unsigned int width,height;
	switch (answer) {
	case BTN_OK:
		sscanf(convertString(Dialog.input),"%uX%u",&width,&height);
		resizePicture(&Imagimp.picture,width,height);
		Imagimp_refresh(1);
	default:
		break;
	}
}

void handleLayerDeletingAskDialog(DIALOGBTNS answer){
    switch (answer) {
    case BTN_YES:
        Imagimp_removeCurrentLayer();
        break;
    default:
        break;
    }
}

void handleEffectDialog(DIALOGBTNS answer){
	switch (answer) {
	case BTN_CANCEL:
		if(Imagimp.picture.current->element->luts!=NULL){
			LUTsList_removeLast(&Imagimp.picture.current->element->luts);
			Imagimp_refresh(1);
		}
		break;
	default:
		break;
	}
}

void handleBlendModeDialogButton(){
    int blendMode = NBBLEND-1;
    ComponentsList* buttons = Dialog.radioButtons;
    while(buttons!=NULL){
        if(buttons->componenent->extends.RadioButton.isSelected){
            setBlendMode(Imagimp.picture.current->element,blendMode);
            Imagimp_refresh(1);
            return;
        }
        else{
            blendMode--;
            buttons = buttons->next;
        }
    }
}

void handleEffectSlider(const void* value){
	if(Imagimp.picture.current->element->luts!=NULL){
		LUT* lut = LUTsList_getLast(Imagimp.picture.current->element->luts);
		setLUT(lut, lut->LUTEffect, *((const float*)value));
		Imagimp_refresh(1);
	}
}


void handleBlendModeDialog(DIALOGBTNS answer){
    switch (answer) {
    case BTN_CANCEL:
		setBlendMode(Imagimp.picture.current->element,initial_Blend);
		Imagimp_refresh(1);
        break;
    default:
        break;
    }
}

void handleLutListButtonDialog(){
	ComponentsList* pointer = Dialog.componentsSet;
	int i=Dialog.nbComponentsInSet-1;
	while(pointer!=NULL){
		if(pointer->componenent->type==DELETABLEBUTTON)
			if(pointer->componenent->extends.DeletableButton.needToBeDelete)
				break;
		i--;
		pointer = pointer->next;
	}
	if(pointer!=NULL){
		LUTsList_removeAt(&Imagimp.picture.current->element->luts,i);
		Imagimp_refresh(1);
	}
	desactiveDialog();
	openLUTListDialog();
}

/////////////////////////////////////////////////////
/// openDialog
//////////////////////////////////////////////////

void openOpacityDialog(){
	initial_opacity = Imagimp.picture.current->element->opacity;

	activeDialog("Opacite",FLAGS_SLIDER | FLAGS_CANCEL | FLAGS_OK, handleOpacityDialog);
	setSliderMin(&Dialog.slider,0);
	setSliderMax(&Dialog.slider,1);
	setSliderValue(&Dialog.slider,Imagimp.picture.current->element->opacity);
	Dialog.slider.clickHandle = opacitySliderHandle;
}


void openResizeDialog(){
	activeDialog("Redimensionner la zone de travail (\"largeurXHauteur\"):",
				 FLAGS_PROMPT | FLAGS_CANCEL | FLAGS_OK, handleResizeDialog);
}

void openPPMImportDialog(){
    activeDialog("Importer un fichier PPM \n Saississez le nom du fichier:",
                 FLAGS_PROMPT | FLAGS_CANCEL | FLAGS_OK, handlePPMImportDialog);
}

void openPPMExportDialog(){
    activeDialog("Exporter un fichier PPM \n Saississez le nom du fichier:",
                 FLAGS_PROMPT | FLAGS_CANCEL | FLAGS_OK, handlePPMExportDialog);
}

void openLUTSetDialog(Effect e, const char* effectName, float min, float max, float current){
	addLUTToLayer(makeLUT(e,0),Imagimp.picture.current->element);
	desactiveDialog();
	activeDialog(effectName,FLAGS_CANCEL | FLAGS_OK | FLAGS_SLIDER,handleEffectDialog);
	setSliderMin(&Dialog.slider,min);
	setSliderMax(&Dialog.slider,max);
	setSliderValue(&Dialog.slider,current);
	Dialog.slider.clickHandle = handleEffectSlider;
	Imagimp_refresh(1);
}

void openContrastDialog(){
	openLUTSetDialog(CONTRAST, "Contraste",-1,1,0);
}

void openBrightnessDialog(){
	openLUTSetDialog(BRIGHTNESS, "Luminosite",-1,1,0);
}
void openSepiaDialog(){
	openLUTSetDialog(SEPIA, "Sepia",0,1,0);
}

void openLUTAddingDialog(){
	activeDialog("Ajout LUT",FLAGS_CANCEL | FLAGS_COMPONENTSET,NULL);
	int i;
	for(i=0;i<NBEFFECTS;i++){
		addComponentInDialog(Imagimp.effectsButtons + i);
	}
}

void openLUTListDialog(){
	activeDialog("Liste LUT",FLAGS_CANCEL | FLAGS_OK | FLAGS_COMPONENTSET,freeDialogSetButtons);
	LUTsList* lut = Imagimp.picture.current->element->luts;
	Component* c;
	char btnText[50];
	while(lut!=NULL)
	{
		c = malloc(sizeof(Component));
		memset(btnText,0,50);
		switch(lut->element->LUTEffect){
		case CONTRAST:
			sprintf(btnText,"Contraste %3.2f%%",lut->element->effectAmount*100.);
			break;
		case BRIGHTNESS:
			sprintf(btnText,"Luminosite %3.2f%%",lut->element->effectAmount*100.);
			break;
		case SEPIA:
			sprintf(btnText,"Sepia %3.2f%%",lut->element->effectAmount*100.);
			break;
		case INVERSED:
			sprintf(btnText,"Negatif");
			break;
		case GRAYEFFECT:
			sprintf(btnText,"Noir&Blanc");
			break;
		default:
			sprintf(btnText,"Inconnue %3.2f%%",lut->element->effectAmount*100.);
			break;
		}
		(*c) = makeDeletableButton(btnText,makeBounds(0,0,0,0),IMAGIMPFORE,IMAGIMPBACK,handleLutListButtonDialog);
		addComponentInDialog(c);
		lut = lut->next;
	}
}

void openBlendDialog(){
    activeDialog("Modifier le mode de melange:",
				 FLAGS_RADIOBUTTON | FLAGS_CANCEL | FLAGS_OK, handleBlendModeDialog);
    int i;
    for(i=0;i<NBBLEND;i++){
        addRadioButtonInDialog(Imagimp.blendButtons + i);
        if(i==Imagimp.picture.current->element->blendMode)
        {
            selectRadioButton(Imagimp.blendButtons+i);
			initial_Blend = i;
        }
    }
}

void openLayerDeletingAskDialog(){
	activeDialog("Souhaitez-vous reellement supprimer le calque?",FLAGS_YES | FLAGS_NO,
				 handleLayerDeletingAskDialog);
}

void makeImageBase(int width, int height){
    const unsigned long size = 3*width*height;
    Imagimp.image_base = malloc(size*sizeof(unsigned char));
    //memset(Imagimp.image_base,0xaa,size);
    int nbValuesPerWidth = width*3;
    int block = 30;
    int i,j,k,l;
    for(i=0;i<nbValuesPerWidth;i += block){
        if((i/block)%4==0)
            for(k=0;k<height;k++)
                for(l=0;l<block;l++)
                {
                    int pos = k*3*width+l*3;
                    for(j=0;j<3;j++)
                    {
                        Imagimp.image_base[i+j+pos] = (255 - (50*(i%(5-j))));
                    }
                }
        else if((i/block)%4==1)
            for(k=0;k<height;k++)
                for(l=0;l<block;l++)
                {
                    int pos = k*3*width+l*3;
                    for(j=0;j<3;j++)
                    {
                        Imagimp.image_base[i+j+pos] = (255 - (50*(i%(6-j))));
                    }
                }
        else if((i/block)%4==2)
            for(k=0;k<height;k++)
                for(l=0;l<block;l++)
                {
                    int pos = k*3*width+l*3;
                    for(j=0;j<3;j++)
                    {
                        Imagimp.image_base[i+j+pos] = (255 - (50*(i%(7-j))));
                    }
                }
        else
            for(k=0;k<height;k++)
                for(l=0;l<block;l++)
                {
                    int pos = k*3*width+l*3;
                    for(j=0;j<3;j++)
                    {
                        Imagimp.image_base[i+j+pos] = (255 - (50*(i%(10-j))));
                    }
                }
    }
}

///////////////////////////////////////////////////////////
/// Imagimp
///////////////////////////////////////////////////////////


void Imagimp_setButtons(){
	float startX = getUIStartX() + 0.005;
	float btnsizeX = 0.195f;
	float btnsizeY = 0.04f;
	int i;
	Imagimp.mainButtons[BTN_LOAD] = makeButton("Charger...",makeBounds(startX,0.85f,btnsizeX,btnsizeY),
										   IMAGIMPFORE,IMAGIMPBACK,openPPMImportDialog);
	Imagimp.mainButtons[BTN_SAVE] = makeButton("Sauvegarder...",makeBounds(startX,0.8f,btnsizeX,btnsizeY),
										   IMAGIMPFORE,IMAGIMPBACK,openPPMExportDialog);
	Imagimp.mainButtons[BTN_QUIT] = makeButton("Quitter",makeBounds(startX,0.75f,btnsizeX,btnsizeY),
										   IMAGIMPFORE,IMAGIMPBACK,Imagimp_quit);


	Imagimp.mainButtons[BTN_DISPLAYMODE] = makeButton("Afficher Calque Original",makeBounds(startX,0.6f,btnsizeX,btnsizeY),
												  IMAGIMPFORE,IMAGIMPBACK,Imagimp_switchDisplay);
	Imagimp.mainButtons[BTN_MAINHISTOGRAMME] = makeButton("Histogramme Rouge",makeBounds(startX,0.55f,btnsizeX,btnsizeY),
												  IMAGIMPFORE,IMAGIMPBACK,Imagimp_switchHistogramme);
	Imagimp.mainButtons[BTN_RESIZE] = makeButton("Redimensionner",makeBounds(startX,0.5f,btnsizeX,btnsizeY),
												  IMAGIMPFORE,IMAGIMPBACK,openResizeDialog);

	Imagimp.mainButtons[BTN_DELETELAYER] = makeButton("Supprimer Calque Courant",makeBounds(startX,0.35f,btnsizeX,btnsizeY),
												  IMAGIMPFORE,IMAGIMPBACK,openLayerDeletingAskDialog);
	Imagimp.mainButtons[BTN_UPLAYER] = makeButton("Avancer",makeBounds(startX,0.3f,btnsizeX/2.,btnsizeY),
										   IMAGIMPFORE,IMAGIMPBACK,Imagimp_putCurrentLayerFront);
	Imagimp.mainButtons[BTN_DOWNLAYER] = makeButton("Reculer",makeBounds(startX+btnsizeX/2.,0.3f,btnsizeX/2.,btnsizeY),
										   IMAGIMPFORE,IMAGIMPBACK,Imagimp_putCurrentLayerBehind);
	Imagimp.mainButtons[BTN_ADDLUT] = makeButton("Ajouter LUT",makeBounds(startX,0.25f,btnsizeX,btnsizeY),
										   IMAGIMPFORE,IMAGIMPBACK,openLUTAddingDialog);
	Imagimp.mainButtons[BTN_LUTLIST] = makeButton("Liste LUT",makeBounds(startX,0.2f,btnsizeX,btnsizeY),
										   IMAGIMPFORE,IMAGIMPBACK,openLUTListDialog);
	Imagimp.mainButtons[BTN_OPACITY] = makeButton("Opacite...",makeBounds(startX,0.15f,btnsizeX,btnsizeY),
											  IMAGIMPFORE,IMAGIMPBACK,openOpacityDialog);
	Imagimp.mainButtons[BTN_BLENDMODE] = makeButton("Melange...",makeBounds(startX,0.1f,btnsizeX,btnsizeY),
												  IMAGIMPFORE,IMAGIMPBACK,openBlendDialog);

	btnsizeX = 0.095f;
	btnsizeY = 0.05f;
	Imagimp.blendButtons[BLEND_ADD] = makeRadioButton("Addition",makeBounds(0,0,btnsizeX,btnsizeY),
										   IMAGIMPFORE,IMAGIMPBACK,handleBlendModeDialogButton);
	Imagimp.blendButtons[BLEND_DIV] = makeRadioButton("Division",makeBounds(0,0,btnsizeX,btnsizeY),
										   IMAGIMPFORE,IMAGIMPBACK,handleBlendModeDialogButton);
	Imagimp.blendButtons[BLEND_MOY] = makeRadioButton("Moyenne",makeBounds(0,0,btnsizeX,btnsizeY),
										   IMAGIMPFORE,IMAGIMPBACK,handleBlendModeDialogButton);
	Imagimp.blendButtons[BLEND_MULT] = makeRadioButton("Produit",makeBounds(0,0,btnsizeX,btnsizeY),
											  IMAGIMPFORE,IMAGIMPBACK,handleBlendModeDialogButton);
	Imagimp.blendButtons[BLEND_SUB] = makeRadioButton("Difference",makeBounds(0,0,btnsizeX,btnsizeY),
												  IMAGIMPFORE,IMAGIMPBACK,handleBlendModeDialogButton);
	Imagimp.blendButtons[BLEND_DARK] = makeRadioButton("Sombre",makeBounds(0,0,btnsizeX,btnsizeY),
												  IMAGIMPFORE,IMAGIMPBACK,handleBlendModeDialogButton);
	Imagimp.blendButtons[BLEND_LIGHT] = makeRadioButton("Lumineux",makeBounds(0,0,btnsizeX,btnsizeY),
												  IMAGIMPFORE,IMAGIMPBACK,handleBlendModeDialogButton);

	Imagimp.effectsButtons[CONTRAST] = makeButton("Contraste",makeBounds(0,0,btnsizeX,btnsizeY),
										   IMAGIMPFORE,IMAGIMPBACK,openContrastDialog);
	Imagimp.effectsButtons[BRIGHTNESS] = makeButton("Luminosite",makeBounds(0,0,btnsizeX,btnsizeY),
										   IMAGIMPFORE,IMAGIMPBACK,openBrightnessDialog);
	Imagimp.effectsButtons[SEPIA] = makeButton("Sepia",makeBounds(0,0,btnsizeX,btnsizeY),
										   IMAGIMPFORE,IMAGIMPBACK,openSepiaDialog);
	Imagimp.effectsButtons[INVERSED] = makeButton("Negatif",makeBounds(0,0,btnsizeX,btnsizeY),
										   IMAGIMPFORE,IMAGIMPBACK,putInversion);
	Imagimp.effectsButtons[GRAYEFFECT] = makeButton("Noir&Blanc",makeBounds(0,0,btnsizeX,btnsizeY),
										   IMAGIMPFORE,IMAGIMPBACK,putGrayEffect);

	Imagimp.components = makeComponentsList(Imagimp.mainButtons);
	Imagimp_updateButton();
	for(i=1;i<MAIN_NBBUTTONS;i++)
	{
		addComponent(Imagimp.mainButtons+i,&Imagimp.components);
	}
}

void Imagimp_setLabels(){
	float startX = getUIStartX();
	float btnsizeX = getUIWidth();
	float btnsizeY = 0.05;
	int i;
	Imagimp.labels[LBL_CURRENTLAYER] = makeLabel("Calque Courant", makeBounds(startX,0.4,btnsizeX,btnsizeY),
												 IMAGIMPFORE,GLUT_BITMAP_HELVETICA_12);
	Imagimp.labels[LBL_FILE] = makeLabel("Fichier", makeBounds(startX,0.9,btnsizeX,btnsizeY),
												 IMAGIMPFORE,GLUT_BITMAP_HELVETICA_12);
	Imagimp.labels[LBL_LAYERLIST] = makeLabel("Rendu", makeBounds(startX,0.65,btnsizeX,btnsizeY),
												 IMAGIMPFORE,GLUT_BITMAP_HELVETICA_12);
	for(i=0;i<MAIN_NBLABEL;i++)
	{
		addComponent(Imagimp.labels+i,&Imagimp.components);
	}
}

void Imagimp_handleArgs(int argc, char *argv[], unsigned int* w, unsigned int* h){
	int i;
	for (i=1;i<argc;i++) {
		if (strcmp(argv[i],"-h") == 0) {
			printf("Affichage de l'aide\n");
			exit(1);
		}
		else if (strcmp(argv[i],"-f") == 0) {
			printf("Chargement fichier :\n");
			i++;
			if(i<argc){
				char input[100];
				printf("Entrez le fichier a ouvrir:");
				scanf("%s",input);
				Imagimp.image_base = readPPM(input,w,h);
				if(Imagimp.image_base==NULL){
					fprintf(stderr, "Impossible de lire le fichier %s",argv[i]);
					exit(1);
				}
			}
			else{
				Imagimp.image_base = readPPM(argv[i],w,h);
				i++;
				if(Imagimp.image_base==NULL){
					fprintf(stderr, "Impossible de lire le fichier %s",argv[i]);
					exit(1);
				}
			}
			addNewLayer(&Imagimp.picture,Imagimp.image_base,*w,*h);
		}
		else if (strcmp(argv[i],"-n") == 0) {
			printf("Chargement calque :\n");
			i++;
			if(i<argc-1){
				makeImageBase(600,800);
				addNewLayer(&Imagimp.picture,Imagimp.image_base,*w,*h);
			}
			else
			{
				sscanf(argv[i],"%d",w);
				i++;
				sscanf(argv[i],"%d",h);
				i++;
				makeImageBase(*w,*h);
				addNewLayer(&Imagimp.picture,Imagimp.image_base,*w,*h);
			}

		}
	}
}

void Imagimp_launch(int argc, char *argv[]) {
    unsigned int w=800, h=600, ihm_w=200;
    makeEmptyPicture(&Imagimp.picture, w,h);
	Imagimp_handleArgs(argc,argv,&w,&h);
    updateCfLayer(&Imagimp.picture,1);
    Imagimp.hoveredButton = NULL;
    Imagimp.pressedButton = NULL;
    Imagimp.mouseButtonPressed=0;
    Imagimp.displayMode = 1;
    Imagimp.dialogMode = 0;
    Imagimp.dragImage = Imagimp.dragLayer = 0;
	Imagimp.histogrammeMode = -1;
    initGLIMAGIMP_IHM(w, h, Imagimp.picture.Cf.rgb, w+ihm_w, h,0);
    setBackground(makeColor(0.02,0.03,0.05,1),IMAGIMPBACK);
    fixeFonctionClicSouris(Imagimp_handleMouseClick);
    fixeFonctionMotionSouris(Imagimp_handleMouseMotion);
    fixeFonctionClavier(Imagimp_handleKeyboard);
    fixeFonctionClavierSpecial(Imagimp_handleKeyboardSpecial);
    fixeFonctionDessin(Imagimp_draw);

	Imagimp_setButtons();
	Imagimp_setLabels();

    initDialog();
    launchApp();
}

void Imagimp_handleKeyboard(unsigned char ascii, int x, int y, char CTRL, char ALT) {
    if(Imagimp.dialogMode){
		handleKeyboardTexte(ascii, x, y);
        return;
    }
    char saisie[100] = {'\0'};
    int modifier = glutGetModifiers();
    switch(ascii) {
	case 'l': openLUTAddingDialog();
        break;
    case 'L': /* (LUT_3) Supprimer la dernière LUT au calque actif */ break;
	case 'a': /* (LUT_2) Appliquer une LUT au calque actif */ break;
	case 'v': Imagimp_switchDisplay(); break;
	case 'q': Imagimp_quit(); break;
    case 'h': /* Imprimer l'historique dans le terminal */
        /* L'historique conserve :
         * IM_1, CAL_1, CAL_3, CAL_4, CAL_5, LUT_1, LUT_3
         * Action annulables :
         * CAL_1, CAL_3, CAL_4, LUT_1, LUT_3 */
        break;
    case 'z': /* Annuler */ break;
    case 'c': /* (CAL_1) Ajouter un calque vierge */
        /* Calque multiplicatif blanc d'opacité 0, inséré à la fin de la
         * liste de calques. */
        break;
	case 'x': Imagimp_removeCurrentLayer();
        break;
	case 'm': openBlendDialog(); break;
    case 't': // Exemple de saisie sur le terminal
        printf("Debraillement sur le terminal\n");
        printf("Merci de rentrer une chaine de caract�re :");
        scanf("%s",saisie);
        printf("On a tape %s\n",saisie);
        break;
    case 'i': // Exemple d'utilisation des fonctions de la bibliotheque glimagimp
        if(modifier==GLUT_ACTIVE_SHIFT)
            modeDebug(0);
        else
            modeDebug(1);
        break;
    case 's': // Exemple d'utilisation des fonctions de la bibliotheque glimagimp
        saveDessin();
        break;
    case 'f':
        setFullsreen(!isFullscreen());
        break;
    case '+':
        if(CTRL){
            zoomPlus();
            glutPostRedisplay();
        }
        break;
    case '-':
        if(CTRL){
            zoomMoins();
            glutPostRedisplay();
        }
        break;
    case '\t':
        if(glutGetModifiers()==GLUT_ACTIVE_SHIFT){
            if(Imagimp.picture.currentID == 0){
                changeCurrentTo(&Imagimp.picture,Imagimp.picture.nbLayers-1);
                Imagimp_updateButton();

            }
            else{
                changeCurrentToBelowLayer(&Imagimp.picture);
                Imagimp_updateButton();
            }
            if(Imagimp.picture.current->previous==NULL)
                setComponentInactiv(Imagimp.mainButtons + BTN_DELETELAYER,1);
            else
                setComponentInactiv(Imagimp.mainButtons + BTN_DELETELAYER,0);
            Imagimp_refresh(1);
        }
        else{
            if(Imagimp.picture.currentID >= Imagimp.picture.nbLayers-1){
                changeCurrentTo(&Imagimp.picture,0);
                Imagimp_updateButton();
            }
            else{
                changeCurrentToAboveLayer(&Imagimp.picture);
                Imagimp_updateButton();
            }
            if(Imagimp.picture.current->previous==NULL)
                setComponentInactiv(Imagimp.mainButtons + BTN_DELETELAYER,1);
            else
                setComponentInactiv(Imagimp.mainButtons + BTN_DELETELAYER,0);
            Imagimp_refresh(1);
        }
        break;
    case ESCAPE_KEY: // Touche Escape
        Imagimp_quit();
        break;
    default :
		break;
    }
}

void Imagimp_handleKeyboardSpecial(int touche, int x, int y) {
    switch(touche) {
    case GLUT_KEY_F1:
        activeDialog("Coucou toi",FLAGS_OK | FLAGS_PROMPT, NULL);
        break;
    case GLUT_KEY_F2: break;
    case GLUT_KEY_F3: break;
    case GLUT_KEY_F4: break;
    case GLUT_KEY_F5: break;
    case GLUT_KEY_F6: break;
    case GLUT_KEY_F7: break;
    case GLUT_KEY_F8: break;
    case GLUT_KEY_F9: break;
    case GLUT_KEY_F10: break;
    case GLUT_KEY_F11: break;
    case GLUT_KEY_F12: break;
    case GLUT_KEY_LEFT:
        if(Imagimp.dialogMode){
            if(!Dialog.slider.invisible)
            {
                setSliderValue(&Dialog.slider,Dialog.slider.extends.Slider.value-0.1);
                releaseComponent(&Dialog.slider,1);
            }
        }
        else
			translateCurrentLayer(&Imagimp.picture,-5,0);
        break;
    case GLUT_KEY_RIGHT:
        if(Imagimp.dialogMode){
            if(!Dialog.slider.invisible)
            {
                setSliderValue(&Dialog.slider,Dialog.slider.extends.Slider.value+0.1);
                releaseComponent(&Dialog.slider,1);
            }
        }
        else
			translateCurrentLayer(&Imagimp.picture,5,0);
        break;
	case GLUT_KEY_UP: translateCurrentLayer(&Imagimp.picture,0,5); break;
	case GLUT_KEY_DOWN: translateCurrentLayer(&Imagimp.picture,0,-5); break;
    case GLUT_KEY_PAGE_UP: break;
    case GLUT_KEY_PAGE_DOWN: break;
    case GLUT_KEY_HOME: break;
    case GLUT_KEY_END: break;
    case GLUT_KEY_INSERT: break;
    default:
        break;
    }
}

Bounds currentLayerBounds(){
	Bounds bounds = imageBounds();

	Bounds retour;
	retour = layerBoundsInWindow(Imagimp.picture.current->element,screenWidth(),screenHeight());
	if(Imagimp.displayMode)
	{
		retour.x = retour.x*zoom() + bounds.x;
		retour.y = bounds.y2 - retour.y2*zoom();
		retour.width *= zoom();
		retour.height *= zoom();
	}
	else{
		retour.x = bounds.x;
		retour.y = bounds.y;
	}
	retour.x2 = retour.x+retour.width;
	retour.y2 = retour.y+retour.height;
	return retour;
}

void Imagimp_handleMouseClick(int button, int state, float xGL, float yGL) {
    Component* b;
    int redisplay = 0;
    if(Imagimp.dialogMode){
        b = findComponentInList(xGL,yGL,Dialog.components);
        if(!Dialog.slider.invisible){
            unsigned char sliderHover = isInBounds(xGL,yGL,&Dialog.slider.extends.Slider.cursorBounds);
            if(sliderHover){
                if (state == GLUT_DOWN) {
                    pressComponent(&Dialog.slider);
                    redisplay = 1;
                }
                else{
                    releaseComponent(&Dialog.slider,1);
                    redisplay = 1;
                }
            }
            else{
                if (state != GLUT_DOWN) {
                    if(isInBounds(xGL,yGL,&Dialog.slider.bounds)){
                        setSliderValueFromPos(&Dialog.slider,xGL);
                    }
                    releaseComponent(&Dialog.slider,1);
                    redisplay = 1;
                }
            }
        }
    }
    else
        b = findComponentInList(xGL,yGL,Imagimp.components);
    if (state == GLUT_DOWN) {
		Bounds bounds;

        if(!Imagimp.dialogMode){
            if (button == GLUT_RIGHT_BUTTON){
				bounds = imageBounds();
                Imagimp.dragImage = isInBounds(xGL,yGL,&bounds);
            }
            else if(button == GLUT_LEFT_BUTTON){
				bounds = currentLayerBounds();
				Imagimp.dragLayer = isInBounds(xGL,yGL,&bounds);
            }
        }
        Imagimp.mouseButtonPressed=1;
        if(b!=NULL){
            Imagimp.pressedButton = b;
            pressComponent(b);
            redisplay = 1;
        }
    }
    else { // state == GLUT_UP
        Imagimp.dragImage = Imagimp.dragLayer = 0;
        Imagimp_refresh(1);
        if(Imagimp.mouseButtonPressed){
            Imagimp.mouseButtonPressed=0;
            if(Imagimp.pressedButton != NULL){
                releaseComponent(Imagimp.pressedButton,b==Imagimp.pressedButton);
                Imagimp.pressedButton=NULL;
                redisplay = 1;
            }
        }
    }
    if(redisplay) glutPostRedisplay();
}

void Imagimp_handleMouseMotion(float xGL, float yGL, float deltaX, float deltaY, int pressedButton) {
    Component* b;
    int redisplay = 0;
    if(Imagimp.dragImage){
        translateImage(-deltaX,-deltaY);
        glutPostRedisplay();
        return;
    }
    else if(Imagimp.dragLayer){
        translateCurrentLayer(&Imagimp.picture,deltaX * screenWidth(), -deltaY * screenHeight());
        Imagimp_refresh(4);
        return;
    }
    else if(Imagimp.dialogMode){
        b = findComponentInList(xGL,yGL,Dialog.components);
        if(!Dialog.slider.invisible){
            unsigned char sliderHover = isInBounds(xGL,yGL,&Dialog.slider.extends.Slider.cursorBounds);
            if(sliderHover){
                if(!Dialog.slider.hover)
                {
                    hoverComponent(&Dialog.slider);
                    redisplay = 1;
                }
            }
            else{
                if(Dialog.slider.hover)
                {
                    leaveComponent(&Dialog.slider);
                    redisplay = 1;
                }
            }
            if(Dialog.slider.press)
            {
                setSliderValueFromPos(&Dialog.slider,xGL);
                redisplay = 1;
            }
        }
    }
    else{
        b = findComponentInList(xGL,yGL,Imagimp.components);
    }

    if(Imagimp.hoveredButton!=b && Imagimp.hoveredButton!=NULL){
        leaveComponent(Imagimp.hoveredButton);
        Imagimp.hoveredButton=NULL;
        redisplay = 1;
    }
    if(b!=NULL){
        Imagimp.hoveredButton = b;
        hoverComponent(b);
        redisplay = 1;
    }
    if(redisplay) glutPostRedisplay();
}

void Imagimp_draw() {
	glColor4f(0.75,0.7,0.74,0.9);
	Bounds b = currentLayerBounds();
	glLineWidth(2);
	drawCarreVide(b.x+0.002,b.y,b.x2-0.002,b.y2);
	glColor4f(0.075,0.07,0.074,0.9);
	b = imageBounds();
	drawCarreVide(b.x,b.y-0.002,b.x2,b.y2+0.002);
	glLineWidth(1);
	drawAllComponents(Imagimp.components);
	b = makeBounds(0,0,0.2,0.17);
	if(Imagimp.histogrammeMode>=0){
		if(Imagimp.displayMode){
			if(!Imagimp.picture.Cf.histogrammeUpdated)
				updateHistogramme(&Imagimp.picture.Cf);
			glTranslatef(0,0.005,0);
			glColor4f(0.1,0.1,0.12,0.7);
			drawCarre(b.x,b.y,b.x2,b.y2);
			switch (Imagimp.histogrammeMode) {
			case RED_H:
				glColor4f(1,0,0,0.3);
				drawHistorgramme(Imagimp.picture.Cf.histogrammes+RED_H,&b);
				break;
			case BLUE_H:
				glColor4f(0,1,0,0.3);
				drawHistorgramme(Imagimp.picture.Cf.histogrammes+BLUE_H,&b);
				break;
			case GREEN_H:
				glColor4f(0,0,1,0.3);
				drawHistorgramme(Imagimp.picture.Cf.histogrammes+GREEN_H,&b);
				break;
			case GRAYSCALE_H:
				glColor4f(0.8,0.8,0.8,0.3);
				drawHistorgramme(Imagimp.picture.Cf.histogrammes+GRAYSCALE_H,&b);
				break;
			default:
				glColor4f(1,0,0,0.3);
				drawHistorgramme(Imagimp.picture.Cf.histogrammes+RED_H,&b);
				glColor4f(0,1,0,0.3);
				drawHistorgramme(Imagimp.picture.Cf.histogrammes+BLUE_H,&b);
				glColor4f(0,0,1,0.3);
				drawHistorgramme(Imagimp.picture.Cf.histogrammes+GREEN_H,&b);
				glColor4f(0.8,0.8,0.8,0.3);
				drawHistorgramme(Imagimp.picture.Cf.histogrammes+GRAYSCALE_H,&b);
				break;
			}
		}
	}
    if(Imagimp.dialogMode){
        Dialog_draw();
    }
}

void Imagimp_updateButton(){
	Imagimp.mainButtons[BTN_UPLAYER].inactiv =	Imagimp.picture.current->next == NULL ||
												Imagimp.picture.current->previous == NULL;
	Imagimp.mainButtons[BTN_DELETELAYER].inactiv = Imagimp.picture.current->previous == NULL;
	Imagimp.mainButtons[BTN_DOWNLAYER].inactiv = Imagimp.mainButtons[BTN_DELETELAYER].inactiv ?
													1:
													Imagimp.picture.current->previous->previous == NULL;
}


void Imagimp_setOpacityToCurrentLayer(float opacity){
	Imagimp.picture.current->element->opacity = fmaxf(0,fminf(1,opacity));
	Imagimp_refresh(1);
}

void Imagimp_refresh(int pixelsize){
	if(Imagimp.displayMode){
		updateCfLayer(&Imagimp.picture,pixelsize);
		actualiseImage(Imagimp.picture.Cf.rgb, Imagimp.picture.Cf.width,Imagimp.picture.Cf.height);
	}
	else{
		actualiseImage(Imagimp.picture.current->element->rgb,
					   Imagimp.picture.current->element->width,Imagimp.picture.current->element->height);
	}
}

void Imagimp_addEmptyLayer(){
	addNewEmptyLayer(&Imagimp.picture);
	Imagimp_refresh(1);
}

void Imagimp_removeCurrentLayer(){
	if(Imagimp.picture.current->previous !=NULL)
	{
		removeCurrentLayer(&Imagimp.picture);
		if(Imagimp.picture.current->previous==NULL)
			setComponentInactiv(Imagimp.mainButtons + BTN_DELETELAYER,1);
		Imagimp_refresh(1);
	}
	else
		activeDialog("Impossible de supprimer le calque d'arrière plan", FLAGS_OK,NULL);
}

void Imagimp_quit(){
	desactiveDialog();
	freePicture(&Imagimp.picture);
	freeAllComponents(Imagimp.components);
	printf("Fin du programme\n");
	exit(0);
}

