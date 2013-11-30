//Implementace interpretu jazyka IFJ13.
//xbucht18, Buchta David

#include "types.h"
#include <stdbool.h>
#include "stack.h"
#include <stdlib.h>
#include "ial.h"
#include "ast_tree.h"

//funkce vytvori a inicializuje zasobnik, vraci ukazatel na zasobnik
tStack* SInit(){

	tStack *stack;

	if((stack = (tStack*)malloc(sizeof(stack))) == NULL) return NULL;

	stack->top = NULL;
	stack->bottom = NULL;

	return stack;
}

int push(tStack *stack, void* data){

	tStackItem *prvek;

	//pridame novy prvek na zasobnik
	if((prvek = (tStackItem*)malloc(sizeof(tStackItem))) == NULL) return ERROR_INTER;
	prvek->prev =  NULL;
	prvek->next = NULL;
	prvek->data = data;

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


int push_back(tStack *stack, void* data){

	tStackItem *prvek;

	

	//pridame novy prvek na zasobnik
	if((prvek = (tStackItem*)malloc(sizeof(tStackItem))) == NULL) return ERROR_INTER;
	prvek->prev =  NULL;
	prvek->next = NULL;
	prvek->data = data;

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



int empty(tStack *stack, bool (*function)(void*)){

	tStackItem *temp; 
	// nekonecna smycka cyklime tak dlouho dokud nam pop neposle NULL coz znamena prazdy zasobnik
	while(true){

		temp = pop_top(stack);
		if(temp == NULL) break;
		(*function)(temp->data);
		free(temp);
	}

	return OK;
}

int deleteSt(tStack *stack, bool (*function)(void*)){
	
	if (stack == NULL) return OK;

	empty(stack, function);
	free(stack);
	return OK;
}
void printStack(tStack *stack){
	if (stack->top == NULL) return;
	printf("Tisknu zasobnik\n===============\n");
	tStackItem *pom = stack->top;
	while (pom != NULL){
		printf("prvek: %d\n", ((T_Token*)(pom)->data)->type);
		pom = pom->prev;
	}

	printf("\n");
}

void printStackInt(tStack *stack){
	if (stack->top == NULL) return;
	printf("Tisknu zasobnik int\n===============\n");
	tStackItem *pom = stack->top;
	while (pom != NULL){
		printf("prvek: %d\n", *((int*)(pom)->data));
		pom = pom->prev;
	}

	printf("======end======\n");
}


// funkce ktere lze pridat do GC ktery je pak zavola na uvolneni pameti, v techto funkcich pak volam delte kde druhy parametr je opet funkce na uvolneni pameti ktera je stejna jak v GC
bool emptySTVar(void * data){ 	deleteSt((tStack*)data, &freeVarST); return true;}
bool emptySTFunc(void * data) { 	deleteSt((tStack*)data, &freeFuncST); return true;}
bool emptyToken(void * data) { 	deleteSt((tStack*)data, &tokenFree);  return true;}


bool destroyST(void * data) {
 	tStackItem *temp;

	// nekonecna smycka cyklime tak dlouho dokud nam pop neposle NULL coz znamena prazdy zasobnik
	while(true){

		temp = pop_top((tStack*)data);
		if(temp == NULL) break;
		free(temp->data);
		free(temp);
	}

	return OK;
 }


//funkce pro dealokaci tokenu
bool tokenFree(void *token){ 
	// zkontrolujeme si jestli to neni E token... ten obsahuje odkazy na dalsi token
	if(((T_Token *)token)->type == S_E){
		//pro jistotu
		if(((T_Token *)token)->value != NULL)
		{
			if(((Tleaf*)((T_Token*)token)->value)->op1 != NULL) tokenFreepom(((Tleaf*)((T_Token*)token)->value)->op1);
			if(((Tleaf*)((T_Token*)token)->value)->op2 != NULL) tokenFreepom(((Tleaf*)((T_Token*)token)->value)->op2);
			if(((Tleaf*)((T_Token*)token)->value)->op != NULL)  free(((Tleaf*)((T_Token*)token)->value)->op);
		}
		
	}
	if(((T_Token *)token)->value != NULL) free(((T_Token *)token)->value ); 
	return true;
}

bool tokenFreepom(T_Token *token)
{	
	// zkontrolujeme si jestli to neni E token... ten obsahuje odkazy na dalsi token
	if(((T_Token *)token)->type == S_E){
		//pro jistotu
		if(((T_Token *)token)->value != NULL)
		{
			if(((Tleaf*)((T_Token*)token)->value)->op1 != NULL) tokenFreepom(((Tleaf*)((T_Token*)token)->value)->op1);
			if(((Tleaf*)((T_Token*)token)->value)->op2 != NULL) tokenFreepom(((Tleaf*)((T_Token*)token)->value)->op2);
			if(((Tleaf*)((T_Token*)token)->value)->op != NULL)  free(((Tleaf*)((T_Token*)token)->value)->op);
		}
	}
	if(token->value != NULL) free(token->value ); 
	free(token);
	return true;
}

/*
int cpyAndPush(T_Token token, tStack *stack){

	T
}
*/