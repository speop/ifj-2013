#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "ial.h"
#define debug 1

extern TGarbageList trash; //z main.c

int parser(){

	T_ST_Vars *strom = (T_ST_Vars*)malloc(sizeof(T_ST_Vars));
	varSTInit(strom);
	garbage_add(strom,&freeVarST);

	
	
	

	return 0;

}
