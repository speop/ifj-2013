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
T_ST_Funcs *functionTable;
T_Token token;

int parser(){

	//vytvorime inicializujeme a pridame do garbage colloectoru tabulky symbolu
	symbolTable = (T_ST_Vars*)malloc(sizeof(T_ST_Vars));
	varSTInit(symbolTable);
	garbage_add(symbolTable,&freeVarST);

	functionTable = (T_ST_Funcs*)malloc(sizeof(T_ST_Funcs));
	functionSTInit(functionTable);
	garbage_add(functionTable,&freeFuncST);

	prevToken = NULL;
	int result;

	if ((result = getToken(&token)) != OK) return result;
	
	result = program();	
	return result;

}

int program(){

	int result = OK;

 	switch (token.type){
 		// pravidllo 1. <program> → php <st-list>
 		case S_PHP:
 			if ((result = getToken(&token)) != OK) return result;
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
	T_Token pomToken;

	switch (token.type){

		// pravidlo: 2. <st-list> → id <expr> ; <st-list>
		case S_FUNC:
		case S_ID: 
			

			// z duvodu semantiky zkontrolujeme jestli nechceme prirazovat do funkce
			pomToken = token;
			if ((result = getToken(&token)) != OK) return result;

			if(token.type == S_IS){
				if(pomToken.type  != S_ID) return SEM_OTHER_ERROR;
			}

			result = expr();
			if(result != OK ) return result;

			if (token.type != S_SEM){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \";\"\n",row);
				return ERROR_SYN;
			} 

			if ((result = getToken(&token)) != OK) return result;
			result = st_list();
			return result;
			break;


		// pravidlo 5. <st-list> → while ( <expr> <cond> { <st-list> }
		case WHILE: pom = false;
		// pravidlo 4. <st-list> → if ( <expr> <cond> { <st-list> } <if-extra>
		case IF: 

			if(token.type == IF) pom = true;

 			if ((result = getToken(&token)) != OK) return result;
			if(token.type != S_LBRA ){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \")\"\n",row);
				return ERROR_SYN;
			}

			if ((result = getToken(&token)) != OK) return result;
			result = expr();
			if(result != OK ) return result;

			if ((result = getToken(&token)) != OK) return result;
			result = cond();
			if(result != OK ) return result;

			if ((result = getToken(&token)) != OK) return result;
			if(token.type != S_BLOCK_START ){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \")\"\n",row);
				return ERROR_SYN;
			}

			
			if ((result = getToken(&token)) != OK) return result;
			// otestujeme jestli se nejedna o pravidlo 3. <st-list> → ε
			if(token.type != S_BLOCK_END ){
				result = st_list();
				if(result != OK ) return result;
				if ((result = getToken(&token)) != OK) return result;
			}

			// mel byt konece
			if(token.type != S_BLOCK_END ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"}\"\n",row);
				return ERROR_SYN;
			}

			//v tokenu byl if pridavame podminku if extra
			if(pom){
				if ((result = getToken(&token)) != OK) return result;
				
				//  nejedna se o pravidlo 18. <if-extra> → ε
				if( token.type == ELSE || token.type == ELSEIF){
					result = if_extra();
					if(result != OK ) return result;
				}
				// jednalo se tak vratime token abychom ho priste znova dostali
				else putToken(&token);
				
			} 

			return OK;
			break;

		// pravidlo 6. <st-list> → function funcId ( <functionList> { <st-list> }
		case FUNCTION: 
			
			if ((result = getToken(&token)) != OK) return result;
			if(token.type != S_FUNC){
				fprintf(stderr, "Row: %d, unexpected symbol it should be some identificator \n",row);
				return ERROR_SYN;
			}

			if ((result = getToken(&token)) != OK) return result;
			if(token.type != S_LBRA){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"(\" \n",row);
				return ERROR_SYN;
			}

			if ((result = getToken(&token)) != OK) return result;
			result = functionList();
			if(result != OK ) return result;

			if ((result = getToken(&token)) != OK) return result;
			if(token.type != S_BLOCK_START){
				fprintf(stderr, "Row: %d, unexpected symbol it should be some identificator \n",row);
				return ERROR_SYN;
			}

			if ((result = getToken(&token)) != OK) return result;
			// otestujeme jestli se nejedna o pravidlo 3. <st-list> → ε
			if(token.type != S_BLOCK_END ){
				result = st_list();
				if(result != OK ) return result;
				if ((result = getToken(&token)) != OK) return result;
			}

			if(token.type != S_BLOCK_END ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"}\"\n",row);
				return ERROR_SYN;
			}

			return OK;
			break;

		// pravidlo 7. <st-list> → return <expr>
		case RETURN:
			if ((result = getToken(&token)) != OK) return result;
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

			if ((result = getToken(&token)) != OK) return result;
			result = expr();
			if(result != OK ) return result;

			if ((result = getToken(&token)) != OK) return result;
			result = cond();
			if(result != OK ) return result;

			return OK;
			break;

		// pravidlo 9. <cond> → )
		case S_LBRA: 
			if ((result = getToken(&token)) != OK) return result;
			return OK;

	}

	return ERROR_SYN;
}

int if_extra(){
	int result;

	switch(token.type){

		// pravidlo 16. <if-extra> → else { <st-list> }
		case ELSE: 

			if ((result = getToken(&token)) != OK) return result;
			if(token.type != S_BLOCK_START ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"{\"\n",row);
				return ERROR_SYN;
			}

			if ((result = getToken(&token)) != OK) return result;

			// otestujeme jestli se nejedna o pravidlo 3. <st-list> → ε
			if(token.type != S_BLOCK_END ){
				result = st_list();
				if(result != OK ) return result;
				if ((result = getToken(&token)) != OK) return result;
			}

			if(token.type != S_BLOCK_END ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"}\"\n",row);
				return ERROR_SYN;
			}

			return OK;
			break;

		// pravidlo 17. <if-extra> → elseif ( <expr> <cond> { <st-list> } <if-extra>
		case ELSEIF:
			if ((result = getToken(&token)) != OK) return result;
			if(token.type != S_LBRA ){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \")\"\n",row);
				return ERROR_SYN;
			}

			if ((result = getToken(&token)) != OK) return result;
			result = expr();
			if(result != OK ) return result;

			if ((result = getToken(&token)) != OK) return result;
			result = cond();
			if(result != OK ) return result;

			if ((result = getToken(&token)) != OK) return result;
			if(token.type != S_BLOCK_START ){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \")\"\n",row);
				return ERROR_SYN;
			}

			
			if ((result = getToken(&token)) != OK) return result;
			// otestujeme jestli se nejedna o pravidlo 3. <st-list> → ε
			if(token.type != S_BLOCK_END ){
				result = st_list();
				if(result != OK ) return result;
				if ((result = getToken(&token)) != OK) return result;
			}

			// mel byt konece
			if(token.type != S_BLOCK_END ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"}\"\n",row);
				return ERROR_SYN;
			}

			
			if ((result = getToken(&token)) != OK) return result;

			//  nejedna se o pravidlo 18. <if-extra> → ε
			if( token.type == ELSE || token.type == ELSEIF){
				result = if_extra();
				if(result != OK ) return result;
			}
			// jednalo se tak vratime token abychom ho priste znova dostali
			else putToken(&token);
			
			

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
			
			if ((result = getToken(&token)) != OK) return result;
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

			if ((result = getToken(&token)) != OK) return result;
			if(token.type != S_ID){
				fprintf(stderr, "Row: %d, unexpected symbol it should be some identificator \n",row);
				return ERROR_SYN;
			}

			if ((result = getToken(&token)) != OK) return result;
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


int expr(){

	return OK;
}

