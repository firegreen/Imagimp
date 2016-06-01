#ifndef IMAGIMP_H
#define IMAGIMP_H

#include "interface.h"
#include "picture.h"
#include "imagimp_dialog.h"

typedef enum { BTN_QUIT=0, BTN_SAVE, BTN_LOAD, BTN_OPACITY, BTN_UPLAYER, BTN_DOWNLAYER, BTN_ADDLUT,
			   BTN_LUTLIST, BTN_DISPLAYMODE, BTN_DELETELAYER, BTN_BLENDMODE, BTN_MAINHISTOGRAMME,
			   BTN_RESIZE, MAIN_NBBUTTONS} MAINBTNS;
typedef enum { FLAGS_YES=1,FLAGS_NO=2,FLAGS_OK=4,
				FLAGS_CANCEL=8, FLAGS_PROMPT=16, FLAGS_SLIDER=32, FLAGS_RADIOBUTTON=64, FLAGS_COMPONENTSET=128} DIALOGFLAGS;
typedef enum { LBL_FILE, LBL_CURRENTLAYER, LBL_LAYERLIST, MAIN_NBLABEL} MAINLABELS;

struct {
    Component mainButtons[MAIN_NBBUTTONS];
    Component blendButtons[NBBLEND];
	Component effectsButtons[NBEFFECTS];
	Component labels[MAIN_NBLABEL];
    Component* pressedButton;
    Component* hoveredButton;
    ComponentsList* components;
    unsigned char* image_base;
    unsigned char mouseButtonPressed;
    unsigned char dialogMode;
	char histogrammeMode;
    unsigned char displayMode;
    Picture picture;
    unsigned char dragImage;
	unsigned char dragLayer;
} Imagimp;

void Imagimp_launch(int argc, char *argv[]);
void Imagimp_handleKeyboard(unsigned char ascii, int x, int y, char CTRL, char ALT);
void Imagimp_handleKeyboardSpecial(int touche, int x, int y);
void Imagimp_handleMouseClick(int button, int state, float xGL, float yGL);
void Imagimp_handleMouseMotion(float xGL, float yGL, float deltaX, float deltaY, int pressedButton);
void Imagimp_draw();
void Imagimp_refresh(int pixelsize);
void Imagimp_quit();
void Imagimp_switchDisplay();
void Imagimp_removeCurrentLayer();
void Imagimp_addEmptyLayer();
void Imagimp_putCurrentLayerFront();
void Imagimp_putCurrentLayerBehind();
void Imagimp_switchHistogramme();
void Imagimp_setOpacityToCurrentLayer(float opacity);

void openOpacityDialog();
void openPPMImportDialog();
void openPPMExportDialog();
void openLUTAddingDialog();
void openLUTListDialog();
void openBlendDialog();
void openResizeDialog();

Bounds currentLayerBounds();
#endif /* IMAGIMP_H */
