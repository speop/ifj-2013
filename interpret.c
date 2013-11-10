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

varSTInit(varsTable);       //inicializace tabulky symbolu

/* Funkce arithmetic provadi aritmeticke operace nad op1, op2 a vysledkem (+,-,*,/)

 */
void arithmetic(double op1, double op2, T_ST_Vars vysl, TERMINALY operace, TERMINALY vysl_typ)
{
    switch(operace) {
        case S_PLUS:
            if(vysl_typ == S_INT) vysl->data->value = (int)(op1 + op2);
            else if(vysl_typ == S_DOUB) vysl->data->value = (double)(op1 + op2);
            else return SEM_TYPE_ERROR;
            break;
        case S_MINUS:
            vysl->data->value = op1 - op2;
            break;
        case S_MUL:
            vysl->data->value = op1 * op2;
            break;
        case S_DIV:
            vysl->data->value = op1 / op2;
            break;
    }
}

int main()
{
    TAC Instr;          //Instrukce
    int i = 0;          //index/pozice na pasce
    double op1, op2;      //tohle se bude posilat do arithmetic()
    int op1_typ, op2_typ;   //typy operandu
    T_ST_Vars *aux;             //pomocny uzel tabulky symbolu
    //místo čísel používat enumy!!!!
    //kontrolovat jenom typovost proměnných!!
    //Instr.type je bud promenna nebo prima hodnota

    while(1)
    {   
        Instr = paska[i++];   //jenom zhruba
        switch (Instr->operaror){
            case  FUNCTION: DefineFunction(Instr->operand1);
                            break;
            case  RETURN:
                            //zahodí aktuální tabulku proměnných
                            //návrat na místo, kde jsem byl volanej  

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //plus je skoro hotovy, jen je treba zapsat vysledek do tabulky symbolu
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            case S_PLUS:
                if(Instr->operand1.type == S_ID) { //je to promenna
                    aux = findVarST(Instr->operand1->value, varsTable);    //vyhledam ji v tabulce symbolu a ulozim si odkaz
                    op1 = (double)aux->data->value;                //data operandu
                    if(aux->data.type != S_INT || aux->data.type != S_DOUB) return SEM_TYPE_ERROR;  //kdyz promenna pro scitani neni int ani double
                    op1_typ = aux->data.type;         //datovy typ operandu
                }
                else {      //op1 neni promenna
                    if(Instr->operand1.type == S_INT) {     //op1 je celociselna hodota
                        op1 = (int)Instr->operand1->value;
                        op1_typ = S_INT;
                    }
                    if(Instr->operand1.type == S_DOUB) {     //op1 je realna hodota
                        op1 = (double)Instr->operand1->value;
                        op1_typ = S_DOUB;
                    }
                }
                if(Instr->operand2.type == S_ID) { //je to promenna
                    aux = findVarST(Instr->operand2->value, varsTable);    //vyhledam v tabulce symbolu a ulozim si odkaz
                    op2 = (double)aux->data->value;                //data operandu
                    if(aux->data.type != S_INT || aux->data.type != S_DOUB) return SEM_TYPE_ERROR;
                    op2_typ = aux->data.type;         //datovy typ operandu
                }
                else {      //op2 neni promenna
                    if(Instr->operand2.type == S_INT) {     //op2 je celociselna hodota
                        op2 = (int)Instr->operand2->value;
                        op2_typ = S_INT;
                    }
                    if(Instr->operand2.type == S_DOUB) {     //op2 je realna hodota
                        op2 = (double)Instr->operand2->value;
                        op2_typ = S_DOUB;
                    }
                }
                //semanticka kontrola typovosti
                switch(op1_typ){
                  case S_INT: 
                    if(op2_typ == S_DOUB) Instr->vysledek.type = S_DOUB;
                    else Instr->vysledek.type = S_INT;      //predpokladam, ze nic jineho ne int a double tam nebude
                    break;
                  case S_DOUB: 
                    if(op2_typ == S_INT || op2_typ == S_DOUB) Instr->vysledek.type = S_DOUB;
                    break;
                }                    //vvv zde bude asi treba pridat pomoci addVarNodeToST()
                arithmetic(op1, op2, /*doplnit vysledek*/, Instr->operator, Instr->vysledek.type);
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
            case assigment:
                            assigment(FindVar(Instr->operand1), FindVar(Instr->vysledek));
                                break;                                
            case S_FUNC:       
                            &FindVar(Instr.vysledek) = funkce v FindVar(Instr.operand1)
            
            case S_LST:
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