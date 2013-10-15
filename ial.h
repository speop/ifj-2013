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
	T_ST_Vars *symbolTable;

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
void varSTInit (T_ST_Vars *);
ReturnCodesST addVarNodeToST(T_ST_VarsItem *, T_ST_Vars *);
T_ST_Vars* findVarST( char *, T_ST_Vars *);
bool removeVarST (char* , T_ST_Vars *);
void printTree(T_ST_Vars *);

//tabulka funkci
void functionSTInit (T_ST_Funcs *);
ReturnCodesST addFuncNodeToST(T_ST_FuncsItem *, T_ST_Funcs *);
T_ST_Funcs* findFunctionST( char *, T_ST_Funcs *);
bool removeFunctionST (char* , T_ST_Funcs *);

//garbage collector funkce
bool freeVarST(void *);
void freeVarSTpom(T_ST_Vars* , int );

bool freeFuncST(void *);
void freeFuncSTpom(T_ST_Funcs* , int );




#endif
