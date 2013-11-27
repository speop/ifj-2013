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


#define true 1
#define false 0

extern TAC *paska;                  //z vnitrni.c
extern T_ST_Vars *symbolTable;      //obe tabulky z parser.c
extern T_ST_Funcs *functionTable;

int interpret()
{
		typedef struct TRetValue{
  int adress;         //sem se vraci po vykonani funkce
  T_Token *returnvalue;    //sem se uklada vysledek funkce
}TRetValue;

typedef struct Tparam{
  tStack paramstack;
  int free;
  T_ST_FuncsItem funkce;
}Tparam;



	TAC *Instr;          //Instrukce
	int i = 0;          //index/pozice na pasce
	void *op1, *op2, *pomptr;
	int op1_typ, op2_typ,len;   //typy operandu
	T_ST_Vars *aux, *res;        //pomocny uzel a uzel pro vysledek
	//T_ST_Funcs *funkce;        //pomocna promenna na uchovani dat o funkci
	TRetValue *RetValue;                //prvek zasobniku pro navraty
	tStack *returnStack = SInit();      //zasobnik navratovach hodnot
	tStack *tableStack = SInit();         //zasobnik tabulek symbolu
	tStack *funcStack = SInit();         //zasobnik s funkcemi a parametry pro volani
	tStackItem StackHelpItem;             //pomocny prvek pro zasobniky
	Tparam param;           //struktura pro parametry
	T_Token pom1, pom2, pom3;   //pomocnĂ© tokeny



	while(1)
	{
		Instr = &(paska[i++]);         //nacitame z pasky a posouvame se po ni
		switch (Instr->operator) {
			case  FUNCTION: //DefineFunction(Instr->operand1);
							break;
			case  RETURN:
				garbage_add((pop_top(tableStack)),&garbage_default_erase); //odebere z vrcholu zasobniku jednu tabulku funkci
				StackHelpItem = *(pop_top(returnStack)); //odebere hodnotu z vrcholu zasobniku
				i= (((TRetValue *)(StackHelpItem).data)->adress);       //nastavi vykonavani nasledujici instrukce
				(((TRetValue *)(StackHelpItem).data)->returnvalue) = &Instr->operand1;
							//zahodĂ­ aktuĂˇlnĂ­ tabulku promÄ›nnĂ˝ch
							//nĂˇvrat na mĂ­sto, kde jsem byl volanej
			break;
			case S_PLUS:
			case S_MINUS:
			case S_MUL:
			case S_DIV:
				if(Instr->operand1.type == S_ID) { //operand1 je to promenna
					aux = findVarST(Instr->operand1.value, symbolTable);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					op1_typ = aux->data->type;
					if(op1_typ != S_INT && op1_typ != S_DOUB) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
					else op1 = aux->data->value;
				}
				else {     //operand1 neni promenna
					op1_typ = Instr->operand1.type;
					if(op1_typ != S_INT && op1_typ != S_DOUB) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
					else op1 = Instr->operand1.value;
				}

				if(Instr->operand2.type == S_ID) { //operand2 je to promenna
					aux = findVarST(Instr->operand2.value, symbolTable);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					op2_typ = aux->data->type;
					if(op2_typ != S_INT && op2_typ != S_DOUB) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
					else op2 = aux->data->value;
				}
				else {      //operand2 neni promenna
					op2_typ = Instr->operand2.type;
					if(op2_typ != S_INT && op2_typ != S_DOUB) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
					else op2 = Instr->operand1.value;
				}

				res = findVarST(Instr->vysledek.value, symbolTable);
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
				aux = findVarST(Instr->operand1.value, symbolTable);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
				op1_typ = aux->data->type;
				if(op1_typ != S_STR) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
				else op1 = aux->data->value;
			  }
			  else {     //operand1 neni promenna
				op1_typ = Instr->operand1.type;
				if(op1_typ != S_STR) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
				else op1 = Instr->operand1.value;
			  }

			  if(Instr->operand2.type == S_ID) { //operand2 je to promenna
				aux = findVarST(Instr->operand2.value, symbolTable);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
				op2_typ = aux->data->type;
				if(op2_typ != S_STR) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
				else op2 = aux->data->value;
			  }
			  else {      //operand2 neni promenna
				op2_typ = Instr->operand2.type;
				if(op2_typ != S_STR) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
				else op2 = Instr->operand1.value;
			  }

			  res = findVarST(Instr->vysledek.value, symbolTable);
			  if (res->data->value != NULL) free(res->data->value);

			  len = strlen((char*)op1) + strlen((char*)op2) +1; // vysledna delka retezce
			  res->data->value = (char*)malloc(sizeof(char)*len);

			  strcpy ((char*)(res->data)->value,(char*)op1);
			  strcat ((char*)(res->data)->value,(char*)op2);
			  res->data->type = S_STR;

			  break;


			case S_IS:
				if(Instr->operand1.type == S_ID) { //operand1 je to promenna
				  aux = findVarST(Instr->operand1.value, symbolTable);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
				  op1_typ = aux->data->type;
				  op1 = aux->data->value;
				}
				else {     //operand1 neni promenna
				  op1_typ = Instr->operand1.type;
				  op1 = Instr->operand1.value;
				}

				res = findVarST(Instr->vysledek.value, symbolTable);
				if (res->data->value != NULL) free(res->data->value);

				//ulozeni hodnoty
				switch(op1_typ){
				  case S_STR:
					  //res->data->value = mystrdup((char*)op1);
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

			case S_FUNC:    //volani funkce get_string
				if(strcmp(Instr->operand1.value, "get_string")==0); {
					 param.free = 0;           //param je statická proměnná
					 param.funkce.name="get_string";
					 param.funkce.paramCount=0;

					 if (push(funcStack, &param)==INTERNAL_ERROR) //prida na vrchol zasobniku pole s parametry
						 return ERROR_INTER;

					  funcStack->top->data= malloc(sizeof(Tparam));
					  if(funcStack->top->data==NULL)return ERROR_INTER;
					  *(Tparam*)funcStack->top->data = param;
					 garbage_add(funcStack->top->data,&garbage_default_erase);

					 break;
					}

			   if(strcmp(Instr->operand1.value, "put_string")==0) {
					 param.free = -1;               //oznaceni teto funkce (pak se nemusi porovnavat nazvy)
					 param.funkce.paramCount=0;
					 param.funkce.name="put_string";

					 if (push(funcStack, &param)==INTERNAL_ERROR) //prida na vrchol zasobniku pole s parametry
						return ERROR_INTER;

					 funcStack->top->data= malloc(sizeof(Tparam));
					 if(funcStack->top->data==NULL)return ERROR_INTER;
					 *(Tparam*)funcStack->top->data = param;
					 garbage_add(funcStack->top->data,&garbage_default_erase);

					 break;
					}

				if(strcmp(Instr->operand1.value, "strlen")==0) {
					 param.free = 1;
					 param.funkce.paramCount=1;
					 param.funkce.name="strlen";
					 if (push(funcStack, &param)==INTERNAL_ERROR) //prida na vrchol zasobniku pole s parametry
						return ERROR_INTER;

					 funcStack->top->data= malloc(sizeof(Tparam));
					 if(funcStack->top->data==NULL)return ERROR_INTER;
					 *(Tparam*)funcStack->top->data = param;
					 garbage_add(funcStack->top->data,&garbage_default_erase);

					 break;
					}

				if(strcmp(Instr->operand1.value, "get_substring")==0) {
					 param.free = 3;
					 param.funkce.name="get_substring";
					 param.funkce.paramCount=3;
					 if (push(funcStack, &param)==INTERNAL_ERROR) //prida na vrchol zasobniku pole s parametry
						return ERROR_INTER;

					 funcStack->top->data= malloc(sizeof(Tparam));
					 if(funcStack->top->data==NULL)return ERROR_INTER;
					 *(Tparam*)funcStack->top->data = param;
					 garbage_add(funcStack->top->data,&garbage_default_erase);

					 break;
					}

				if(strcmp(Instr->operand1.value, "find_string")==0) {
					 param.free = 2;
					 param.funkce.name="find_string";
					 param.funkce.paramCount=2;

					 if (push(funcStack, &param)==INTERNAL_ERROR) //prida na vrchol zasobniku pole s parametry
						return ERROR_INTER;

					 funcStack->top->data= malloc(sizeof(Tparam));
					 if(funcStack->top->data==NULL)return ERROR_INTER;
					 *(Tparam*)funcStack->top->data = param;
					 garbage_add(funcStack->top->data,&garbage_default_erase);

					 break;
					}

			   if(strcmp(Instr->operand1.value, "sort_string")==0) {
					 param.free = 1;
					 param.funkce.name="sort_string";
					 param.funkce.paramCount=1;

					 if (push(funcStack, &param)==INTERNAL_ERROR) //prida na vrchol zasobniku pole s parametry
						return ERROR_INTER;

					 funcStack->top->data= malloc(sizeof(Tparam));
					 if(funcStack->top->data==NULL)return ERROR_INTER;
					 *(Tparam*)funcStack->top->data = param;
					 garbage_add(funcStack->top->data,&garbage_default_erase);

					 break;
					}

				 param.free = (((Tparam *)((Instr)->operand1).value)->funkce).paramCount;
				 param.funkce = *(T_ST_FuncsItem *)(Instr->operand1.value);
				 param.funkce.paramCount = (((Tparam *)((Instr)->operand1).value)->funkce).paramCount;

				if (push(funcStack, &param)==INTERNAL_ERROR)
				   return ERROR_INTER;
				 funcStack->top->data= malloc(sizeof(Tparam));
				 if(funcStack->top->data==NULL)return ERROR_INTER;

				 *(Tparam*)funcStack->top->data = param;
				 garbage_add(funcStack->top->data,&garbage_default_erase);


				 *RetValue->returnvalue = Instr->vysledek;
				 if(push(returnStack, RetValue)==INTERNAL_ERROR)
				   return ERROR_INTER;
				 break;

			case S_LST:
			case S_GRT:
			case S_LEQ:
			case S_GEQ:
			case S_EQ:
			case S_NEQ:
				if(Instr->operand1.type == S_ID) { //operand1 je promenna
					aux = findVarST(Instr->operand1.value, symbolTable);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					op1_typ = (*aux).data->type;
					op1 = aux->data->value;
				}
				else {     //operand1 neni promenna
					op1_typ = Instr->operand1.type;
					op1 = Instr->operand1.value;
				}

				if(Instr->operand2.type == S_ID) { //operand2 je promenna
					aux = findVarST(Instr->operand2.value, symbolTable);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					op2_typ = (*aux).data->type;
					op2 = aux->data->value;
				}
				else {      //operand2 neni promenna
					op2_typ = Instr->operand2.type;
					op2 = Instr->operand1.value;
				}

				res = findVarST(Instr->vysledek.value, symbolTable);
				if (res->data->value != NULL) free(res->data->value);
				res->data->type = S_BOOL;
				res->data->value = (int*)malloc(sizeof(int));

				//jestli se typy nerovnaji, je to false nebo chyba
				if(op1_typ != op2_typ) {
				  if(Instr->operator == S_EQ || Instr->operator == S_NEQ)
					res->data->value = false;
				  else
					return SEM_TYPE_ERROR;
				}
				//typy se rovnaji
				else {
				  switch(Instr->operator) {
					  case S_LST:
						*((int*)(res->data)->value) = (op1 < op2);
						break;
					  case S_GRT:
						*((int*)(res->data)->value) = (op1 > op2);
						break;
					  case S_LEQ:
						*((int*)(res->data)->value) = (op1 <= op2);
						break;
					  case S_GEQ:
						*((int*)(res->data)->value) = (op1 >= op2);
						break;
					  case S_EQ:
						*((int*)(res->data)->value) = (op1 == op2);
						break;
					  case S_NEQ:
						*((int*)(res->data)->value) = (op1 != op2);
						break;
				  }
				}
				break;

			case CALL:  //zjisti, kterou funkci volam a zkontroluj jeji parametry

				if(strcmp(((Tparam *)((funcStack)->top)->data)->funkce.name, "get_string") == 0) {
					(char *)((TRetValue *)(returnStack->top->data)->RetValue->returnvalue).value = get_string();	//
					Instr->vysledek.type = S_STR;
					StackHelpItem = *(pop_top(&((Tparam *)((funcStack)->top)->data)->paramstack));
					break;
				}

				if(strcmp(((Tparam *)((funcStack)->top)->data)->funkce.name, "put_string") == 0) {
					StackHelpItem = *(pop_top(&((Tparam *)((funcStack)->top)->data)->paramstack));
					break;
				}

				if(strcmp(((Tparam *)((funcStack)->top)->data)->funkce.name, "strlen") == 0) {
				  if(((Tparam *)((funcStack)->top)->data)->free == 0) {
					 StackHelpItem = *((Tparam *)((funcStack)->top)->data)->paramstack.top;
					 if((int *)(((int *)(Tparam *)(StackHelpItem).data)->paramstack->top->data)->type == S_STR)
					   *(int *)Instr->vysledek.value=strlen(((Tparam *)StackHelpItem.data)->paramstack->top->data->value);
					 else
					   return SEM_OTHER_ERROR;
				   }
				  StackHelpItem = *(pop_top(((Tparam *)((funcStack)->top)->data)->paramstack));
				  break;
				}

			 if(strcmp(funcStack->top->data->funkce->name,  "find_string") == 0) {
				 if(funcStack->top->data->free == 0) {
					 StackHelpItem = funcStack->top->data->paramstack->top;
					 pom1 = (pop_back(&StackHelpItem)->data);
					 pom2 = (pop_back(&StackHelpItem)->data);
					 pom3 = (pop_back(&StackHelpItem)->data);
					 if(pom1.type== S_STR && pom2.type==S_INT && pom3.type==S_INT)
						 Instr->vysledek.value=strlen(StackHelpItem.data->paramstack->top->data->value);
					 else return SEM_OTHER_ERROR;        //ostatni
					}
				 else return SEM_MISSING_PARAMETER;      //chybi parametr
				 StackHelpItem = pop_top(funcStack->top->data->paramstack);
				 break;
				}

				if(funcStack->top->data->funkce->name == "sort_string") {
				 if(funcStack->top->data->free == 0) {
					 StackHelpItem = funcStack->top->data->paramstack->top;
					 pom1 = (pop_back(&StackHelpItem)->data);
					 if(pom1.type== S_STR)
						 Instr->vysledek.value=sort_string(StackHelpItem.data->paramstack.top->data->value);
					 else return SEM_OTHER_ERROR;        //ostatni
					}
				 else return SEM_MISSING_PARAMETER;      //chybi parametr
				 StackHelpItem = pop_top(funcStack->top->data->paramstack);
				 break;
				}

					//kontrola poctu parametru
				 if(funcStack->top->data->free != 0)
					return SEM_MISSING_PARAMETER;

				 //kopiruji tabulku symbolu
			   if(push(tableStack, copyTable(tableStack->top))==INTERNAL_ERROR) //prida na vrchol zasobniku novou tabulku
					return ERROR_INTER;

					//prepisu v ni hodnoty parametru
				for(int j=((Tparam *)funcStack->top->data)->funkce.paramCount; j>0; j--) {
					pomptr = pop_top(&((Tparam *)funcStack->top->data)->paramstack);
					aux = findVarST( ((char *)pom1.value), ((T_ST_Vars *)tableStack->top->data));
					//aux->data->value = ((T_Token *)pomptr).value;
				}
				((TRetValue *)returnStack->top->data)->adress = i;
				i = (*(int *)(((Instr)->operand1).value)-1);   //operand1 je dalsi instrukce, ale na zacatku cyklu se i inkrementuje
															   //proto -1

				StackHelpItem = *(pop_top(&((Tparam *)((funcStack)->top)->data)->paramstack));
				break;

			case STORE_PARAM:
				switch (((Tparam *)((funcStack)->top)->data)->free) {
					case -1:               //tisknou se parametry funkce put_string()
						if(Instr->operand1.type==S_STR) printf("%s", (char *)(Instr->operand1).value);
						else return SEM_OTHER_ERROR;
						break;
					case 0:
						break;      //pole s parametry je plne, prebytecne se zahazuji
					default:
						if(push(&(((Tparam *)((funcStack)->top)->data)->paramstack), &Instr->operand1)==INTERNAL_ERROR)
							return INTERNAL_ERROR;
						((Tparam *)((funcStack)->top)->data)->free--;       //snizeni hodnoty pocitadla volnych mist pro parametry
						break;
				}
				break;

			case JMP:
				i = Instr->operand1.type - 1;      //zmeni se index na pasce
				break;

			case JMP_NOT:
				//if(!boolval(Instr->operand1.value))
				  i = Instr->operand1.type - 1;
				break;
		}
	}

	return 0;
}
