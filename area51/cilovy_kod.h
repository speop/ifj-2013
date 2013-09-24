//Implementace interpretu imperativního jazyka IFJ12.
//xbucht18, Buchta David
//xrajca00, Rajca Tomas

#ifndef CILOVY_KOD_H
#define CILOVY_KOD_H

#include "ial.h"
#include "vnitrni_kod.h"

typedef struct{
	int jmp;
	T_bt_root *tabulka;
}stack_calls;

char *strduplicit (char **str);
void free_all(int pst_function_index, stack_calls *pst_function );
int cilovy_kod (TAC pole[],int recipe_size);

#endif
