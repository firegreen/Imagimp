/***************************************************************************
                          interface.h  -  description
                             -------------------
    begin                : Tue 28 Jul 2008
    copyright            : (C) 1999 by Biri
    email                : biri@univ-mlv.fr
 ***************************************************************************/
/***************************************************************************
 *   Copyright (C) 2008 by Venceslas BIRI                                  *
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
#ifndef _GLIMAGIMP__INTERFACE_H___
#define _GLIMAGIMP__INTERFACE_H___


#define FLAG_FICHIER	1
#define PAS_FICHIER		1

#ifndef ESCAPE_KEY
#define ESCAPE_KEY 27
#define DEL_KEY 127
#define ENTER_KEY 13
#define BACKSPACE_KEY 8
#define ALT_KEY 116
#define CTRL_KEY 114
#endif

#include <stdio.h>
#include <GL/glut.h>
#include "outils.h"
#include <math.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glext.h>

#ifdef WIN32
PFNGLACTIVETEXTUREARBPROC __myglextActiveTextureARB;
#define glActiveTextureARB __myglextActiveTextureARB
#endif

/// ///////////////////////////////////////////////////////////////////////////
/// variables globales
/// ///////////////////////////////////////////////////////////////////////////


/// ///////////////////////////////////////////////////////////////////////////
/// Fonction de fixation des callbacks
/// ///////////////////////////////////////////////////////////////////////////
// ****************************************************************************
// Appelez cette fonction pour lancer le gestionnaire graphique.
// w,h : Largeur et hauteur de la fenetre a dessiner
//       Attention ces valeurs sont fixes par la suite
// tabRVB : Image initiale pour remplir la fenetre
// ****************************************************************************
extern void initGLIMAGIMP(unsigned int w,unsigned int h,unsigned char* tabRVB);
// ****************************************************************************
// Appelez cette fonction pour lancer le gestionnaire graphique avec
//   de la place pour une eventuelle IHM
// w_im,h_im : Largeur et hauteur de l'image
//             Attention ces valeurs sont fixes par la suite
// tabRVB : Image initiale pour remplir la fenetre
// w_ecran,h_ecran : Taille de la fenetre
// ****************************************************************************
extern void initGLIMAGIMP_IHM(unsigned int w_im,unsigned int h_im,unsigned char* tabRVB,
                                                unsigned int w_ecran,unsigned h_ecran, int fullscreen);
// ****************************************************************************
// Appelez cette fonction pour definir votre propre fonction qui sera
//   donc appelée a chaque fois que l'utilisateur appuyera sur une touche
// fct : Mettez simplement le nom de la fonction que VOUS avez défini
//       Cette fonction doit respecter la signature suivante
//       void xxxxx(unsigned char x,int x,int x)
//       (remplacer les xxxxx et les x par des noms à vous bien sur)
//      les deux derniers paramètres indiquent respectivement si la touche CTRL ou si la touche ALT
//      est active
// ****************************************************************************
extern void fixeFonctionClavier(void (*fct)(unsigned char, int, int, char, char));
// ****************************************************************************
// Appelez cette fonction pour definir votre propre fonction qui sera
//   donc appelée a chaque fois que l'utilisateur appuyera sur une touche "speciale"
// fct : Mettez simplement le nom de la fonction que VOUS avez défini
//       Cette fonction doit respecter la signature suivante
//       void xxxxx(int x,int x,int x)
//       (remplacer les xxxxx et les x par des noms à vous bien sur)
//       Le premier paramètre est un entier representant une touche
//       voir http://www.opengl.org/documentation/specs/glut/spec3/node54.html
// ****************************************************************************
extern void fixeFonctionClavierSpecial(void (*fct)(int,int,int));
// ****************************************************************************
// Appelez cette fonction pour definir votre propre fonction qui sera
//	 donc appelée a chaque fois que l'utilisateur clique sur la souris
// fct : Mettez simplement le nom de la fonction que VOUS avez défini
//       Cette fonction doit respecter la signature suivante
//       void xxxxx(int x,int x,int x,int x)
//       (remplacer les xxxxx et les x par des noms à vous bien sur)
//       Le deux premiers paramètres sont l'etat du clic (touche et appuie ou relache)
//       Les deux suivants correspondent aux coordonnées souris lors du clic dans le repère GLU
// ****************************************************************************
extern void fixeFonctionClicSouris(void (*fct)(int,int,float,float));
// ****************************************************************************
// Appelez cette fonction pour definir votre propre fonction de dessin
// fct : Mettez simplement le nom de la fonction que VOUS avez défini
//       Cette fonction doit respecter la signature suivante
//       void xxxxx()
//       (remplacer xxxxx par un nom à vous bien sur)
// ****************************************************************************
extern void fixeFonctionDessin(void (*fct)(void));
// ****************************************************************************
// Appelez cette fonction pour redefinir l'image a dessiner
// newImage : Buffer de la nouvelle image a dessiner
//	          Attention, elle doit etre en couleur et de même taille que la fenetre
//            c.a.d. la meme taille que l'image fournie a la fonction
//            initGLIMAGIMP
// ****************************************************************************
extern void actualiseImage(unsigned char* newImage, unsigned int width, unsigned int height);
// ****************************************************************************
// Appelez cette fonction pour definir votre propre fonction qui sera
//	 donc appelée a chaque fois que l'utilisateur bouge la souris
// fct : Mettez simplement le nom de la fonction que VOUS avez défini
//       Cette fonction doit respecter la signature suivante
//       void xxxxx(float x, float y, float deltaX, float deltaY, int drag)
//       (remplacer les xxxxx un noms à vous bien sur)
//       Les deux premiers paramètres correspondent aux coordonnées souris lors du clic
//       dans le repère GLU
//       Les deux suivants correspondent aux différence de coordonnée entre la dernière position
//       et la nouvelle
//       le dernier indique le bouton de la souris qui est pressé, -1 s'il n'y a pas de bouton pressé
// ****************************************************************************
extern void fixeFonctionMotionSouris(void (*fct)(float x, float y, float deltaX, float deltaY, int button));
extern void setBackground(Color backUI, Color backImage);
extern float getUIStartX();
extern float getUIWidth();
extern void translateImage(float x, float y);
extern Bounds imageBounds();

extern void launchApp();
//////////////////////////////////////////////////////////////////////////////
/// Fonctions utilitaires
//////////////////////////////////////////////////////////////////////////////
extern void printInfo();

extern unsigned int screenWidth();
extern unsigned int screenHeight();
extern void GlPosToImagePos(float glX, float glY, float* x, float* y);

extern void zoomPlus();
extern void zoomMoins();
extern float zoom();

extern void saveDessin();
extern void modeDebug(int debug);

extern void setFullsreen(int fullscreen);
extern int isFullscreen();

/// ///////////////////////////////////////////////////////////////////////////
/// Boutons
/// ///////////////////////////////////////////////////////////////////////////

typedef struct Component Component;
typedef struct ComponentsList{
    Component* componenent;
    struct ComponentsList* next;
} ComponentsList;

extern ComponentsList* makeComponentsList(Component* c);
extern void addComponent(Component* c, ComponentsList **list);
extern void removeFirstComponent(ComponentsList **list);
extern Component* findComponentInList(float x, float y, ComponentsList* list);
extern Component* findComponentInArray(float x, float y, Component *components, int nbComponents);
extern void drawAllComponents(const ComponentsList *list);

typedef enum { LABEL, BUTTON, RADIOBUTTON, CHECKBUTTON, DELETABLEBUTTON, SLIDER } ComponentType;

typedef struct Component {
    ComponentType type;
    Bounds bounds;
    Color fore;
    Color back;
    void (*clickHandle)(const void*);
    int hover;
    int press;
    int invisible;
    int inactiv;
    union {
		struct { char* text; void* font;} Label;
		struct { char* text; } Button;
		struct { char* text; char needToBeDelete; Bounds closeBounds;} DeletableButton;
		struct { char* text; char isSelected; ComponentsList* othersRadioButton; } RadioButton;
		struct { char* text; char isSelected;} CheckButton;
		struct { Bounds cursorBounds; float value; float min; float max;} Slider;
    } extends;
} Component;

extern void drawComponent(const Component* c);
extern void pressComponent(Component* c);
extern void releaseComponent(Component* c, int activeAction);
extern void hoverComponent(Component* c);
extern void leaveComponent(Component* c);
extern void setComponentInvisible(Component* c, int invisible);
extern void setComponentInactiv(Component* c, int inactiv);


extern Component makeLabel(char *text, Bounds bounds, Color fore, void* font);
extern void setLabelText(Component *c, char* text);

extern Component makeButton(char* text, Bounds bounds,Color fore, Color back,
                  void (*clickHandle)(const void*));
extern void setButtonText(Component *c, char* text);

extern Component makeDeletableButton(char* text, Bounds bounds,Color fore, Color back,
				  void (*clickHandle)(const void*));
void updateDeletableButtonPos(Component *c);

extern Component makeRadioButton(char* text, Bounds bounds,Color fore, Color back,
                                void (*clickHandle)(const void*));
extern void addButtonToRadioButtonList(Component *radioButton1, Component *radioButton2);
extern void selectRadioButton(Component *b);

extern Component makeCheckButton(char* text, Bounds bounds,Color fore, Color back,
							void (*clickHandle)(const void*));
extern void selectCheckButton(Component *b);

extern Component makeSlider(Bounds bounds, Color fore, Color back, void (*setHandle)(const void*));
extern void setComponentValue(Component *s, float value);
extern void setComponentValueFromPos(Component *s, float x);
extern void setSliderValueFromPos(Component *s, float x);
extern void setSliderValue(Component *s, float value);
extern void setSliderMax(Component *s, float max);
extern void setSliderMin(Component *s, float min);

extern void freeComponent(Component *c);
extern void freeAllComponents(ComponentsList *l);

#endif
