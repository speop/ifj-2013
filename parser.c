#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "ial.h"
#include "types.h"
#include "scaner.h"
#define debug 1

extern TGarbageList trash; //z main.c
extern int row; // z main.c
extern T_Token *prevToken; // z main.c

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

	prevToken = NULL;
	token = getToken();

	int result;
	result = program();	
	return result;

}

int program(){

	int result = OK;

 	switch (token.type){
 		// pravidllo 1. <program> → php <st-list>
 		case S_PHP:
 			token = getToken();
 			result = st_list();
 			return result;
 			break;

 		case S_DOLAR:
 			return result;

 	}
 		
	return ERROR_SYN;
 }

int st_list(){

	int result;
	bool pom = false;

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
			return result;
			break;


		// pravidlo 5. <st-list> → while ( <expr> <cond> { <st-list> }
		case WHILE: pom = false;
		// pravidlo 4. <st-list> → if ( <expr> <cond> { <st-list> } <if-extra>
		case IF: 

			if(token.type == IF) pom = true;

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
			if(token.type != S_BLOCK_START ){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \")\"\n",row);
				return ERROR_SYN;
			}

			
			token = getToken();
			// otestujeme jestli se nejedna o pravidlo 3. <st-list> → ε
			if(token.type != S_BLOCK_END ){
				result = st_list();
				if(result != OK ) return result;
				token = getToken();
			}

			// mel byt konece
			if(token.type != S_BLOCK_END ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"}\"\n",row);
				return ERROR_SYN;
			}

			//v tokenu byl if pridavame podminku if extra
			if(pom){
				token = getToken();
				
				//  nejedna se o pravidlo 18. <if-extra> → ε
				if( token.type == ELSE || token.type == ELSEIF){
					result = if_extra();
					if(result != OK ) return result;
				}
				// jednalo se tak vratime token abychom ho priste znova dostali
				else putToken(token);
				
			} 

			return OK;
			break;

		// pravidlo 6. <st-list> → function id ( <functionList> { <st-list> }
		case FUNCTION: 
			
			token = getToken();
			if(token.type != S_ID){
				fprintf(stderr, "Row: %d, unexpected symbol it should be some identificator \n",row);
				return ERROR_SYN;
			}

			token = getToken();
			if(token.type != S_LBRA){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"(\" \n",row);
				return ERROR_SYN;
			}

			token = getToken();
			result = functionList();
			if(result != OK ) return result;

			token = getToken();
			if(token.type != S_BLOCK_START){
				fprintf(stderr, "Row: %d, unexpected symbol it should be some identificator \n",row);
				return ERROR_SYN;
			}

			token =getToken();
			// otestujeme jestli se nejedna o pravidlo 3. <st-list> → ε
			if(token.type != S_BLOCK_END ){
				result = st_list();
				if(result != OK ) return result;
				token = getToken();
			}

			if(token.type != S_BLOCK_END ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"}\"\n",row);
				return ERROR_SYN;
			}

			return OK
			break;

		// pravidlo 7. <st-list> → return <expr>
		case RETURN:
			token = getToken();
			result = expr();
			if(result != OK ) return result;

			return OK;
			break;

		case S_EOF: return OK;
		// jeste je  treba napsat pravidlo pro $ akorat nevim jak na to
	}

	return ERROR_SYN; 

}

int cond(){
	int result;

	switch(token.type){

		//pravidlo 8. <cond> → <comp> <expr> <cond>
		case S_LEQ:
		case S_LST:
		case S_GEQ:
		case S_GRT:
		case S_NEQ:
		case S_EQ:

			// tento case zaroven supluje  vsechny comp podminky, takze neni treba psat funkci pro tento neterminal

			token = getToken();
			result = expr();
			if(result != OK ) return result;

			token = getToken();
			result = cond();
			if(result != OK ) return result;

			return OK;
			break;

		// pravidlo 9. <cond> → )
		case S_LBRA: 
			token = getToken();
			return OK;

	}

	return ERROR_SYN;
}

int if_extra(){
	
	switch(token.type){

		// pravidlo 16. <if-extra> → else { <st-list> }
		case ELSE: 

			token = getToken();
			if(token.type != S_BLOCK_START ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"{\"\n",row);
				return ERROR_SYN;
			}

			token = getToken();

			// otestujeme jestli se nejedna o pravidlo 3. <st-list> → ε
			if(token.type != S_BLOCK_END ){
				result = st_list();
				if(result != OK ) return result;
				token = getToken();
			}

			if(token.type != S_BLOCK_END ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"}\"\n",row);
				return ERROR_SYN;
			}

			return OK
			break;

		// pravidlo 17. <if-extra> → elseif ( <expr> <cond> { <st-list> } <if-extra>
		case ELSEIF:
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
			if(token.type != S_BLOCK_START ){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \")\"\n",row);
				return ERROR_SYN;
			}

			
			token = getToken();
			// otestujeme jestli se nejedna o pravidlo 3. <st-list> → ε
			if(token.type != S_BLOCK_END ){
				result = st_list();
				if(result != OK ) return result;
				token = getToken();
			}

			// mel byt konece
			if(token.type != S_BLOCK_END ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"}\"\n",row);
				return ERROR_SYN;
			}

			
			token = getToken();

			//  nejedna se o pravidlo 18. <if-extra> → ε
			if( token.type == ELSE || token.type == ELSEIF){
				result = if_extra();
				if(result != OK ) return result;
			}
			// jednalo se tak vratime token abychom ho priste znova dostali
			else putToken(token);
			
			

			return OK;
			break;


	}

	return ERROR_SYN;
}

int functionList(){

	int result;

	switch(token.type){
		
		// pravidlo 19. <functionList> → id <functionList>
		case S_ID:
			
			token = getToken();
			result = functionList();
			if(result != OK ) return result;

			return OK;
			break;

		//pravidlo 20. <functionList> → )
		case S_RBRA :
			return OK;
			break;

		//pravidlo 21. <functionList> → , id <functionList>
		case S_COMMA:

			token = getToken();
			if(token.type != S_ID){
				fprintf(stderr, "Row: %d, unexpected symbol it should be some identificator \n",row);
				return ERROR_SYN;
			}

			token = getToken();
			result = functionList();
			
			if(result != OK) return ERROR_SYN;

			return OK;
			break;

	default: 
		fprintf(stderr, "Row: %d, unexpected symbol \n",row);
		return ERROR_SYN;
	}

	return ERROR_SYN;
}




