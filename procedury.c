#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stdarg.h>
#include stack.h

#define true 1
#define false 0

typedef int bool;

T_ST_Vars *FindVar(char *name, tStack *tree_stack)
{
    tStackItem position = *tree_stack.top;
    T_ST_Vars *table = position.data;     //ukazatel na strom, který se bude vracet
    while((table = findVarST(name, table)) == NULL) {
      *position = *position.prev;
      table = position.data;
      if(stack.top == stack.bottom)
        return NULL;
    }
    return table;
}


void sum(T_Token operand1, T_Token operand2, T_Token vysledek)
{   
    switch(operand1.type){
      case S_INT: 
        if(operand2.type == S_DOUB) vysledek.type = S_DOUB;
        break;
      case S_DOUB: 
        if(operand2->type == S_INT || operand2->type == S_DOUB) vysledek.type = S_DOUB;
        break;
    }
    vysledek.value = operand1.value + operand2.value;
}

void minus(T_Token operand1, T_Token operand2, T_Token vysledek)
{   
    switch(operand1.type) {
      case S_INT:
        if(operand2.type == S_INT) vysledek.type = S_INT;
        else if(operand2.type == S_DOUB) vysledek.type = S_DOUB;
        break;
      case S_DOUB: if(operand2->type == S_INT || operand2->type == S_DOUB) vysledek.type = S_DOUB;
    }
    vysledek.value = operand1.value - operand2.value;
}

void mul(T_Token operand1, T_Token operand2, T_Token vysledek)
{   
    switch(operand1.type) {
      case S_INT: 
        if(operand2.type == S_INT) vysledek.type = S_INT;
        else if(operand2.type == S_DOUB) vysledek.type = S_DOUB;
        break;
      case S_DOUB:
        if(operand2->type == S_INT || operand2->type == S_DOUB) vysledek.type = S_DOUB;
        break;
    }
    vysledek.value = operand1.value * operand2.value;
}

void div(T_Token operand1, T_Token operand2, T_Token vysledek )
{
    if(operand2.value==0)
       // return NULL;
   if((operand2->type==S_INT||operand2->type==S_DOUB)&&(operand1.type==S_INT||operand2.type==S_DOUB))
        {   vysledek.type = S_DOUB;
            vysledek.value = operand1.value / operand2.value;
        }
}
