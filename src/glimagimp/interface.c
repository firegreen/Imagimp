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
#include <string.h>

#define COMPUTE_FPS				1
#define NOMBRE_SAMPLE_FPS		100

#ifdef WIN32
void initGLextensions() {
    glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
}
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
    unsigned char *src;
} currentImage;

struct {
    unsigned int width;
    unsigned int height;
    unsigned char fullscreen;
    Color background;
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
    void (*click_souris)(int,int,int,int);
    void (*motion_souris)(int,int);
    void (*press_clavier)(unsigned char,int,int);
    void (*press_spec_clavier)(int,int,int);
} control;


/// ///////////////////////////////////////////////////////////////////////////
/// Fonctions de definition des callbacks user
/// ///////////////////////////////////////////////////////////////////////////
void fixeFonctionClavier(void (*fct)(unsigned char,int,int)) {
    control.press_clavier = fct;
}
void fixeFonctionClavierSpecial(void (*fct)(int,int,int)) {
    control.press_spec_clavier = fct;
}
void fixeFonctionClicSouris(void (*fct)(int button,int state,int x,int y)) {
    control.click_souris = fct;
}

void fixeFonctionMotionSouris(void (*fct)(int, int)){
    control.motion_souris = fct;
}

void fixeFonctionDessin(void (*fct)(void)) {
    dessin.dessin_screen = fct;
    glutPostRedisplay();
}

void actualiseImage(unsigned char* newImage) {
    currentImage.src = newImage;
    glutPostRedisplay();
}


/// ///////////////////////////////////////////////////////////////////////////
/// Corps des fonctions IHM
/// ///////////////////////////////////////////////////////////////////////////
void clickMouse_GLIMAGIMP(int button,int state,int x,int y) {
    if (control.click_souris != NULL) {
        (*control.click_souris)(button,state,x,y);
    }
    else {
        printf("Fonction souris non fixée\n");
    }
}
void motionMouse_GLIMAGIMP(int x,int y) {
    if (control.motion_souris != NULL) {
        (*control.motion_souris)(x,y);
    }
    else {
        printf("Fonction motion souris non fixée\n");
    }
}

void kbdSpFunc_GLIMAGIMP(int c, int x, int y) {
    if (control.press_spec_clavier != NULL) {
        (*control.press_spec_clavier)(c,x,y);
    }
    else {
        printf("Fonction clavier touche spéciales non fixée\n");
    }
}

void kbdFunc_GLIMAGIMP(unsigned char c, int x, int y) {
    if (control.press_clavier != NULL) {
        (*control.press_clavier)(c,x,y);
    }
    else {
        printf("Fonction clavier non fixée\n");
    }
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
    // printf("Nouvelle taille %d %d\n",screen.width,screen.height);
    glViewport( 0, 0, (GLint)width, (GLint)height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D(0.0,1.0,0.0,1.0);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    dessin.modified_tampon = 1;
    glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////////
/// fonction d'exectution de process en attendant un evenement
void idleFunc(void)
{
    dessin.cptidletime++;
    if (dessin.debug && COMPUTE_FPS && (dessin.cptidletime%NOMBRE_SAMPLE_FPS == 0)) {
        int instant = glutGet(GLUT_ELAPSED_TIME);
        double t=(instant-dessin.instantPrec)/1000.0;
        printf("FPS : %f\n",NOMBRE_SAMPLE_FPS/t);
        dessin.instantPrec = instant;
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
    glBindTexture(GL_TEXTURE_2D,0);
    glDisable(GL_TEXTURE_2D);

    /// INITIALISATION DES FONCTIONS SPECIALES ...

    /// INITIALISATION CLASSIQUE OPENGL ...
    glDisable(GL_NORMALIZE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_BLEND);
    glDisable(GL_FOG);
    glBindTexture(GL_TEXTURE_2D,0);
    glDisable(GL_TEXTURE_2D);

    CHECK_GL;
    screen.background = makeColor(0,0,0,0);
    //printf("Fin initialisation display\n");
}

void drawScene_GLIMAGIMP(void) {
    dessin.modified_tampon = 1;

    // ****************** MISE EN PLACE DES MATRICES DE DEPLACEMENT CAMERA ***********
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glClearColor(screen.background.r,screen.background.g,screen.background.b,screen.background.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// ****************** DESSIN DE L'IMAGE ***************************************
    //TODO
    glDrawPixels(currentImage.width,currentImage.height,GL_RGB,GL_UNSIGNED_BYTE,currentImage.src);

    /// ****************** DESSIN CLASSIQUE ****************************************
    if (dessin.dessin_screen != NULL) {
        (*dessin.dessin_screen)();
    }
    /// ****************** SAUVEGARDE FICHIER **************************************
    if (dessin.save) {
        printf("Sauvegarde dans le fichier");
        dessin.save = 0;
    }

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
    currentImage.src = tabRVB;
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);

    control.click_souris = NULL;
    control.press_clavier = NULL;
    control.press_spec_clavier = NULL;
    control.motion_souris = NULL;
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

void setBackground(Color back){
    screen.background = back;
}

/// ///////////////////////////////////////////////////////////////////////////
/// AUTRES OUTILS
/// ///////////////////////////////////////////////////////////////////////////

void printInfo() {
    printf("\n");
    printf("**************************************\n");
    printf("              INFORMATIONS            \n");
    printf("Taille ecran : %d/%d",screen.width,screen.height);
    printf("Taille image : %d/%d",currentImage.width,currentImage.height);
    printf("Fonction dessin ");
    if (dessin.dessin_screen == NULL) printf("non");
    printf(" fixée\n");
    printf("Fonction souris ");
    if (control.click_souris == NULL) printf("non");
    printf(" fixée\n");
    printf("Fonction clavier ");
    if (control.press_clavier == NULL) printf("non");
    printf(" fixée\n");
    printf("Fonction clavier speciale ");
    if (control.press_spec_clavier == NULL) printf("non");
    printf(" fixée\n");
    printf("**************************************\n");
}

int screenHeight() { return screen.height; }
int screenWidth() { return screen.width; }
int isFullscreen() { return screen.fullscreen; }

void saveDessin(){
    dessin.save = 1;
}

void modeDebug(int debug){
    dessin.debug = debug;
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
/// Buttons
/// ///////////////////////////////////////////////////////////////////////////

Button makeButton(char *label, Bounds bounds, Color fore, Color back,
                  void (*clickHandle)(void)){
    Button b;
    b.label = malloc(sizeof(char)*strlen(label));
    strcpy(b.label,label);
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

void privateDrawButton(const Button* b,const Color* fore, const Color* back){
    glColor4f(back->r,back->g,back->b,back->a);
    drawCarre(b->bounds.x,b->bounds.y,b->bounds.x2,b->bounds.y2);
    glColor4f(fore->r,fore->g,fore->b,fore->a);
    drawCarreVide(b->bounds.x,b->bounds.y,b->bounds.x2,b->bounds.y2);
    float width = (float)glutBitmapLength(GLUT_BITMAP_8_BY_13,(b->label))/(float)screen.width;
    float height = (float)glutBitmapWidth(GLUT_BITMAP_8_BY_13,'_') * 8./(13.*(float)screen.height);
    writeString(b->bounds.x + b->bounds.width/2. - width/2.,
               b->bounds.y + b->bounds.height/2. - height/2.,b->label);
}

void drawButton(const Button *b){
    if(!b->invisible){
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

}

void pressButton(Button *b){
    b->press = 1;
}

void releaseButton(Button *b, int activeAction){
    b->press = 0;
    if(activeAction && !b->inactiv && !b->invisible) (*b->clickHandle)();
}

void hoverButton(Button *b){
    b->hover = 1;
}

void leaveButton(Button *b){
    b->hover = 0;
}

void setButtonInactiv(Button *b, int inactiv){
    b->inactiv = inactiv;
}

void setButtonInvisible(Button *b, int invisible){
    b->invisible = invisible;
}

ButtonsList* makeButtonList(Button *b){
    ButtonsList* list = malloc(sizeof(ButtonsList));
    list->button = b;
    list->next = NULL;
    return list;
}

void addButton(Button *b, ButtonsList **list){
    ButtonsList* temp = *list;
    *list = makeButtonList(b);
    (*list)->next = temp;
}


Button* findButtonInList(float x, float y, ButtonsList *list){
    ButtonsList* pointer;
    for(pointer = list; pointer!=NULL;pointer=pointer->next){
        if(isInBounds(x,y,&pointer->button->bounds))
            return pointer->button;
    }
    return NULL;
}

Button* findButtonInArray(float x, float y, Button buttons[], int nbbuttons){
    int i;
    for(i = 0; i<nbbuttons;i++){
        if(isInBounds(x,y,&buttons[i].bounds))
            return buttons+i;
    }
    return NULL;
}
