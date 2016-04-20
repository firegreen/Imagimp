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
#define BACKSPACE_KEY 8
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
void initGLextensions();
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
// ****************************************************************************
extern void fixeFonctionClavier(void (*fct)(unsigned char,int,int));
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
//       Les deux suivants correspondent aux coordonnées souris lors du clic
// ****************************************************************************
extern void fixeFonctionClicSouris(void (*fct)(int,int,int,int));
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
extern void fixeFonctionMotionSouris(void (*fct)(int,int));
extern void actualiseImage(unsigned char* newImage);
extern void setBackground(Color back);

extern void launchApp();
/// ///////////////////////////////////////////////////////////////////////////
/// Fonctions utilitaires
/// ///////////////////////////////////////////////////////////////////////////
extern void printInfo();

extern int screenWidth();
extern int screenHeight();

void saveDessin();
void modeDebug(int debug);

extern void setFullsreen(int fullscreen);
extern int isFullscreen();

/// ///////////////////////////////////////////////////////////////////////////
/// Boutons
/// ///////////////////////////////////////////////////////////////////////////

typedef struct Button {
    char* label;
    Bounds bounds;
    Color fore;
    Color back;
    void (*clickHandle)(void);
    int hover;
    int press;
    int invisible;
    int inactiv;
} Button;

Button makeButton(char* label, Bounds bounds,Color fore, Color back,
                  void (*clickHandle)(void));
void drawButton(const Button* b);
void pressButton(Button* b);
void releaseButton(Button* b, int activeAction);
void hoverButton(Button* b);
void leaveButton(Button* b);
void setButtonInvisible(Button* b, int invisible);
void setButtonInactiv(Button* b, int inactiv);

typedef struct Slider {
    char* label;
    Bounds bounds;
    Bounds cursorBounds;
    Color fore;
    Color back;
    void (*setHandle)(float);
    int hover;
    int press;
    int invisible;
    int inactiv;
    float value;
} Slider;

Slider makeSlider(char* label, Bounds bounds,Color fore, Color back,
                  void (*setHandle)(float));
void drawSlider(const Slider* b);
void pressSlider(Slider* b);
void releaseSlider(Slider* b, int activeAction);
void hoverSlider(Slider* b);
void leaveSlider(Slider* b);
void setSliderValue(Slider *b, float value);
void setSliderValueFromPos(Slider* b, float x);
void setSliderInvisible(Slider* b, int invisible);
void setSliderInactiv(Slider* b, int inactiv);


typedef struct ButtonsList{
    Button* button;
    struct ButtonsList* next;
} ButtonsList;

ButtonsList* makeButtonList(Button* b);
void addButton(Button* b, ButtonsList **list);
Button* findButtonInList(float x, float y, ButtonsList* list);
Button* findButtonInArray(float x, float y, Button buttons[], int nbbuttons);

#endif
