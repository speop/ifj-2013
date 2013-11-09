#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "ial.h"
#include "types.h"
#include "vnitrni.h"

#define true 1
#define false 0

extern TAC *paska;      //globalni promenna z vnitrni.c

int main()
{
    TAC instruction;
    int i;
    //místo čísel používat enumy!!!!
    //kontrolovat jenom typovost proměnných!!
 
    while(1)
    {   instruction=paska[i];   //jenom zhruba
        switch (instruction->operaror){
            case  1: DefineFunction(instruction->operand1)
                            break;
            case  return:
                            //zahodí aktuální tabulku proměnných
                            //návrat na místo, kde jsem byl volanej        
            case plus: 
                            sum(FindVar(instruction->operand1), FindVar(instruction->operand2), FindVar(instruction->vysledek));
                                break;
            case mul:
                            mul(FindVar(instruction->operand1), FindVar(instruction->operand2), FindVar(instruction->vysledek));
                                break;
            case assigment:
                            assigment(FindVar(instruction->operand1), FindVar(instruction->vysledek));
                                break;
            case DIV:
                            div(FindVar(instruction->operand1), FindVar(instruction->operand2), FindVar(instruction->vysledek));
                                break;
            case minus:
                            minus(FindVar(instruction->operand1), FindVar(instruction->operand2), FindVar(instruction->vysledek));
                                break;
            case s_FUNC:       &FindVar(instruction.vysledek) = funkce v FindVar(instruction.operand1)
            
            case S_LST:
                            někamtypu bool=less(FindVar(instruction->operand1), FindVar(instruction->operand2));
                                break;
             case S_GRT:
                            někamtypu bool=greater(FindVar(instruction->operand1), FindVar(instruction->operand2));
                                break;
            case S_LEQ:
                            někamtypu bool=leq(FindVar(instruction->operand1),FindVar(instruction->operand2), FindVar(instruction->vysledek));
                                break;
            case S_GEQ:
                            někam=geq(FindVar(instruction->operand1), FindVar(instruction->operand2), FindVar(instruction->vysledek) );
                                break;
            case S_EQ:
                            někam=eq(FindVar(instruction->operand1), FindVar(instruction->operand2), FindVar(instruction->vysledek));
                                break;
            case S_NEQ:
                            někam = neq(FindVar(instruction->operand1), FindVar(instruction->operand2), FindVar(instruction->vysledek));
                                break;
            case CALL: dalsi instrukce = FindVar(instruction->operand1);
                                break;
            case STORE_PARAM:
                            vysledek=FindVar(instruction->operand1);
                                break;
            case JMP:
                                if(boolval(FindVar(instruction->operand1)))
                                    jmp();
                                break;
            case JMP_NOT:
                                if(boolval(instruction->operand1)!=true)
                                    jmp();
                                break;
        }
    }

    return 0;
}