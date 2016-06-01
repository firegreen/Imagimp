#include "imagimp_dialog.h"
#include "imagimp.h"
#include <string.h>

const Color DIALOGBACK = {0.05,0.17,0.15,1};
const Color DIALOGFORE = {0.75,0.9,0.65,1};

void answerOK() {
	desactiveDialog();
	if(Dialog.closeHandle != NULL)
		(*Dialog.closeHandle)(BTN_OK);
}
void answerYES() { desactiveDialog(); if(Dialog.closeHandle != NULL) (*Dialog.closeHandle)(BTN_YES); }
void answerNO() { desactiveDialog(); if(Dialog.closeHandle != NULL) (*Dialog.closeHandle)(BTN_NO); }
void answerCANCEL() { desactiveDialog(); if(Dialog.closeHandle != NULL) (*Dialog.closeHandle)(BTN_CANCEL);}

void handleUpBtnDialog(){
	Dialog.componentOffSet++;
	updateDialogListing();
}

void handleDownBtnDialog(){
	Dialog.componentOffSet--;
	updateDialogListing();
}

void initDialog(){
	float btnsizeX = 0.1f;
	float btnsizeY = 0.04f;
	Dialog.text = NULL;
	Dialog.bounds = makeBounds(0.25,0.35,0.5,0.35);
	Dialog.buttons[BTN_YES] = makeButton("Oui",makeBounds(0,0,btnsizeX,btnsizeY),
										 DIALOGFORE,DIALOGBACK,answerYES);
	Dialog.buttons[BTN_NO] = makeButton("Non",makeBounds(0,0,btnsizeX,btnsizeY),
										DIALOGFORE,DIALOGBACK,answerNO);
	Dialog.buttons[BTN_OK] = makeButton("Ok",makeBounds(0,0,btnsizeX,btnsizeY),
										DIALOGFORE,DIALOGBACK,answerOK);
	Dialog.buttons[BTN_CANCEL] = makeButton("Annuler",makeBounds(0,0,btnsizeX,btnsizeY),
											DIALOGFORE,DIALOGBACK,answerCANCEL);
	btnsizeX = Dialog.bounds.width*0.5;
	btnsizeY = 0.03;
	Dialog.btnUp = makeButton("^",makeBounds(Dialog.bounds.x + Dialog.bounds.width/4,
											 Dialog.bounds.y2-0.1,btnsizeX,btnsizeY),
											DIALOGFORE,DIALOGBACK,handleUpBtnDialog);
	Dialog.btnDown = makeButton("v",makeBounds(Dialog.bounds.x + Dialog.bounds.width/4,
											   Dialog.bounds.y+0.08,btnsizeX,btnsizeY),
											DIALOGFORE,DIALOGBACK,handleDownBtnDialog);


	Dialog.slider = makeSlider(makeBounds(Dialog.bounds.x+0.1,Dialog.bounds.y+Dialog.bounds.y/3,
										  Dialog.bounds.width-0.2,Dialog.bounds.y/5),
							   DIALOGFORE,DIALOGBACK,NULL);
	Dialog.input = makeString();
	Dialog.promptBounds = makeBounds(Dialog.bounds.x + 0.05,Dialog.bounds.y2-0.2,Dialog.bounds.width-0.1,0.08);
	Dialog.xBtn = Dialog.bounds.x2-0.15;
	Dialog.yBtn = Dialog.bounds.y+0.025;
	Dialog.xText = Dialog.promptBounds.x;
	Dialog.yText = Dialog.promptBounds.y2+0.07;
	Dialog.components = makeComponentsList(Dialog.buttons);
	int i;
	for(i=1;i<DIALOG_NBBUTTONS;i++)
	{
		addComponent(Dialog.buttons+i,&Dialog.components);
	}
	addComponent(&Dialog.btnDown,&Dialog.components);
	addComponent(&Dialog.btnUp,&Dialog.components);
	addComponent(&Dialog.slider,&Dialog.components);
	Dialog.radioButtons = NULL;
	desactiveDialog();
}

void Dialog_draw(){
	glColor4f(DIALOGBACK.r,DIALOGBACK.g,DIALOGBACK.b,0.9);
	drawCarre(Dialog.bounds.x,Dialog.bounds.y,Dialog.bounds.x2,Dialog.bounds.y2);
	float x, y;
	glColor4f(DIALOGFORE.r,DIALOGFORE.g,DIALOGFORE.b,DIALOGFORE.a);
	if(Dialog.text!=NULL){
		writeString(Dialog.xText,Dialog.yText,Dialog.text);
	}
	drawAllComponents(Dialog.components);
	if(Dialog.prompt){
		glColor4f(WHITE.r,WHITE.g,WHITE.b,1);
		drawCarre(Dialog.promptBounds.x,Dialog.promptBounds.y,Dialog.promptBounds.x2,Dialog.promptBounds.y2);
		glColor4f(BLACK.r,BLACK.g,BLACK.b,1);
		drawCarreVide(Dialog.promptBounds.x,Dialog.promptBounds.y,Dialog.promptBounds.x2,Dialog.promptBounds.y2);
		float height = 2.3*(float)glutBitmapWidth(GLUT_BITMAP_8_BY_13,'_') * 8./(13.*(float)screenHeight());
		StringChar* current;
		x = Dialog.promptBounds.x*1.15;
		y = Dialog.promptBounds.y + Dialog.promptBounds.height -height;
		glRasterPos2f(x, y);
		for(current = Dialog.input.first;current!=NULL;current = current->next){
			if(current->c == '\n')
			{
				y -= height;
				glRasterPos2f(x, y);
			}
			else
				glutBitmapCharacter(GLUT_BITMAP_8_BY_13, current->c);
		}
	}
}

void activeDialog(const char *text, int flag, void (*closeHandle)(DIALOGBTNS)){
	if(Dialog.text != NULL) free(Dialog.text);
	if(Dialog.input.size) freeString(&Dialog.input);
	Dialog.text = NULL;
	Imagimp.dialogMode = 1;
	if(text[0]){
		Dialog.text = malloc(sizeof(char)*strlen(text)+1);
		strcpy(Dialog.text,text);
	}
	Dialog.buttons[BTN_OK].invisible = !(flag & FLAGS_OK);
	Dialog.buttons[BTN_YES].invisible = !(flag & FLAGS_YES);
	Dialog.buttons[BTN_NO].invisible = !(flag & FLAGS_NO);
	Dialog.buttons[BTN_CANCEL].invisible = !(flag & FLAGS_CANCEL);

	if(flag & FLAGS_COMPONENTSET){
		Dialog.btnUp.invisible =
		Dialog.btnDown.invisible = 0;
		Dialog.listing = 1;
		updateDialogListing();
	}
	else{
		Dialog.btnUp.invisible =
		Dialog.btnDown.invisible = 1;
		Dialog.listing = 0;
	}

	Dialog.prompt = flag & FLAGS_PROMPT;
	Dialog.choice = flag & FLAGS_RADIOBUTTON;
	Dialog.slider.invisible = !(flag & FLAGS_SLIDER);
	Dialog.closeHandle = closeHandle;

	float x = Dialog.xBtn;
	float y = Dialog.yBtn;
	int i;
	for(i=0;i<DIALOG_NBBUTTONS;i++){
		if(!Dialog.buttons[i].invisible){
			Dialog.buttons[i].bounds.x = x;
			Dialog.buttons[i].bounds.x2 = x + Dialog.buttons[i].bounds.width;
			Dialog.buttons[i].bounds.y = y;
			Dialog.buttons[i].bounds.y2 = y + Dialog.buttons[i].bounds.height;
			x -= 0.10;
		}
		else{
			Dialog.buttons[i].bounds.x = -1;
			Dialog.buttons[i].bounds.x2 =-1;
			Dialog.buttons[i].bounds.y = -1;
			Dialog.buttons[i].bounds.y2 = -1;
		}
	}
	glutPostRedisplay();
}

void handleKeyboardTexte(unsigned char ascii, int x, int y) {
	switch(ascii) {
	case ESCAPE_KEY: // Touche Escape
		if(!Dialog.buttons[BTN_NO].invisible)
			answerNO();
		else if(!Dialog.buttons[BTN_CANCEL].invisible)
			answerCANCEL();
		else if(!Dialog.buttons[BTN_OK].invisible)
			answerOK();
		break;
	case DEL_KEY:
		removeLastCharacter(&(Dialog.input));
		break;
	case BACKSPACE_KEY:
		removeLastCharacter(&(Dialog.input));
		break;
	case ENTER_KEY:
		if(!Dialog.buttons[BTN_YES].invisible)
			answerYES();
		else if(!Dialog.buttons[BTN_OK].invisible)
			answerOK();
		else
			addCharacter(&Dialog.input,'\n');
		break;
	default :
		if(Dialog.prompt){
			addCharacter(&(Dialog.input),ascii);
		}
		break;
	}
	glutPostRedisplay();

}

void desactiveDialog(){
	Imagimp.dialogMode = 0;
	Dialog.prompt = 0;
	Dialog.choice = 0;
	while(Dialog.components->componenent != &Dialog.slider)
		removeFirstComponent(&Dialog.components);

	while(Dialog.componentsSet != NULL){
		removeFirstComponent(&Dialog.componentsSet);
	}

	Dialog.radioButtons = NULL;
	Dialog.componentsSet = NULL;
	Dialog.nbRadioButtons = 0;
	Dialog.nbComponentsInSet = 0;
	Dialog.slider.invisible = 1;
	Dialog.slider.clickHandle = 0;
	Dialog.componentOffSet=0;
	glutPostRedisplay();
}

void addRadioButtonInDialog(Component *radioButton){
	ComponentsList* otherButton = Dialog.radioButtons;
	while(otherButton!=NULL){
		addButtonToRadioButtonList(otherButton->componenent,radioButton);
		otherButton = otherButton->next;
	}
	addComponent(radioButton,&Dialog.radioButtons);
	addComponent(radioButton,&Dialog.components);
	radioButton->bounds = makeBounds(Dialog.bounds.x +0.03+ 0.1*(Dialog.nbRadioButtons%4),
									 Dialog.promptBounds.y2 - (Dialog.nbRadioButtons/4)*0.08,
									 radioButton->bounds.width,radioButton->bounds.height);
	Dialog.nbRadioButtons++;
}

void addComponentInDialog(Component *component){
	addComponent(component,&Dialog.componentsSet);
	addComponent(component,&Dialog.components);
	Dialog.nbComponentsInSet++;
	updateDialogListing();
}

void updateDialogListing(){
	ComponentsList* pointer = Dialog.componentsSet;
	int i = 0;
	while(pointer!=NULL && i<Dialog.componentOffSet){
		i++;
		pointer->componenent->invisible = 1;
		pointer = pointer->next;
	}
	i=0;
	while(pointer!=NULL && i<3){
		pointer->componenent->invisible = 0;
		pointer->componenent->bounds = makeBounds(Dialog.btnDown.bounds.x,
												  Dialog.btnDown.bounds.y+0.04+i*0.05,
												  Dialog.btnDown.bounds.width,
												  Dialog.btnDown.bounds.height);
		if(pointer->componenent->type==DELETABLEBUTTON)
			updateDeletableButtonPos(pointer->componenent);
		i++;
		pointer = pointer->next;
	}
	if(pointer!=NULL){
		do{
			pointer->componenent->invisible = 1;
			pointer = pointer->next;
		}while(pointer!=NULL);
		Dialog.btnUp.inactiv = 0;
	}
	else
	{
		Dialog.btnUp.inactiv = 1;
	}

	if(Dialog.componentOffSet<=0){
		Dialog.btnDown.inactiv = 1;
	}
	else{
		Dialog.btnDown.inactiv = 0;
	}
}

void freeDialogSetButtons(){
	ComponentsList* list = Dialog.componentsSet;
	while(list!=NULL){
		freeComponent(list->componenent);
		list = list->next;
	}
}
