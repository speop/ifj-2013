//Implementace interpretu jazyka IFJ13.
//xbucht18, Buchta David

#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include "types.h"

typedef struct tStackItem{
		void *data;
		struct tStackItem *prev, *next; // next vyse na zasobniku
}tStackItem;

typedef struct sStack
{
    tStackItem * top;
    tStackItem * bottom;
} tStack;


tStack* SInit();
int push(tStack *stack, void *data); //prida na vrchol zasobniku
tStackItem* pop_top(tStack *stack); //odstrani prvek z vrcholu zasobniku a vrati jej
tStackItem *top(tStack *stack); //vrati prvek na vrcholu zasobniku


int push_back(tStack *stack, void* data); //prida prvek na spod zasobniku
tStackItem* pop_back(tStack *stack);  //odstrani ze spod zasobniku a vrati jej
tStackItem* bottom(tStack *stack); //vrati prvek ze spod zasobniku

// 2. parametr je odkay na funkci ktera vyprazdni interni data
int empty(tStack *stack, bool (*function)(void*)); //vyprazdni zasobnik
int deleteSt(tStack *stack, bool (*function)(void*)); //odstrani zasobnik, pokud nebyl prazdny tak se jeho prvky dealokujou

//funcke pro vyprazdeni stacku a jeho dealokaci pro struktury na ktere ho pouzijeme, tyto funkce lze hodit do garbage collectoru
bool emptySTVar(void *);
bool emptySTFunc(void *);
bool emptyToken(void *);
bool destroyST(void *);

void printStack(tStack *stack);
void printStackInt(tStack *stack);
//funkce pro dealokaci tokenu - nebylo ji kam umistit :D
bool tokenFree(void *);
bool tokenFreepom(T_Token *token);


#endif