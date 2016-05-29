#include "imagimp_string.h"
#include <stdlib.h>

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

void freeString(String* s){
	int i;
	StringChar* current;
	for(i=0,current = s->first;i<s->size;i++)
	{
		StringChar* temp = current->next;
		free(current);
		current = temp;
	}
	s->size = 0;
	s->first = s->last = NULL;
}
