//Implementace interpretu jazyka IFJ13.
//xbucht18, Buchta David

#include "ial.h"
#include "types.h"

char* mystrdup2(const char* s)
{
    char* p = malloc(strlen(s)+1);
    if(p==NULL) fprintf(stderr, "Cannot alocate memory\n");
    if (p) strcpy(p, s);
    return p;
}

int registerBuiltIn(T_ST_Funcs *functionTable){
	
	T_ST_FuncsItem *funkce;
	T_ST_Vars *symboly;
	T_ST_VarsItem* promena;
	
	//jsou tu jen rozkopirovane kody pro jednotlive funkce jinac je to principialne stejne

	if((funkce = (T_ST_FuncsItem *)malloc (sizeof(T_ST_FuncsItem)))  == NULL) return ERROR_INTER;
	funkce->name = mystrdup2("boolval");	
	if( (addFuncNodeToST(funkce ,functionTable)) != ADDING_SUCCESSFUL) return ERROR_INTER;
	
	if ((symboly = (T_ST_Vars*) malloc(sizeof(T_ST_Vars)))== NULL ) return ERROR_INTER;
	varSTInit(symboly);
	funkce->symbolTable = symboly;

	if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ) return ERROR_INTER;
	promena->name = mystrdup2("term");
	if((addVarNodeToST(promena , symboly)) != ADDING_SUCCESSFUL)  return ERROR_INTER;

	funkce->paramCount = 1;
	
	/*-----------------------------------------------------------------------------------------*/
	if((funkce = (T_ST_FuncsItem *)malloc (sizeof(T_ST_FuncsItem)))  == NULL) return ERROR_INTER;
	funkce->name = mystrdup2("doubleval");	
	if( (addFuncNodeToST(funkce ,functionTable)) != ADDING_SUCCESSFUL) return ERROR_INTER;
	
	if ((symboly = (T_ST_Vars*) malloc(sizeof(T_ST_Vars)))== NULL ) return ERROR_INTER;
	varSTInit(symboly);
	funkce->symbolTable = symboly;

	if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ) return ERROR_INTER;
	promena->name = mystrdup2("term");
	if((addVarNodeToST(promena , symboly)) != ADDING_SUCCESSFUL)  return ERROR_INTER;

	funkce->paramCount = 1;
	
	/*-----------------------------------------------------------------------------------------*/
	if((funkce = (T_ST_FuncsItem *)malloc (sizeof(T_ST_FuncsItem)))  == NULL) return ERROR_INTER;
	funkce->name = mystrdup2("intval");	
	if( (addFuncNodeToST(funkce ,functionTable)) != ADDING_SUCCESSFUL) return ERROR_INTER;
	
	if ((symboly = (T_ST_Vars*) malloc(sizeof(T_ST_Vars)))== NULL ) return ERROR_INTER;
	varSTInit(symboly);
	funkce->symbolTable = symboly;

	if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ) return ERROR_INTER;
	promena->name = mystrdup2("term");
	if((addVarNodeToST(promena , symboly)) != ADDING_SUCCESSFUL)  return ERROR_INTER;

	funkce->paramCount = 1;
	
	/*-----------------------------------------------------------------------------------------*/
	if((funkce = (T_ST_FuncsItem *)malloc (sizeof(T_ST_FuncsItem)))  == NULL) return ERROR_INTER;
	funkce->name = mystrdup2("strval");	
	if( (addFuncNodeToST(funkce ,functionTable)) != ADDING_SUCCESSFUL) return ERROR_INTER;
	
	if ((symboly = (T_ST_Vars*) malloc(sizeof(T_ST_Vars)))== NULL ) return ERROR_INTER;
	varSTInit(symboly);
	funkce->symbolTable = symboly;

	if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ) return ERROR_INTER;
	promena->name = mystrdup2("term");
	if((addVarNodeToST(promena , symboly)) != ADDING_SUCCESSFUL)  return ERROR_INTER;

	funkce->paramCount = 1;
	
	/*-----------------------------------------------------------------------------------------*/
	if((funkce = (T_ST_FuncsItem *)malloc (sizeof(T_ST_FuncsItem)))  == NULL) return ERROR_INTER;
	funkce->name = mystrdup2("get_string");	
	if( (addFuncNodeToST(funkce ,functionTable)) != ADDING_SUCCESSFUL) return ERROR_INTER;
	
	if ((symboly = (T_ST_Vars*) malloc(sizeof(T_ST_Vars)))== NULL ) return ERROR_INTER;
	varSTInit(symboly);
	funkce->symbolTable = symboly;
	funkce->paramCount = 0;
	
	
	/*-----------------------------------------------------------------------------------------*/
	if((funkce = (T_ST_FuncsItem *)malloc (sizeof(T_ST_FuncsItem)))  == NULL) return ERROR_INTER;
	funkce->name = mystrdup2("put_string");	
	if( (addFuncNodeToST(funkce ,functionTable)) != ADDING_SUCCESSFUL) return ERROR_INTER;
	
	if ((symboly = (T_ST_Vars*) malloc(sizeof(T_ST_Vars)))== NULL ) return ERROR_INTER;
	varSTInit(symboly);
	funkce->symbolTable = symboly;
	funkce->paramCount = 0;
	
	/*-----------------------------------------------------------------------------------------*/
	if((funkce = (T_ST_FuncsItem *)malloc (sizeof(T_ST_FuncsItem)))  == NULL) return ERROR_INTER;
	funkce->name = mystrdup2("strlen");	
	if( (addFuncNodeToST(funkce ,functionTable)) != ADDING_SUCCESSFUL) return ERROR_INTER;
	
	if ((symboly = (T_ST_Vars*) malloc(sizeof(T_ST_Vars)))== NULL ) return ERROR_INTER;
	varSTInit(symboly);
	funkce->symbolTable = symboly;

	if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ) return ERROR_INTER;
	promena->name = mystrdup2("string");
	if((addVarNodeToST(promena , symboly)) != ADDING_SUCCESSFUL)  return ERROR_INTER;
	

	funkce->paramCount = 1;
	
	/*-----------------------------------------------------------------------------------------*/
	if((funkce = (T_ST_FuncsItem *)malloc (sizeof(T_ST_FuncsItem)))  == NULL) return ERROR_INTER;
	funkce->name = mystrdup2("get_substring");	
	if( (addFuncNodeToST(funkce ,functionTable)) != ADDING_SUCCESSFUL) return ERROR_INTER;
	
	if ((symboly = (T_ST_Vars*) malloc(sizeof(T_ST_Vars)))== NULL ) return ERROR_INTER;
	varSTInit(symboly);
	funkce->symbolTable = symboly;

	if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ) return ERROR_INTER;
	promena->name = mystrdup2("string");
	if((addVarNodeToST(promena , symboly)) != ADDING_SUCCESSFUL)  return ERROR_INTER;
	
	if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ) return ERROR_INTER;
	promena->name = mystrdup2("integer1");
	if((addVarNodeToST(promena , symboly)) != ADDING_SUCCESSFUL)  return ERROR_INTER;
	
	if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ) return ERROR_INTER;
	promena->name = mystrdup2("integer2");
	if((addVarNodeToST(promena , symboly)) != ADDING_SUCCESSFUL)  return ERROR_INTER;

	funkce->paramCount = 3;
	
	/*-----------------------------------------------------------------------------------------*/
	if((funkce = (T_ST_FuncsItem *)malloc (sizeof(T_ST_FuncsItem)))  == NULL) return ERROR_INTER;
	funkce->name = mystrdup2("find_string");	
	if( (addFuncNodeToST(funkce ,functionTable)) != ADDING_SUCCESSFUL) return ERROR_INTER;
	
	if ((symboly = (T_ST_Vars*) malloc(sizeof(T_ST_Vars)))== NULL ) return ERROR_INTER;
	varSTInit(symboly);
	funkce->symbolTable = symboly;

	if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ) return ERROR_INTER;
	promena->name = mystrdup2("string1");
	if((addVarNodeToST(promena , symboly)) != ADDING_SUCCESSFUL)  return ERROR_INTER;
	
	if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ) return ERROR_INTER;
	promena->name = mystrdup2("string2");
	if((addVarNodeToST(promena , symboly)) != ADDING_SUCCESSFUL)  return ERROR_INTER;

	funkce->paramCount = 2;
	
	/*-----------------------------------------------------------------------------------------*/
	if((funkce = (T_ST_FuncsItem *)malloc (sizeof(T_ST_FuncsItem)))  == NULL) return ERROR_INTER;
	funkce->name = mystrdup2("sort_string");	
	if( (addFuncNodeToST(funkce ,functionTable)) != ADDING_SUCCESSFUL) return ERROR_INTER;
	
	if ((symboly = (T_ST_Vars*) malloc(sizeof(T_ST_Vars)))== NULL ) return ERROR_INTER;
	varSTInit(symboly);
	funkce->symbolTable = symboly;

	if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ) return ERROR_INTER;
	promena->name = mystrdup2("string");
	if((addVarNodeToST(promena , symboly)) != ADDING_SUCCESSFUL)  return ERROR_INTER;;

	funkce->paramCount = 1;
		

	return OK;
}