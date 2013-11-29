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


const char *variables[2];
variables[0] = "a";
variables[1] = "b";
variables[2] = "c";
variables[3] = "d";

extern TAC *paska;                  //z vnitrni.c
extern T_ST_Vars *symbolTable;      //obe tabulky z parser.c
extern T_ST_Funcs *functionTable;

int interpret()
{
		

	TAC *Instr;          //Instrukce
	int i = 0;          //index/pozice na pasce 
	char *funcName;
	void *op1, *op2, *pomptr;
	int op1_typ, op2_typ,len;   //typy operandu
	T_ST_Vars *AuxSTVar, *res, *res1, *res2;        //pomocny uzel a uzel pro vysledek
	T_ST_Funcs *funkce;        //pomocna promenna na uchovani dat o funkci
	TCallStack *CallStack;                //prvek zasobniku pro navraty
	Tparam *param;
	tStack *returnStack = SInit();      //zasobnik navratovach hodnot
	tStack *tableStack = SInit();         //zasobnik tabulek symbolu
	tStack *funcStack = SInit();        //zasobnik s funkcemi a parametry pro volani
	tStack *callStack = SInit();
	tStack *paramStack = SInit();
	tStackItem *StackHelpItem;             //pomocny prvek pro zasobniky
	//Tparam param;           //struktura pro parametry
	T_Token pom1, pom2, pom3;   //pomocnÄ‚Â© tokeny
	T_Token backup;

	T_ST_Vars *actualST, *newST;

	while(1)
	{
		Instr = &(paska[i++]);         //nacitame z pasky a posouvame se po ni
		
		switch (Instr->operator) {
			case  FUNCTION:  //definice funkce.. tu jen preskocime

							do {
								Instr = &(paska[i++]);
							}
							while (Instr->operator != FUNCTION_BLOCK_END); printf("%d\n", Instr->operator );
							break;

			case FUNCTION_BLOCK_START:
					//skok pri volani funkce
					break;
					

			case  RETURN: /*
				printf("neco\n");
				garbage_add((pop_top(tableStack)),&garbage_default_erase); //odebere z vrcholu zasobniku jednu tabulku funkci
				StackHelpItem = *(pop_top(returnStack)); //odebere hodnotu z vrcholu zasobniku
				i= (((TRetValue *)(StackHelpItem).data)->adress);       //nastavi vykonavani nasledujici instrukce
				(((TRetValue *)(StackHelpItem).data)->returnvalue) = &Instr->operand1;
							//zahodÄ‚Â­ aktuÄ‚Ë‡lnÄ‚Â­ tabulku promĂ„â€şnnÄ‚Ëťch
							//nÄ‚Ë‡vrat na mÄ‚Â­sto, kde jsem byl volanej 
							*/
				break;
			case S_PLUS:
			case S_MINUS:
			case S_MUL:
			case S_DIV:
				if(Instr->operand1.type == S_ID) { //operand1 je to promenna
					AuxSTVar = findVarST(Instr->operand1.value, actualST);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					op1_typ = AuxSTVar->data->type;
					if(op1_typ != S_INT && op1_typ != S_DOUB) { fprintf(stderr, "Incompatible types in expression\n" ); return SEM_TYPE_ERROR; }  //typ promenne  neni int ani double
					else op1 = AuxSTVar->data->value;
				}
				else {     //operand1 neni promenna
					op1_typ = Instr->operand1.type;
					if(op1_typ != S_INT && op1_typ != S_DOUB) { fprintf(stderr, "Incompatible types in expression\n" ); return SEM_TYPE_ERROR; }   //typ promenne  neni int ani double
					else op1 = Instr->operand1.value;

				}

				if(Instr->operand2.type == S_ID) { //operand2 je to promenna
					AuxSTVar = findVarST(Instr->operand2.value, actualST);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					op2_typ = AuxSTVar->data->type;
					if(op2_typ != S_INT && op2_typ != S_DOUB) { fprintf(stderr, "Incompatible types in expression\n" ); return SEM_TYPE_ERROR; }  //typ promenne  neni int ani double
					else op2 = AuxSTVar->data->value;
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
				  res->data->value = (double*)malloc(sizeof(double));

				if(Instr->operator == S_MUL) *((double*)(res->data)->value) = *((double*)(op1)) * *((double*)(op2));
				else if (Instr->operator == S_DIV) *((double*)(res->data)->value) = *((double*)(op1)) / *((double*)(op2));
				else if(Instr->operator == S_PLUS) *((double*)(res->data)->value) = *((double*)(op1)) + *((double*)(op2));
				else *((double*)(res->data)->value) = *((double*)(op1)) - *((double*)(op2));

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
				op1_typ = AuxSTVar->data->type;
				if(op1_typ != S_STR) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
				else op1 = AuxSTVar->data->value;
			  }
			  else {     //operand1 neni promenna
				op1_typ = Instr->operand1.type;
				if(op1_typ == S_DOUB);  //typ promenne  neni istr musime provest konverzi
				else if (op1_typ == S_INT);
				else if  (op1_typ == S_BOOL);
				else if (op1_typ == S_NULL);
				else op1 = Instr->operand1.value;
			  }

			  if(Instr->operand2.type == S_ID) { //operand2 je to promenna
				AuxSTVar = findVarST(Instr->operand2.value, actualST);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
				op2_typ = AuxSTVar->data->type;
				if(op2_typ != S_STR) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
				else op2 = AuxSTVar->data->value;
			  }
			  else {      //operand2 neni promenna
				op2_typ = Instr->operand2.type;
				if(op2_typ != S_STR) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
				else op2 = Instr->operand1.value;
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
				  op1_typ = AuxSTVar->data->type;
				  op1 = AuxSTVar->data->value;
				}
				else {     //operand1 neni promenna
				  op1_typ = Instr->operand1.type;
				  op1 = Instr->operand1.value;
				}

				res = findVarST(Instr->vysledek.value, actualST);
				if (res->data->value != NULL) free(res->data->value);

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
					  res->data->type = S_INT;
					  break;

				  case S_DOUB:
					  res->data->value = malloc(sizeof(double));
					  *((double*)(res->data)->value) = *(double*)op1;
					  res->data->type = S_DOUB;
					  break;
				}
				break;

			case S_FUNC:   
					if((param = (Tparam*)malloc(sizeof(Tparam))) == NULL) return ERROR_INTER;

					param->funcName = mystrdup(Instr->operand1.value);
					funkce = findFunctionST(param->funcName, functionTable);
					param = copyTable(funkce->data->symbolTable);
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


					push(paramstack, param);

					break;
					

			case S_LST:
			case S_GRT:
			case S_LEQ:
			case S_GEQ:
			case S_EQ:
			case S_NEQ:
				if(Instr->operand1.type == S_ID) { //operand1 je promenna
					AuxSTVar = findVarST(Instr->operand1.value, actualST);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					op1_typ = (*AuxSTVar).data->type;
					op1 = AuxSTVar->data->value;
				}
				else {     //operand1 neni promenna
					op1_typ = Instr->operand1.type;
					op1 = Instr->operand1.value;
				}

				if(Instr->operand2.type == S_ID) { //operand2 je promenna
					AuxSTVar = findVarST(Instr->operand2.value, actualST);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
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
				  if(Instr->operator == S_EQ || Instr->operator == S_NEQ)
						res->data->value = false;
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
				funcName = (Tparam *)(StackHelpItem->data)->funcName;

				newST = (Tparam *)(StackHelpItem->data)->symbolTable;
				// alokujeme novy prvek pro stack tabulku symbolu
				


				

			 	//zpracujeme interni funkce
			 	AuxSTVar = findVarST((Tparam *)(StackHelpItem->data)->returnvalue, actualST); 

				if(strcmp(funcName, "get_string") == 0) {
					AuxSTVar->data->value = get_string();//
					AuxSTVar->data->type = S_STR;

					StackHelpItem = pop_top(paramStack);
					break;
				}

				if(strcmp(funcName, "put_string") == 0) {
					AuxSTVar->data->value = malloc(sizeof(int));
				 	AuxSTVar->data->type = S_NULL;

				 	*(int*)(AuxSTVar->data)->value = 0;

					StackHelpItem = pop_top(paramStack);
					break;
				}

				if(strcmp(funcName, "strlen") == 0) {
				 	
				 	AuxSTVar->data->value = malloc(sizeof(int));
				 	AuxSTVar->data->type = S_INT;

				 	res = findVarST(variables[0], newST);
				 	*(int*)(AuxSTVar->data)->value = strlen(res->value);
				
					 StackHelpItem = pop_top(paramStack);
				  
				  	break;
				}

			 
				if(strcmp(funcName,  "find_string") == 0) {
					AuxSTVar->data->value = malloc(sizeof(int));
				 	AuxSTVar->data->type = S_INT;

				 	res = findVarST(variables[0], newST);
				 	res1 = findVarST(variables[1], newST);
				 	//res2 = findVarST(variables[0], newST);

				 	*(int*)(AuxSTVar->data)->value = find_string(res->value ,res1->value);
				
					 StackHelpItem = pop_top(paramStack);
				  
				  	break;
				
				}

				
				if(strcmp(funcName, "sort_string") == 0) {
					AuxSTVar->data->type = S_STR;
					res = findVarST(variables[0], newST);

					AuxSTVar->data->value = sort_string(res->value);
					StackHelpItem = pop_top(paramStack);
					break;
				}

				StackHelpItem = pop_top(paramStack);

				CallStack = (TCallStack*)malloc(sizeof(TCallStack));
				CallStack->symbolTable = actualST; 
				CallStack->adress = i;			

				if(push(callStack, CallStack)==INTERNAL_ERROR) {fprintf(stderr, "Could not push actual symbolTableon stack\n" ); return ERROR_INTER;}
				//zmenime aktualni tabulku symbolu
				actualST = (Tparam *)(StackHelpItem->data)->symbolTable;

				//a zmenime i i
				i = Instr->operand1.type;

				break;


			case STORE_PARAM:

				//vestavene funkce je treba zpracovat speicfikovanym zpusobem
				//zjistime si jmeno funkce
				param = ((Tparam *)((paramstack)->top)->data);
				funcName = ((Tparam *)((paramstack)->top)->data)->funcName;

				//pokud je to putstring tak se vypisujou parametry

				if(strcmp(funcName, "put_string")){
					switch(Instr->operand1.type) {
							case S_INT:
								printf("%d", *((int *)(Instr->operand1).value)); break;
							case S_DOUB:
								printf("%f", *((double *)(Instr->operand1).value)); break;
							case S_STR:
								printf("%s", (char *)(Instr->operand1).value); break;
							case S_BOOL:
								printf("%d", (bool)(Instr->operand1).value); break;
							case S_NULL:
								printf("null\n"); break;
							case S_ID:
								AuxSTVar = findVarST(Instr->operand1.value, actualST);
								op1 = AuxSTVar->data->value;

								switch(AuxSTVar->data->type) {
									case S_INT:
										printf("%d", *(int*)op1); break;
									case S_DOUB:
										printf("%f", *(double *)op1); break;
									case S_STR:
										printf("%s", (char *)op1); break;
									case S_BOOL:
										printf("%d", (bool)op1); break;
									case S_NULL:
										printf("null\n"); break;
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
						AuxSTVar = findVarST(variables[BIfPointer], param->symbolTable); 
						param->BIfPointer++;
						//je to BI funkce promenou kam to ulozit zjistime z pole retezcu
					}
					else{
						//je to normal funkce jmeno kam ulozit je ve vysledku
						AuxSTVar = findVarST(Instr->vysledek.value, param->symbolTable); 
					}

					//ted zkopirujem do nalezene promene to hodnoty
					op1 = Instr->operand1.value;
					switch(Instr->operand1.type){
				  	case S_STR:
					  	AuxSTVar->data->value = mystrdup((char*)op1);
					  	AuxSTVar->data->type = S_STR;
					  	break;

				  	case S_BOOL:
				  	case S_NULL:
				  	case S_INT:
					 	 AuxSTVar->data->value = malloc(sizeof(int));
					  	*((int*)(AuxSTVar->data->)->value) = *(int*)op1;
					  	AuxSTVar->data->type = S_INT;
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
					  	backup.type = S_INT;
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
				return OK;
		}
	}

	return 0;
}
