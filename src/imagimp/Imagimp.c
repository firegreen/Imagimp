#include <stdio.h>
#include <string.h>
#include <GL/glut.h>
#include "interface.h"
#include "Imagimp.h"
#include "outils.h"

const Color BLACK = {0,0,0,1};
const Color DIALOGBACK = {0.5,0.7,0.55,1};
const Color IMAGIMPBACK = {0.05,0.01,0.07,1};
const Color DIALOGFORE = {0.02,0.1,0.05,1};
const Color IMAGIMPFORE = {0.3,0.6,0.4,1};
const Color TRANSLUCIDE = {0.,0.,0.,0.};
const Color WHITE = {1,1,1,1};

void quit(){
    printf("Fin du programme\n");
    exit(0);
}

void answerOK() { Dialog.userAnswer = BTN_OK; desactiveDialog(); }
void answerYES() { Dialog.userAnswer = BTN_YES; desactiveDialog();}
void answerNO() { Dialog.userAnswer = BTN_NO; desactiveDialog();}
void answerCANCEL() { Dialog.userAnswer = BTN_CANCEL; desactiveDialog();}

void Imagimp_launch(int argc, char *argv[]) {
    const unsigned w=800, h=600, ihm_w=200;
    const unsigned long size = 3*w*h;
    int i;
    for (i=1;i<argc;i++) {
        if (strcmp(argv[i],"-h") == 0) {
            printf("Affichage de l'aide\n");
            exit(1);
        }
    }
    Imagimp.image_base = malloc(size*sizeof(unsigned char));
    memset( Imagimp.image_base, 0x8e, size);
    Imagimp.hoveredButton = NULL;
    Imagimp.pressedButton = NULL;
    Imagimp.mouseButtonPressed=0;
    Imagimp.dialogMode = 0;
    initGLIMAGIMP_IHM(w, h, Imagimp.image_base, w+ihm_w, h,0);
    fixeFonctionClicSouris(Imagimp_handleMouseClick);
    fixeFonctionMotionSouris(Imagimp_handleMouseMotion);
    fixeFonctionClavier(Imagimp_handleKeyboard);
    fixeFonctionClavierSpecial(Imagimp_handleKeyboardSpecial);
    fixeFonctionDessin(Imagimp_draw);

    setBackground(IMAGIMPBACK);


    Imagimp.buttons[BTN_QUIT] = makeButton("Quitter",makeBounds(0.8f,0.15f,0.15f,0.04f),
                                           IMAGIMPFORE,IMAGIMPBACK,quit);
    Imagimp.buttons[BTN_SAVE] = makeButton("Sauvegarder",makeBounds(0.8f,0.2f,0.15f,0.04f),
                                           IMAGIMPFORE,IMAGIMPBACK,quit);
    Imagimp.buttons[BTN_LOAD] = makeButton("Charger",makeBounds(0.8f,0.8f,0.15f,0.04f),
                                           IMAGIMPFORE,IMAGIMPBACK,quit);

    Dialog.text = NULL;
    Dialog.buttons[BTN_YES] = makeButton("Oui",makeBounds(0.8f,0.15f,0.15f,0.04f),
                                         DIALOGFORE,DIALOGBACK,answerYES);
    Dialog.buttons[BTN_NO] = makeButton("Non",makeBounds(0.8f,0.15f,0.15f,0.04f),
                                        DIALOGFORE,DIALOGBACK,answerNO);
    Dialog.buttons[BTN_OK] = makeButton("Ok",makeBounds(0.8f,0.15f,0.15f,0.04f),
                                        DIALOGFORE,DIALOGBACK,answerOK);
    Dialog.buttons[BTN_CANCEL] = makeButton("Annuler",makeBounds(0.8f,0.15f,0.15f,0.04f),
                                            DIALOGFORE,DIALOGBACK,answerCANCEL);
    Dialog.bounds = makeBounds(0.25,0.35,0.5,0.35);
    Dialog.input = makeString();
    Dialog.promptBounds = makeBounds(0.3,0.45,0.4,0.1);
    Dialog.xBtn = 0.6;
    Dialog.yBtn = 0.375;
    Dialog.xText = 0.3;
    Dialog.yText = 0.6;
    desactiveDialog();
    launchApp();
}

void Imagimp_handleKeyboardTexte(unsigned char ascii, int x, int y) {
    printf("Touche tapee %d (coord souris %d/%d)\n",(unsigned int)ascii,x,y);
    switch(ascii) {
    case ESCAPE_KEY: // Touche Escape
        desactiveDialog();
        return;
    case DEL_KEY:
        removeLastCharacter(&(Dialog.input));
        return;
    case BACKSPACE_KEY:
        removeLastCharacter(&(Dialog.input));
        return;
    default :
        break;
    }
    if(Dialog.prompt){
        addCharacter(&(Dialog.input),ascii);
    }
}
void Imagimp_handleKeyboard(unsigned char ascii, int x, int y) {
    if(Imagimp.dialogMode){
        Imagimp_handleKeyboardTexte(ascii, x, y);
        return;
    }
    char saisie[100] = {'\0'};
    int modifier = glutGetModifiers();
    switch(ascii) {
    case 'l': /* (LUT_1) Ajouter une LUT au calque actif */
        /* ADDLUM, DIMLUM, ADDCON, DIMCON, INVERT, SEPIA */
        break;
    case 'L': /* (LUT_3) Supprimer la dernière LUT au calque actif */ break;
    case 'a': /* (LUT_2) Appliquer une LUT au calque actif */ break;
    case 'v': /* (IHM_1) Vue source-calque ou image-finale */ break;
    case 'q': /* (IHM_4) Quitter, en libérant la mémoire. */ break;
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
    case 'x': /* (CAL_5) Supprimer le calque actif */
        /* On ne peut supprimer le dernier calque. */
        break;
    case 'm': /* (CAL_4) Changer la fonction de mélange du calque */ break;
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
    case 27: // Touche Escape
        quit();
        break;
    default :
        printf("Touche non fonctionnelle\n");
    }
}

void Imagimp_handleKeyboardSpecial(int touche, int x, int y) {
    printf("Touche spéciale : %d (souris: %d, %d)\n", touche, x, y);
    switch(touche) {
    case GLUT_KEY_F1:
        activeDialog("Coucou toi",FLAGS_OK | FLAGS_PROMPT);
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
    case GLUT_KEY_LEFT: /* (CAL_3) Opacité d'un calque */ break;
    case GLUT_KEY_RIGHT: /* (CAL_3) Opacité d'un calque */ break;
    case GLUT_KEY_UP: /* (CAL_2) Calque suivant */ break;
    case GLUT_KEY_DOWN: /* (CAL_2) Calque précédent */ break;
    case GLUT_KEY_PAGE_UP: break;
    case GLUT_KEY_PAGE_DOWN: break;
    case GLUT_KEY_HOME: break;
    case GLUT_KEY_END: break;
    case GLUT_KEY_INSERT: break;
    }
}

void Imagimp_handleMouseClick(int button, int state, int x, int y) {
    float xGL = (float)x/(float)screenWidth();
    float yGL = 1.-(float)y/(float)screenHeight();
    Button* b;
    if(Imagimp.dialogMode)
        b = findButtonInArray(xGL,yGL,Dialog.buttons,DIALOG_NBBUTTONS);
    else
        b = findButtonInArray(xGL,yGL,Imagimp.buttons,MAIN_NBBUTTONS);
    int redisplay = 0;
    if (button == GLUT_LEFT_BUTTON) {
        printf("Button gauche ");
    }
    else if (button == GLUT_MIDDLE_BUTTON) {
        printf("Button milieu ");
    }
    else { // button == GLUT_RIGHT_BUTTON
        printf("Button droit ");
    }
    if (state == GLUT_DOWN) {
        printf("clique\n");
        Imagimp.mouseButtonPressed=1;
        if(b!=NULL){
            Imagimp.pressedButton = b;
            pressButton(b);
            redisplay = 1;
        }
    }
    else { // state == GLUT_UP
        printf("relache\n");
        if(Imagimp.mouseButtonPressed){
            Imagimp.mouseButtonPressed=0;
            if(Imagimp.pressedButton != NULL){
                releaseButton(Imagimp.pressedButton,b==Imagimp.pressedButton);
                Imagimp.pressedButton=NULL;
                redisplay = 1;
            }
        }
    }
    if(redisplay) glutPostRedisplay();
}

void Imagimp_handleMouseMotion(int x, int y) {
    float xGL = (float)x/(float)screenWidth();
    float yGL = 1.-(float)y/(float)screenHeight();
    Button* b;
    if(Imagimp.dialogMode)
        b = findButtonInArray(xGL,yGL,Dialog.buttons,DIALOG_NBBUTTONS);
    else
        b = findButtonInArray(xGL,yGL,Imagimp.buttons,MAIN_NBBUTTONS);
    int redisplay = 0;

    if(Imagimp.hoveredButton!=b && Imagimp.hoveredButton!=NULL){
        leaveButton(Imagimp.hoveredButton);
        Imagimp.hoveredButton=NULL;
        redisplay = 1;
    }
    if(b!=NULL){
        Imagimp.hoveredButton = b;
        hoverButton(b);
        redisplay = 1;
    }
    if(redisplay) glutPostRedisplay();
    printf("Coordonnees du point motion %f %f\n",xGL,yGL);
}

void Dialog_draw(){
    glColor4f(DIALOGBACK.r,DIALOGBACK.g,DIALOGBACK.b,0.8);
    drawCarre(Dialog.bounds.x,Dialog.bounds.y,Dialog.bounds.x2,Dialog.bounds.y2);
    float x, y;
    int i;
    glColor4f(DIALOGFORE.r,DIALOGFORE.g,DIALOGFORE.b,DIALOGFORE.a);
    if(Dialog.text!=NULL){
        writeString(Dialog.xText,Dialog.yText,Dialog.text);
    }
    x = Dialog.xBtn;
    y = Dialog.yBtn;
    for(i=0;i<DIALOG_NBBUTTONS;i++){
        if(!Dialog.buttons[i].invisible){
            Dialog.buttons[i].bounds = makeBounds(x,y,0.07,0.05);
            drawButton(Dialog.buttons+i);
            x -= 0.10;
        }
    }
    if(Dialog.prompt){
        glColor4f(WHITE.r,WHITE.g,WHITE.b,WHITE.a);
        drawCarre(Dialog.promptBounds.x,Dialog.promptBounds.y,Dialog.promptBounds.x2,Dialog.promptBounds.y2);
        float height = (float)glutBitmapWidth(GLUT_BITMAP_8_BY_13,'_') * 8./(13.*(float)screenHeight());
        StringChar* current;
        glColor4f(BLACK.r,BLACK.g,BLACK.b,BLACK.a);
        x = Dialog.promptBounds.x*1.1;
        y = Dialog.promptBounds.y + Dialog.promptBounds.height/2 -height/2;
        for(current = Dialog.input.first;current!=NULL;current = current->next){
            writeChar(x,y,current->c);
            x+=glutBitmapWidth(GLUT_BITMAP_8_BY_13,current->c)/(float)screenWidth();
        }
    }
}

void activeDialog(const char *text, int flag){
    if(Dialog.text != NULL) free(Dialog.text);
    if(Dialog.input.size) freeString(Dialog.input);
    Dialog.text = NULL;
    Imagimp.dialogMode = 1;
    if(text[0]){
        Dialog.text = malloc(sizeof(char)*strlen(text));
        strcpy(Dialog.text,text);
    }
    Dialog.buttons[BTN_OK].invisible = !(flag & FLAGS_OK);
    Dialog.buttons[BTN_YES].invisible = !(flag & FLAGS_YES);
    Dialog.buttons[BTN_NO].invisible = !(flag & FLAGS_NO);
    Dialog.buttons[BTN_CANCEL].invisible = !(flag & FLAGS_CANCEL);
    Dialog.prompt = flag & FLAGS_PROMPT;
    glutPostRedisplay();
}

void desactiveDialog(){
    Imagimp.dialogMode = 0;
    Dialog.userAnswer = -1;
    Dialog.prompt = 0;
    glutPostRedisplay();
}

void Imagimp_draw() {
    int i;
    for(i=0;i<MAIN_NBBUTTONS;i++)
        drawButton(Imagimp.buttons+i);
    if(Imagimp.dialogMode){
        Dialog_draw();
    }
}


String makeString(){
    String s;
    s.size = 0;
    s.first = NULL;
    s.last = s.first;
    return s;
}

void addCharacter(String* s, char c){
    StringChar* newC = malloc(sizeof(StringChar));
    newC->c = c;
    newC->next = NULL;
    newC->previous = s->last;
    if(s->last!=NULL)
        s->last->next = newC;
    else
        s->first = newC;
    s->last = newC;
    s->size++;
    glutPostRedisplay();
}

void removeLastCharacter(String *s){
    if(s->last!=NULL){
        StringChar* tmp = s->last->previous;
        free(s->last);
        s->last = tmp;
        if(s->last!=NULL)
            s->last->next = NULL;
        else
            s->first = NULL;
        s->size--;
    }
    glutPostRedisplay();
}

char* convertString(String s){
    char* retour = malloc(sizeof(char)*s.size+1);
    retour[s.size] = 0;
    int i;
    StringChar* current;
    for(i=0,current = s.first;i<s.size;i++,current = current->next)
        retour[i] = current->c;
    return retour;
}

void freeString(String s){
    s.size = 0;
    int i;
    StringChar* current;
    for(i=0,current = s.first;i<s.size;i++)
    {
        StringChar* temp = current->next;
        free(current);
        current = temp;
    }
    s.first = s.last = NULL;
}
