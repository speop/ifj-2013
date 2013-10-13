#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "ial.h"
#include "types.h"
#define debug 1

extern TGarbageList trash; //z main.c
extern int row; // z main.c

T_ST_Vars *symbolTable;
T_ST_Funcs *funcionTable;
T_Token token;

int parser(){

	//vytvorime inicializujeme a pridame do garbage colloectoru tabulky symbolu
	symbolTable = (T_ST_Vars*)malloc(sizeof(T_ST_Vars));
	varSTInit(symbolTable);
	garbage_add(symbolTable,&freeVarST);

	funcionTable = (T_ST_Funcs*)malloc(sizeof(T_ST_Funcs));
	varSTInit(funcionTable);
	garbage_add(funcionTable,&freeFuncST);

	token = getToken();

	// pravidlo 1. <program> → php <st-list>
	if(token.type != S_PHP) {
		fprintf(stderr, "Row: %d, unexpected symbol it shoud be  \"<?php\"\n",row);
		return ERROR_SYN;
	
	}else{
		token = getToken();
		int ret = st_list();
	} 
	

	return ret;

}

int st_list(){

	int result;

	switch (token.type){

		// pravidlo: 2. <st-list> → id <expr> ; <st-list>
		case S_ID: 
			token = getToken();

			// z duvodu semantiky zkontrolujeme jestli nechceme prirazovat do funkce
			if(token.type == S_IS){
				if((findFunctionST( token.((char*)value), functionTable)) != NULL) return SEM_OTHER_ERROR;
			}

			result = expr();
			if(result != OK ) return result;

			if (token.type != S_SEM){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \";\"\n",row);
				return ERROR_SYN;
			} 

			token = getToken();
			result = st_list();
			break;


		// pravidlo 4. <st-list> → if ( <expr> <cond> { <st-list> } <if-extra>
		case IF: 

			token = getToken();
			if(token.type != S_LBRA ){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \")\"\n",row);
				return ERROR_SYN;
			}

			token = getToken();
			result = expr();
			if(result != OK ) return result;

			token = getToken();
			result = cond();
			if(result != OK ) return result;

			token = getToken();
			if(token.type != S_LBRA ){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \")\"\n",row);
				return ERROR_SYN;
			}

			

	} 

}



void printError(char *str, int type){

}

