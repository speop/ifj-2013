#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "stack.h"
#include "ial.h"
#include "types.h"
#include "vnitrni.h"
#include "parser.h"
#include "vestavene_funkce.h"
#include "scaner.h" // mystrdup

#include "interpret.h"


const char *variables[4] = {"a", "b", "c", "d"};
/*
variables[0] = "a";
variables[1] = "b";
variables[2] = "c";
variables[3] = "d";
*/
extern TAC *paska;                  //z vnitrni.c
extern T_ST_Vars *symbolTable;      //obe tabulky z parser.c
extern T_ST_Funcs *functionTable;

int interpret()
{
		

	TAC *Instr;          //Instrukce
	int i = 0;          //index/pozice na pasce 
	char *funcName;
	void *op1, *op2;
	int op1_typ, op2_typ,len;   //typy operandu
	T_ST_Vars *AuxSTVar, *res, *res1, *res2;        //pomocny uzel a uzel pro vysledek
	T_ST_Funcs *funkce;        //pomocna promenna na uchovani dat o funkci
	TCallStack *CallStack;                //prvek zasobniku pro navraty
	Tparam *param;
	double doub1, doub2;
	int ret = 0;
	//tStack *returnStack = SInit();      //zasobnik navratovach hodnot
	//tStack *tableStack = SInit();         //zasobnik tabulek symbolu
	//tStack *funcStack = SInit();        //zasobnik s funkcemi a parametry pro volani
	tStack *callStack = SInit();
	tStack *paramStack = SInit();
	tStackItem *StackHelpItem, *StackHelpItem2;            //pomocny prvek pro zasobniky
	//Tparam param;           //struktura pro parametry
	//T_Token pom1, pom2, pom3;   //pomocnÄ‚Â© tokeny
	T_Token backup;

	T_ST_Vars *actualST, *newST;
	actualST = symbolTable;

	while(1)
	{
		Instr = &(paska[i++]);         //nacitame z pasky a posouvame se po ni
		
		switch (Instr->operator) {
			case  FUNCTION:  //definice funkce.. tu jen preskocime

							do {
								Instr = &(paska[i++]);
							}
							while (Instr->operator != FUNCTION_BLOCK_END); 
							break;

			case FUNCTION_BLOCK_START: 
					//skok pri volani funkce
					break;
			
			case FUNCTION_BLOCK_END: 
					StackHelpItem = pop_top(callStack);
					newST = ((TCallStack *)(StackHelpItem->data))->symbolTable; //tabulka symbolu z ktere byla tato funkce volana
					
					StackHelpItem2 = pop_top(paramStack);
				

					
					if(((Tparam *)(StackHelpItem2->data))->returnvalue != NULL) 
					{
						res = findVarST(((Tparam *)(StackHelpItem2->data))->returnvalue ,newST);
						if(res->data->value != NULL) free (res->data->value);
					
						res->data->value = malloc(sizeof(int));
						*(int*)(res->data->value) = 0;
						res->data->type = S_NULL;
					}

					
					//dealokace ST
					//freeVarST(actualST);
					//jeste zmenit tabulky symbolu
					actualST = newST;

					//nastavime index na pasce
					i = ((TCallStack *)(StackHelpItem->data))->adress;

					//uvolnime pamet
					freeVarST(((Tparam *)(StackHelpItem2->data))->symbolTable);
					free(((Tparam *)(StackHelpItem2->data))->funcName);
					free(((Tparam *)(StackHelpItem2->data))->returnvalue);
					free(StackHelpItem2->data);
					free(StackHelpItem2);

					free(StackHelpItem);

					break;

			case  RETURN:
				StackHelpItem = pop_top(callStack);

				//pokud je to NULL returnuju se z mainu takze konci provadeni
				if(StackHelpItem == NULL){
					destroyPaska(paska);
					return OK;
				}
				

				newST = ((TCallStack *)(StackHelpItem->data))->symbolTable; //tabulka symbolu z ktere byla tato funkce volana
				
				StackHelpItem2 = pop_top(paramStack);
				
				//mame hodnotu na ulozeni tak ji ulozime
				if(Instr->operand1.type != NOT_EXIST){ 
					
					res = findVarST(((Tparam *)(StackHelpItem2->data))->returnvalue ,newST);
					if(res->data->value != NULL) free (res->data->value);

					//neukladame promenou ale hodnotou
					if( Instr->operand1.type != S_ID ){
						op1 = Instr->operand1.value;
						op1_typ = Instr->operand1.type;
					}
					else{
						AuxSTVar = findVarST(Instr->operand1.value, actualST);
						if(AuxSTVar == NULL || AuxSTVar->data->value == NULL){
							fprintf(stderr, "Undefined variable\"%s\"\n",(char*)(Instr->operand1).value);
							return SEM_UNDECLARED_PARAMETER;
						}
						op1 = AuxSTVar->data->value;
						op1_typ =AuxSTVar->data->type;
					}
					

					
					switch(op1_typ){
				  		case S_STR:
					  		res->data->value = mystrdup((char*)op1);
					  		res->data->type = S_STR;
					  		break;

				  		case S_BOOL:
				  		case S_NULL:
				  		case S_INT:
					 		res->data->value = malloc(sizeof(int));
					  		*(int*)(res->data->value) = *(int*)op1;
					  		res->data->type = op1_typ;
					  		//printf("returnuju %d\n", *(int*)(res->data->value));
					  		break;

				  		case S_DOUB:
							 res->data->value = malloc(sizeof(double));
					  		*(double*)(res->data->value) = *(double*)op1;
					  		//printf("returnuju2 %f\n", *(double*)(res->data->value));
					  		res->data->type = S_DOUB;
					  		break;
					}

					
				}
				//neni navratova hodnota nahrajem NULL
				else
				{
					if(((Tparam *)(StackHelpItem->data))->returnvalue != NULL) 
					{
						res = findVarST(((Tparam *)(StackHelpItem->data))->returnvalue ,newST);
						if(res->data->value != NULL) free (res->data->value);
					
						res->data->value = malloc(sizeof(int));
						*(int*)(res->data->value) = 0;
						res->data->type = S_NULL;
					}

				}

				
				//jeste zmenit tabulky symbolu
				actualST = newST;

				//a nastavit index na pasce
				i = ((TCallStack *)(StackHelpItem->data))->adress; 
				
				//uvolnime pamet
				freeVarST(((Tparam *)(StackHelpItem2->data))->symbolTable);
				free(((Tparam *)(StackHelpItem2->data))->funcName);
				free(((Tparam *)(StackHelpItem2->data))->returnvalue);
				free(StackHelpItem2->data);
				free(StackHelpItem2);

				free(StackHelpItem);

				break;
			case S_PLUS:
			case S_MINUS:
			case S_MUL:
			case S_DIV:
				if(Instr->operand1.type == S_ID) { //operand1 je to promenna
					AuxSTVar = findVarST(Instr->operand1.value, actualST);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					if(AuxSTVar == NULL || AuxSTVar->data->value == NULL){
							fprintf(stderr, "Undefined variable\n");
							return SEM_UNDECLARED_PARAMETER;
					}
					
					op1_typ = AuxSTVar->data->type;
					if(op1_typ != S_INT && op1_typ != S_DOUB) { fprintf(stderr, "Incompatible types in expression\n" ); return SEM_TYPE_ERROR; }  //typ promenne  neni int ani double
					else op1 = AuxSTVar->data->value;
					//printf(" \t pocitam s: %s\n", Instr->operand1.value );
				}
				else {     //operand1 neni promenna
					op1_typ = Instr->operand1.type;
					if(op1_typ != S_INT && op1_typ != S_DOUB) { fprintf(stderr, "Incompatible types in expression\n" ); return SEM_TYPE_ERROR; }   //typ promenne  neni int ani double
					else op1 = Instr->operand1.value;

				}

				if(Instr->operand2.type == S_ID) { //operand2 je to promenna
					AuxSTVar = findVarST(Instr->operand2.value, actualST);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					if(AuxSTVar == NULL || AuxSTVar->data->value == NULL ){
							fprintf(stderr, "Undefined variable\n");
							return SEM_UNDECLARED_PARAMETER;
					}

					op2_typ = AuxSTVar->data->type;
					if(op2_typ != S_INT && op2_typ != S_DOUB) { fprintf(stderr, "Incompatible types in expression\n" ); return SEM_TYPE_ERROR; }  //typ promenne  neni int ani double
					else op2 = AuxSTVar->data->value;
					//printf(" \t pocitam s: %s\n", Instr->operand2.value );

				}
				else {      //operand2 neni promenna
					op2_typ = Instr->operand2.type;
					if(op2_typ != S_INT && op2_typ != S_DOUB) { fprintf(stderr, "Incompatible types in expression\n" ); return SEM_TYPE_ERROR; }  //typ promenne  neni int ani double
					else op2 = Instr->operand2.value;
					
				}

				res = findVarST(Instr->vysledek.value, actualST);
				if (res->data->value != NULL) free(res->data->value);

					  //vypocet
				if(op1_typ == S_DOUB || op2_typ == S_DOUB || Instr->operator == S_MUL || Instr->operator == S_DIV) {
					
					if(op1_typ == S_INT) doub1 = (double)(*(int*)op1);
					else  doub1 = *(double*)op1;
					
					if(op2_typ == S_INT) doub2 = (double)(*(int*)op2);
					else  doub2 = *(double*)op2;

				  res->data->value = (double*)malloc(sizeof(double));

				if(Instr->operator == S_MUL) *((double*)(res->data)->value) = doub1 * doub2;
				else if (Instr->operator == S_DIV) { 
						//kontrola jestli nedelim 0
						if(doub2 == 0){ fprintf(stderr, "Error division by zero\n" ); return SEM_ZERRO_DIVISION; }
						*((double*)(res->data)->value) =doub1 / doub2;
				}
				else if(Instr->operator == S_PLUS) *((double*)(res->data)->value) = doub1 + doub2;
				else *((double*)(res->data)->value) = doub1 - doub2;
				//printf("\tpocitani -%f- -%f- = -%f- \n", doub1, doub2, *((double*)(res->data)->value) );
				res->data->type = S_DOUB;
				}
				else { 
				  res->data->value = (int*)malloc(sizeof(int));
				  if(Instr->operator == S_PLUS)  *((int*)(res->data)->value) = *((int*)(op1)) + *((int*)(op2));
				  else *((int*)(res->data)->value) = *((int*)(op1)) - *((int*)(op2));
				  res->data->type = S_INT;
				}

			break;

			case S_CONCATENATE:

			  if(Instr->operand1.type == S_ID) { //operand1 je to promenna
				AuxSTVar = findVarST(Instr->operand1.value, actualST);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
				if(AuxSTVar == NULL || AuxSTVar->data->value == NULL){
							fprintf(stderr, "Undefined variable\n");
							return SEM_UNDECLARED_PARAMETER;
				}

				op1_typ = AuxSTVar->data->type;
				if(op1_typ != S_STR) {
					fprintf(stderr, "Not a string as a first operand for concatenation\n");
					return SEM_TYPE_ERROR;  //typ promenne  neni string
			  } 
				else op1 = AuxSTVar->data->value;
			  }
			  else {     //operand1 neni promenna
				op1_typ = Instr->operand1.type;
				if(op1_typ != S_STR) {
					fprintf(stderr, "Not a string as a first operand for concatenation\n");
					return SEM_TYPE_ERROR;  //typ hodnoty neni string
				}
				else op1 = Instr->operand1.value;
			  }

			  if(Instr->operand2.type == S_ID) { //operand2 je to promenna
				AuxSTVar = findVarST(Instr->operand2.value, actualST);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
				if(AuxSTVar == NULL || AuxSTVar->data->value == NULL){
							fprintf(stderr, "Undefined variable\n");
							return SEM_UNDECLARED_PARAMETER;
				}

				op2_typ = AuxSTVar->data->type;
				if(op2_typ != S_STR) {
					op2 = strval(*((T_ST_VarsItem *)(AuxSTVar->data)));		//pretypovani					
				}
				else op2 = AuxSTVar->data->value;
			  }
			  else {      //operand2 neni promenna
				op2_typ = Instr->operand2.type;
				switch(op2_typ) {
					case S_INT:
							op2 = IntToStr(*(int *)(Instr->operand2.value));
							break;
					case S_DOUB:
							op2 = DoubleToStr(*(double*)(Instr->operand2.value));
							break;
					case S_STR:
							op2 = (char*)(Instr->operand2.value);
							break;
					case S_BOOL:
							op2 = BoolToStr(*(int*)(Instr->operand2.value)); // vnitrne je bool reprezentovan jako int
							break;					              
					case S_NULL:
							op2 = "";
							break;
					}
			  }

			  res = findVarST(Instr->vysledek.value, actualST);
			  if (res->data->value != NULL) free(res->data->value);

			  len = strlen((char*)op1) + strlen((char*)op2) +1; // vysledna delka retezce
			  res->data->value = (char*)malloc(sizeof(char)*len);

			  strcpy ((char*)(res->data)->value,(char*)op1);
			  strcat ((char*)(res->data)->value,(char*)op2);
			  res->data->type = S_STR;

			  break;


			case S_IS:
				if(Instr->operand1.type == S_ID) { //operand1 je to promenna
				  AuxSTVar = findVarST(Instr->operand1.value, actualST);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
				  if(AuxSTVar == NULL || AuxSTVar->data->value == NULL){
							fprintf(stderr, "Undefined variable\n");
							return SEM_UNDECLARED_PARAMETER;
					}
				  op1_typ = AuxSTVar->data->type;
				  op1 = AuxSTVar->data->value;
				  //printf("\prirazuju promena \"%s\"\n",Instr->operand1.value );
				}
				else {     //operand1 neni promenna
				  op1_typ = Instr->operand1.type;
				  op1 = Instr->operand1.value;
				  //printf("%d %d \n",i,*(int*)op1 );
				}

				res = findVarST(Instr->vysledek.value, actualST);
				if (res->data->value != NULL) free(res->data->value);
				//printf("\tkam  \"%s\"\n",Instr->vysledek.value );
				//ulozeni hodnoty
				switch(op1_typ){
				  case S_STR:
					  res->data->value = mystrdup((char*)op1);
					  res->data->type = S_STR;
					  break;

				  case S_BOOL:
				  case S_NULL:
				  case S_INT:
					  res->data->value = malloc(sizeof(int));
					  *((int*)(res->data)->value) = *(int*)op1;
					  res->data->type = op1_typ;
					  //printf("prirazena hodnota: %d\n",*((int*)(res->data)->value) );
					  break;

				  case S_DOUB:
					  res->data->value = malloc(sizeof(double));
					  *((double*)(res->data)->value) = *(double*)op1;
					  res->data->type = S_DOUB;
					  //printf("prirazena hodnota#:  %f\n", *((double*)(res->data)->value) );
					  break;
				}

				break;

			case S_FUNC:   
					if((param = (Tparam*)malloc(sizeof(Tparam))) == NULL) {fprintf(stderr,"Error in alocating stack for params\n"); return ERROR_INTER;}

					param->funcName = mystrdup(Instr->operand1.value); 
					funkce = findFunctionST(param->funcName, functionTable); 
					param->symbolTable = copyTable(funkce->data->symbolTable); 
					param->returnvalue = mystrdup(Instr->vysledek.value);
					param->paramCount = funkce->data->paramCount; 

					param->BIfPointer = 0; // ukazatatel do pole nazvu promenych

					// jeste nastavime priznak ze se jedna o vestavenou funkci
					if(strcmp(Instr->operand1.value, "get_string")==0) param->BIf =  true;
					else if(strcmp(Instr->operand1.value, "put_string")==0) param->BIf =  true;
					else if(strcmp(Instr->operand1.value, "strlen")==0) param->BIf =  true;
					else if(strcmp(Instr->operand1.value, "get_substring")==0) param->BIf =  true;
					else if(strcmp(Instr->operand1.value, "find_string")==0) param->BIf =  true;
					else if(strcmp(Instr->operand1.value, "sort_string")==0) param->BIf =  true;
					else if(strcmp(Instr->operand1.value, "strval")==0) param->BIf =  true;
					else if(strcmp(Instr->operand1.value, "intval")==0) param->BIf =  true;	
					else if(strcmp(Instr->operand1.value, "doubleval")==0) param->BIf =  true;
					else if(strcmp(Instr->operand1.value, "boolval")==0) param->BIf =  true;
					else param->BIf =  false;


					push(paramStack, param);

					break;
					

			case S_LST:
			case S_GRT:
			case S_LEQ:
			case S_GEQ:
			case S_EQ:
			case S_NEQ:
				if(Instr->operand1.type == S_ID) { //operand1 je promenna
					AuxSTVar = findVarST(Instr->operand1.value, actualST);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					if(AuxSTVar == NULL || AuxSTVar->data->value == NULL){
							fprintf(stderr, "Undefined variable \"%s\"\n",(char*)(Instr->operand1).value);
							return SEM_UNDECLARED_PARAMETER;
					}

					op1_typ = (*AuxSTVar).data->type;
					op1 = AuxSTVar->data->value;
				}
				else {     //operand1 neni promenna
					op1_typ = Instr->operand1.type;
					op1 = Instr->operand1.value;
				}

				if(Instr->operand2.type == S_ID) { //operand2 je promenna
					AuxSTVar = findVarST(Instr->operand2.value, actualST);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					if(AuxSTVar == NULL || AuxSTVar->data->value == NULL){
							fprintf(stderr, "Undefined variable \"%s\"\n",(char*)(Instr->operand1).value);
							return SEM_UNDECLARED_PARAMETER;
					}
					op2_typ = (*AuxSTVar).data->type;
					op2 = AuxSTVar->data->value;
				}
				else {      //operand2 neni promenna
					op2_typ = Instr->operand2.type;
					op2 = Instr->operand2.value;
				}

				res = findVarST(Instr->vysledek.value, actualST);
				if (res->data->value != NULL) free(res->data->value);
				
				res->data->type = S_BOOL;
				res->data->value = (int*)malloc(sizeof(int));

				//jestli se typy nerovnaji, je to false nebo chyba
				if(op1_typ != op2_typ) {
				  if(Instr->operator == S_EQ || Instr->operator == S_NEQ){
				  	if(Instr->operator == S_NEQ)	*((int*)(res->data)->value) = 1;
				  	else *((int*)(res->data)->value) = 0;
						break;
				  }
				  else {
				  	fprintf(stderr, "porovnavani rozdilnych typu\n");
						return SEM_TYPE_ERROR;
				  }
				}
				else {		//typy se rovnaji
					switch(op1_typ) {		//kdyz se rovnaji, staci kontrolovat jen prvni
						case S_INT:
						case S_BOOL:
							  switch(Instr->operator) {
									case S_LST:
										if((*((int *)op1) < *((int *)op2))) *((int*)(res->data)->value) = 1;
										else *((int*)(res->data)->value) = 0;
										break;
									case S_GRT:
										if((*((int *)op1) > *((int *)op2))) *((int*)(res->data)->value) = 1;
										else *((int*)(res->data)->value) = 0;
										break;
									case S_LEQ:
										if((*((int *)op1) <= *((int *)op2))) *((int*)(res->data)->value) = 1;
										else *((int*)(res->data)->value) = 0;
										break;
									case S_GEQ:
										if((*((int *)op1) >= *((int *)op2))) *((int*)(res->data)->value) = 1;
										else *((int*)(res->data)->value) = 0;
										break;
									case S_EQ:
										if(((*(int *)op1) == *((int *)op2))) *((int*)(res->data)->value) = 1;
										else *((int*)(res->data)->value) = 0;
										break;
									case S_NEQ:
										if((*((int *)op1) != *((int *)op2))) *((int*)(res->data)->value) = 1;
										else *((int*)(res->data)->value) = 0;
										break;
							  }
							  break;
						case S_DOUB:
							  switch(Instr->operator) {
									case S_LST:
										*((int*)(res->data)->value) = (*((double *)op1) < *((double *)op2));
										break;
									case S_GRT:
										*((int*)(res->data)->value) = (*((double *)op1) > *((double *)op2));
										break;
									case S_LEQ:
										*((int*)(res->data)->value) = (*((double *)op1) <= *((double *)op2));
										break;
									case S_GEQ:
										*((int*)(res->data)->value) = (*((double *)op1) >= *((double *)op2));
										break;
									case S_EQ:
										*((int*)(res->data)->value) = (*((double *)op1) == *((double *)op2));
										break;
									case S_NEQ:
										*((int*)(res->data)->value) = (*((double *)op1) != *((double *)op2));
										break;
							  }
							  break;
						case S_STR:
							  switch(Instr->operator) {
							  	case S_LST:
							  		if(strcmp((char *)op1, (char *)op2) < 0) *((int*)(res->data)->value) = 1;
							  		else *((int*)(res->data)->value) = 0;
							  		break;
							  	case S_GRT:
							  		if(strcmp((char *)op1, (char *)op2) > 0) *((int*)(res->data)->value) = 1;
							  		else *((int*)(res->data)->value) = 0;
							  		break;
							  	case S_LEQ:
							  		if(strcmp((char *)op1, (char *)op2) <= 0) *((int*)(res->data)->value) = 1;
							  		else *((int*)(res->data)->value) = 0;
							  		break;
							  	case S_GEQ:
							  		if(strcmp((char *)op1, (char *)op2) >= 0) *((int*)(res->data)->value) = 1;
							  		else *((int*)(res->data)->value) = 0;
							  		break;
							  	case S_EQ:
							  		if(strcmp((char *)op1, (char *)op2) == 0) *((int*)(res->data)->value) = 1;
							  		else *((int*)(res->data)->value) = 0;
							  		break;
							  	case S_NEQ:
							  		if(strcmp((char *)op1, (char *)op2) != 0) *((int*)(res->data)->value) = 1;
							  		else *((int*)(res->data)->value) = 0;
							  		break;
							  }
					}				 
				}
				break;

			case CALL:  //zjisti, kterou funkci volam a zkontroluj jeji parametry
				 //kopiruji tabulku symbolu

				//zjistime si jmeno funkce

				StackHelpItem = top(paramStack);
				funcName = ((Tparam *)(StackHelpItem->data))->funcName;

				newST = ((Tparam *)(StackHelpItem->data))->symbolTable;
				
					
			 	//zpracujeme interni funkce
			 	AuxSTVar = findVarST(((Tparam *)(StackHelpItem->data))->returnvalue, actualST); 
			 	//printf("a tu uz to neprojde jo?\n");
				if(strcmp(funcName, "get_string") == 0) {
					AuxSTVar->data->value = get_string();//
					AuxSTVar->data->type = S_STR;

					StackHelpItem = pop_top(paramStack);	

					//uvolnime pamet
					freeVarST(((Tparam *)(StackHelpItem->data))->symbolTable);
					free(((Tparam *)(StackHelpItem->data))->funcName);
					free(((Tparam *)(StackHelpItem->data))->returnvalue);
					free(StackHelpItem->data);
					free(StackHelpItem);

					break;
				}

				if(strcmp(funcName, "get_substring") == 0) {
					AuxSTVar->data->type = S_STR;
					res = findVarST((char *)variables[0], newST);
					res1 = findVarST((char *)variables[1], newST);
					res2 = findVarST((char *)variables[2], newST);
					int retVal;
					char* pom;

					retVal = get_substring((char *)(res->data->value),&pom ,*(int *)(res1->data->value),*(int*)(res2->data->value));
					if(retVal != OK) return retVal;

					if(AuxSTVar->data->value  != NULL) free(AuxSTVar->data->value); 
					AuxSTVar->data->value = pom;
					
					StackHelpItem = pop_top(paramStack);

					//uvolnime pamet
					freeVarST(((Tparam *)(StackHelpItem->data))->symbolTable);
					free(((Tparam *)(StackHelpItem->data))->funcName);
					free(((Tparam *)(StackHelpItem->data))->returnvalue);
					free(StackHelpItem->data);
					free(StackHelpItem);
					
					break;
				}

				if(strcmp(funcName, "put_string") == 0) { 
					//AuxSTVar->data->value = malloc(sizeof(int));
				 	//AuxSTVar->data->type = S_NULL;

				 	//*(int*)(AuxSTVar->data)->value = 0;
				 	//navratova hodnota se mi nastavi pri tisku parametru ktery se provadi pri storu

					StackHelpItem = pop_top(paramStack);

					//uvolnime pamet
					freeVarST(((Tparam *)(StackHelpItem->data))->symbolTable);
					free(((Tparam *)(StackHelpItem->data))->funcName);
					free(((Tparam *)(StackHelpItem->data))->returnvalue);
					free(StackHelpItem->data);
					free(StackHelpItem);

					break;
				}

				if(strcmp(funcName, "strlen") == 0) {
				 	
				 	AuxSTVar->data->value = malloc(sizeof(int));
				 	AuxSTVar->data->type = S_INT;

				 	res = findVarST((char *)variables[0], newST);
				 
				 	*(int*)(AuxSTVar->data)->value = strlen((char *)(res->data->value));
				
					 StackHelpItem = pop_top(paramStack);

					//uvolnime pamet
					freeVarST(((Tparam *)(StackHelpItem->data))->symbolTable);
					free(((Tparam *)(StackHelpItem->data))->funcName);
					free(((Tparam *)(StackHelpItem->data))->returnvalue);
					free(StackHelpItem->data);
					free(StackHelpItem);
				  
				  	break;
				}

			 	if(strcmp(funcName, "boolval") == 0) {
					
					AuxSTVar->data->value = malloc(sizeof(int));
					AuxSTVar->data->type = S_BOOL;
					
					res = findVarST((char *)variables[0], newST);
					
					*(int*)(AuxSTVar->data)->value = newboolval(*(res->data));
					
					StackHelpItem = pop_top(paramStack);

					//uvolnime pamet
					freeVarST(((Tparam *)(StackHelpItem->data))->symbolTable);
					free(((Tparam *)(StackHelpItem->data))->funcName);
					free(((Tparam *)(StackHelpItem->data))->returnvalue);
					free(StackHelpItem->data);
					free(StackHelpItem);
				  
					break;
				}
				
				if(strcmp(funcName, "doubleval") == 0) {
					
					AuxSTVar->data->value = malloc(sizeof(int));
					AuxSTVar->data->type = S_DOUB;

					ret = 0;

					res = findVarST((char *)variables[0], newST);
					
					*(double*)(AuxSTVar->data)->value = doubleval(&ret, *(res->data));
					
					 StackHelpItem = pop_top(paramStack);

					//uvolnime pamet
					freeVarST(((Tparam *)(StackHelpItem->data))->symbolTable);
					free(((Tparam *)(StackHelpItem->data))->funcName);
					free(((Tparam *)(StackHelpItem->data))->returnvalue);
					free(StackHelpItem->data);
					free(StackHelpItem);
				  
				  	if(ret) return ret;
					break;
				}
			
				if(strcmp(funcName, "intval") == 0) {
					
					AuxSTVar->data->value = malloc(sizeof(int));
					AuxSTVar->data->type = S_INT;

					res = findVarST((char *)variables[0], newST);
					*(int*)(AuxSTVar->data)->value = intval(*(res->data));
					
					 StackHelpItem = pop_top(paramStack);

					//uvolnime pamet
					freeVarST(((Tparam *)(StackHelpItem->data))->symbolTable);
					free(((Tparam *)(StackHelpItem->data))->funcName);
					free(((Tparam *)(StackHelpItem->data))->returnvalue);
					free(StackHelpItem->data);
					free(StackHelpItem);
				  
					break;
				}
				
				if(strcmp(funcName, "strval") == 0) {
					
					AuxSTVar->data->value = malloc(sizeof(int));
					AuxSTVar->data->type = S_STR;

					res = findVarST((char *)variables[0], newST);
					/**(char *)*/(AuxSTVar->data)->value = strval(*(res->data));
					
					StackHelpItem = pop_top(paramStack);
					//uvolnime pamet
					freeVarST(((Tparam *)(StackHelpItem->data))->symbolTable);
					free(((Tparam *)(StackHelpItem->data))->funcName);
					free(((Tparam *)(StackHelpItem->data))->returnvalue);
					free(StackHelpItem->data);
					free(StackHelpItem);
				  
					break;
				}
				
				if(strcmp(funcName,  "find_string") == 0) {
					AuxSTVar->data->value = malloc(sizeof(int));
				 	AuxSTVar->data->type = S_INT;

				 	res = findVarST((char *)variables[0], newST);
				 	res1 = findVarST((char *)variables[1], newST);
				 	//res2 = findVarST((char *)variables[0], newST);

				 	*(int*)(AuxSTVar->data)->value = find_string((char *)(res->data->value) ,(char *)(res1->data->value));
				
					StackHelpItem = pop_top(paramStack);

					//uvolnime pamet
					freeVarST(((Tparam *)(StackHelpItem->data))->symbolTable);
					free(((Tparam *)(StackHelpItem->data))->funcName);
					free(((Tparam *)(StackHelpItem->data))->returnvalue);
					free(StackHelpItem->data);
					free(StackHelpItem);
				  
				  	break;
				
				}

				
				if(strcmp(funcName, "sort_string") == 0) {
					AuxSTVar->data->type = S_STR;
					res = findVarST((char *)variables[0], newST);
					//printf("radim \"%s\"\n", (char *)(res->data->value));
					AuxSTVar->data->value = sort_string((char *)(res->data->value));
					StackHelpItem = pop_top(paramStack);

					//uvolnime pamet
					freeVarST(((Tparam *)(StackHelpItem->data))->symbolTable);
					free(((Tparam *)(StackHelpItem->data))->funcName);
					free(((Tparam *)(StackHelpItem->data))->returnvalue);
					free(StackHelpItem->data);
					free(StackHelpItem);

					break;
				}

				//StackHelpItem = pop_top(paramStack);
				// alokujeme novy prvek pro stack tabulku symbolu
				
				CallStack = (TCallStack*)malloc(sizeof(TCallStack));
				CallStack->symbolTable = actualST; 
				CallStack->adress = i;			

				if(push(callStack, CallStack)==INTERNAL_ERROR) {fprintf(stderr, "Could not push actual symbolTableon stack\n" ); return ERROR_INTER;}
				//zmenime aktualni tabulku symbolu
				actualST = ((Tparam *)(StackHelpItem->data))->symbolTable;

				//a zmenime i i
				i = Instr->operand1.type;

				break;


			case STORE_PARAM:

				//vestavene funkce je treba zpracovat speicfikovanym zpusobem
				//zjistime si jmeno funkce
				param = ((Tparam *)((paramStack)->top)->data);
				funcName = ((Tparam *)((paramStack)->top)->data)->funcName;

				//pokud je to putstring tak se vypisujou parametry
				if(strcmp(funcName, "put_string") == 0){ 
					
					AuxSTVar = findVarST(param->returnvalue, actualST);
					//tiskneme prvni parametr
					if(!param->BIfPointer){
						
						if(AuxSTVar->data->value != NULL) free(AuxSTVar->data->value);
						AuxSTVar->data->value = malloc(sizeof(int));
						*((int*)(AuxSTVar->data)->value) = 1;
						AuxSTVar->data->type = S_INT; 

					} 
					else{
						//dalsi parametry

						(*((int*)(AuxSTVar->data)->value))++;
					} 
					param->BIfPointer++;

					switch(Instr->operand1.type) {
							case S_INT:
								printf("%d", *((int *)(Instr->operand1).value)); break;
							case S_DOUB:
								printf("%g", *((double *)(Instr->operand1).value)); break;
							case S_STR:
								printf("%s", (char *)(Instr->operand1).value); break;
							case S_BOOL:
								if(*(int*)(Instr->operand1).value) printf("1");
								//else printf("");
								break;
							case S_NULL:
								//printf(""); 
								break;

							case S_ID:
								AuxSTVar = findVarST(Instr->operand1.value, actualST);
								if(AuxSTVar == NULL || AuxSTVar->data->value == NULL){
									fprintf(stderr, "Undefined variable \"%s\"\n",(char*)(Instr->operand1).value);
									return SEM_UNDECLARED_PARAMETER;
								}
								op1 = AuxSTVar->data->value;

								switch(AuxSTVar->data->type) {
									case S_INT:
										printf("%d", *(int*)op1); break;
									case S_DOUB:
										printf("%g", *(double *)op1); break;
									case S_STR: 
										printf("%s", (char *)op1); break;
									case S_BOOL:
										if(*(int*)op1) printf("1");
										//else printf("");
										break;
									case S_NULL:
										//printf("");
										 break;
									}
								break;

							default:
								return SEM_OTHER_ERROR;
								break;
						}
				}

				//zkontrolujeme pokud to mame jeste kam ukladat
				else if (param->paramCount > 0){

					//zkontrolujeme si jestli to neni BI funkce
					if(param->BIf){
						AuxSTVar = findVarST((char *)variables[param->BIfPointer], param->symbolTable); 
						param->BIfPointer++;
						//je to BI funkce promenou kam to ulozit zjistime z pole retezcu
					}
					else{
						//je to normal funkce jmeno kam ulozit je ve vysledku
						AuxSTVar = findVarST(Instr->vysledek.value, param->symbolTable); 
					}

					// vyhledame hodnotu promene
					if(Instr->operand1.type == S_ID){
						res = findVarST((char *) (Instr->operand1).value, actualST); 
						op1 = res->data->value;
						op1_typ = res->data->type;
						
					}
					else{
						op1_typ = Instr->operand1.type;
						op1 = Instr->operand1.value;
					}
					//ted zkopirujem do nalezene promene to hodnoty
					
					switch(op1_typ){
				  	case S_STR:
					  	AuxSTVar->data->value = mystrdup((char*)op1);
					  	AuxSTVar->data->type = S_STR;
					  	break;

				  	case S_BOOL:
				  	case S_NULL:
				  	case S_INT:
					 	 AuxSTVar->data->value = malloc(sizeof(int));
					  	*((int*)(AuxSTVar->data)->value) = *(int*)op1;
					  	AuxSTVar->data->type = op1_typ;
					  	break;

				  	case S_DOUB:
						 AuxSTVar->data->value = malloc(sizeof(double));
					  	*((double*)(AuxSTVar->data)->value) = *(double*)op1;
					  	AuxSTVar->data->type = S_DOUB;
					  	break;
					}
					//snizeni pocitadla promenych
					param->paramCount--;

				}
				
				break;

			case JMP:
				i = Instr->vysledek.type;      //zmeni se index na pasce
				break;

			case JMP_NOT:
				if(Instr->operand1.type == S_ID) { 
					AuxSTVar = findVarST(Instr->operand1.value, actualST);
					if(AuxSTVar == NULL || AuxSTVar->data->value == NULL){
							fprintf(stderr, "Undefined variable \"%s\"\n",(char*)(Instr->operand1).value);
							return SEM_UNDECLARED_PARAMETER;
					}
					op1 = AuxSTVar->data->value;

				switch(AuxSTVar->data->type){
				  	case S_STR:
					  	backup.value = mystrdup((char*)op1);
					  	backup.type = S_STR;
					  	break;

				  	case S_BOOL:
				  	case S_NULL:
				  	case S_INT:
					 	 backup.value = malloc(sizeof(int));
					  	*((int*)(backup).value) = *(int*)op1;
					  	backup.type = AuxSTVar->data->type;
					  	break;

				  	case S_DOUB:
						  backup.value = malloc(sizeof(double));
					  	*((double*)(backup).value) = *(double*)op1;
					  	backup.type = S_DOUB;
					  	break;
					}
					
					if(!boolval(backup)) i = Instr->vysledek.type;
					break;
					
				}

				else {
					if(!boolval(Instr->operand1)) i = Instr->vysledek.type;
					break;
				}
				

			case THE_END:
				
				destroyPaska(paska);
				return OK;
		}
	}

	return 0;
}
