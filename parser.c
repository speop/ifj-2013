#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "ial.h"
#include "types.h"
#include "scaner.h"
#include "ast_tree.h"
#include "stack.h"
#define debug 1
#define POLE 23

extern TGarbageList trash; //z main.c
extern int row; // z main.c
extern T_Token *prevToken; // z main.c
extern FILE* pSource_File; // z main.c

T_ST_Vars *symbolTable, *actualST;
T_ST_Funcs *functionTable;
Tleaf *ass, *exprTree;
T_Token token;
tStack *zasobnik;
int konecBloku = 0;

//precedenci tabulka
// TODO: dle zadani doplnit priority do tabulky					
static int prtable [POLE][POLE] = {
/*
				0		1		2		3		4		5		6		7		8		9		10		11		12		13		14		15		16		18		19		20		21		22		23
				+		*		(		)		=		.		/		-		,		f		id		i		d		s		b		n		$  		<		>		<=		>=		===		!==*/
/*  0  + */	{	H,		L,		L,		H,		X,		H,		L,		H,		H,		L,		L,		L,		L,		L,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  1  * */ {	H,		H,		L,		H,		X,		H,		H,		H,		H,		L,		L,		L,		L,		L,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  2  ( */ {	L,		L,		L,		EQ,		X,		L,		L,		L,		EQ,		X,		L,		L,		L,		L,		X,		X,		X,		L,		L,		L,		L,		L,		L},
/*  3  ) */ {	H,		H,		X,		H,		X,		H,		H,		H,		H,		X,		X,		X,		X,		X,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  4  = */	{	L,		L,		L,		X,		X,		L,		L,		L,		X,		L,		L,		L,		L,		L,		X,		X,		H,		X,		X,		X,		X,		X,		X},
/*  5  . */ {	H,		L,		L,		H,		X,		H,		L,		H,		X,		L,		L,		L,		L,		L,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  6  / */ {	H,		H,		L,		H,		X,		H,		H,		H,		X,		L,		L,		L,		L,		L,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  7  - */ {	H,		L,		L,		H,		X,		H,		L,		H,		X,		L,		L,		L,		L,		L,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  8  , */ {	L,		L,		L,		H,		X,		L,		L,		L,		EQ,		L,		L,		L,		L,		L,		L,		L,		X,		L,		L,		L,		L,		L,		L},
/*  9  f */ {	X,		X,		EQ,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X},
/*  10 id */{	H,		H,		X,		H,		H,		H,		H,		H,		H,		X,		X,		X,		X,		X,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  11  i */{	H,		H,		X,		H,		X,		H,		H,		H,		H,		X,		X,		X,		X,		X,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  12  d */{	H,		H,		X,		H,		X,		H,		H,		H,		H,		X,		X,		X,		X,		X,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  13  s */{	H,		H,		X,		H,		X,		H,		H,		H,		H,		X,		X,		X,		X,		X,		X,		X,		H,		H,		H,		H,		H,		H,		H},
/*  14  b */{	X,		X,		X,		X,		X,		X,		X,		X,		H,		X,		X,		X,		X,		X,		X,		X,		X,		H,		H,		H,		H,		H,		H},
/*  15  n */{	X,		X,		X,		X,		X,		X,		X,		X,		H,		X,		X,		X,		X,		X,		X,		X,		X,		H,		H,		H,		H,		H,		H},
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

	//vytvorime zasobik 
	zasobnik = SInit();
	garbage_add(zasobnik, &emptyToken);

	prevToken = NULL;
	token.value = NULL;

	row = 1;

	int result;

	

	result = program();	
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

			#if debug 
				printf("hlavicky zpracovane zadne se v programu nenachazi\n");		
			#endif

			//rewindli jsme... zase se nam posle <?php tak to preskocime
			if ((result = getToken(&token)) != OK) return result;
			token.type = -1;
			if ((result = getToken(&token)) != OK) return result;


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
	T_Token pomToken;
	//T_ST_VarsItem *promena;
	T_ST_Funcs *func;

	switch (token.type){

		// pravidlo: 2. <st-list> → id <expr> ; <st-list>
		case S_FUNC:
		case S_ID: 
			
			result = expr();

			#if debug 
				printf("Zpracovany epxr\n");		
			#endif
				
			if(result != OK ) return result;
			if (token.type != S_SEM){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \";\"\n",row);
				return ERROR_SYN;
			} 

			if ((result = getToken(&token)) != OK) return result;
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

 			if ((result = getToken(&token)) != OK) return result;
 			
			result = expr();
			if(result != OK ) return result;
		
			
			if(token.type != S_BLOCK_START ){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"{\"\n",row);
				return ERROR_SYN;
			}

			
			if ((result = getToken(&token)) != OK) return result;

			// otestujeme jestli se nejedna o pravidlo 3. <st-list> → ε
			if(token.type != S_BLOCK_END ){ printf("BLOK IF: %d\n",token.type);
				result = st_list();
				if(result != OK ) return result;
			}
			printf(" konec je: %d\n",token.type);
			// mel byt konece
			if(token.type != S_BLOCK_END ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"}\"\n",row);
				return ERROR_SYN;
			}
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
				else putToken(&token);
				
			} 

			if ((result = getToken(&token)) != OK) return result;
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
			pomToken = token;

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
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"{\" \n",row);
				return ERROR_SYN;
			}

			if ((result = getToken(&token)) != OK) return result;
			
			// otestujeme jestli se nejedna o pravidlo 3. <st-list> → ε
			if(token.type != S_BLOCK_END ){
				
				//zmenime tabulku symbolu na danou funkci
				func = findFunctionST(pomToken.value , functionTable);
				if(func == NULL ) return ERROR_INTER;
				actualST = func->data->symbolTable;

				result = st_list();
				if(result != OK ) return result;
				//if ((result = getToken(&token)) != OK) return result;
			}

			if(token.type != S_BLOCK_END ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"}\"\n",row);
				return ERROR_SYN;
			}
			konecBloku--;

			if ((result = getToken(&token)) != OK) return result;
			result = st_list();
			if(result != OK) return result;

			return OK;
			break;

		// pravidlo 7. <st-list> → return <expr> <st-list>
		case RETURN:
			if ((result = getToken(&token)) != OK) return result;
			result = expr();
			if(result != OK ) return result;

			//nastavime zpet na hlavni tabulku symbolu.. tu me rekurze nemusi trapit 
			actualST = symbolTable;

			if ((result = getToken(&token)) != OK) return result;
			result = st_list();
			if(result != OK) return result;

			return OK;
			break;

		case S_EOF: return OK;
		default: 
			if(token.type == S_BLOCK_END && konecBloku) return OK;
			else return  ERROR_SYN; 
	}

	return ERROR_SYN; 

}


int if_extra(){
	int result;

	switch(token.type){

		// pravidlo 16. <if-extra> → else { <st-list> }
		case ELSE: 
			konecBloku++;
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
				//if ((result = getToken(&token)) != OK) return result;
			}

			if(token.type != S_BLOCK_END ){ 
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"}\"\n",row);
				return ERROR_SYN;
			}
			konecBloku--;

			if ((result = getToken(&token)) != OK) return result;
			result = st_list();
			if(result != OK) return result;

			return OK;
			break;

		// pravidlo 17. <if-extra> → elseif { <st-list> } <if-extra> <st-lsit>
		case ELSEIF:
			
			konecBloku++;
			
			if ((result = getToken(&token)) != OK) return result;
			
			result = expr();
			if(result != OK ) return result;
			
			
			if(token.type != S_BLOCK_START ){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"{\"\n",row);
				return ERROR_SYN;
			}

			
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
	ret = addFuncNodeToST(funkce ,functionTable);

	if(ret == ITEM_EXIST) return SEM_DEF_ERROR;
	else if (ret ==  INTERNAL_ERROR) return ERROR_INTER;

	if ((symboly = (T_ST_Vars*) malloc(sizeof(T_ST_Vars)))== NULL ) return INTERNAL_ERROR;
	varSTInit(symboly);
	funkce->name = mystrdup(token.value);
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

		if(token.type == S_RBRA) break;
		else if(token.type!= S_COMMA){
			fprintf(stderr, "Row: %d, unexpected symbol it should be \",\" \n",row);
			return ERROR_SYN;
		}

		funkce->paramCount = paramCount;
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
	int radek, sloupec,result, typ1,typ2;
	char *retezec;
	bool projimadlo; //stejne to tu nikdo ty komentare necte ale je to promena ktera zname ze nastala nejaka chyba a je treba uvolnit. je to z duvodu kde tohle muzem byt 10x pod sebou
	T_Token *exprToken, exprTempToken, *eToken, *pomToken;
	Tleaf *vetev;
	T_ST_VarsItem *promena;

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
		
		printf("Typ tokenu pro expr je: %d \n",token.type);
		printf("Typ tokenu pro porovnavani je: %d \n",((T_Token*)(pomItem)->data)->type);
		printStack(zasobnik);	
		// nacitame na zasobnik
		if(prtable[radek][sloupec] == L){
			#if debug 
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
						printf("ze by tady?\n");
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
			printf("Token na zasobniku: %d \n", (((T_Token*)(pomItem)->data)->type));
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

					vetev = makeLeaf(pomItem->data , NULL, NULL);
					if (vetev == NULL){
						freeToken(pomItem->data);
						free(eToken);
						free(pomItem);
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
					
					//pomItem je E
					pomItem = pop_top(zasobnik); 

					if((((T_Token*)(pomItem)->data)->type) != S_E) {
						tokenFree (((T_Token*)(pomItem)->data));
						free(pomItem);
						fprintf(stderr, "Row: %d, unexpected symbol in expresion\n",row );
						return ERROR_SYN;
					}

					
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

					pomItem3 = top(zasobnik); 


					// pravidlo 3. E -> (E) 
					if(((T_Token*)(pomItem3)->data)->type != S_FUNC){
						//v pomItem mame ulozene E.. takze nam staci jen pushnout zpatky pomItem na zasobnik						
						if((push(zasobnik, (T_Token*)(pomItem)->data)) != OK ) {free((T_Token*)(pomItem)->data);return ERROR_INTER;}
						free(pomItem);
						break;
					}
					
					//dale pokracuje pravidlo 9. E -> f(E)
					pomItem2 = pop_top(zasobnik);

					//semanticka akce kontrolujem jestli je funkce deklarovana
					if(	findFunctionST(((T_Token*)(pomItem2)->data)->value, functionTable) == NULL ){
						//volame nedefinovanou funkci vracime semantickou chybu
						freeAss(pomItem->data);
						printf("Row: %d, missing definition of \"%s\"",row,(char*)((T_Token*)(pomItem)->data)->value);
						tokenFree((T_Token*)(pomItem2)->data);
						free(pomItem);
						free(pomItem2);
						return SEM_DEF_ERROR;
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

					vetev = makeTree(pomItem2->data,  ((T_Token*)(pomItem)->data)->value, NULL);
					eToken->type = S_E;
					eToken->value = vetev;
					
					free(pomItem->data);
					free(pomItem);
					free(pomItem2);
					
					if (vetev == NULL){
						freeToken(pomItem2->data);
						freeAss( ((T_Token*)(pomItem)->data)->value);
						free(eToken);
						return ERROR_INTER;
					}

					//nahrajem E zpet na zasobnik
					if((push(zasobnik, eToken)) != OK ) {
						tokenFree(eToken);
						return ERROR_INTER;
					}



				case S_E:
					pomItem2 = pop_top(zasobnik); 					
					pomItem3 = top(zasobnik);

					//pomToken = (T_Token*)(pomItem)->data;
					//pomVetev = (Tleaf*)(pomToken)->value;	
					//typ1 = pomVetev->op1->type;
					//typ1 = (Tleaf*)(pomToken->value)->op1->type;
					typ1 = ((Tleaf*)((T_Token*)(pomItem)->data)->value)->op1->type;

					//pomToken = (T_Token*)(pomItem3)->data;
					//pomVetev = (Tleaf*)(pomToken)->value;	
					typ2 = ((Tleaf*)((T_Token*)(pomItem3)->data)->value)->op1->type;
					
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
								if((typ1 != S_INT && typ1 != S_DOUB) || (typ2 != S_INT && typ2 != S_DOUB)) projimadlo = true;
								break;
							
							case S_CONCATENATE:
								if(typ1 != S_STR || typ2 != S_STR) projimadlo = true;
								break;
							case S_EQ: 
									if(typ2 != S_ID){ fprintf(stderr, "Row: %d, request variable for assigment\n", row);
										tokenFree (((T_Token*)(pomItem)->data));
										tokenFree (((T_Token*)(pomItem2)->data));
										free(pomItem);
										free(pomItem);
										return SEM_OTHER_ERROR;
									}
									//pridame promenou do tabulky symbolu
									else{

										if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ){
											tokenFree (((T_Token*)(pomItem)->data));
											tokenFree (((T_Token*)(pomItem2)->data));
											free(pomItem);
											free(pomItem);
											return ERROR_INTER;
										} 
									
										promena->name = mystrdup(((Tleaf*)((T_Token*)(pomItem3)->data)->value)->op1->value);
										//pridame promenou do tabulky symbolu promenych u funkce
										if( addVarNodeToST(promena , actualST) == INTERNAL_ERROR ){
											tokenFree (((T_Token*)(pomItem)->data));
											tokenFree (((T_Token*)(pomItem2)->data));
											free(pomItem);
											free(pomItem);
											free(promena);
											return ERROR_INTER;
										}
									}
									break;													
							case S_COMMA:
							case S_LEQ:
							case S_LST:
							case S_GEQ:
							case S_GRT:
							case S_NEQ:							
							case S_IS:
									
									
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

					if(projimadlo){
							tokenFree (((T_Token*)(pomItem)->data));
							tokenFree (((T_Token*)(pomItem2)->data));
							free(pomItem);
							free(pomItem);
							fprintf(stderr, "Row: %d, incompatible types in expression\n",row );
							return SEM_TYPE_ERROR;
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
					//to co bylo v zasobniku nize je levy operand
					vetev = makeTree(pomItem2->data, ((T_Token*)(pomItem3)->data)->value, ((T_Token*)(pomItem)->data)->value);

					eToken->type = S_E;
					eToken->value = vetev;
					free(pomItem->data);
					free(pomItem);
					free(pomItem2);
					free(pomItem3->data);
					free(pomItem3);

					if (vetev == NULL){
						freeToken(pomItem2->data);
						freeAss( ((T_Token*)(pomItem)->data)->value);
						freeAss(((T_Token*)(pomItem3)->data)->value);
						free(eToken);
						return ERROR_INTER;
					}

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
						exprTree = (Tleaf*)(pomToken)->value;
						free(pomItem);
						free(pomToken);
						token.type = exprTempToken.type;
						token.value = exprTempToken.value;
						return OK;

						
			}
		}
		else return ERROR_INTER;

		
	}while(true);
	return OK;
}