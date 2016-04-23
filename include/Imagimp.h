#ifndef IMAGIMP_H
#define IMAGIMP_H

#include "interface.h"
#include "layer.h"
typedef struct StringChar{
    char c;
    struct StringChar* next;
    struct StringChar* previous;
} StringChar;

typedef struct String{
    unsigned int size;
    StringChar* first;
    StringChar* last;
}String;

String makeString();
void addCharacter(String *s, char c);
void removeLastCharacter(String *s);
char* convertString(String s);
void freeString(String s);

typedef enum { BTN_QUIT=0, BTN_SAVE, BTN_LOAD, BTN_OPACITY, BTN_DISPLAYMODE, MAIN_NBBUTTONS } MAINBTNS;
typedef enum { BTN_YES=0, BTN_NO, BTN_OK, BTN_CANCEL, DIALOG_NBBUTTONS} DIALOGBTNS;
typedef enum { FLAGS_YES=1,FLAGS_NO=2,FLAGS_OK=4,FLAGS_CANCEL=8, FLAGS_PROMPT=16, FLAGS_SLIDER=32} DIALOGFLAGS;

struct {
    Button buttons[MAIN_NBBUTTONS];
    Button* pressedButton;
    Button* hoveredButton;
    unsigned char* image_base;
    unsigned char mouseButtonPressed;
    unsigned char dialogMode;
    unsigned char displayMode;
    Picture picture;
    unsigned char dragImage;
    unsigned char dragLayer;
} Imagimp;

struct {
    char* text;
    String input;
    unsigned char prompt;
    Button buttons[DIALOG_NBBUTTONS];
    Slider slider;
    Bounds bounds;
    float xText, yText;
    float xBtn, yBtn;
    Bounds promptBounds;
    void (*closeHandle)(DIALOGBTNS);
} Dialog;

//void Imagimp_init(int argc, char *argv[]);
void Imagimp_launch(int argc, char *argv[]);
void Imagimp_handleKeyboardTexte(unsigned char ascii, int x, int y);
void Imagimp_handleKeyboard(unsigned char ascii, int x, int y, char CTRL, char ALT);
void Imagimp_handleKeyboardSpecial(int touche, int x, int y);
void Imagimp_handleMouseClick(int button, int state, float xGL, float yGL);
void Imagimp_handleMouseMotion(float xGL, float yGL, float deltaX, float deltaY, int pressedButton);
void Imagimp_draw();
void Imagimp_refresh();
void Imagimp_quit();
void Imagimp_switchDisplay();
void Imagimp_removeCurrentLayer();
void Imagimp_addEmptyLayer();
void Imagimp_setOpacityToCurrentLayer(float opacity);

void openOpacityDialog();
void openPPMImportDialog();
void openPPMExportDialog();

void activeDialog(const char* text, int flag, void (*closeHandle)(DIALOGBTNS));
void desactiveDialog();
void Dialog_draw();
#endif /* IMAGIMP_H */
