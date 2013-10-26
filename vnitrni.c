#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "types.h"
#include "ial.h"
#include "vnitrni.h"
#include "stack.h"
#include "ast_tree.h"
#include "scaner.h"

//#include "cilovy_kod.h"

extern tStack *alejStromu; //z parseru, je to ASS
T_ST_Vars *symbolTable;
T_ST_Funcs *functionTable;
T_ST_Vars *actualST;
tStack *pomStack;
T_Token LastVar;

int size = 64;
int index = 0;
int temp_var=0;
TAC *paska;

int generateCode(){
	
	actualST = symbolTable;
	T_ST_Funcs *funkce;

	LastVar.type =  S_ID;
	tStackItem* item;
	tExpr ret;
	int result;

	pomStack = SInit();
	if (pomStack == NULL) return ERROR_INTER;
	garbage_add(pomStack, &emptyToken);

	paska = (TAC*) malloc(size * sizeof(TAC));
	if(paska != NULL) return ERROR_INTER;
	
	


	item = pop_back(alejStromu);

	while (item != NULL){
	
		switch(((T_Token*)(item)->data)->type){

			case S_E:  	

				switch (((Tleaf*)((T_Token*)(item)->data)->value)->op->type){



					case WHILE:
					case IF:	
						if ((result = generate(((Tleaf*)((T_Token*)(item)->data)->value)->op->type, ((Tleaf*)((T_Token*)(item)->data)->value)->op1, NULL,NULL)) != OK ) return result;  
						
					default:
						ret = exprGC(((Tleaf*)((T_Token*)(item)->data)->value)->op1->value, RIGHT);
						if (ret.ret != OK ) return ret.ret;
						break;

			 	}
			 case FUNCTION:		
						//nastavime si tabulku symbolu na funkci 
						funkce = findFunctionST(((Tleaf*)((T_Token*)(item)->data)->value)->op->value, functionTable);
						actualST = 	funkce->data->symbolTable;
						if ((result = generate(FUNCTION, item->data, NULL,NULL)) != OK ) return result; 
						break;
			case RETURN:

					actualST = symbolTable;
					if ((result = generate(FUNCTION, NULL, NULL,NULL)) != OK ) return result; 
					break;

			 //BLOCK_END, BLOCK_START
			 default:
			 		if ((result = generate(((Tleaf*)((T_Token*)(item)->data)->value)->op->type, NULL, NULL,NULL)) != OK ) return result;  
		}

		item = pop_back(alejStromu);
	}

	return OK;
}
 
 

tExpr exprGC(Tleaf *tree, Smery smer){

	T_Token tempVar;
	int result = OK;
	tExpr ret;

	ret.rightVar = NULL;
	ret.leftVar = NULL;

	// je tam volani funkce zpracujeme si ji jinac nez ostatni
	if(tree->op->type ==  S_FUNC){
		if((generateTempVar(&tempVar)) == OK) {ret.ret= ERROR_INTER; return ret;}
		if ((generate(S_FUNC, tree->op, NULL, &tempVar))  != OK ) {ret.ret= ERROR_INTER; return ret;}
		//zpracujem parametru funkce
		if ((result  = funGC(tree->op1->value)) != OK ) {ret.ret= ERROR_INTER; return ret;}

		//vygenerujeme volani funkce
		if((generate(CALL, NULL, NULL, NULL)) != OK ) {ret.ret= ERROR_INTER; return ret;}

		//vse v poradku
		ret.ret = OK;
		return ret;
	}

	//prochazime strom do prava 
	//((Tleaf*)((T_Token*)(item)->data)->value)->op->type
	if(((Tleaf*)(tree->op2->value))->op1->type == S_E){
		ret = exprGC(tree->op2->value, RIGHT);
		if(ret.ret != OK)  return ret;
	}

	//v pravo uz nesjou podstromy jdemem do leva
	if(((Tleaf*)(tree->op1->value))->op1->type == S_E){
		ret = exprGC(tree->op1->value, LEFT);
		if(ret.ret != OK)  return ret;
	}

	//dosli jsme na dno
	else{

		if(tree->op->type ==  S_IS){  		
			if((generate(S_IS, ((Tleaf*)(tree->op2->value))->op1, NULL, ((Tleaf*)(tree->op1->value))->op1))  != OK ){ ret.ret = ERROR_INTER; return ret; }
		}
		else{
			if((generateTempVar(&tempVar)) == OK) { ret.ret = ERROR_INTER; return ret; }
			if((generate(tree->op->type, ((Tleaf*)(tree->op1->value))->op1, ((Tleaf*)(tree->op2->value))->op1, &tempVar))  != OK ) { ret.ret = ERROR_INTER; return ret; }

			if(smer == LEFT){
				if((ret.leftVar = (T_Token*)malloc(sizeof(T_Token))) == NULL ) { ret.ret = ERROR_INTER; return ret; }
				ret.leftVar->type = S_ID;
				if ((ret.leftVar->value = mystrdup(tempVar.value)) == NULL) { ret.ret = ERROR_INTER; return ret; }
			}

			else {
				if((ret.rightVar = (T_Token*)malloc(sizeof(T_Token))) == NULL) { ret.ret = ERROR_INTER; return ret; }
				ret.rightVar->type = S_ID;
				if ((ret.rightVar->value = mystrdup(tempVar.value)) == NULL) { ret.ret = ERROR_INTER; return ret; }
			}
		}

		ret.ret = OK;
		return ret;
	}

	free(LastVar.value);
	if(tree->op->type ==  S_IS){

		//vysledek nam muze probublat jen zprava
		if(ret.rightVar != NULL) { if((generate(tree->op->type, ret.rightVar, NULL, ((Tleaf*)(tree->op1->value))->op1 ))  != OK ) { ret.ret = ERROR_INTER; return ret; } }
		else {
			if((generate(tree->op->type, ((Tleaf*)(tree->op2->value))->op1, NULL, ((Tleaf*)(tree->op1->value))->op1 ))  != OK ) { ret.ret = ERROR_INTER; return ret; }
		}
		LastVar.value = mystrdup(((Tleaf*)(tree->op1->value))->op1->value);
	} 
	
	else {

		if((generateTempVar(&tempVar)) == OK) { ret.ret = ERROR_INTER; return ret; }
	
		//mame operaci se dvema pomocnyma promenyma
		if(ret.rightVar != NULL && ret.leftVar != NULL){		
		
			if((generate(tree->op->type, ret.leftVar, ret.rightVar, &tempVar))  != OK ) { ret.ret = ERROR_INTER; return ret; }
		}
		//jen jedna pomocna promena

		else if (ret.rightVar != NULL ){
			if((generate(tree->op->type, ((Tleaf*)(tree->op1->value))->op1, ret.rightVar, &tempVar))  != OK ) { ret.ret = ERROR_INTER; return ret; }
		}
		else if (ret.leftVar != NULL){
			if((generate(tree->op->type, ret.leftVar, ((Tleaf*)(tree->op2->value))->op1, &tempVar))  != OK ) { ret.ret = ERROR_INTER; return ret; }
		}

		free(ret.rightVar->value);
		free(ret.leftVar->value);

		if (smer == LEFT ) {
			ret.leftVar->value = mystrdup(tempVar.value);
			free(ret.rightVar);		
			ret.rightVar = NULL;	
		}

		else  {
			ret.rightVar->value = mystrdup(tempVar.value);
			free(ret.leftVar);
			ret.leftVar = NULL;			
		}

		LastVar.value = mystrdup(tempVar.value);

	}

	ret.ret = OK;
	return ret;

}


// zpracovani funkce
int funGC(Tleaf *tree){
	T_Token tempVar;
	int result = OK;
	tExpr ret;
	
	//podivame se co je vpravem operandu
	//operator neco obsahuje bud mat. operaci nebo carku nebo volani dalsi funkce
	if((tree->op) != NULL){

	if((generateTempVar(&tempVar)) == OK ) return ERROR_INTER;

		//okontrolujem funkci a podle toho se zaridime		
		if((tree->op->type)== S_FUNC){

			if((generate(S_FUNC, tree->op, NULL, &tempVar)) != OK ) return ERROR_INTER;
			//rekurzivne vytvarime kod pro volani funkce
			if ((result  = funGC(tree->op->value)) != OK ) return result;
			if((generate(CALL, NULL, NULL, NULL)) != OK ) return ERROR_INTER;
 		}

 		//mame E->E,E 
 		else if(tree->op->type == S_COMMA){
 			if ((funGC(tree->op1->value)) != OK ) return ERROR_INTER; //leva vetev
 			if ((funGC(tree->op2->value)) != OK ) return ERROR_INTER; //prava vetev
 		}
		
		//mat. operace
		else {
			//vyhodnotime si pod vyraz
			ret= exprGC(tree, RIGHT);
			if(ret.ret != OK) return ret.ret;
			if((generate(STORE_PARAM, &LastVar, NULL, NULL)) != OK ) return ERROR_INTER;
		}
	}

	//v levem operadnu je nejaky eToken ktery je  cislo, promena atd.. z principu funkce generovani ASSu jsou ostatni veci prazdne
	if((generate(STORE_PARAM, tree->op1, NULL, NULL)) != OK ) return ERROR_INTER;

	return result;

}

int generate(int operator, T_Token* tok1, T_Token* tok2, T_Token* vysTok){
	
	TAC *delsiPaska;
	T_Token *iToken;

	//zvetsime pasku
	if (index >= size ){ 

			size += 64;
			delsiPaska =  (TAC*) realloc(paska, size * sizeof(TAC));
			
			if(delsiPaska == NULL ){
				free(paska);
				return ERROR_INTER;
			}

			else paska = delsiPaska;
	}

	//zkopirujeme data
	(paska[index]).operator = operator;
	
	if(tok1 != NULL ){
		(paska[index]).operand1.type = tok1->type;
		(paska[index]).operand1.value = mystrdup(tok1->value);
	}else (paska[index]).operand1.type = NOT_EXIST;

	if(tok2 != NULL ){
		(paska[index]).operand2.type = tok2->type;
		(paska[index]).operand2.value = mystrdup(tok2->value);
	}else (paska[index]).operand2.type = NOT_EXIST;

	if(vysTok != NULL ){
		(paska[index]).vysledek.type = vysTok->type;
		(paska[index]).vysledek.value = mystrdup(vysTok->value);
	}else (paska[index]).vysledek.type = NOT_EXIST;

	// udelame si adresu funkce abychom mohli doplnit adresu skoku do call
	if(operator == FUNCTION){
			//i kdyz pouyziejem token tak ho pouzivame trochu jinak jelikoz misto typu tam vkladame adresu kam skocit
			iToken = (T_Token*) malloc(sizeof(T_Token));

			if(iToken == NULL) return ERROR_INTER;

			iToken->type = index;
			iToken->value = mystrdup(tok1->value);
			push(pomStack, iToken);
	}
	
	index++;
	return OK;
}

int addJump(){

	int i, fun;
	tStackItem *item;


	for ( i = 0, fun = 0; i<index; i++){
	
		// najdeme si na stacku adresu funkce
		if(paska[i].operator == S_FUNC){

			item = top(pomStack);
			while(strcmp(((T_Token*)(item->data))->value, paska[i].operand1.value)) item = item->prev;
			fun = ((T_Token*)(item->data))->type;
		}

		//nastavime adresu kam skocit
		if(paska[i].operator == CALL){
			paska[i].operand1.type = fun;
			fun = 0;

		}

	}

	return OK;

}

//vygenerujeme nazev pomocne promene a pridame ji do tabulky sumbolu
int generateTempVar(T_Token *tok){
	
	char *name;
	int lenght=7,pom=temp_var;
	T_ST_VarsItem* promena;
	int ret;


	while(pom>9){
		pom=pom/10;
		lenght++;
	}
	if((name=malloc(sizeof(char)*lenght))==NULL)
		return ERROR_INTER;
	if(sprintf(name,"=temp%d",temp_var)==-1)
		return ERROR_INTER;
	temp_var++;
	tok->value = name;

	if((promena = (T_ST_VarsItem*) malloc(sizeof(T_ST_VarsItem))) == NULL ){
		free(name);
		tok->value = NULL;
		return ERROR_INTER;
	}

	promena->name = mystrdup(name);

	if( (ret = addVarNodeToST(promena , actualST)) != OK ){
			free(name);
			tok->value = NULL;
			return ERROR_INTER;
	}
	return OK;
}