#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "ial.h"
#include "types.h"
#include "scaner.h"
#include "stack.h"
#define debug 1
#define POLE 17

extern TGarbageList trash; //z main.c
extern int row; // z main.c
extern T_Token *prevToken; // z main.c

T_ST_Vars *symbolTable, *actualST;
T_ST_Funcs *functionTable;
T_Token token;
tStack *zasobnik;

//precedenci tabulka
// TODO: dle zadani doplnit priority do tabulky					
static int prtable [POLE][POLE] = {
/*
				0		1		2		3		4		5		6		7		8		9		10		11		12		13		14		15		16	
				+		*		(		)		=		.		/		-		,		f		id		i		d		s		b		n		$  */
/*  0  + */	{	H,		L,		L,		H,		X,		H,		L,		H,		H,		L,		L,		L,		L,		L,		X,		X,		H},
/*  1  * */ {	H,		H,		L,		H,		X,		H,		H,		H,		H,		L,		L,		L,		L,		L,		X,		X,		H},
/*  2  ( */ {	L,		L,		L,		EQ,		X,		L,		L,		L,		EQ,		X,		L,		L,		L,		L,		X,		X,		X},
/*  3  ) */ {	H,		H,		X,		H,		X,		H,		H,		H,		H,		X,		X,		X,		X,		X,		X,		X,		H},
/*  4  = */	{	L,		L,		L,		X,		X,		H,		L,		L,		X,		L,		L,		L,		L,		L,		X,		X,		H},
/*  5  . */ {	H,		L,		L,		H,		X,		H,		L,		H,		X,		L,		L,		L,		L,		L,		X,		X,		H},
/*  6  / */ {	H,		H,		L,		H,		X,		H,		H,		H,		X,		L,		L,		L,		L,		L,		X,		X,		H},
/*  7  - */ {	H,		L,		L,		H,		X,		H,		L,		H,		X,		L,		L,		L,		L,		L,		X,		X,		H},
/*  8  , */ {	L,		L,		L,		H,		X,		L,		L,		L,		EQ,		L,		L,		L,		L,		L,		L,		L,		X},
/*  9  f */ {	X,		X,		EQ,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X},
/*  10 id */{	H,		H,		X,		H,		X,		H,		H,		H,		H,		X,		X,		X,		X,		X,		X,		X,		H},
/*  11  i */{	H,		H,		X,		H,		X,		H,		H,		H,		H,		X,		X,		X,		X,		X,		X,		X,		H},
/*  12  d */{	H,		H,		X,		H,		X,		H,		H,		H,		H,		X,		X,		X,		X,		X,		X,		X,		H},
/*  13  s */{	H,		H,		X,		H,		X,		H,		H,		H,		H,		X,		X,		X,		X,		X,		X,		X,		H},
/*  14  b */{	X,		X,		X,		X,		X,		X,		X,		X,		H,		X,		X,		X,		X,		X,		X,		X,		X},
/*  15  n */{	X,		X,		X,		X,		X,		X,		X,		X,		H,		X,		X,		X,		X,		X,		X,		X,		X},
/*  16  $ */{	X,		X,		L,		X,		L,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X,		X}		
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


	int result;


	result = functionHeaders();
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

	int result, ret;
	bool pom = false;
	T_Token pomToken;
	T_ST_VarsItem *promena;
	T_ST_Funcs *func;

	switch (token.type){

		// pravidlo: 2. <st-list> → id <expr> ; <st-list>
		case S_FUNC:
		case S_ID: 
			

			// z duvodu semantiky zkontrolujeme jestli nechceme prirazovat do funkce
			pomToken = token;
			if ((result = getToken(&token)) != OK) return result;

			if(token.type == S_IS){
				if(pomToken.type  != S_ID) return SEM_OTHER_ERROR;
				
				//priradime promenou do tabulky symbolu
				if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ) return INTERNAL_ERROR;
				promena->name = mystrdup(pomToken.value);

				ret = addVarNodeToST(promena , actualST); // v actualSt je bud hlavni tabulka symbolu nebo tanulka funkce ve ktere jsme, tyto veci se preinaji v definici funkce a pri returnu
				if(ret == ITEM_EXIST) return SEM_DEF_ERROR;
				else if (ret ==  INTERNAL_ERROR) return ERROR_INTER;


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
		//edited cele v zavorce je expr
		case IF: 

			if(token.type == IF) pom = true;

 			if ((result = getToken(&token)) != OK) return result;
			if(token.type != S_LBRA ){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \")\"\n",row);
				return ERROR_SYN;
			}

			//if ((result = getToken(&token)) != OK) return result;
			result = expr();
			if(result != OK ) return result;
		
		
			if(token.type != S_RBRA ){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"{\"\n",row);
				return ERROR_SYN;
			}

			/*
			if ((result = getToken(&token)) != OK) return result;
			result = cond();
			if(result != OK ) return result; 
			*/ 
			

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

			//nastavime zpet na hlavni tabulku symbolu.. tu me rekurze nemusi trapit 
			actualST = symbolTable;
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

		//bude tu treba pro generovani ass kontrolovat priositu porovnavani

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
			
			if(token.type != S_RBRA ){
				fprintf(stderr, "Row: %d, unexpected symbol it should be \"{\"\n",row);
				return ERROR_SYN;
			}
	
			/*
			if ((result = getToken(&token)) != OK) return result;
			result = cond();
			if(result != OK ) return result; */

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
	}while(token.type == FUNCTION);


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
	


	if ((result = getFunctionHeader(&token, CONTINUE_READ)) != OK) return result;
	if (token.type != S_LBRA) return ERROR_SYN;

	
	if ((result = getFunctionHeader(&token, CONTINUE_READ)) != OK) return result;
	//zacneme zpracovavat parametry
	// funkce nema parametr
	if(token.type == S_RBRA){
		funkce->paramCount = paramCount;
		return OK; 
	}
	
	//funkce ma nejake paramtery
	while(true){
		if(token.type != S_ID) return ERROR_SYN;

		//vytvorime  novou promenou v tabulce symbolu
		if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ) return INTERNAL_ERROR;
		promena->name = mystrdup(token.value);

		//pridame promenou do tabulky symbolu promenych u funkce
		ret = addVarNodeToST(promena , symboly);
		if(ret == ITEM_EXIST) return SEM_DEF_ERROR;
		else if (ret ==  INTERNAL_ERROR) return ERROR_INTER;

		paramCount++;

		if ((result = getFunctionHeader(&token, CONTINUE_READ)) != OK) return result;

		//docetli jsme vsechny parametry
		if(token.type == S_RBRA) break;
		else if(token.type!= S_COMMA){
			fprintf(stderr, "Row: %d, unexpected symbol it should be some \",\" \n",row);
			return ERROR_SYN;
		}

		//nacteme si dalsi id a smycku zopakujem
		if ((result = getFunctionHeader(&token, CONTINUE_READ)) != OK) return result;

	}

	funkce->paramCount = paramCount;

	}

	return OK;
}


//precedenci analyza
int expr(){

	//((int)pomItem->)->data
	//(T_Token*)(pomItem->data)->type

	//(((int)((T_Token*)pomItem)->data)->type

	int porovnavani = 0; // promena ktera mi zaruci ze budu porovnavat jen kdyz muzu, kazde pushnuti funkce  nebo = ji inkrementuje, kazde popnuti dekremenutuje,
						 // avsak pokud je prazdny zasobnik  tak se vracim tak mi je jedno ze se to dekrementuje na 0
	
	tStackItem *pomItem;
	int radek, sloupec;
	char *retezec;
	bool ukoncovac;
	T_Token *exprToken, exprTempToken;
	

	
	//nahrajeme dolar mame prazdny zasobnik
	if((exprToken = (T_Token*)malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;
	exprToken->type = S_DOLAR;
	if((push(zasobnik, exprToken)) != OK ) return ERROR_INTER;

	//jestli pporovnavat povolene jset, povime promene ctene
	if(token.type == S_FUNC) porovnavani++; // pri function(...) nemuze byt jiz nikde porovnavani
	
	
	//tabulka -radek  co je na zasobniku, sloupec prichozi token

	do{	
		//zkontrolujeme si co mame delat na zaklade precedenci tabulky	
		pomItem = top(zasobnik);
		//jelikoz na vrcholu zasobniku muze byt neterrminal E, ktery je fyzicky reprzentovan jako token jehoz typ je E, ve skutecnosti bude stacit sebrat jen ten dalsi co je pod nim ale radeji to udelam obecne
		while((((T_Token*)(pomItem)->data)->type) == S_E)	pomItem = pomItem->prev;


		radek = (((T_Token*)(pomItem)->data)->type) - 20;
		sloupec = (token.type) - 20;
		
		//pokud nahodou se objevil token ktery nema co v zasobniku delat nebo prijde na vstup, tak skocim mimo velikost pole coz je szyntakticka chyba
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
		
			
		// nacitame na zasobnik
		if(prtable[radek][sloupec] == L){
			//mame nahravat tak si pro jistotu ykontrolujem jelikoz = muze byt nahrano jen  za stavu $ E =
			
			
		 	//nahrajeme mensitko na stack
			if((exprToken = (T_Token*)malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;
			exprToken->value = NULL;
			exprToken->type = L;
			if((push(zasobnik, exprToken)) != OK ) {free(exprToken);return ERROR_INTER;}

			//vytvareni si noveho tokenu je z duvodu ze pri returnu s chybou a naslednme uvolnovani pameti bychom mohli uvolnovat znova stejny token
			//coz by hodilo segfault, nepomuze ani nastavit pointer pak na NULL, jelikoz v garbage collectoru by ten pointer NULL nebyl
			if((exprToken = (T_Token*)malloc(sizeof(T_Token))) == NULL) return ERROR_INTER;

			//pri puvodnim vytvoreni tokenu nebo vzdy pri zpracovani dat z neho se nahraje na ukazatel ukazujici na data NULL
			//zkopirujeme si data
			if(token.value != NULL){

				switch(token.type){
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
		}
		// syntakticka chyba
		else if (prtable[radek][sloupec] == X){
			fprintf(stderr, "Row: %d, syntax error\n",row );
			return ERROR_SYN;

		}

		//redukujeme
		else if (prtable[radek][sloupec] == H){

		   do{ // ve smycce abychom pri dolaru sjeli cely zasobnik, neocekavame zaden vstup
		   	ukoncovac = false; // nastavi si ho dolar na true takze se pojede znova
			if(token.type == S_DOLAR) ukoncovac = true; 

			//redukujeme tak ze budeme postupne brat tokeny ze zasobniku a zezadu budeme porovnavat s pravidlem dokud nenarazime
			// na mensitko, pokud bude porad se nam v nekter fazi stane ze uz nemuzeme  pokracovat a nemame mensitko je to syntax error
			pomItem = pop_top(zasobnik);
			
			//je tu treba hodit smycku ktera se postara ze se to vycisti pri dolaru
			switch(((T_Token*)(pomItem)->data)->type){

				//pravidla 10. E -> id, 11. E -> i //int, 12. E -> d //double, 13. E -> b //bool, 14. E -> s //string, 15. E -> n // null
				case S_ID:
				case S_INT:
				case S_DOUB:
				case S_BOOL:
				case S_STR: 
				case S_NULL:
					//todo: pred uvolnenim tokenu ho nahrat do ASS

					tokenFree (((T_Token*)(pomItem)->data));
					pomItem = top(zasobnik); //usetrime si alokaci noveho tokenu tim ze zmenime typ toho stareho a dealokujeme mu data

					if((((T_Token*)(pomItem)->data)->type) != L) {
						fprintf(stderr, "Row: %d, unexpected symbol in expresion\n",row );
						return ERROR_SYN;
					}

					if(((T_Token*)(pomItem)->data)->value != NULL){ free(((T_Token*)(pomItem)->data)->value); ((T_Token*)(pomItem)->data)->value= NULL;}
					((T_Token*)(pomItem)->data)->type= S_E; // nemusime nahravat na zasobnik pracovali jsme s tokenem ktery porad byl na zasobniku, a nahravame neterminal tak ani nemusime resit mensitko vetsitko rovna se
					break;

				//pravidla 3. E -> (E), 9. E -> f(E)
				case S_RBRA:
					//todo: pred uvolnenim tokenu ho nahrat do ASS
					tokenFree (((T_Token*)(pomItem)->data));
					pomItem = pop_top(zasobnik); 

					if((((T_Token*)(pomItem)->data)->type) != S_E) {
						tokenFree (((T_Token*)(pomItem)->data));
						fprintf(stderr, "Row: %d, unexpected symbol in expresion\n",row );
						return ERROR_SYN;
					}

					//todo: pred uvolnenim tokenu ho nahrat do ASS
					tokenFree (((T_Token*)(pomItem)->data));
					pomItem = pop_top(zasobnik); 

					if((((T_Token*)(pomItem)->data)->type) != S_RBRA){
						tokenFree (((T_Token*)(pomItem)->data));
						fprintf(stderr, "Row: %d, unexpected symbol in expresion\n",row );
						return ERROR_SYN;
					}


					//todo: pred uvolnenim tokenu ho nahrat do ASS
					tokenFree (((T_Token*)(pomItem)->data));
					pomItem = pop_top(zasobnik); 

					// pravidlo 3. E -> (E)
					if((((T_Token*)(pomItem)->data)->type)== L){
						if(((T_Token*)(pomItem)->data)->value != NULL){ free(((T_Token*)(pomItem)->data)->value); ((T_Token*)(pomItem)->data)->value= NULL;}
						((T_Token*)(pomItem)->data)->type = S_E;
						if((push(zasobnik, (T_Token*)(pomItem)->data)) != OK ) {free(exprToken);return ERROR_INTER;}

						break;
					}
					//dale pokracuje pravidlo 9. E -> f(E)
					else if((((T_Token*)(pomItem)->data)->type) != S_FUNC) {
						tokenFree (((T_Token*)(pomItem)->data));
						fprintf(stderr, "Row: %d, unexpected symbol in expresion\n",row );
						return ERROR_SYN;
					}

					
					tokenFree (((T_Token*)(pomItem)->data));
					pomItem = top(zasobnik); //usetrime si alokaci noveho tokenu tim ze zmenime typ toho stareho a dealokujeme mu data

					if((((T_Token*)(pomItem)->data)->type) != L) {
						fprintf(stderr, "Row: %d, unexpected symbol in expresion\n",row );
						return ERROR_SYN;
					}

					if(((T_Token*)(pomItem)->data)->value != NULL){ free(((T_Token*)(pomItem)->data)->value); ((T_Token*)(pomItem)->data)->value= NULL;}
					((T_Token*)(pomItem)->data)->type = S_E; // nemusime nahravat na zasobnik pracovali jsme s tokenem ktery porad byl na zasobniku, a nahravame neterminal tak ani nemusime resit mensitko vetsitko rovna se
					break;


				case S_E:
					tokenFree (((T_Token*)(pomItem)->data));
					pomItem = pop_top(zasobnik); 

					switch(((T_Token*)(pomItem)->data)->type){

							//pravidla: 1. E -> E+E, 2. E -> E*E, 5. E -> E.E, 6. E -> E/E, 7. E -> E-E, 8. E -> E,E, 16. E -> E <= E, 17. E -> E < E, 18. E -> E >= E, 19. E -> E > E, 20. E -> E !== E, 21. E -> E === E
							case S_PLUS:
							case S_MUL:
							case S_CONCATENATE:
							case S_DIV:
							case S_COMMA:
							case S_LEQ:
							case S_LST:
							case S_GEQ:
							case S_GRT:
							case S_NEQ:
							case S_EQ: 
									break;									

							//pravidlo 4. E ->  = E 
							case S_IS:
									tokenFree (((T_Token*)(pomItem)->data));
									pomItem = pop_top(zasobnik); //usetrime si alokaci noveho tokenu tim ze zmenime typ toho stareho a dealokujeme mu data

									if((((T_Token*)(pomItem)->data)->type) != S_DOLAR) {
										fprintf(stderr, "Row: %d, unexpected symbol in expresion\n",row );
										return ERROR_SYN;
									}

									tokenFree (((T_Token*)(pomItem)->data));
									if (zasobnik->top != NULL)	return ERROR_INTER;

							// stav zasobniku by mel byt $E
							case S_DOLAR:
								if(token.type == S_DOLAR){
									//vratime si zpet co bylo v tokenu nez jsme ho prehrali dolarem
									token.type = exprTempToken.type;
									token.value = exprTempToken.value;
									tokenFree (((T_Token*)(pomItem)->data));
									return OK;
								}

								//dle pravidel a kosntrukce muzu mit na stacku $= nebo $( pak se tam neco muze nahrat takze muzu s jistotou prohlasit ze se jedna o syntax error
								fprintf(stderr, "Row: %d, unexpected symbol in expresion\n",row );
								return ERROR_SYN;

							default: 
								tokenFree (((T_Token*)(pomItem)->data));
								fprintf(stderr, "Row: %d, unexpected symbol in expresion\n",row );
								return ERROR_SYN;
					}

					tokenFree (((T_Token*)(pomItem)->data));
					pomItem = top(zasobnik); //usetrime si alokaci noveho tokenu tim ze zmenime typ toho stareho a dealokujeme mu data

					if((((T_Token*)(pomItem)->data)->type) != L) {
							fprintf(stderr, "Row: %d, unexpected symbol in expresion\n",row );
							return ERROR_SYN;
					}

					if(((T_Token*)(pomItem)->data)->value != NULL){ free(((T_Token*)(pomItem)->data)->value); ((T_Token*)(pomItem)->data)->value= NULL;}
					((T_Token*)(pomItem)->data)->type = S_E; // nemusime nahravat na zasobnik pracovali jsme s tokenem ktery porad byl na zasobniku, a nahravame neterminal tak ani nemusime resit mensitko vetsitko rovna se
					break;


			} 


			pomItem = pop_top(zasobnik);	
		  }while(ukoncovac);


		}

		else return ERROR_INTER;

	}while(true);
	return OK;
}