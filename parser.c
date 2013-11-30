//Implementace interpretu jazyka IFJ13.
//xbucht18, Buchta David

#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "ial.h"
#include "types.h"
#include "scaner.h"
#include "ast_tree.h"
#include "stack.h"
#include "vnitrni.h"
#include "built_in.h"
#include "interpret.h"

#define debug 0 //  0 - vypnuto, 1 - lehka verze, > 1 vypisuje se i stack a porovnavaci tokeny
#define POLE 23

extern TGarbageList trash; //z main.c
extern int row; // z main.c
extern T_Token *prevToken; // z main.c
extern FILE* pSource_File; // z main.c

int por = 0;

T_ST_Vars *symbolTable, *actualST;
T_ST_Funcs *functionTable;
Tleaf *ass,  *pomVetev;
T_Token token, *eTokenAlej, *exprTree;
tStack *zasobnik,*alejStromu;
int konecBloku = 0;

//precedenci tabulka
// TODO: dle zadani doplnit priority do tabulky					
static int prtable [POLE][POLE] = {
/*
				0		1		2		3		4		5		6		7		8		9		10		11		12		13		14		15		16		18		19		20		21		22		23
				+		*		(		)		=		.		/		-		,		f		id		i		d		s		b		n		$  		<		>		<=		>=		===		!==*/
/*  0  + */	{	H,		L,		L,		H,		X,		H,		L,		H,		H,		L,		L,		L,		L,		L,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  1  * */ {	H,		H,		L,		H,		X,		H,		H,		H,		H,		L,		L,		L,		L,		L,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  2  ( */ {	L,		L,		L,		EQ,		X,		L,		L,		L,		L,		X,		L,		L,		L,		L,		L,		L,		X,		L,		L,		L,		L,		L,		L},
/*  3  ) */ {	H,		H,		X,		H,		X,		H,		H,		H,		H,		X,		X,		X,		X,		X,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  4  = */	{	L,		L,		L,		X,		X,		L,		L,		L,		X,		L,		L,		L,		L,		L,		L,		L,		H,		X,		X,		X,		X,		X,		X},
/*  5  . */ {	H,		L,		L,		H,		X,		H,		L,		H,		X,		L,		L,		L,		L,		L,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  6  / */ {	H,		H,		L,		H,		X,		H,		H,		H,		X,		L,		L,		L,		L,		L,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  7  - */ {	H,		L,		L,		H,		X,		H,		L,		H,		X,		L,		L,		L,		L,		L,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  8  , */ {	L,		L,		L,		H,		X,		L,		L,		L,		H,		L,		L,		L,		L,		L,		L,		L,		X,		L,		L,		L,		L,		L,		L},
/*  9  f */ {	X,		X,		L,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X},
/*  10 id */{	H,		H,		X,		H,		H,		H,		H,		H,		H,		X,		X,		X,		X,		X,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  11  i */{	H,		H,		X,		H,		X,		H,		H,		H,		H,		X,		X,		X,		X,		X,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  12  d */{	H,		H,		X,		H,		X,		H,		H,		H,		H,		X,		X,		X,		X,		X,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  13  s */{	H,		H,		X,		H,		X,		H,		H,		H,		H,		X,		X,		X,		X,		X,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  14  b */{	X,		X,		X,		H,		X,		X,		X,		X,		H,		X,		X,		X,		X,		X,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  15  n */{	X,		X,		X,		H,		X,		X,		X,		X,		H,		X,		X,		X,		X,		X,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  17  $ */{	L,		L,		L,		X,		L,		L,		L,		L,		X,		L,		L,		L,		L,		L,		L,		L,		EQ,		L,		L,		L,		L,		L,		L},
/*  18  < */{	L,		L,		L,		H,		X,		L,		L,		L,		L,		L,		L,		L,		L,		L,		L,		L,		H,		H,		H,		H,		H,		H,		H},	
/*  19  > */{	L,		L,		L,		H,		X,		L,		L,		L,		L,		L,		L,		L,		L,		L,		L,		L,		H,		H,		H,		H,		H,		H,		H},
/*  20  <=*/{	L,		L,		L,		H,		X,		L,		L,		L,		L,		L,		L,		L,		L,		L,		L,		L,		H,		H,		H,		H,		H,		H,		H},
/*  21  >=*/{	L,		L,		L,		H,		X,		L,		L,		L,		L,		L,		L,		L,		L,		L,		L,		L,		H,		H,		H,		H,		H,		H,		H},
/*  22 ===*/{	L,		L,		L,		H,		X,		L,		L,		L,		L,		L,		L,		L,		L,		L,		L,		L,		H,		L,		L,		L,		L,		H,		H},
/*  23 !==*/{	L,		L,		L,		H,		X,		L,		L,		L,		L,		L,		L,		L,		L,		L,		L,		L,		H,		L,		L,		L,		L,		H,		H}

};
int parser(){

	//vytvorime inicializujeme a pridame do garbage colloectoru tabulky symbolu
	symbolTable = (T_ST_Vars*)malloc(sizeof(T_ST_Vars));
	varSTInit(symbolTable);
	garbage_add(symbolTable,&freeVarST);

	functionTable = (T_ST_Funcs*)malloc(sizeof(T_ST_Funcs));
	functionSTInit(functionTable);
	garbage_add(functionTable,&freeFuncST);
	
	//zaregistrujem built_in funkce
	if ((registerBuiltIn(functionTable)) != OK) { fprintf(stderr, "ERROR at registering built in functions\n"); return ERROR_INTER;}

	//vytvorime zasobik 
	zasobnik = SInit();
	garbage_add(zasobnik, &emptyToken);

	alejStromu = SInit();
	garbage_add(alejStromu, &emptyToken);

	prevToken = NULL;
	token.value = NULL;
	actualST = symbolTable;

	row = 1;

	int result;

	

	result = program();	
	if(result!= OK) return result;

	#if debug 
		printf("\nVolam generator\n");
	#endif
	
	result = generateCode();
	if(result!= OK) return result;

	#if debug
		printf("\nChtel bych volat interpet :)\n");
	#endif
	result = interpret(); 
	return result;


}

int program(){

	int result = OK;
	if ((result = getToken(&token)) != OK) return result;

 	switch (token.type){
 		// pravidllo 1. <program> → php <st-list>
 		case S_PHP:
 			
 			#if debug 
				printf("byl znak php zpracuju hlavicky funkci\n");		
			#endif
			
 			//zaciname php zajistime si hlavicky funkci
			//if ((result = getToken(&token)) != OK) return result;
 			result = functionHeaders();
			if(result != OK) return result;

			
			//rewindli jsme... zase se nam posle <?php tak to preskocime
			if ((result = getToken(&token)) != OK) return result;
			token.type = -1;
			if ((result = getToken(&token)) != OK) return result;

			#if debug
				printf("Dokoncene zpracovani hlavicek\n");
			#endif
 			result = st_list();
 			return result;
 			break;

 		default: 
 			return result;

 	}
 		
	return ERROR_SYN;
 }

int st_list(){

	int result ; //ret;
	bool pom = false;
	T_Token *pomToken;
	char* funcName;
	//T_ST_VarsItem *promena;
	T_ST_Funcs *func;

	switch (token.type){

		// pravidlo: 2. <st-list> → id <expr> ; <st-list>
		case S_FUNC:
		case S_ID: 
			
			result = expr();
			if(result != OK ) return result;

			//nahraju strom do aleje
			if ((eTokenAlej = (T_Token*) malloc(sizeof(T_Token))) == NULL) return INTERNAL_ERROR;
			eTokenAlej->type = S_E;
			eTokenAlej->value = exprTree->value;
			//printf("typ v aleji: %d\n",((Tleaf*)(eTokenAlej->value))->op->type );
			if (push(alejStromu,eTokenAlej) != OK) return ERROR_INTER;

			#if debug 
				printf("Zpracovany epxr\n");		
			#endif
				
			if(result != OK ) return result;
			if (token.type != S_SEM){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \";\"\n",row);
				return ERROR_SYN;
			} 

			if ((result = getToken(&token)) != OK) return result;

			//printASS((Tleaf*)((T_Token*)(exprTree->value)->value));
			//konec bloku je jen ve while a if a elseif, nemuye byt v hlavnim programu ale stlist je stejny jako v tam tech jako v tehle takze si to jen otestujeme
			result = st_list();
			return result;
			break;


		// pravidlo 5. <st-list> → while ( <expr> <cond> { <st-list> } <st-list>
		case WHILE: pom = false;
		// pravidlo 4. <st-list> → if  <expr>  { <st-list> } <if-extra>
		//edited cele v zavorce je expr
		//edited zavorka vubec neni :D
		case IF: 
			konecBloku++;
			if(token.type == IF) pom = true;

			//zalohujeme si token
			if ((pomToken = (T_Token*) malloc(sizeof(T_Token))) == NULL) return INTERNAL_ERROR;
			pomToken->type = token.type;
			pomToken->value = NULL;
	
			


 			if ((result = getToken(&token)) != OK) return result;
 			
			result = expr();
			if(result != OK ) return result;

			//udelame vetev a pushneme ji na stack
			if ((pomVetev =   makeLeaf(pomToken, exprTree , NULL)) == NULL) {tokenFree(pomToken); return INTERNAL_ERROR; }
			if ((eTokenAlej = (T_Token*) malloc(sizeof(T_Token))) == NULL) return INTERNAL_ERROR;
			eTokenAlej->type = S_E;
			eTokenAlej->value = pomVetev;
			if (push(alejStromu,eTokenAlej) != OK) return ERROR_INTER;


			if(token.type != S_BLOCK_START ){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"{\"\n",row);
				return ERROR_SYN;
			}

			//nahrajem token do aleje
			if ((pomToken = (T_Token*) malloc(sizeof(T_Token))) == NULL) return INTERNAL_ERROR;
			pomToken->type = S_BLOCK_START;
			pomToken->value = NULL;
			if (push(alejStromu,pomToken) != OK) {tokenFree(pomToken); return ERROR_INTER;}

			
			if ((result = getToken(&token)) != OK) return result;

			// otestujeme jestli se nejedna o pravidlo 3. <st-list> → ε

			if(token.type != S_BLOCK_END ){ 
				result = st_list();
				if(result != OK ) return result;
			}
			
			// mel byt konec
			if(token.type != S_BLOCK_END ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"}\"\n",row);
				return ERROR_SYN;
			}

			//nahrajem token do aleje
			if ((pomToken = (T_Token*) malloc(sizeof(T_Token))) == NULL) return INTERNAL_ERROR;
			// kvuli generatoru ciloveho kodu
			if(pom) pomToken->type = S_BLOCK_END;
			else pomToken->type = WHILE_BLOCK_END;
			pomToken->value = NULL;
			if (push(alejStromu,pomToken) != OK) {tokenFree(pomToken); return ERROR_INTER;}

			konecBloku--;
			
			//v tokenu byl if pridavame podminku if extra
			if(pom){
				if ((result = getToken(&token)) != OK) return result;
				
				//  nejedna se o pravidlo 18. <if-extra> → ε
				if( token.type == ELSE || token.type == ELSEIF){
					result = if_extra();
					if(result != OK ) return result;
				}
				// jednalo se tak vratime token abychom ho priste znova dostali
				//else putToken(&token);
				
			} 
			//hack kvuli while.. nevim proc ale jinak to neprojde
			else { if ((result = getToken(&token)) != OK) return result;}

			result = st_list();
			if(result != OK) return result;

			return OK;
			break;

		// pravidlo 6. <st-list> → function funcId ( <functionList> { <st-list> } <st-list>
		case FUNCTION: 
			konecBloku++;
			if ((result = getToken(&token)) != OK) return result;
			if(token.type != S_FUNC){
				fprintf(stderr, "Row: %d, unexpected symbol it should be some identificator \n",row);
				return ERROR_SYN;
			}
			
			if ((pomToken = (T_Token*) malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;
			pomToken->type = FUNCTION;
			pomToken->value = mystrdup(token.value);
			funcName = pomToken->value;
			token.value = NULL;
			if (push(alejStromu,pomToken) != OK) {tokenFree(pomToken); fprintf(stderr,"Leaf pushing error\n"); return ERROR_INTER;}
			
			

			if ((result = getToken(&token)) != OK) return result;
			if(token.type != S_LBRA){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"(\" \n",row);
				return ERROR_SYN;
			}

			do {
				if ((result = getToken(&token)) != OK) return result; 
				
				//ulozime si jmeno parametru
				if(token.type == S_ID){
				
					if ((pomToken = (T_Token*) malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;
						pomToken->type = STORE_PARAM;
						pomToken->value = mystrdup(token.value);
						//funcName = pomToken->value;
						token.value = NULL;
						if (push(alejStromu,pomToken) != OK) {fprintf(stderr,"Leaf pushing error\n"); tokenFree(pomToken); return ERROR_INTER;}
				}
				
			}
			while(token.type != S_RBRA);
			//printf("Zde mam typ tokenu %d\n",token.type);
			
			if ((result = getToken(&token)) != OK) return result;
			if(token.type != S_BLOCK_START){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"{\" \n",row);
				return ERROR_SYN;
			}

			if ((pomToken = (T_Token*) malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;
			pomToken->type = FUNCTION_BLOCK_START;
			pomToken->value = NULL;
			if (push(alejStromu,pomToken) != OK) {tokenFree(pomToken); fprintf(stderr,"Leaf pushing error\n"); return ERROR_INTER;}

			if ((result = getToken(&token)) != OK) return result;
			
			// otestujeme jestli se nejedna o pravidlo 3. <st-list> → ε
			
			if(token.type != S_BLOCK_END ){
				//printf("zmena tabulky \n");
				//zmenime tabulku symbolu na danou funkci
				func = findFunctionST(funcName , functionTable);
				if(func == NULL ) {fprintf(stderr,"Error in changing symbol table\n");return ERROR_INTER;}
				actualST = func->data->symbolTable;

				result = st_list();
				if(result != OK ) return result; 
				//if ((result = getToken(&token)) != OK) return result;
			}
			

			if(token.type != S_BLOCK_END ){ 
				fprintf(stderr, "Row: %d,fg unexpected symbol it should be \"}\"\n",row);
				return ERROR_SYN;
			}
			konecBloku--;
			//hodime si info pro sebe ze konci blok funkce
			if ((pomToken = (T_Token*) malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;
			pomToken->type = INTER_RETURN;
			pomToken->value = NULL;
			if (push(alejStromu,pomToken) != OK) {tokenFree(pomToken); fprintf(stderr,"Leaf pushing error\n"); return ERROR_INTER;}

			actualST = symbolTable;

			if ((pomToken = (T_Token*) malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;
			pomToken->type = FUNCTION_BLOCK_END;
			pomToken->value = NULL;
			if (push(alejStromu,pomToken) != OK) {tokenFree(pomToken); fprintf(stderr,"Leaf pushing error\n"); return ERROR_INTER;}

			if ((result = getToken(&token)) != OK) return result;
			result = st_list();
			if(result != OK) return result;

			return OK;
			break;

		// pravidlo 7. <st-list> → return <expr> <st-list>
		case RETURN:
			if ((result = getToken(&token)) != OK) return result;
			if(token.type != S_SEM){
				result = expr();

				if(result != OK ) return result;

				if ((pomToken = (T_Token*) malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;
				pomToken->type = RETURN;
				pomToken->value = exprTree;
				//printf("v returnu expr: %d\n",((Tleaf*)(exprTree->value))->op->type );
				if ((pomVetev =   makeLeaf(pomToken, exprTree , NULL)) == NULL) {tokenFree(pomToken); fprintf(stderr,"Leaf pushing error\n"); return ERROR_INTER; }
				if ((eTokenAlej = (T_Token*) malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;
				eTokenAlej->type = S_E;
				eTokenAlej->value = pomVetev;
				if (push(alejStromu,eTokenAlej) != OK) { tokenFree(eTokenAlej); return ERROR_INTER;}
			}
			else{
				if ((eTokenAlej = (T_Token*) malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;
				eTokenAlej->type = RETURN;
				eTokenAlej->value = NULL;
				if (push(alejStromu,eTokenAlej) != OK) { free(eTokenAlej); fprintf(stderr,"Leaf pushing error\n"); return ERROR_INTER;}
			}

			if ((result = getToken(&token)) != OK) return result;
			
			result = st_list();
			if(result != OK) return result;

			return OK;
			break;

		case S_EOF: return OK;
		default: 
			if(token.type == S_BLOCK_END && konecBloku) return OK;
			else {fprintf(stderr, "Row: %d, unexpected symbol\n", row); 
				return  ERROR_SYN; }
	}

	return ERROR_SYN; 

}


int if_extra(){
	int result;
	T_Token *pomToken;


	switch(token.type){

		// pravidlo 16. <if-extra> → else { <st-list> }
		case ELSE: 
			konecBloku++;

			//hodime si else na ASS .. 
			if ((pomToken = (T_Token*) malloc(sizeof(T_Token))) == NULL) return INTERNAL_ERROR;
			pomToken->type = token.type;
			pomToken->value = NULL;

			if ((pomVetev =   makeLeaf(pomToken, NULL , NULL)) == NULL) {tokenFree(pomToken); return INTERNAL_ERROR; }
			if ((eTokenAlej = (T_Token*) malloc(sizeof(T_Token))) == NULL) return INTERNAL_ERROR;
			eTokenAlej->type = S_E;
			eTokenAlej->value = pomVetev;
			if (push(alejStromu,eTokenAlej) != OK) return ERROR_INTER;




			if ((result = getToken(&token)) != OK) return result;
			if(token.type != S_BLOCK_START ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"{\"\n",row);
				return ERROR_SYN;
			}


			//nahrajem token do aleje
			if ((pomToken = (T_Token*) malloc(sizeof(T_Token))) == NULL) return INTERNAL_ERROR;
			pomToken->type = S_BLOCK_START;
			pomToken->value = NULL;
			if (push(alejStromu,pomToken) != OK) {tokenFree(pomToken); return ERROR_INTER;}


			if ((result = getToken(&token)) != OK) return result;



			// otestujeme jestli se nejedna o pravidlo 3. <st-list> → ε
			if(token.type != S_BLOCK_END ){
				result = st_list();
				if(result != OK ) return result;
				//if ((result = getToken(&token)) != OK) return result;
			}
			
			if(token.type != S_BLOCK_END ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"}\"\n",row);
				return ERROR_SYN;
			}


			//nahrajem token do aleje
			if ((pomToken = (T_Token*) malloc(sizeof(T_Token))) == NULL) return INTERNAL_ERROR;
			pomToken->type = S_BLOCK_END;
			pomToken->value = NULL;
			if (push(alejStromu,pomToken) != OK) {tokenFree(pomToken); return ERROR_INTER;}

			konecBloku--;

			if ((result = getToken(&token)) != OK) return result;
			
			result = st_list();
			if(result != OK) return result;

			return OK;
			break;

		// pravidlo 17. <if-extra> → elseif (expr) { <st-list> } <if-extra> <st-lsit>
		case ELSEIF:
			
			konecBloku++;
			
			//zalohujeme si token
			if ((pomToken = (T_Token*) malloc(sizeof(T_Token))) == NULL) return INTERNAL_ERROR;
			pomToken->type = token.type;
			pomToken->value = NULL;
	
			if ((result = getToken(&token)) != OK) return result;
			
			result = expr();
			if(result != OK ) return result;
			

			//udelame vetev a pushneme ji na stack
			if ((pomVetev =   makeLeaf(pomToken, exprTree , NULL)) == NULL) {tokenFree(pomToken); return INTERNAL_ERROR; }
			if ((eTokenAlej = (T_Token*) malloc(sizeof(T_Token))) == NULL) return INTERNAL_ERROR;
			eTokenAlej->type = S_E;
			eTokenAlej->value = pomVetev;
			if (push(alejStromu,eTokenAlej) != OK) return ERROR_INTER;

			
			if(token.type != S_BLOCK_START ){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"{\"\n",row);
				return ERROR_SYN;
			}

			//nahrajem token do aleje
			if ((pomToken = (T_Token*) malloc(sizeof(T_Token))) == NULL) return INTERNAL_ERROR;
			pomToken->type = S_BLOCK_START;
			pomToken->value = NULL;
			if (push(alejStromu,pomToken) != OK) {tokenFree(pomToken); return ERROR_INTER;}

			
			if ((result = getToken(&token)) != OK) return result;
			// otestujeme jestli se nejedna o pravidlo 3. <st-list> → ε
			if(token.type != S_BLOCK_END ){
				result = st_list();
				if(result != OK ) return result;
			}

			// mel byt konece
			if(token.type != S_BLOCK_END ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"}\"\n",row);
				return ERROR_SYN;
			}

			//nahrajem token do aleje
			if ((pomToken = (T_Token*) malloc(sizeof(T_Token))) == NULL) return INTERNAL_ERROR;
			pomToken->type = S_BLOCK_END;
			pomToken->value = NULL;
			if (push(alejStromu,pomToken) != OK) {tokenFree(pomToken); return ERROR_INTER;}

			
			if ((result = getToken(&token)) != OK) return result;
			konecBloku--;

			//  nejedna se o pravidlo 18. <if-extra> → ε
			if( token.type == ELSE || token.type == ELSEIF){
				result = if_extra();
				if(result != OK ) return result;
			}
			// nejednalo se tak vratime token abychom ho priste znova dostali
			//else putToken(&token);
			
			//if ((result = getToken(&token)) != OK) return result;
			result = st_list();
			if(result != OK) return result;

			return OK;
			break;


	}

	return ERROR_SYN;
}

int functionList(){

	int result;
	//printf("\n");
	switch(token.type){
		
		// pravidlo 19. <functionList> → id <functionList>
		case S_ID:
		
			free(token.value);
			token.value = NULL;
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


int functionHeaders(){
	// funkce implementuje tohle <st-list> → function id ( <functionList> ) pouze zjistujeme hlavicky fci

	T_ST_FuncsItem *funkce;
	T_ST_VarsItem *promena;
	T_ST_Vars *symboly;
	int result, paramCount;
	ReturnCodesST ret;


	//scanner nam posle klicove slovo function
	while(true)
	{
			
	paramCount = 0;
	

	//pockame az nam  scanner posle tokken FUNCTION popripad pokud neni (uz) zadna definice funkce dostaname EOF a to se vratime 
	do{
		if ((result = getFunctionHeader(&token, NEXT_READ)) != OK) return result;
		if(token.type == S_EOF) return OK;
		
	}while(token.type != FUNCTION);
	
	
	if ((result = getFunctionHeader(&token, CONTINUE_READ)) != OK) return result;	
	// ocekavame jmeno funkce
	if (token.type != S_FUNC) return ERROR_SYN;
	
	//pridame do tabulky funkci novou funkci a vytvorime ji tabulku symbolu
	if (( funkce = (T_ST_FuncsItem *)malloc (sizeof(T_ST_FuncsItem)))  == NULL) return INTERNAL_ERROR;
	funkce->name = mystrdup(token.value);	
	ret = addFuncNodeToST(funkce ,functionTable);
	
	#if debug
		printf("Registruji funkci: %s\n",(char*)(funkce->name));
	#endif
	

	if(ret == ITEM_EXIST) { fprintf(stderr,"Row: %d, redefiniton of function \"%s\"\n",row,funkce->name); free(funkce->name); free(funkce); return SEM_DEF_ERROR;}
	else if (ret ==  INTERNAL_ERROR) return ERROR_INTER;

	if ((symboly = (T_ST_Vars*) malloc(sizeof(T_ST_Vars)))== NULL ) {free(funkce->name); free(funkce);return INTERNAL_ERROR;}
	varSTInit(symboly);
	funkce->symbolTable = symboly;
	free(token.value);
	token.value = NULL;
	


	if ((result = getFunctionHeader(&token, CONTINUE_READ)) != OK) return result;
	if (token.type != S_LBRA) return ERROR_SYN;

	
	if ((result = getFunctionHeader(&token, CONTINUE_READ)) != OK) return result;
	//zacneme zpracovavat parametry
	// funkce nema parametr
	if(token.type == S_RBRA){
		funkce->paramCount = paramCount;
		continue;
	}
	
	//funkce ma nejake paramtery
	while(true){
		if(token.type != S_ID) {
			fprintf(stderr, "Row: %d, unexpected symbol it should be some id \n",row);
			return ERROR_SYN;
		}

		//vytvorime  novou promenou v tabulce symbolu
		if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ) return INTERNAL_ERROR;
		promena->name = mystrdup(token.value);
		free(token.value);
		token.value = NULL;

		//pridame promenou do tabulky symbolu promenych u funkce
		ret = addVarNodeToST(promena , symboly);
		if(ret == ITEM_EXIST) return SEM_DEF_ERROR;
		else if (ret ==  INTERNAL_ERROR) return ERROR_INTER;

		paramCount++;

		if ((result = getFunctionHeader(&token, CONTINUE_READ)) != OK) return result;

		//docetli jsme vsechny parametry
		funkce->paramCount = paramCount;
		if(token.type == S_RBRA) break;
		else if(token.type!= S_COMMA){
			fprintf(stderr, "Row: %d, unexpected symbol it should be \",\" \n",row);
			return ERROR_SYN;
		}

		//nacteme si dalsi id a smycku zopakujem
		if ((result = getFunctionHeader(&token, CONTINUE_READ)) != OK) return result;

	}

	
	}

	return OK;
}


//precedenci analyza
int expr(){

	
	#if debug 
		printf("jsem v expr: \n\n");		
	#endif

	
	tStackItem *pomItem, *pomItem2, *pomItem3;
	int radek, sloupec,result, typ1,typ2,znamenko1, znamenko2, ret, params = 0;
	char *retezec;
	bool projimadlo; //stejne to tu nikdo ty komentare necte ale je to promena ktera zname ze nastala nejaka chyba a je treba uvolnit. je to z duvodu kde tohle muzem byt 10x pod sebou
	T_Token *exprToken, exprTempToken, *eToken, *pomToken;
	Tleaf *vetev;
	T_ST_VarsItem *promena;
	T_ST_Funcs *funkce;

	//nahrajeme dolar mame prazdny zasobnik
	if((exprToken = (T_Token*)malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;
	exprToken->type = S_DOLAR;
	if((push(zasobnik, exprToken)) != OK ) return ERROR_INTER;

	//jestli pporovnavat povolene jset, povime promene ctene
	//if(token.type == S_FUNC) porovnavani++; // pri function(...) nemuze byt jiz nikde porovnavani
	
	//tabulka -radek  co je na zasobniku, sloupec prichozi token

	do{	
		//zkontrolujeme si co mame delat na zaklade precedenci tabulky
		
		pomItem = top(zasobnik);
		//jelikoz na vrcholu zasobniku muze byt neterrminal E, ktery je fyzicky reprzentovan jako token jehoz typ je E, ve skutecnosti bude stacit sebrat jen ten dalsi co je pod nim ale radeji to udelam obecne
		while((((T_Token*)(pomItem)->data)->type) == S_E)	pomItem = pomItem->prev;

		radek = (((T_Token*)(pomItem)->data)->type) - 20;
		sloupec = (token.type) - 20;
		
		//pokud nahodou se objevil token ktery nema co v zasobniku delat 
		if(radek< 0 || radek > (POLE - 1)  ){
			fprintf(stderr,"Row %d, unexpected symbol\n",row );
			return ERROR_SYN; 
		}

		//jedna se uz o jiny neterminal takze konec vstupu expru 
		if(sloupec < 0 || sloupec > (POLE - 1)  ){
			// zalohujeme si token
			exprTempToken.type = token.type;
			exprTempToken.value = token.value;
			token.type = S_DOLAR;
			sloupec = POLE - 1;
		}
		
		#if debug > 1
			printf("Typ tokenu pro expr je: %d \n",token.type);
			printf("Typ tokenu pro porovnavani je: %d \n",((T_Token*)(pomItem)->data)->type);
			printStack(zasobnik);
		#endif
		// nacitame na zasobnik
		if(prtable[radek][sloupec] == L){
			#if debug >1
				//printf("Nahravam na stack: \n\n");		
			#endif
			
			//vytvareni si noveho tokenu je z duvodu ze pri returnu s chybou a naslednme uvolnovani pameti bychom mohli uvolnovat znova stejny token
			//coz by hodilo segfault, nepomuze ani nastavit pointer pak na NULL, jelikoz v garbage collectoru by ten pointer NULL nebyl
			if((exprToken = (T_Token*)malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;

			//pri puvodnim vytvoreni tokenu nebo vzdy pri zpracovani dat z neho se nahraje na ukazatel ukazujici na data NULL
			//zkopirujeme si data
			if(token.value != NULL){
				switch(token.type){
					case S_ID:
					case S_FUNC: 
					case S_STR:
							 	retezec = mystrdup((char*)token.value);
							 	exprToken->value = retezec;
							 	break;

					case S_BOOL:
					case S_NULL:
					case S_INT:
								exprToken->value = malloc(sizeof(int));
								*(int*)exprToken->value = *(int*)token.value;
								break;

					case S_DOUB:
								exprToken->value = malloc(sizeof(double));
								*(double*)exprToken->value = *(double*)token.value;
								break;

				}
				
				if(exprToken->value == NULL){ 
						free(exprToken); 
						return ERROR_INTER;
				}
			
				free(token.value);
				token.value =  NULL; 				
			}
			else exprToken->value = NULL;
			//zkopirujeme typ do nami vytvoreneho tokenu
			exprToken->type= token.type;

			//pushneme nami nove vytvoreny token na zasobnik
			if((push(zasobnik, exprToken)) != OK ) {
					if(exprToken->value != NULL) free(exprToken->value);
					free(exprToken); 
					return ERROR_INTER;
			}

			if ((result = getToken(&token)) != OK) return result; 


		}
		// syntakticka chyba
		else if (prtable[radek][sloupec] == X){
			//printf("Token na zasobniku: %d \n", (((T_Token*)(pomItem)->data)->type));
			fprintf(stderr, "Row: %d, syntax error\n",row );
			return ERROR_SYN;

		}

		//redukujeme
		else if (prtable[radek][sloupec] == H){
			#if debug 
				//printf("redukuju \n");		
			#endif

			//if(token.type == S_DOLAR) ukoncovac = true; 

			//redukujeme tak ze budeme postupne brat tokeny ze zasobniku a zezadu budeme porovnavat s pravidlem dokud je kam jit
			pomItem = pop_top(zasobnik);
			
			//je tu treba hodit smycku ktera se postara ze se to vycisti pri dolaru
			//printf("Token na zasobniku: %d \n", (((T_Token*)(pomItem)->data)->type));
			switch(((T_Token*)(pomItem)->data)->type){

				//pravidla 10. E -> id, 11. E -> i //int, 12. E -> d //double, 13. E -> b //bool, 14. E -> s //string, 15. E -> n // null
				case S_ID:
				case S_INT:
				case S_DOUB:
				case S_BOOL:
				case S_STR: 
				case S_NULL:
					//alokujeme novy token jehoz typ je E a v datech ma vetev 
					eToken =  (T_Token*)malloc(sizeof(T_Token));
					if (eToken == NULL ){
						free(pomItem);
						return ERROR_INTER;
					}

					vetev = makeLeaf(NULL, pomItem->data,  NULL);

					if (vetev == NULL){
						tokenFree(pomItem->data);
						free(eToken);
						free(pomItem);
						fprintf(stderr,"Inter error, token should have some value\n");
						return ERROR_INTER;
					}
					eToken->type = S_E;
					eToken->value = vetev;
					free(pomItem);

					if((push(zasobnik, eToken)) != OK ) return ERROR_INTER;
					break;

				//pravidla 3. E -> (E), 9. E -> f(E)
				case S_RBRA:
						
					//zrusime zavorku
					tokenFree (((T_Token*)(pomItem)->data)); 
					free(pomItem);
					
					//pomItem je E, nebo zavorka
					pomItem = pop_top(zasobnik); 
					
					//je to E
					if((((T_Token*)(pomItem)->data)->type) == S_E) {
											
						pomItem2 = pop_top(zasobnik); 
						
						if((((T_Token*)(pomItem2)->data)->type) != S_LBRA){
							tokenFree (((T_Token*)(pomItem2)->data));
							free(pomItem2);
							//uvolnime abstraktni strom
							freeAss(pomItem->data);
							free(pomItem); // uvolnime si E
							fprintf(stderr, "Row: %d, unexpected symbol in expresion\n",row );
							return ERROR_SYN;
						}

						//muzeme pomItem2 coz je zavorka uvolnit
						tokenFree (((T_Token*)(pomItem2)->data));
						free(pomItem2);

					
						//zkontrolujeme jestli e neni nedefinovana promena

						if(((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1->type == S_ID){
							if((findVarST( ((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1->value, actualST)) == NULL){ 
											fprintf(stderr, "Row: %d, undefined variable \"%s\"\n",row, (char*)(((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1)->value );
											tokenFree (((T_Token*)(pomItem)->data));
											free(pomItem);
											free(pomItem2); 
											return  SEM_UNDECLARED_PARAMETER;
							} 

						}

						if(((Tleaf*)((T_Token*)(pomItem)->data)->value)->op2 != NULL){
							if(((Tleaf*)((T_Token*)(pomItem)->data)->value)->op2->type == S_ID){
								if((findVarST( ((Tleaf*)((T_Token*)(pomItem)->data)->value)->op2->value, actualST)) == NULL){ 
											fprintf(stderr, "Row: %d, undefined variable \"%s\"\n",row, (char*)(((Tleaf*)((T_Token*)(pomItem)->data)->value)->op2)->value );
											tokenFree (((T_Token*)(pomItem)->data));
											free(pomItem);
											free(pomItem2); 
											return  SEM_UNDECLARED_PARAMETER;
								} 

							}

						}	
					}
					
					
					pomItem3 = top(zasobnik);
					// pravidlo 3. E -> (E) 
					if(((T_Token*)(pomItem3)->data)->type != S_FUNC){
						//v pomItem mame ulozene E.. takze nam staci jen pushnout zpatky pomItem na zasobnik prazdna zavorka je proste error
						if((((T_Token*)(pomItem)->data)->type) == S_E) {
						
							if((push(zasobnik, (T_Token*)(pomItem)->data)) != OK ) {free((T_Token*)(pomItem)->data);return ERROR_INTER;}
						}
						//jinac tam jsou () takze je ignorujem
						free(pomItem);
						break;
					}
					
					
					//dale pokracuje pravidlo 9. E -> f(E)
					pomItem2 = pop_top(zasobnik);
					
					//jenom jeden atribut					
					//if ( ((T_Token*)(pomItem)->data)->value!= NULL && ((Tleaf*)(((T_Token*)(pomItem)->data)->value))->op == NULL) 
					if((((T_Token*)(pomItem)->data)->type) == S_E) params++;
					// dame si do pom item vetev ktera ma NULL, nic se tim nezkazi snad..
					else {
						 if((pomToken=(T_Token*)malloc(sizeof(T_Token))) == NULL ){free(pomItem); free(((T_Token*)(pomItem2)->data)->value); free(pomItem2); return ERROR_INTER;}
						 if((pomToken->value=malloc(sizeof(int))) == NULL ){free(pomToken); free(pomItem); free(((T_Token*)(pomItem2)->data)->value); free(pomItem2); return ERROR_INTER;}
						 
						*((int*)(pomToken)->value) = 0;
						pomToken->type = S_NULL;
						
						eToken =  (T_Token*)malloc(sizeof(T_Token));
						if (eToken == NULL ){
							free(pomToken->value); free(pomToken); free(pomItem); free(((T_Token*)(pomItem2)->data)->value); free(pomItem2); 
							return ERROR_INTER;
						}	

						vetev = makeLeaf(NULL, pomToken,  NULL);

						if (vetev == NULL){
							free(eToken); free(pomToken->value); free(pomToken); free(pomItem); free(((T_Token*)(pomItem2)->data)->value); free(pomItem2); 
							fprintf(stderr,"Inter error, allocating memory\n");
							return ERROR_INTER;
						}
						eToken->type = S_E;
						eToken->value = vetev;
						free(pomItem->data);
						pomItem->data = eToken;
					}
					
					//semanticka akce kontrolujem jestli je funkce deklarovana
					if((funkce = findFunctionST(((T_Token*)(pomItem2)->data)->value, functionTable)) == NULL ){
						//volame nedefinovanou funkci vracime semantickou chybu
						if((((T_Token*)(pomItem)->data)->type) == S_E) freeAss(pomItem->data);
						fprintf(stderr,"Row: %d, missing definition of \"%s\"\n",row,(char*)((T_Token*)(pomItem2)->data)->value);
						tokenFree((T_Token*)(pomItem2)->data);
						free(pomItem);
						free(pomItem2);
						return SEM_DEF_ERROR;
					}

					//funkce ma malo parametru
					#if debug
						printf("Params def: %d, params call: %d \n", funkce->data->paramCount,params);
					#endif
					if(funkce->data->paramCount > params ){
						if((((T_Token*)(pomItem)->data)->type) == S_E) freeAss(pomItem->data);
						fprintf(stderr,"Row: %d, too a few parameters in funciton  \"%s\"\n",row,(char*)((T_Token*)(pomItem2)->data)->value);
						tokenFree((T_Token*)(pomItem2)->data);
						free(pomItem);
						free(pomItem2);
						return SEM_MISSING_PARAMETER;
					}
					
					params = 0;

					eToken = (T_Token*)malloc(sizeof(T_Token));
					if (eToken == NULL){
						//vsechno treba uvolnit
						if((((T_Token*)(pomItem)->data)->type) == S_E) freeAss(pomItem->data);
						tokenFree((T_Token*)(pomItem2)->data);
						free(pomItem);
						free(pomItem2);
						return ERROR_INTER;
					}

					
					if((((T_Token*)(pomItem)->data)->type) == S_E) vetev = makeLeaf(pomItem2->data, pomItem->data, NULL);
					else vetev = makeLeaf(pomItem2->data, NULL, NULL);
					
					eToken->type = S_E;
					eToken->value = vetev;
					
				
					free(pomItem);
					free(pomItem2);
					
					if (vetev == NULL){
						tokenFree(pomItem2->data);
						freeAss( ((T_Token*)(pomItem)->data)->value);
						free(eToken);
						return ERROR_INTER;
					}

					//nahrajem E zpet na zasobnik
					if((push(zasobnik, eToken)) != OK ) {
						tokenFree(eToken);
						return ERROR_INTER;
					}


					break;
				case S_E:
					pomItem2 = pop_top(zasobnik); 					
					pomItem3 = top(zasobnik);

					
					// v tokenech se muze nalezat i E z ktereho v principu nezkontrolujem typovou kompatibilitu, avsak tato kompatibilita se drive kontrolovala kdzy jsem poprve prevadel
					// E -> E op E  takze mi staci kontrolovat operaci v minulem prevodu a na zaklade toho rozhodnout

					//stack obsahuje E [ S_E | value], value ukazuje na leaf [op1 | op | op2], kde op1 nebo op2 ukazujou bud na E token [ S_E | value], nebo na nejaky konkretni typ napr int [ S_INT | value ]
					
					//printf("tisknu strom: \n");	
					//printAss(((T_Token*)(pomItem)->data)->value );
					//printAss(((T_Token*)(pomItem3)->data)->value );
				
					if( ((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1 != NULL ) typ1 = ((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1->type;	
					else typ1 = NOT_EXIST;

					if( ((Tleaf*)((T_Token*)(pomItem3)->data)->value)->op1 != NULL ) typ2 = ((Tleaf*)((T_Token*)(pomItem3)->data)->value)->op1->type;
					else typ2 = NOT_EXIST;
					//pokud by to byl jednoduchy E ma v typu kokretni typ, pokud se jedna o slozeny ma v typu E takze musi existovt i znamenko mezi tema dvema
					znamenko1 = 0;
					znamenko2 =0 ;

					if(typ1== S_E)  znamenko1 = ((Tleaf*)((T_Token*)(pomItem)->data)->value)->op->type;
					if(typ2 ==S_E)  znamenko2 = ((Tleaf*)((T_Token*)(pomItem3)->data)->value)->op->type;
					
					//printf("typ1: %d\n",typ1);
					//printf("typ2: %d\n",typ2);
					//okenFree (((T_Token*)(pomItem)->data));
					//free(pomItem);
					projimadlo = false;
					
					switch(((T_Token*)(pomItem2)->data)->type){

							//pravidla: 1. E -> E+E, 2. E -> E*E, 5. E -> E.E, 6. E -> E/E, 7. E -> E-E, 8. E -> E,E, 16. E -> E <= E, 17. E -> E < E, 18. E -> E >= E, 19. E -> E > E, 20. E -> E !== E, 21. E -> E === E, 4. E=E
							//pouze zkontrolujeme semanticke akce jelikoz pri jinacim pravidle se returne, takze redukce se provadi mimo siwtch pro vsechnz stejne
							case S_MUL:
							case S_DIV:	
							case S_MINUS:
							case S_PLUS:
								switch(typ1){
									case S_INT:
									//case S_FUNC:
									case S_DOUB: break;
									case S_ID:
										if((findVarST( ((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1->value, actualST)) == NULL){
											fprintf(stderr, "Row: %d, undefined variable \"%s\"\n",row, (char*)(((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1)->value  );
											ret= SEM_UNDECLARED_PARAMETER;
											projimadlo = true;

										} 
										break;
									case S_E:
										
										switch(znamenko1){
											case S_PLUS:
											case S_MINUS:
											case S_DIV:
											case S_FUNC:
											case S_MUL: break;
											default: 
												fprintf(stderr, "Row: %d, incompatible types in expression\n",row );
												ret= SEM_TYPE_ERROR;
												projimadlo = true;
										}
										break;
									default: 	fprintf(stderr, "Row: %d, incompatible types in expression\n",row );
												ret= SEM_TYPE_ERROR;
												projimadlo = false;
								}
								if (projimadlo) break;
								
								switch(typ2){
									case S_INT:
									case S_FUNC:
									case S_DOUB: break;
									case S_ID: 
										if((findVarST( ((Tleaf*)((T_Token*)(pomItem3)->data)->value)->op1->value, actualST)) == NULL){
											fprintf(stderr, "Row: %d, undefined variable \"%s\"\n",row, (char*)(((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1)->value );
											ret= SEM_UNDECLARED_PARAMETER;
											projimadlo = true;

										} 
										break;
									case S_E:
										switch(znamenko2){
											case S_PLUS: 
											case S_MINUS:
											case S_DIV:
											case S_FUNC:
											case S_MUL: break;
											default: 
												fprintf(stderr, "Row: %d, incompatible types in expression\n",row );
												ret= SEM_TYPE_ERROR;
												projimadlo = true;
										}
										break;
									default: 	fprintf(stderr, "Row: %d, incompatible types in expression\n",row );
												ret= SEM_TYPE_ERROR;
												projimadlo = true;
								}
								break;
							
							case S_CONCATENATE:
								switch(typ1){
									case S_STR: break;
									case S_ID:  
										if((findVarST( ((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1->value, actualST)) == NULL){
											fprintf(stderr, "Row: %d, undefined variable \"%s\"\n",row, (char*)(((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1)->value );
											ret= SEM_UNDECLARED_PARAMETER;
											projimadlo = true;

										} 
										break;
									/* neni to semanticka chyba
									case S_E:
										if(znamenko1!= S_CONCATENATE && znamenko1 != S_FUNC){
												fprintf(stderr, "Row: %d, 5 incompatible types in expression\n",row );
												ret= SEM_TYPE_ERROR;
												projimadlo = true;
										}
										break;
									default: 	fprintf(stderr, "Row: %d, 6 incompatible types in expression\n",row );
												ret= SEM_TYPE_ERROR;
												projimadlo = true; */
								}
								if (projimadlo) break;
								switch(typ2){
									case S_STR: break;
									case S_ID: 
										if((findVarST( ((Tleaf*)((T_Token*)(pomItem3)->data)->value)->op1->value, actualST)) == NULL){
											fprintf(stderr, "Row: %d, undefined variable \"%s\"\n",row, (char*)(((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1)->value );
											ret= SEM_UNDECLARED_PARAMETER;
											projimadlo = true;

										} 
										break;
									/*case S_E: neni to semanticka chyba
										if(znamenko2!= S_CONCATENATE && znamenko2 != S_FUNC){
												fprintf(stderr, "Row: %d, 7 incompatible types in expression\n",row );
												ret= SEM_TYPE_ERROR;
												projimadlo = true;
										}
										break;
									default: 	fprintf(stderr, "Row: %d, 8 incompatible types in expression\n",row );
												ret= SEM_TYPE_ERROR;
												projimadlo = true; */
								}
								break;


							case S_IS: 
									// prirazujeme neinicializovanou promenou
									if(typ1 == S_ID){ 
										if((findVarST( ((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1->value, actualST)) == NULL){ 
											fprintf(stderr, "Row: %d, undefined variable \"%s\"\n",row, (char*)(((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1)->value );
											tokenFree (((T_Token*)(pomItem)->data));
											tokenFree (((T_Token*)(pomItem3)->data));
											free(pomItem);
											free(pomItem2);
											return  SEM_UNDECLARED_PARAMETER;
										} 
									}
									//padnu zde
									//prirazujeme do neceho jineho nez je promena
									if(typ2 != S_ID){ fprintf(stderr, "Row: %d, request variable for assigment\n", row);
										tokenFree (((T_Token*)(pomItem)->data));
										tokenFree (((T_Token*)(pomItem2)->data));
										free(pomItem);
										free(pomItem2);
										return ERROR_SYN;
									}
									//vytvorime novou promenou
									

									if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ){
											tokenFree (((T_Token*)(pomItem)->data));
											tokenFree (((T_Token*)(pomItem2)->data));
											free(pomItem);
											free(pomItem2);
											return ERROR_INTER;
									} 
									
									promena->name = mystrdup(((Tleaf*)((T_Token*)(pomItem3)->data)->value)->op1->value);
										
									//pridame promenou do aktualni tabulky symbolu
									if( (ret = addVarNodeToST(promena , actualST)) == INTERNAL_ERROR ){
											tokenFree (((T_Token*)(pomItem)->data));
											tokenFree (((T_Token*)(pomItem2)->data));
											free(pomItem);
											free(pomItem2);
											free(promena);
											return ERROR_INTER;
									}
										//jestli pomena existuje tak proste smazneme to co jsme vytvorili
									else if(ret == ITEM_EXIST) free (promena);
									

									break;													
							case S_COMMA: params++;
							case S_LEQ:
							case S_LST:
							case S_GEQ:
							case S_GRT:
							case S_NEQ:							
							case S_EQ:
									//zkontrolujem jsetli jsou inicializovane promene
									if(typ1 == S_ID){
										if((findVarST( ((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1->value, actualST)) == NULL){
											fprintf(stderr, "Row: %d, undefined variable \"%s\"\n",row, (char*)(((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1)->value  );
											ret= SEM_UNDECLARED_PARAMETER;
											projimadlo = true;
										} 
									}

									if(typ2 == S_ID){
										if((findVarST( ((Tleaf*)((T_Token*)(pomItem3)->data)->value)->op1->value, actualST)) == NULL){
											fprintf(stderr, "Row: %d, undefined variable \"%s\"\n",row, (char*)(((Tleaf*)((T_Token*)(pomItem3)->data)->value)->op1)->value );
											ret= SEM_UNDECLARED_PARAMETER;
											projimadlo = true;
										} 
									}
									
									break;
							// stav zasobniku by mel byt $E
							case S_DOLAR:
								#if debug
										printf("Vyprazdnili jsme zasobnik\n");
								#endif
								if(token.type == S_DOLAR){
									//vratime si zpet co bylo v tokenu nez jsme ho prehrali dolarem
									token.type = exprTempToken.type;
									token.value = exprTempToken.value;
									tokenFree (((T_Token*)(pomItem)->data));
									free(pomItem);

									return OK;
								}
								
								fprintf(stderr, "Row: %d, unexpected symbol in expression\n",row );
								return ERROR_SYN;

							default: 
								tokenFree (((T_Token*)(pomItem)->data));
								tokenFree (((T_Token*)(pomItem2)->data));
								free(pomItem);
								free(pomItem);
								fprintf(stderr, "Row: %d, unexpected symbol in expression\n",row );
								return ERROR_SYN;
					}

					if(projimadlo){ //printf("uyivam projimadlo")
							tokenFree (((T_Token*)(pomItem)->data));
							tokenFree (((T_Token*)(pomItem2)->data));
							free(pomItem);
							free(pomItem2);
							return ret;
					}

					eToken = (T_Token*)malloc(sizeof(T_Token));
					if (eToken == NULL){
						//vsechno treba uvolnit
						freeAss(pomItem->data);
						tokenFree((T_Token*)(pomItem2)->data);
						free(pomItem);
						free(pomItem2);
						return ERROR_INTER;
					}

					pomItem3 = pop_top(zasobnik);
					//to co bylo v zasobniku nize je pravy operand
					vetev = makeLeaf(pomItem2->data, pomItem3->data, pomItem->data);
					
					if (vetev == NULL){
						tokenFree(pomItem2->data);
						freeAss( ((T_Token*)(pomItem)->data)->value);
						freeAss(((T_Token*)(pomItem3)->data)->value);
						free(eToken);
						return ERROR_INTER;
					}
					
					//por = 0;
					

					
					free(pomItem);
					free(pomItem2);
					free(pomItem3);

					
					
					eToken->type = S_E;
					eToken->value = vetev;
					//printf("etok %d\n",eToken->type);
					//nahrajem E zpet na zasobnik
					if((push(zasobnik, eToken)) != OK ) {
						tokenFree(eToken);
						return ERROR_INTER;
					}
					
					break;

				default:
					fprintf(stderr, "Row: %d, mising symbol in expr\n",row );
					return ERROR_SYN;
					
					break;

			} 



		}
		else if (prtable[radek][sloupec] == EQ){
			
			//zatim nevim jak toudelat lepe ale z podstaty veci je to treba pushnout na zasobnik
			if(token.type == S_RBRA){
				if((exprToken = (T_Token*)malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;
				exprToken->value = NULL;
				exprToken->type = S_RBRA;

				if((push(zasobnik, exprToken)) != OK ) {
					if(exprToken->value != NULL) free(exprToken->value);
					free(exprToken); 
					return ERROR_INTER;
				}

				if ((result = getToken(&token)) != OK) return result;

			}
			else pomItem = pop_top(zasobnik);

			//printf("Token na zasobniku: %d \n", (((T_Token*)(pomItem)->data)->type));
			switch (((T_Token*)(pomItem)->data)->type ){				
				case S_E: 
						pomItem2 = pop_top(zasobnik);
						if(pomItem == NULL) return ERROR_INTER;

						if(((T_Token*)(pomItem2)->data)->type!= S_DOLAR){
							fprintf(stderr, "Row: %d, unexpected symbol in expresion\n",row );
							tokenFree(pomItem);
							free(pomItem);
							return ERROR_SYN;
						}
						pomToken = (T_Token*)(pomItem)->data;
						exprTree = (T_Token*)(pomItem)->data;
						//pomToken = ((Tleaf*) (pomToken->value))->op;
						//printf("pom token: %d\n", pomToken->type);
						//exprTree = (Tleaf*)(pomToken)->value;
						
						//((Tleaf*)(T_Token*)(pomItem)->data)->value;

						//printf("EPXR: %d \n",((T_Token*)(pomItem->data)->type);
						//printf("tiskn epxr\n");
						//printf("EPXR: %d \n",((Tleaf*)(exprTree->value))->op->type);

						free(pomItem);
						//free(pomToken);
						token.type = exprTempToken.type;
						token.value = exprTempToken.value;

						#if debug
							printf("expr end\n");
						#endif
						return OK;

						
			}
		}
		else return ERROR_INTER;

		
	}while(true);
	return OK;
}