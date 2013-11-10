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
    double op1, op2;      //tohle se bude posilat do arithmetic()
    int op1_typ, op2_typ;   //typy operandu
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
                if(Instr->operand1.type == S_ID) { //je to promenna
                    aux = findVarST(Instr->operand1->value, symbolTable);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					op1_typ = aux->data.type;
                    if(op1_typ == S_INT ) op1 = *((int)aux->data->value);
					else if (op1_typ == S_DOUB) op1 = *((double)aux->data->value);
					else return SEM_TYPE_ERROR;  //kdyz promenna pro scitani neni int ani double
                }
                else {      //operand1 neni promenna
                    //operand1 je realna hodota, i kdyz je to int, budu s tim pracovat jako s double
					op1_typ = Instr->operand1->type;
                    if(op1_typ == S_INT ) op1 = *((int)aux->data->value);
					else if (op1_typ == S_DOUB) op1 = *((double)aux->data->value);
					else return SEM_TYPE_ERROR;  //kdyz promenna pro scitani neni int ani double
                }
				
                if(Instr->operand2.type == S_ID) { //je to promenna
                    aux = findVarST(Instr->operand2->value, symbolTable);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
					op2_typ = aux->data.type;
                    if(op2_typ == S_INT ) op2 = *((int)aux->data->value);
					else if (op2_typ == S_DOUB) op2 = *((double)aux->data->value);
					else return SEM_TYPE_ERROR;  //kdyz promenna pro scitani neni int ani double
                }
                else {      //operand1 neni promenna
                    //operand1 je realna hodota, i kdyz je to int, budu s tim pracovat jako s double
					op2_typ = Instr->operand1->type;
                    if(op2_typ == S_INT ) op2 = *((int)aux->data->value);
					else if (op2_typ == S_DOUB) op2 = *((double)aux->data->value);
					else return SEM_TYPE_ERROR;  //kdyz promenna pro scitani neni int ani double
                }
				
				res = findVarST(Instr->vysledek->value, symbolTable);
				if (res->data->value != NULL) free(res->data->value);
				
                //vypocet
                if(op1_typ == S_DOUB || op2_typ == S_DOUB) {
					res->data->value = (double*)malloc(sizeof(double));
					*(res->data->value) = op1 + op2;
					res->data->type = S_DOUB;
				}
                else {
					res->data->value = (int*)malloc(sizeof(int));
					*(res->data->value) = op1 + op2;
					res->data->type = S_INT;
				}

                break;
            case S_MINUS:
                            minus(FindVar(Instr->operand1), FindVar(Instr->operand2), FindVar(Instr->vysledek));
                                break;            
            case S_MUL:
                            mul(FindVar(Instr->operand1), FindVar(Instr->operand2), FindVar(Instr->vysledek));
                                break;
            case S_DIV:
                            div(FindVar(Instr->operand1), FindVar(Instr->operand2), FindVar(Instr->vysledek));
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