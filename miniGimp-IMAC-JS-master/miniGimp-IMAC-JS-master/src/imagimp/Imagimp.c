#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include "interface.h"
#include "Imagimp.h"
#include "PPM.h"
#include "outils.h"

const Color BLACK = {0,0,0,1};
const Color DIALOGBACK = {0.5,0.7,0.55,1};
const Color IMAGIMPBACK = {0.05,0.01,0.07,1};
const Color DIALOGFORE = {0.02,0.1,0.05,1};
const Color IMAGIMPFORE = {0.3,0.6,0.4,1};
const Color TRANSLUCIDE = {0.,0.,0.,0.};
const Color WHITE = {1,1,1,1};
float initial_opacity;
char inputStr[256];

void Imagimp_quit(){
    printf("Fin du programme\n");
    exit(0);
}

void Imagimp_setOpacityToCurrentLayer(float opacity){
    Imagimp.picture.current->element->opacity = fmaxf(0,fminf(1,opacity));
    Imagimp_refresh(1);
}

void opacitySliderHandle(const void* opacity){
    Imagimp_setOpacityToCurrentLayer(*((float*)opacity));
}

void handleOpacityDialog(DIALOGBTNS answer){
    switch (answer) {
    case BTN_CANCEL:
        Imagimp_setOpacityToCurrentLayer(initial_opacity);
        break;
    default:
        break;
    }
}

void openOpacityDialog(){
    initial_opacity = Imagimp.picture.current->element->opacity;
    Dialog.slider.clickHandle = opacitySliderHandle;
    setSliderValue(&Dialog.slider,Imagimp.picture.current->element->opacity);
    activeDialog("Opacité",FLAGS_SLIDER | FLAGS_CANCEL | FLAGS_OK, handleOpacityDialog);
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

void Imagimp_switchDisplay(){
    if(Imagimp.displayMode){
        Imagimp.displayMode = 0;
        actualiseImage(Imagimp.picture.current->element->rgb,
                       Imagimp.picture.current->element->width,Imagimp.picture.current->element->height);
        setButtonLabel(Imagimp.buttons + BTN_DISPLAYMODE,"Affiche Rendu");
    }
    else{
        Imagimp.displayMode = 1;
        updateCfLayer(&Imagimp.picture,1);
        actualiseImage(Imagimp.picture.Cf.rgb, Imagimp.picture.Cf.width,Imagimp.picture.Cf.height);
        setButtonLabel(Imagimp.buttons + BTN_DISPLAYMODE,"Affiche Calque Original");
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
        setComponentInactiv(Imagimp.buttons + BTN_DELETELAYER,0);
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

void handleLayerDeletingAskDialog(DIALOGBTNS answer){
    switch (answer) {
    case BTN_YES:
        Imagimp_removeCurrentLayer();
        break;
    default:
        break;
    }
}

void openPPMImportDialog(){
    activeDialog("Importer un fichier PPM \n Saississez le nom du fichier:",
                 FLAGS_PROMPT | FLAGS_CANCEL | FLAGS_OK, handlePPMImportDialog);
}

void openPPMExportDialog(){
    activeDialog("Exporter un fichier PPM \n Saississez le nom du fichier:",
                 FLAGS_PROMPT | FLAGS_CANCEL | FLAGS_OK, handlePPMExportDialog);
}

void Imagimp_addEmptyLayer(){
    addNewEmptyLayer(&Imagimp.picture);
    Imagimp_refresh(1);
}

void openLayerDeletingAskDialog(){
    activeDialog("Souhaitez-vous reellement supprimer le calque?",FLAGS_YES | FLAGS_NO,
                 handleLayerDeletingAskDialog);
}

void Imagimp_removeCurrentLayer(){
    if(Imagimp.picture.current->previous !=NULL)
    {
        removeCurrentLayer(&Imagimp.picture);
        if(Imagimp.picture.current->previous==NULL)
            setComponentInactiv(Imagimp.buttons + BTN_DELETELAYER,1);
        Imagimp_refresh(1);
    }
    else
        activeDialog("Impossible de supprimer le calque d'arrière plan", FLAGS_OK,NULL);
}

void answerOK() {
    desactiveDialog();
    if(Dialog.closeHandle != NULL)
        (*Dialog.closeHandle)(BTN_OK);
}
void answerYES() { desactiveDialog(); if(Dialog.closeHandle != NULL) (*Dialog.closeHandle)(BTN_YES); }
void answerNO() { desactiveDialog(); if(Dialog.closeHandle != NULL) (*Dialog.closeHandle)(BTN_NO); }
void answerCANCEL() { desactiveDialog(); if(Dialog.closeHandle != NULL) (*Dialog.closeHandle)(BTN_CANCEL);}

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
    Dialog.slider = makeSlider(makeBounds(Dialog.bounds.x+0.1,Dialog.bounds.y+Dialog.bounds.y/3,
                                          Dialog.bounds.width-0.2,Dialog.bounds.y/5),
                               DIALOGFORE,DIALOGBACK,NULL);
    Dialog.input = makeString();
    Dialog.promptBounds = makeBounds(0.3,0.45,0.4,0.1);
    Dialog.xBtn = 0.6;
    Dialog.yBtn = 0.375;
    Dialog.xText = 0.3;
    Dialog.yText = 0.6;
    Dialog.components = makeComponentsList(Dialog.buttons);
    int i;
    for(i=1;i<DIALOG_NBBUTTONS;i++)
    {
        addComponent(Dialog.buttons+i,&Dialog.components);
    }
    Dialog.radioButtons = NULL;
    desactiveDialog();
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

void Imagimp_launch(int argc, char *argv[]) {
    unsigned int w=800, h=600, ihm_w=200;
    int i,fileImport=0;
    for (i=1;i<argc;i++) {
        if (strcmp(argv[i],"-h") == 0) {
            printf("Affichage de l'aide\n");
            exit(1);
        }
        else if (strcmp(argv[i],"-f") == 0) {
            printf("Chargement fichier :\n");
            i++;
            fileImport = 1;
            if(i<argc){
                Imagimp.image_base = readPPM(argv[i],&w,&h);
                if(Imagimp.image_base==NULL){
                    fprintf(stderr, "Impossible de lire le fichier %s",argv[i]);
                    exit(1);
                }
            }
            else{
                fgets(inputStr,sizeof(inputStr),stdin);
                Imagimp.image_base = readPPM(inputStr,&w,&h);
                if(Imagimp.image_base==NULL){
                    fprintf(stderr, "Impossible de lire le fichier %s",argv[i]);
                    exit(1);
                }
            }
            exit(1);
        }
    }
    if(!fileImport)
        makeImageBase(w,h);

    makeEmptyPicture(&Imagimp.picture, w,h);
    addNewLayer(&Imagimp.picture,Imagimp.image_base,w,h);
    updateCfLayer(&Imagimp.picture,1);
    Imagimp.hoveredButton = NULL;
    Imagimp.pressedButton = NULL;
    Imagimp.mouseButtonPressed=0;
    Imagimp.displayMode = 1;
    Imagimp.dialogMode = 0;
    Imagimp.dragImage = Imagimp.dragLayer = 0;
    initGLIMAGIMP_IHM(w, h, Imagimp.picture.Cf.rgb, w+ihm_w, h,0);
    setBackground(makeColor(0.02,0.03,0.05,1),IMAGIMPBACK);
    fixeFonctionClicSouris(Imagimp_handleMouseClick);
    fixeFonctionMotionSouris(Imagimp_handleMouseMotion);
    fixeFonctionClavier(Imagimp_handleKeyboard);
    fixeFonctionClavierSpecial(Imagimp_handleKeyboardSpecial);
    fixeFonctionDessin(Imagimp_draw);


    float startX = getUIStartX() + 0.001;
    float btnsizeX = 0.199f;
    float btnsizeY = 0.04f;
    Imagimp.buttons[BTN_QUIT] = makeButton("Quitter",makeBounds(startX,0.15f,btnsizeX,btnsizeY),
                                           IMAGIMPFORE,IMAGIMPBACK,Imagimp_quit);
    Imagimp.buttons[BTN_SAVE] = makeButton("Sauvegarder",makeBounds(startX,0.2f,btnsizeX,btnsizeY),
                                           IMAGIMPFORE,IMAGIMPBACK,openPPMExportDialog);
    Imagimp.buttons[BTN_LOAD] = makeButton("Charger",makeBounds(startX,0.8f,btnsizeX,btnsizeY),
                                           IMAGIMPFORE,IMAGIMPBACK,openPPMImportDialog);
    Imagimp.buttons[BTN_OPACITY] = makeButton("Changer Opacite",makeBounds(startX,0.75f,btnsizeX,btnsizeY),
                                              IMAGIMPFORE,IMAGIMPBACK,openOpacityDialog);
    Imagimp.buttons[BTN_DISPLAYMODE] = makeButton("Afficher Calque Original",makeBounds(startX,0.7f,btnsizeX,btnsizeY),
                                                  IMAGIMPFORE,IMAGIMPBACK,Imagimp_switchDisplay);
    Imagimp.buttons[BTN_DELETELAYER] = makeButton("Supprimer Calque Courant",makeBounds(startX,0.65f,btnsizeX,btnsizeY),
                                                  IMAGIMPFORE,IMAGIMPBACK,openLayerDeletingAskDialog);
    Imagimp.components = makeComponentsList(Imagimp.buttons);
    for(i=1;i<MAIN_NBBUTTONS;i++)
    {
        addComponent(Imagimp.buttons+i,&Imagimp.components);
    }
    initDialog();
    launchApp();
}

void Imagimp_handleKeyboardTexte(unsigned char ascii, int x, int y) {
    switch(ascii) {
    case ESCAPE_KEY: // Touche Escape
        if(!Dialog.buttons[BTN_NO].invisible)
            answerNO();
        else if(!Dialog.buttons[BTN_CANCEL].invisible)
            answerCANCEL();
        else if(!Dialog.buttons[BTN_OK].invisible)
            answerOK();
        return;
    case DEL_KEY:
        removeLastCharacter(&(Dialog.input));
        return;
    case BACKSPACE_KEY:
        removeLastCharacter(&(Dialog.input));
        return;
    case ENTER_KEY:
        if(!Dialog.buttons[BTN_YES].invisible)
            answerYES();
        else if(!Dialog.buttons[BTN_OK].invisible)
            answerOK();
        else
            addCharacter(&Dialog.input,'\n');
        return;
    default :
        break;
    }
    if(Dialog.prompt){
        addCharacter(&(Dialog.input),ascii);
    }
}
void Imagimp_handleKeyboard(unsigned char ascii, int x, int y, char CTRL, char ALT) {
    printf("%c", ascii);
    printf(" : %u\n",ascii);
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
            }
            else{
                changeCurrentToBelowLayer(&Imagimp.picture);
            }
            if(Imagimp.picture.current->previous==NULL)
                setComponentInactiv(Imagimp.buttons + BTN_DELETELAYER,1);
            else
                setComponentInactiv(Imagimp.buttons + BTN_DELETELAYER,0);
            Imagimp_refresh(1);
        }
        else{
            if(Imagimp.picture.currentID >= Imagimp.picture.nbLayers-1){
                changeCurrentTo(&Imagimp.picture,0);
            }
            else{
                changeCurrentToAboveLayer(&Imagimp.picture);
            }
            if(Imagimp.picture.current->previous==NULL)
                setComponentInactiv(Imagimp.buttons + BTN_DELETELAYER,1);
            else
                setComponentInactiv(Imagimp.buttons + BTN_DELETELAYER,0);
            Imagimp_refresh(1);
        }
        break;
    case ESCAPE_KEY: // Touche Escape
        Imagimp_quit();
        break;
    default :
        printf("Touche non fonctionnelle\n");
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
            Imagimp_setOpacityToCurrentLayer(Imagimp.picture.current->element->opacity-0.1);
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
            Imagimp_setOpacityToCurrentLayer(Imagimp.picture.current->element->opacity+0.1);
        break;
    case GLUT_KEY_UP: /* (CAL_2) Calque suivant */ break;
    case GLUT_KEY_DOWN: /* (CAL_2) Calque précédent */ break;
    case GLUT_KEY_PAGE_UP: break;
    case GLUT_KEY_PAGE_DOWN: break;
    case GLUT_KEY_HOME: break;
    case GLUT_KEY_END: break;
    case GLUT_KEY_INSERT: break;
    default:
        break;
    }
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
        Bounds bounds,bounds2;
        bounds = imageBounds();
        if(!Imagimp.dialogMode){
            if (button == GLUT_RIGHT_BUTTON){
                Imagimp.dragImage = isInBounds(xGL,yGL,&bounds);
            }
            else if(button == GLUT_LEFT_BUTTON){
                bounds2 = layerBoundsInWindow(Imagimp.picture.current->element,screenWidth(),screenHeight());
                bounds2.x += bounds.x;
                bounds2.y += bounds.y;
                bounds2.width *= zoom();
                bounds2.height *= zoom();
                bounds2.x2 = bounds2.x+bounds2.width;
                bounds2.y2 = bounds2.y+bounds2.height;
                Imagimp.dragLayer = isInBounds(xGL,yGL,&bounds2);
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
        Imagimp_refresh(3);
        return;
    }
    else if(Imagimp.dialogMode){
        b = findComponentInArray(xGL,yGL,Dialog.buttons,DIALOG_NBBUTTONS);
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
        b = findComponentInArray(xGL,yGL,Imagimp.buttons,MAIN_NBBUTTONS);
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

void Dialog_draw(){
    glColor4f(DIALOGBACK.r,DIALOGBACK.g,DIALOGBACK.b,0.8);
    drawCarre(Dialog.bounds.x,Dialog.bounds.y,Dialog.bounds.x2,Dialog.bounds.y2);
    float x, y;
    int i;
    glColor4f(DIALOGFORE.r,DIALOGFORE.g,DIALOGFORE.b,DIALOGFORE.a);
    if(Dialog.text!=NULL){
        writeString(Dialog.xText,Dialog.yText,Dialog.text);
    }
    for(i=0;i<DIALOG_NBBUTTONS;i++){
        drawComponent(Dialog.buttons+i);
    }
    if(Dialog.prompt){
        glColor4f(WHITE.r,WHITE.g,WHITE.b,WHITE.a);
        drawCarre(Dialog.promptBounds.x,Dialog.promptBounds.y,Dialog.promptBounds.x2,Dialog.promptBounds.y2);
        float height = 2.3*(float)glutBitmapWidth(GLUT_BITMAP_8_BY_13,'_') * 8./(13.*(float)screenHeight());
        StringChar* current;
        glColor4f(BLACK.r,BLACK.g,BLACK.b,BLACK.a);
        x = Dialog.promptBounds.x*1.1;
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
            //x+=glutBitmapWidth(GLUT_BITMAP_8_BY_13,current->c)/(float)screenWidth();
        }
    }
    else if(!Dialog.slider.invisible){
        drawComponent(&Dialog.slider);
    }
    else if(Dialog.choice){
        ComponentsList* buttons = Dialog.radioButtons;
        while(buttons!=NULL){
            addButtonToRadioButtonList(buttons->componenent,radioButton);
            otherButton = otherButton->next;
        }

    }
}

void activeDialog(const char *text, int flag, void (*closeHandle)(DIALOGBTNS)){
    if(Dialog.text != NULL) free(Dialog.text);
    if(Dialog.input.size) freeString(Dialog.input);
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
    }
    glutPostRedisplay();
}

void addRadioButtonInDialog(Component *radioButton){
    ComponentsList* otherButton = Dialog.radioButtons;
    while(otherButton!=NULL){
        addButtonToRadioButtonList(otherButton->componenent,radioButton);
        otherButton = otherButton->next;
    }
    addComponent(radioButton,&Dialog.radioButtons);
}

void desactiveDialog(){
    Imagimp.dialogMode = 0;
    Dialog.prompt = 0;
    Dialog.radioButtons = 0;
    Dialog.slider.inactiv = 1;
    glutPostRedisplay();
}

void Imagimp_draw() {
    int i;
    for(i=0;i<MAIN_NBBUTTONS;i++)
        drawComponent(Imagimp.buttons+i);
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
