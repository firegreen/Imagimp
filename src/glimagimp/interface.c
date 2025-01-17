/***************************************************************************
 *   Copyright (C) 2005 by Venceslas BIRI                                  *
 *   biri@univ-mlv.fr                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "outils.h"
#include "interface.h"
#include <stdlib.h>
#include <string.h>

#define COMPUTE_FPS				1
#define NOMBRE_SAMPLE_FPS		100

#ifdef WIN32
void initGLextensions() {
	glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
}
#include <windows.h>
#else
#include <unistd.h>
#endif

/// ///////////////////////////////////////////////////////////////////////////
/// outil GL
/// ///////////////////////////////////////////////////////////////////////////

void __check_gl(int line, const char *filename) {
	int err = glGetError();
	if(err != GL_NO_ERROR) {
		printf("ERROR GL : erreur dans le fichier %s à la ligne %d : %s\n",filename,line,gluErrorString(err));
		exit(0);
	}
}

#define CHECK_GL __check_gl(__LINE__, __FILE__)


/// ///////////////////////////////////////////////////////////////////////////
/// variables globales
/// ///////////////////////////////////////////////////////////////////////////

/// L'ecran

struct {
	unsigned int width;
	unsigned int height;
	float zoom;
	Bounds bounds;
	GLuint textureID;
} currentImage;

struct {
	unsigned int width;
	unsigned int height;
	unsigned char fullscreen;
	float UIstartX;
	float UIwidth;
	Color backgroundUI;
	Color backgroundImage;
} screen;

/// Flag pour le dessin
struct {
	int flags[5];
	int debug;
	int save;
	int modified_tampon;
	unsigned int cptidletime;
	unsigned int instantPrec;
	void (*dessin_screen)(void);
} dessin;

/// Fonction de callback fixé par l'utilisateur
struct {
	void (*click_souris)(int,int,float,float);
	void (*motion_souris)(float,float,float,float,int);
	void (*press_clavier)(unsigned char,int,int, char ,char);
	void (*press_spec_clavier)(int,int,int);
	float oldX,oldY;
	int pressedButton;
	char ALT,CTRL;
} control;


/// ///////////////////////////////////////////////////////////////////////////
/// Fonctions de definition des callbacks user
/// ///////////////////////////////////////////////////////////////////////////
void fixeFonctionClavier(void (*fct)(unsigned char,int,int, char CTRL, char ALT)) {
	control.press_clavier = fct;
}
void fixeFonctionClavierSpecial(void (*fct)(int,int,int)) {
	control.press_spec_clavier = fct;
}
void fixeFonctionClicSouris(void (*fct)(int, int, float, float)) {
	control.click_souris = fct;
}

void fixeFonctionMotionSouris(void (*fct)(float x, float y, float deltaX, float deltaY, int drag)){
	control.motion_souris = fct;
}

void fixeFonctionDessin(void (*fct)(void)) {
	dessin.dessin_screen = fct;
	glutPostRedisplay();
}

void actualiseImage(unsigned char* newImage, unsigned int width, unsigned int height) {
	currentImage.height = height;
	currentImage.width = width;
	currentImage.bounds = makeBounds(currentImage.bounds.x,currentImage.bounds.y,
									 (float)width/(float)screen.width,(float)height/(float)screen.height);
	glBindTexture(GL_TEXTURE_2D, currentImage.textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, newImage);
	glBindTexture(GL_TEXTURE_2D, 0);
	glutPostRedisplay();
}


/// ///////////////////////////////////////////////////////////////////////////
/// Corps des fonctions IHM
/// ///////////////////////////////////////////////////////////////////////////
void clickMouse_GLIMAGIMP(int button,int state,int x,int y) {
	if (control.click_souris != NULL) {
		float xGL = (float)x/(float)screenWidth();
		float yGL = 1.-(float)y/(float)screenHeight();
		if(state == GLUT_DOWN)
			control.pressedButton = button;
		else
			control.pressedButton = -1;
		(*control.click_souris)(button,state,xGL,yGL);
	}
	else {
		printf("Fonction souris non fixée\n");
	}
}
void motionMouse_GLIMAGIMP(int x,int y) {
	float xGL = (float)x/(float)screenWidth();
	float yGL = 1.-(float)y/(float)screenHeight();

	float deltaX,deltaY;
	if(control.oldX<0)
		deltaX = 0;
	else
		deltaX = xGL - control.oldX;
	if(control.oldY<0)
		deltaY = 0;
	else
		deltaY = yGL - control.oldY;
	control.oldX = xGL;
	control.oldY = yGL;
	if (control.motion_souris != NULL) {
		(*control.motion_souris)(xGL,yGL,deltaX,deltaY,control.pressedButton);
	}
	else {
		printf("Fonction motion souris non fixée\n");
	}
}

void kbdSpFunc_GLIMAGIMP(int c, int x, int y) {
	if (control.press_spec_clavier != NULL) {
		control.ALT = glutGetModifiers() == GLUT_ACTIVE_ALT;
		control.CTRL =glutGetModifiers() == GLUT_ACTIVE_CTRL;
		(*control.press_spec_clavier)(c,x,y);
		printf("%d %d\n",control.CTRL,control.ALT);
	}
	else {
		printf("Fonction clavier touche spéciales non fixée\n");
	}
}

void kbdSpUpFunc_GLIMAGIMP(int c, int x, int y) {

}

void kbdFunc_GLIMAGIMP(unsigned char c, int x, int y) {
	if (control.press_clavier != NULL) {
		(*control.press_clavier)(c,x,y, control.CTRL, control.ALT);
	}
	else {
		printf("Fonction clavier non fixée\n");
	}
	control.ALT = glutGetModifiers() == GLUT_ACTIVE_ALT;
	control.CTRL =glutGetModifiers() == GLUT_ACTIVE_CTRL;
}


/// ///////////////////////////////////////////////////////////////////////////
/// Corps des fonctions de rendu GLUT
/// ///////////////////////////////////////////////////////////////////////////

/// ///////////////////////////////////////////////////////////////////////////
/// fonction de changement de dimension de la fenetre param?tres :
/// - width  : largeur (x) de la zone de visualisation
/// - height : hauteur (y) de la zone de visualisation
void reshapeFunc(int width,int height) {
	// ON IGNORE LE RESIZE
	if(!screen.fullscreen){
		screen.width = width;
		screen.height = height;
	}
	currentImage.bounds.width = (float)currentImage.width / (float)width;
	currentImage.bounds.height = (float)currentImage.height / (float)height;
	// printf("Nouvelle taille %d %d\n",screen.width,screen.height);
	glViewport( 0, 0, (GLint)width, (GLint)height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D(0.0,1.0,0.0,1.0);
	dessin.modified_tampon = 1;
	glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////////
/// fonction d'exectution de process en attendant un evenement
void idleFunc(void)
{
	dessin.cptidletime++;
	int instant = glutGet(GLUT_ELAPSED_TIME);
	int t=(instant-dessin.instantPrec);
	dessin.instantPrec = instant;
	int fpms = 1. / (NOMBRE_SAMPLE_FPS/1000.);
	if(fpms > t)
#ifdef WIN32
		Sleep(fpms - t);
#else
		usleep((fpms - t)*1000);
#endif
	if (dessin.debug && COMPUTE_FPS) {
		printf("time elapsed : %d\n",t);
		printInfo();
		glutPostRedisplay();
	}
}


/// ///////////////////////////////////////////////////////////////////////////
/// Corps des fonctions de dessin
/// ///////////////////////////////////////////////////////////////////////////
void initDisplay() {

	dessin.flags[0] = 1;
	dessin.flags[1] = 0;
	dessin.flags[2] = 0;
	dessin.flags[3] = 0;
	dessin.flags[4] = 0;

	dessin.instantPrec = glutGet(GLUT_ELAPSED_TIME);

	/// INITIALISATION DES TEXTURES ...
	glDisable(GL_TEXTURE_2D);
#ifdef WIN32
	initGLextensions();
#endif
	glActiveTextureARB(GL_TEXTURE0_ARB);

	/// INITIALISATION CLASSIQUE OPENGL ...
	glDisable(GL_NORMALIZE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glMatrixMode(GL_MODELVIEW);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_BLEND);
	glDisable(GL_FOG);
	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);

	CHECK_GL;
	screen.backgroundUI = makeColor(0,0,0,0);
	screen.backgroundImage = makeColor(0,0,0,0);
	screen.UIstartX = 0.8;
	screen.UIwidth =  0.2;
	//printf("Fin initialisation display\n");
}

void drawImage(){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, currentImage.textureID);

	glBegin(GL_QUADS);

	glColor3f(1, 1, 1);

	glTexCoord2f(0, 0);
	glVertex2f(0, 1);

	glTexCoord2f(0, 1);
	glVertex2f(0,0);

	glTexCoord2f(1, 1);
	glVertex2f(1,0);

	glTexCoord2f(1, 0);
	glVertex2f(1,1);

	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void drawScene_GLIMAGIMP(void) {
	dessin.modified_tampon = 1;

	// ****************** MISE EN PLACE DES MATRICES DE DEPLACEMENT CAMERA ***********
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glClearColor(screen.backgroundImage.r,screen.backgroundImage.g,screen.backgroundImage.b,screen.backgroundImage.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/// ****************** DESSIN DE L'IMAGE ***************************************
	glTranslatef(screen.UIstartX/2.+currentImage.bounds.x,0.5+currentImage.bounds.y,0);
	glScalef(currentImage.zoom, currentImage.zoom,1);
	glTranslatef( -currentImage.bounds.width/2.,
				  -currentImage.bounds.height/2.,0);
	glScalef(currentImage.bounds.width,currentImage.bounds.height,1);
	drawImage();
	glLoadIdentity();

	glColor3f(screen.backgroundUI.r,screen.backgroundUI.g,screen.backgroundUI.b);
	drawCarre(screen.UIstartX,0,1,1);

	glColor3f(0.3,0.42,0.45);
	drawCarre(screen.UIstartX-0.005,0,screen.UIstartX,1);
	glColor3f(0.25,0.3,0.26);
	glLineWidth(3);
	drawCarreVide(screen.UIstartX-0.005,-0.1,screen.UIstartX,1.1);
	glLineWidth(1);
	/// ****************** DESSIN CLASSIQUE ****************************************
	if (dessin.dessin_screen != NULL) {
		(*dessin.dessin_screen)();
	}
	/// ****************** SAUVEGARDE FICHIER **************************************
	if (dessin.save) {
		printf("Sauvegarde dans le fichier");
		dessin.save = 0;
	}
	glDisable(GL_BLEND);
	glutSwapBuffers();
}

/// ///////////////////////////////////////////////////////////////////////////
/// Fonction d'initialisation
/// ///////////////////////////////////////////////////////////////////////////
void initGLIMAGIMP_IHM(unsigned int w_im,unsigned int h_im,unsigned char *tabRVB,
					   unsigned int w_ecran,unsigned int h_ecran, int fullscreen)
{
	////// parametres GL /////////////////////////////////////
	dessin.cptidletime = 0;

	dessin.debug = 0;
	dessin.save = 0;

	screen.width = w_ecran;
	screen.height = h_ecran;
	screen.fullscreen = fullscreen;
	currentImage.width = w_im;
	currentImage.height = h_im;
	currentImage.zoom = 1;
	currentImage.bounds = makeBounds(0,0,(float)w_im/(float)w_ecran,(float)h_im/(float)h_ecran);

	control.click_souris = NULL;
	control.press_clavier = NULL;
	control.press_spec_clavier = NULL;
	control.motion_souris = NULL;
	control.oldX = control.oldY = -1;
	control.pressedButton = -1;
	control.ALT = control.CTRL = 0;

	dessin.dessin_screen = NULL;


	int argc = 0;
	char** argv = (char**) malloc(sizeof(char*));
	argv[0] = (char*) calloc(20,sizeof(char));
	sprintf(argv[0],"imagimp");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);


	float width = glutGet(GLUT_SCREEN_WIDTH);
	float height = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitWindowPosition(width/2 - screen.width/2,
						   height/2 - screen.height/2);
	glutInitWindowSize(screen.width, screen.height);

	if (glutCreateWindow("IMAGIMP") == GL_FALSE) {
		printf("Impossible de créer la fenetre GL\n");
		exit(1);
	}
	initDisplay();
	glGenTextures(1, &currentImage.textureID);
	if(currentImage.textureID){
		glBindTexture(GL_TEXTURE_2D, currentImage.textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w_im, h_im, 0, GL_RGB, GL_UNSIGNED_BYTE, tabRVB);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	/* association de la fonction de dimensionnement */
	glutReshapeFunc(reshapeFunc);
	/* association de la fonction d'affichage */
	glutDisplayFunc(drawScene_GLIMAGIMP);
	/* association de la fonction d'événement souris */
	glutMouseFunc(clickMouse_GLIMAGIMP);
	/* association de la fonction de DRAG */
	glutPassiveMotionFunc(motionMouse_GLIMAGIMP);
	glutMotionFunc(motionMouse_GLIMAGIMP);
	/* association de la fonction d'événement du clavier */
	glutKeyboardFunc(kbdFunc_GLIMAGIMP);
	/* association de la fonction de traitement des touches*/
	/* spéciales du clavier                                */
	glutSpecialFunc(kbdSpFunc_GLIMAGIMP);
	glutSpecialUpFunc(kbdSpUpFunc_GLIMAGIMP);
	/* fonction d'attente */
	glutIdleFunc(idleFunc);
	if (fullscreen){
		glutFullScreen();
	}
}

/// ///////////////////////////////////////////////////////////////////////////
/// Fonction d'initialisation
/// ///////////////////////////////////////////////////////////////////////////
void initGLIMAGIMP(unsigned int w,unsigned int h,unsigned char *tabRVB)
{
	initGLIMAGIMP_IHM(w,h,tabRVB,w,h,1);
}


void launchApp(){
	glutMainLoop();
}

void setBackground(Color backUI, Color backImage){
	screen.backgroundUI = backUI;
	screen.backgroundImage = backImage;
}

float getUIStartX(){
	return screen.UIstartX;
}

float getUIWidth(){
	return screen.UIwidth;
}

/// ///////////////////////////////////////////////////////////////////////////
/// AUTRES OUTILS
/// ///////////////////////////////////////////////////////////////////////////

void printInfo() {
	printf("\n");
	printf("**************************************\n");
	printf("              INFORMATIONS            \n");
	printf("Taille ecran : %d/%d\n",screen.width,screen.height);
	printf("Taille image : %d/%d\n",currentImage.width,currentImage.height);
	printf("Bouton de la souris presse:");
	switch (control.pressedButton) {
	case GLUT_LEFT_BUTTON:
		printf("GAUCHE");
		break;
	case GLUT_RIGHT_BUTTON:
		printf("DROITE");
		break;
	case GLUT_MIDDLE_BUTTON:
		printf("MILIEU");
		break;
	default:
		printf("AUCUN");
		break;
	}
	printf("\n");
	printf("Position Souris:(%f,%f)\n",control.oldX,control.oldY);
	printf("**************************************\n");
}

unsigned int screenHeight() { return screen.height; }
unsigned int screenWidth() { return screen.width; }
int isFullscreen() { return screen.fullscreen; }

void saveDessin(){
	dessin.save = 1;
}

void modeDebug(int debug){
	dessin.debug = debug;
}

void translateImage(float x, float y){
	currentImage.bounds =makeBounds(currentImage.bounds.x + x, currentImage.bounds.y +y,
									currentImage.bounds.width, currentImage.bounds.height);
}

void zoomPlus(){
	currentImage.zoom *= 1.25;
	if(currentImage.zoom>10.)
		currentImage.zoom = 3.;
	else if(currentImage.zoom>0.9 && currentImage.zoom<1.1)
		currentImage.zoom = 1;
}


void zoomMoins(){
	currentImage.zoom *= 0.8;
	if(currentImage.zoom<0.33)
		currentImage.zoom = 0.3;
	else if(currentImage.zoom>0.9 && currentImage.zoom<1.1)
		currentImage.zoom = 1;
}

float zoom(){
	return currentImage.zoom;
}

Bounds imageBounds(){
	Bounds b;

	b.x = -currentImage.bounds.width/2.;
	b.x *= currentImage.zoom;
	b.x += screen.UIstartX/2.+currentImage.bounds.x;

	b.y = -currentImage.bounds.height/2.;
	b.y *= currentImage.zoom;
	b.y += 0.5+currentImage.bounds.y;

	b.width = currentImage.bounds.width*currentImage.zoom;
	b.height = currentImage.bounds.height*currentImage.zoom;
	b.x2 = b.x+b.width;
	b.y2 = b.y+b.height;
	return b;
}

void GlPosToImagePos(float glX, float glY, float *x, float *y){
	//x = glX/currentImage.zoom - currentImage.bounds.x
	*x = glX - screen.UIstartX/2.-currentImage.bounds.x;
	*x /= currentImage.zoom;
	*x += currentImage.bounds.width/2.;
	*x /= currentImage.bounds.width;

	*y = glY - 0.5-currentImage.bounds.y;
	*y /= currentImage.zoom;
	*y += currentImage.bounds.height/2.;
	*y /= currentImage.bounds.height;
}

void setFullsreen(int fullscreen){
	screen.fullscreen = fullscreen;
	if(fullscreen) {
		glutFullScreen();
		float width = glutGet(GLUT_SCREEN_WIDTH);
		float height = glutGet(GLUT_SCREEN_HEIGHT);
		glutReshapeWindow(width,height);
	}
	else{
		glutReshapeWindow(screen.width,screen.height);
	}
}

/// ///////////////////////////////////////////////////////////////////////////
/// Label
/// ///////////////////////////////////////////////////////////////////////////

Component makeLabel(char *text, Bounds bounds, Color fore, void *font){
	Component l;
	l.type = LABEL;
	l.extends.Button.text = malloc(sizeof(char)*strlen(text)+1);
	strcpy(l.extends.Button.text,text);
	l.extends.Label.font = font;
	l.back = makeColor(0,0,0,0);
	l.fore = fore;
	l.bounds = bounds;
	l.clickHandle = NULL;
	l.press = 0;
	l.hover = 0;
	l.inactiv =0;
	l.invisible=0;
	return l;
}

void drawLabel(const Component* l){
	glColor4f(l->fore.r,l->fore.g,l->fore.b,l->fore.a);
	float width = 1.1*(float)glutBitmapLength(l->extends.Label.font, (unsigned char*)(l->extends.Label.text))
			/(float)screen.width;
	float height = (float)glutBitmapWidth(l->extends.Label.font,'_')/(float)screen.height;
	writeString(l->bounds.x + l->bounds.width/2. - width/2., l->bounds.y + l->bounds.height/2. - height/2. ,
				l->extends.Button.text);
}

/// ///////////////////////////////////////////////////////////////////////////
/// Buttons
/// ///////////////////////////////////////////////////////////////////////////

Component makeButton(char *text, Bounds bounds, Color fore, Color back,
					 void (*clickHandle)(const void*)){
	Component b;
	b.type = BUTTON;
	b.extends.Button.text = malloc(sizeof(char)*(strlen(text)+1));
	strcpy(b.extends.Button.text,text);
	b.bounds = bounds;
	b.fore = fore;
	b.back = back;
	b.clickHandle = clickHandle;
	b.press = 0;
	b.hover = 0;
	b.inactiv =0;
	b.invisible=0;
	return b;
}

void privateDrawButton(const Component* b,const Color* fore, const Color* back){
	glColor4f(back->r,back->g,back->b,back->a);
	drawCarre(b->bounds.x,b->bounds.y,b->bounds.x2,b->bounds.y2);
	glColor4f(fore->r,fore->g,fore->b,fore->a);
	drawCarreVide(b->bounds.x,b->bounds.y,b->bounds.x2,b->bounds.y2);
	float width = (float)glutBitmapLength(GLUT_BITMAP_8_BY_13,
										  (unsigned char*)(b->extends.Button.text))/(float)screen.width;
	float height = (float)glutBitmapWidth(GLUT_BITMAP_8_BY_13,'_') * 8./(13.*(float)screen.height);
	if(b->type==CHECKBUTTON || b->type==RADIOBUTTON){
		if(b->extends.RadioButton.isSelected){
			drawDisque(b->bounds.x2 - b->bounds.width*0.1,
					   b->bounds.y2 - b->bounds.height*0.5,
					   b->bounds.height*0.07);
		}
		writeString(b->bounds.x + b->bounds.width*0.8/2. - width/2.,
					b->bounds.y + b->bounds.height/2. - height/2.,b->extends.Button.text);
	}
	else if(b->type==DELETABLEBUTTON){
		drawLigne(b->extends.DeletableButton.closeBounds.x,
				  b->extends.DeletableButton.closeBounds.y,
				  b->extends.DeletableButton.closeBounds.x2,
				  b->extends.DeletableButton.closeBounds.y2);
		drawLigne(b->extends.DeletableButton.closeBounds.x,
				  b->extends.DeletableButton.closeBounds.y2,
				  b->extends.DeletableButton.closeBounds.x2,
				  b->extends.DeletableButton.closeBounds.y);
		writeString(b->bounds.x + (b->bounds.width-b->extends.DeletableButton.closeBounds.width)/2. - width/2.,
					b->bounds.y + b->bounds.height/2. - height/2.,b->extends.Button.text);
	}
	else{
		writeString(b->bounds.x + b->bounds.width/2. - width/2.,
					b->bounds.y + b->bounds.height/2. - height/2.,b->extends.Button.text);
	}

}

void drawButton(const Component *b){
	if(b->inactiv){
		float moy = (b->fore.r+b->fore.g+b->fore.b)/3.;
		Color fore = makeColor(moy,moy,moy,b->fore.a);
		moy = (b->back.r+b->back.g+b->back.b+moy)/3.;
		Color back = makeColor(moy,moy,moy,b->fore.a);
		privateDrawButton(b,&back,&fore);
	}
	else{
		if(b->press){
			privateDrawButton(b,&b->back,&b->fore);
		}
		else if(b->hover)
		{
			Color fore = makeColor(b->fore.r+0.2,b->fore.g+0.4,
								   b->fore.b+0.4,b->fore.a);
			privateDrawButton(b,&fore,&b->back);
		}
		else{
			privateDrawButton(b,&b->fore,&b->back);
		}
	}
}

void pressButton(Component *b){
}

void releaseButton(Component *b, int activeAction){
	if(activeAction && !b->inactiv && !b->invisible && b->clickHandle!=NULL)
		(*b->clickHandle)(NULL);
}

void releaseRadioButton(Component *b, int activeAction){
	if(activeAction	&& !b->invisible && b->clickHandle!=NULL){
		selectRadioButton(b);
		if(b->extends.RadioButton.isSelected)
			(*b->clickHandle)(NULL);
	}
}

void selectRadioButton(Component *b){
	if(!b->inactiv){
		b->extends.RadioButton.isSelected = 1;
		ComponentsList* l = b->extends.RadioButton.othersRadioButton;
		while(l!=NULL){
			l->componenent->extends.RadioButton.isSelected = 0;
			l = l->next;
		}
	}
}


void hoverButton(Component *b){
}

void leaveButton(Component *b){
}

void setButtonText(Component *b, char* text){
	free(b->extends.Button.text);
	b->extends.Button.text = malloc(sizeof(char)*strlen(text)+1);
	strcpy(b->extends.Button.text,text);
}

Component makeRadioButton(char *text, Bounds bounds, Color fore, Color back, void (*clickHandle)(const void *)){
	Component c = makeButton(text,bounds,fore,back,clickHandle);
	c.type = RADIOBUTTON;
	c.extends.RadioButton.isSelected = 0;
	c.extends.RadioButton.othersRadioButton = NULL;
	return c;
}

void addButtonToRadioButtonList(Component *radioButton1, Component *radioButton2){
	addComponent(radioButton1,&radioButton2->extends.RadioButton.othersRadioButton);
	addComponent(radioButton2,&radioButton1->extends.RadioButton.othersRadioButton);
}

Component makeCheckButton(char *text, Bounds bounds, Color fore, Color back, void (*clickHandle)(const void *)){
	Component c = makeButton(text,bounds,fore,back,clickHandle);
	c.type = CHECKBUTTON;
	c.extends.CheckButton.isSelected = 0;
	return c;
}


Component makeDeletableButton(char *text, Bounds bounds, Color fore, Color back, void (*clickHandle)(const void *)){
	Component c = makeButton(text,bounds,fore,back,clickHandle);
	c.type = DELETABLEBUTTON;
	c.extends.DeletableButton.closeBounds = makeBounds(c.bounds.x2 - c.bounds.width*0.12,
													   c.bounds.y2 - c.bounds.height*0.7,
													   c.bounds.width*0.04,
													   c.bounds.height*0.4);
	c.extends.DeletableButton.needToBeDelete = 0;
	return c;
}

void updateDeletableButtonPos(Component *c){
	c->extends.DeletableButton.closeBounds = makeBounds(c->bounds.x2 - c->bounds.width*0.12,
													   c->bounds.y2 - c->bounds.height*0.7,
													   c->bounds.width*0.04,
													   c->bounds.height*0.4);
}

void releaseDeleatableButton(Component *b, int activeAction){
	if(activeAction && !b->invisible && b->clickHandle!=NULL){
		b->extends.DeletableButton.needToBeDelete = 1;
		(*b->clickHandle)(NULL);
	}
}

/// ///////////////////////////////////////////////////////////////////////////
/// Sliders
/// ///////////////////////////////////////////////////////////////////////////
void makeCursorBounds(Component* s){
	float height = (float)glutBitmapWidth(GLUT_BITMAP_8_BY_13,'_') * 8./(13.*(float)screen.height);
	float h = (s->bounds.y2-height) - s->bounds.y;
	s->extends.Slider.cursorBounds =
			makeBounds(s->bounds.x*1.1 + ((s->extends.Slider.value-s->extends.Slider.min)/(s->extends.Slider.max - s->extends.Slider.min)-0.01)
					   * s->bounds.width*0.8,
					   s->bounds.y+h*0.3,0.016 * s->bounds.width,h*0.5);
}

Component makeSlider(Bounds bounds, Color fore, Color back,
					 void (*setHandle)(const void*)){
	Component s;
	s.type = SLIDER;
	s.bounds = bounds;
	s.fore = fore;
	s.back = back;
	s.clickHandle = setHandle;
	s.press = 0;
	s.hover = 0;
	s.inactiv =0;
	s.invisible=0;
	s.extends.Slider.value = 0.5f;
	s.extends.Slider.max = 1;
	s.extends.Slider.min = 0;
	makeCursorBounds(&s);
	return s;
}

#define scursorBounds s->extends.Slider.cursorBounds
void privateDrawSlider(const Component* s,const Color* foreCursor,const Color* backCursor, const Color* fore){
	float height = (float)glutBitmapWidth(GLUT_BITMAP_8_BY_13,'_') * 8./(13.*(float)screen.height);
	char strMIN[10], strMAX[10], valueStr[10];
	float h2 = (s->bounds.y2-height) - s->bounds.y;
	float width = (float)glutBitmapLength(GLUT_BITMAP_8_BY_13,(unsigned char*)(valueStr))/(float)screen.width;

	memset(strMIN,0,10); memset(strMAX,0,10); memset(valueStr,0,10);

	glColor4f(fore->r,fore->g,fore->b,fore->a);
	drawLigne(s->bounds.x*1.1,s->bounds.y + h2/2. + height/2.,
			  s->bounds.x*1.1+s->bounds.width*0.8,s->bounds.y + h2/2. + height/2.);
	sprintf(strMIN,"%3.1f%%",s->extends.Slider.min*100);
	sprintf(strMAX,"%3.1f%%",s->extends.Slider.max*100);
	writeString(s->bounds.x-0.02,s->bounds.y+ h2/2,strMIN);
	writeString(s->bounds.x2,s->bounds.y + h2/2,strMAX);
	sprintf(valueStr,"%3.2f%%",s->extends.Slider.value*100);
	writeString(s->bounds.x + s->bounds.width/2. - width/2.,
				s->bounds.y,valueStr);
	glColor4f(backCursor->r,backCursor->g,backCursor->b,backCursor->a);
	drawCarre(scursorBounds.x,scursorBounds.y,scursorBounds.x2,scursorBounds.y2);
	glColor4f(foreCursor->r,foreCursor->g,foreCursor->b,foreCursor->a);
	drawCarreVide(scursorBounds.x,scursorBounds.y,scursorBounds.x2,scursorBounds.y2);
}

void drawSlider(const Component *s){
	if(s->inactiv){
		float moy = (s->fore.r+s->fore.g+s->fore.b)/3.;
		Color fore = makeColor(moy,moy,moy,s->fore.a);
		moy = (s->back.r+s->back.g+s->back.b+moy)/3.;
		Color back = makeColor(moy,moy,moy,s->fore.a);
		privateDrawSlider(s,&back,&fore,&s->fore);
	}
	else{
		if(s->press){
			privateDrawSlider(s,&s->back,&s->fore,&s->fore);
		}
		else if(s->hover)
		{
			Color fore = makeColor(s->fore.r+0.2,s->fore.g+0.4,
								   s->fore.b+0.4,s->fore.a);
			privateDrawSlider(s,&fore,&s->back,&s->fore);
		}
		else{
			privateDrawSlider(s,&s->fore,&s->back,&s->fore);
		}
	}
}

void pressSlider(Component *s){
}

void releaseSlider(Component *s, int activeAction){
	if(activeAction && !s->inactiv && !s->invisible && s->clickHandle!=NULL)
		(*s->clickHandle)(&s->extends.Slider.value);
}

void hoverSlider(Component *s){
}

void leaveSlider(Component *s){
}

void setSliderValueFromPos(Component *s, float x){
	s->extends.Slider.value = s->extends.Slider.min +
			((x-s->bounds.x*1.1)/(s->bounds.width*0.8)) * (s->extends.Slider.max - s->extends.Slider.min);
	s->extends.Slider.value = fminf(s->extends.Slider.max,fmaxf(s->extends.Slider.min,s->extends.Slider.value));
	makeCursorBounds(s);
}

void setSliderValue(Component *s, float value){
	s->extends.Slider.value = value;
	s->extends.Slider.value = fminf(s->extends.Slider.max,fmaxf(s->extends.Slider.min,s->extends.Slider.value));
	makeCursorBounds(s);
}

void setSliderMax(Component *s, float max){
	s->extends.Slider.max = fmax(s->extends.Slider.min+0.01,max);
	makeCursorBounds(s);
}

void setSliderMin(Component *s, float min){
	s->extends.Slider.min = fmin(s->extends.Slider.max-0.01,min);
	makeCursorBounds(s);
}

///////////////////////////
/// ComponentsList
///////////////////////////

ComponentsList* makeComponentsList(Component *b){
	ComponentsList* list = malloc(sizeof(ComponentsList));
	list->componenent = b;
	list->next = NULL;
	return list;
}

void addComponent(Component *b, ComponentsList **list){
	ComponentsList* temp = *list;
	*list = makeComponentsList(b);
	(*list)->next = temp;
}

void removeFirstComponent(ComponentsList **list){
	ComponentsList* temp = (*list)->next;
	free(*list);
	*list = temp;
}

Component* findComponentInList(float x, float y, ComponentsList *list){
	ComponentsList* pointer;
	for(pointer = list; pointer!=NULL;pointer=pointer->next){
		if(!pointer->componenent->invisible && isInBounds(x,y,&pointer->componenent->bounds))
			return pointer->componenent;
	}
	return NULL;
}

Component* findComponentInArray(float x, float y, Component* buttons, int nbbuttons){
	int i;
	for(i = 0; i<nbbuttons;i++){
		if(!buttons[i].invisible && isInBounds(x,y,&buttons[i].bounds))
			return buttons+i;
	}
	return NULL;
}

void drawAllComponents(const ComponentsList* list){
	const ComponentsList* pointer;
	for(pointer = list; pointer!=NULL;pointer=pointer->next){
		drawComponent(pointer->componenent);
	}
}

void drawComponent(const Component* c){
	if(!c->invisible){
		switch (c->type) {
		case BUTTON:
		case RADIOBUTTON:
		case CHECKBUTTON:
		case DELETABLEBUTTON:
			drawButton(c);
			break;
		case SLIDER:
			drawSlider(c);
			break;
		case LABEL:
			drawLabel(c);
		default:
			break;
		}
	}
}

void pressComponent(Component* c){
	c->press = 1;
	switch (c->type) {
	case BUTTON:
	case RADIOBUTTON:
		pressButton(c);
		break;
	case SLIDER:
		pressSlider(c);
		break;
	default:
		break;
	}
}

void releaseComponent(Component* c, int activeAction){
	c->press = 0;
	switch (c->type) {
	case BUTTON:
		releaseButton(c,activeAction);
		break;
	case RADIOBUTTON:
		releaseRadioButton(c,activeAction);
		break;
	case DELETABLEBUTTON:
		releaseDeleatableButton(c,activeAction);
	case SLIDER:
		releaseSlider(c,activeAction);
		break;
	default:
		break;
	}
}


void hoverComponent(Component* c){
	c->hover = 1;
	switch (c->type) {
	case BUTTON:
	case RADIOBUTTON:
		hoverButton(c);
		break;
	case SLIDER:
		hoverSlider(c);
		break;
	default:
		break;
	}
}

void leaveComponent(Component* c){
	c->hover = 0;
	switch (c->type) {
	case BUTTON:
	case RADIOBUTTON:
		leaveButton(c);
		break;
	case SLIDER:
		leaveSlider(c);
		break;
	default:
		break;
	}
}


void setComponentInvisible(Component* c, int invisible){
	c->invisible = invisible;
}

void setComponentInactiv(Component* c, int inactiv){
	c->inactiv = inactiv;
}

void freeComponent(Component *c){
	switch(c->type){
	case BUTTON:
	case CHECKBUTTON:
	case DELETABLEBUTTON:
	case RADIOBUTTON:
	case LABEL:
		free(c->extends.Label.text);
	default:
		break;
	}
	free(c);
}

void freeAllComponents(ComponentsList *l){
	if(l!=NULL){
		ComponentsList* current = l;
		while(current->next!=NULL){
			ComponentsList* next = current->next;
			switch(current->componenent->type){
			case BUTTON:
			case CHECKBUTTON:
			case DELETABLEBUTTON:
			case RADIOBUTTON:
			case LABEL:
				free(current->componenent->extends.Label.text);
			default:
				break;
			}
			free(current);
			current = next;
		}
	}
}
