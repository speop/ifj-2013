#ifndef STACK_H
#define STACK_H

typedef struct tStackItem{
		T_Token *data;
		struct tStackItem *prev, *next; // next vyse na zasobniku
}tStackItem;

typedef struct sStack
{
    tStackItem * top;
    tStackItem * bottom;
} tStack;


tStack* SInit();
int push(tStack *stack, T_Token data); //prida na vrchol zasobniku
tStackItem* pop_top(tStack *stack); //odstrani prvek z vrcholu zasobniku a vrati jej
tStackItem *top(tStack *stack); //vrati prvek na vrcholu zasobniku


int push_back(tStack *stack, T_Token data); //prida prvek na spod zasobniku
tStackItem* pop_back(tStack *stack);  //odstrani ze spod zasobniku a vrati jej
tStackItem* bottom(tStack *stack); //vrati prvek ze spod zasobniku

int empty(tStack *stack); //vyprazdni zasobnik
int delete(tStack *stack); //odstrani zasobnik, pokud nebyl prazdny tak se jeho prvky dealokujou

#endif