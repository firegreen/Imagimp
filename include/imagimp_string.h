#ifndef IMAGIMP_STR_H
#define IMAGIMP_STR_H

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
void freeString(String *s);

#endif
