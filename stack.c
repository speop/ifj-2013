//xbucht18

#include "types.h"
#include <stdbool.h>
#include "stack.h"
#include <stdlib.h>

//funkce vytvori a inicializuje zasobnik, vraci ukazatel na zasobnik
tStack* SInit(){

	tStack *stack;

	if((stack = (tStack*)malloc(sizeof(stack))) == NULL) return NULL;

	stack->top = NULL;
	stack->bottom = NULL;

	return stack;
}

int push(tStack *stack, T_Token data){

	T_Token *terminal;
	tStackItem *prvek;

	//alokujeme novy token kam nakopirujeme informace z tokenu co nam prisel
	if((terminal = (T_Token*)malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;

	terminal->type=data.type;
	terminal->value = data.value;

	//pridame novy prvek na zasobnik
	if((prvek = (tStackItem*)malloc(sizeof(tStackItem))) == NULL) return ERROR_INTER;
	prvek->prev =  NULL;
	prvek->next = NULL;
	prvek->data = terminal;

	//zasobnik je prazdny
	if(stack->top == NULL){
		stack->top = prvek;
		stack->bottom =prvek;
		
	}

	else {
		stack->top->next = prvek;
		prvek->prev =  stack->top;
		stack->top = prvek;
	}
	

	return OK;
}

tStackItem* pop_top(tStack *stack){

	tStackItem *temp;

	//prazdny zasobnik
	if(stack->top == NULL) return NULL;

	//jediny prvek na zasobniku
	if(stack->top == stack->bottom && stack->top != NULL){
		temp = stack->top ;

		stack->top = NULL;
		stack->bottom = NULL;
		return temp;
	}

	//mnoho prvku na zasobniku
	temp = stack->top;
	stack->top = temp->prev;
	temp->prev->next = NULL;

	return temp;

}

tStackItem *top(tStack *stack){

	return stack->top;
}


int push_back(tStack *stack, T_Token data){


	T_Token *terminal;
	tStackItem *prvek;

	//alokujeme novy token kam nakopirujeme informace z tokenu co nam prisel
	if((terminal = (T_Token*)malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;

	terminal->type=data.type;
	terminal->value = data.value;

	//pridame novy prvek na zasobnik
	if((prvek = (tStackItem*)malloc(sizeof(tStackItem))) == NULL) return ERROR_INTER;
	prvek->prev =  NULL;
	prvek->next = NULL;
	prvek->data = terminal;

	//zasobnik je prazdny
	if(stack->top == NULL){
		stack->top = prvek;
		stack->bottom =prvek;
		
	}

	else {
		stack->bottom->prev = prvek;
		prvek->next =  stack->bottom;
		stack->bottom = prvek;
	}
	

	return OK;
}

tStackItem* pop_back(tStack *stack){

	tStackItem *temp;

	//prazdny zasobnik
	if(stack->top == NULL) return NULL;

	//jediny prvek na zasobniku
	if(stack->top == stack->bottom && stack->top != NULL){
		temp = stack->top ;

		stack->top = NULL;
		stack->bottom = NULL;
		return temp;
	}

	//mnoho prvku na zasobniku
	temp = stack->bottom;
	stack->bottom = temp->next;
	temp->next->prev = NULL;

	return temp;

}

tStackItem* bottom(tStack *stack){

	return stack->bottom;
}



int empty(tStack *stack){

	tStackItem *temp;

	// nekonecna smycka cyklime tak dlouho dokud nam pop neposle NULL coz znamena prazdy zasobnik
	while(true){

		temp = pop_top(stack);
		if(temp == NULL) break;
		free(temp);
	}

	return OK;
}

int delete(tStack *stack){
	
	if (stack == NULL) return OK;

	empty(stack);
	free(stack);
	return OK;
}