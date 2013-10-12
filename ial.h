#ifndef IAL_H
#define IAL_H

#include "stdlib.h"
#include <stdio.h>
#include "garbage_collector.h"
#include "types.h"
#include <string.h>


typedef struct T_ST_VarsItem // prvek ST promenych, jedna konkretni promena
{
	char *name;
	int type;
	void *value;

}T_ST_VarsItem;

typedef struct T_ST_Vars // tabulka symbolu pro promene
{
	T_ST_VarsItem *data;
	struct T_ST_Vars *right, *left, *parrent;
} T_ST_Vars;

typedef struct T_ST_FuncsItem // Hlavicka funkce
{
	char *name;
	int paramCount;
	T_ST_Vars symbolTable;

}	T_ST_FuncsItem;

typedef struct T_ST_Funcs // tabulka uyivatelsky definovancyh funkci funkci + vestavene funkce
{
	T_ST_FuncsItem *data;
	struct T_ST_Funcs *left, *right, *parrent;
} T_ST_Funcs;

typedef enum
{
	ADDING_SUCCESSFUL =0,
	ITEM_EXIST,
	INTERNAL_ERROR
}ReturnCodesST;

//tabulka symbolu
void varSTInit (T_ST_Vars *pointer);
ReturnCodesST addVarNodeToST(T_ST_VarsItem *newNode, T_ST_Vars *table);
T_ST_Vars* findVarST( char *var, T_ST_Vars *table);
bool removeVarST (char* var, T_ST_Vars *table);

//tabulka funkci
void functionSTInit (T_ST_Funcs *pointer);
ReturnCodesST addFuncNodeToST(T_ST_FuncsItem *newNode, T_ST_Funcs *table);
T_ST_Funcs* findFunctionST( char *funcName, T_ST_Funcs *table);
bool removeFunctionST (char* funcName, T_ST_Funcs *table);

//garbage collector funkce
bool freeVarST(void *tree);
void freeVarSTpom(T_ST_Vars* tree, int smer);

bool freeFuncST(void *tree);
void freeFuncSTpom(T_ST_Funcs* tree, int smer);


#endif
