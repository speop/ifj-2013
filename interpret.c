#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "ial.h"
#include "types.h"
#include "vnitrni.h"
#include "parser.h"

#define true 1
#define false 0

extern TAC *paska;                  //z vnitrni.c
extern T_ST_Vars *symbolTable;      //obe tabulky z parser.c
extern T_ST_Funcs *functionTable;

int main()
{
    TAC Instr;          //Instrukce
    int i = 0;          //index/pozice na pasce
    void op1*, op2*;      //tohle se bude posilat do arithmetic()
    int op1_typ, op2_typ,len;   //typy operandu
    T_ST_Vars *aux, *res;        //pomocny uzel a uzel pro vysledek
    //místo čísel používat enumy!!!!
    //kontrolovat jenom typovost proměnných!!
    //Instr.type je bud promenna nebo prima hodnota

    while(1)
    {   
        Instr = paska[i++];         //nacitame z pasky a posouvame se po ni
        switch (Instr->operaror){
            case  FUNCTION: DefineFunction(Instr->operand1);
                            break;
            case  RETURN:
                            //zahodí aktuální tabulku proměnných
                            //návrat na místo, kde jsem byl volanej  

            case S_PLUS:
			case S_MINUS:
			case S_MUL:
            case S_DIV:
                if(Instr->operand1.type == S_ID) { //operand1 je to promenna
                    aux = findVarST(Instr->operand1->value, symbolTable);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					op1_typ = aux->data.type;
                    if(op1_typ != S_INT && op1_typ != S_DOUB) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
					else op1 = aux->data->value;
                }
                else {     //operand1 neni promenna
					op1_typ = Instr->operand1->type;
                    if(op1_typ != S_INT && op1_typ != S_DOUB) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
					else op1 = Instr->operand1->value;
                }
				
                if(Instr->operand2.type == S_ID) { //operand2 je to promenna
                    aux = findVarST(Instr->operand2->value, symbolTable);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					op2_typ = aux->data.type;
                    if(op2_typ != S_INT && op2_typ != S_DOUB) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
					else op2 = aux->data->value;
                }
                else {      //operand2 neni promenna
					op2_typ = Instr->operand2->type;
                    if(op2_typ != S_INT && op2_typ != S_DOUB) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
					else op2 = Instr->operand1->value;
                }
				
				res = findVarST(Instr->vysledek->value, symbolTable);
				if (res->data->value != NULL) free(res->data->value);
				
                //vypocet
                if(op1_typ == S_DOUB || op2_typ == S_DOUB || Instr->operaror == S_MUL || Instr->operaror == S_DIV) {
					res->data->value = (double*)malloc(sizeof(double));
					
					if(Instr->operaror == S_MUL) *((double*)(res->data)->value) = *((double*)(op1)) * *((double*)(op2))
					else if (Instr->operaror == S_MUL) *((double*)(res->data)->value) = *((double*)(op1)) / *((double*)(op2))
					else if(Instr->operaror == S_PLUS) *((double*)(res->data)->value) = *((double*)(op1)) + *((double*)(op2))
					else *(res->data->value) =  *((double*)(res->data)->value) = *((double*)(op1)) - *((double*)(op2))
					
					res->data->type = S_DOUB;
				}
                else {
					res->data->value = (int*)malloc(sizeof(int));
					if(Instr->operaror == S_PLUS)  *((int*)(res->data)->value) = *((int*)(op1)) + *((int*)(op2))
					else *(res->data->value) = *((int*)(res->data)->value) = *((int*)(op1)) - *((int*)(op2))
					res->data->type = S_INT;
				}

                break;
                
			case S_CONCATENATE:
			
				if(Instr->operand1.type == S_ID) { //operand1 je to promenna
                    aux = findVarST(Instr->operand1->value, symbolTable);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					op1_typ = aux->data.type;
                    if(op1_typ != S_STR) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
					else op1 = aux->data->value;
                }
                else {     //operand1 neni promenna
					op1_typ = Instr->operand1->type;
                    if(op1_typ != S_STR) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
					else op1 = Instr->operand1->value;
                }
				
                if(Instr->operand2.type == S_ID) { //operand2 je to promenna
                    aux = findVarST(Instr->operand2->value, symbolTable);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					op2_typ = aux->data.type;
                    if(op2_typ != S_STR) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
					else op2 = aux->data->value;
                }
                else {      //operand2 neni promenna
					op2_typ = Instr->operand2->type;
                   if(op2_typ != S_STR) return SEM_TYPE_ERROR;  //typ promenne  neni int ani double
					else op2 = Instr->operand1->value;
                }
				
				res = findVarST(Instr->vysledek->value, symbolTable);
				if (res->data->value != NULL) free(res->data->value);
				
				len = strlen((char*)op1) + strlen((char*)op2) +1; // vysledna delka retezce
				res->data->value = (char*)malloc(sizeof(char)*len);
				
				strcpy ((char*)(res->data)->value,(char*)op1);
				strcpy ((char*)(res->data)->value,(char*)op2);				
			
				break;
				
            case S_IS:	
			
				
                
                            assigment(FindVar(Instr->operand1), FindVar(Instr->vysledek));
                                break;                                
            case S_FUNC:       
                            &FindVar(Instr.vysledek) = funkce v FindVar(Instr.operand1) case S_LST:
                            někamtypu bool=less(FindVar(Instr->operand1), FindVar(Instr->operand2));
                                break;
             case S_GRT:
                            někamtypu bool=greater(FindVar(Instr->operand1), FindVar(Instr->operand2));
                                break;
            case S_LEQ:
                            někamtypu bool=leq(FindVar(Instr->operand1),FindVar(Instr->operand2), FindVar(Instr->vysledek));
                                break;
            case S_GEQ:
                            někam=geq(FindVar(Instr->operand1), FindVar(Instr->operand2), FindVar(Instr->vysledek) );
                                break;
            case S_EQ:
                            někam=eq(FindVar(Instr->operand1), FindVar(Instr->operand2), FindVar(Instr->vysledek));
                                break;
            case S_NEQ:
                            někam = neq(FindVar(Instr->operand1), FindVar(Instr->operand2), FindVar(Instr->vysledek));
                                break;
            case CALL: dalsi Instrukce = FindVar(Instr->operand1);
                                break;
            case STORE_PARAM:
                            vysledek=FindVar(Instr->operand1);
                                break;
            case JMP:
                                i = operand1.type;      //zmeni se index na pasce
                                break;
            case JMP_NOT:
                                if(!boolval(Instr->operand1->value))
                                    i = operand1.type;
                                break;
        }
    }

    return 0;
}