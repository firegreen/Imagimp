#ifndef IMAGIMP_DIALOG_H
#define IMAGIMP_DIALOG_H

#include "interface.h"
#include "imagimp_string.h"

extern const Color DIALOGBACK;
extern const Color DIALOGFORE;

typedef enum { BTN_YES=0, BTN_NO, BTN_OK, BTN_CANCEL, DIALOG_NBBUTTONS} DIALOGBTNS;

struct {
	char* text;
	String input;
	char prompt;
	char choice;
	char listing;
	Component buttons[DIALOG_NBBUTTONS];
	Component slider;
	Component btnUp,btnDown;
	ComponentsList* components;
	ComponentsList* componentsSet;
	ComponentsList* radioButtons;
	unsigned int nbRadioButtons;
	unsigned int nbComponentsInSet;
	unsigned int componentOffSet;
	Bounds bounds;
	float xText, yText;
	float xBtn, yBtn;
	Bounds promptBounds;
	void (*closeHandle)(DIALOGBTNS);
} Dialog;

void initDialog();
void activeDialog(const char* text, int flag, void (*closeHandle)(DIALOGBTNS));
void desactiveDialog();
void addRadioButtonInDialog(Component *radioButton);
void addComponentInDialog(Component *c);
void updateDialogListing();
void freeDialogSetButtons();
void Dialog_draw();
void handleKeyboardTexte(unsigned char ascii, int x, int y);

#endif
