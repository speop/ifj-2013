#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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

int interpret()
{
    TAC *Instr;          //Instrukce
    int i = 0;          //index/pozice na pasce
    void *op1, *op2;
    int op1_typ, op2_typ,len;   //typy operandu
    T_ST_Vars *aux, *res;        //pomocny uzel a uzel pro vysledek
    T_ST_Funcs *funkce;        //pomocna promenna na uchovani dat o funkci
    TRetValue *RetValue;                //prvek zasobniku pro navraty
    tStack *returnStack = SInit();      //zasobnik navratovach hodnot
    tStack *tableStack = SInit();         //zasobnik tabulek symbolu
    tStack *funcStack = SInit();         //zasobnik s funkcemi a parametry pro volani
    tStack StackHelpItem;             //pomocny prvek pro zasobniky
    Tparam *param;           //struktura pro parametry
    T_Token pom1, pom2, pom3;   //pomocné tokeny
    //místo čísel používat enumy!!!!
    //kontrolovat jenom typovost proměnných!!
    //Instr.type je bud promenna nebo prima hodnota

    while(1)
    {   
        Instr = &(paska[i++]);         //nacitame z pasky a posouvame se po ni
        switch (Instr->operator) {
            case  FUNCTION: //DefineFunction(Instr->operand1);
                            break;
            case  RETURN:
                StackHelpItem = pop_top(tableStack);   //odebere z vrcholu zasobniku jednu tabulku funkci
                StackHelpItem = pop_top(returnStack); //odebere hodnotu z vrcholu zasobniku
                i= StackHelpItem->data->adress;       //nastavi vykonavani nasledujici instrukce
                *(StackHelpItem->data->returnadress) = Instr->operand1; 
                            //zahodí aktuální tabulku proměnných
                            //návrat na místo, kde jsem byl volanej  
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
                if(Instr->operand1.value=="get_string"); {
                     if(param=malloc(sizeof(Tparam))==NULL)
                        return INTERNAL_ERROR;
                     param->free = 0;
                     param->funkce->name="get_string";
                     param->paramCount=0;
                     garbage_add(array,&garbage_default_erase);
                     if (push(funcStack, param)==INTERNAL_ERROR) //prida na vrchol zasobniku pole s parametry
                         return INTERNAL_ERROR;
                     break;
                    }
                                                                      
               if(Instr->operand1.value=="put_string") {
                     if(param = malloc(sizeof(Tparam))==0)
                        return INTERNAL_ERROR;
                     param->free = -1;               //oznaceni teto funkce (pak se nemusi porovnavat nazvy)
                     param->funkce->paramCount=0;   
                     param->funkce->name="put_string";   
                     garbage_add(param,&garbage_default_erase);
                     if (push(funcStack, param)==INTERNAL_ERROR)
                        return INTERNAL_ERROR;
                     break;
                    }
                    
                if(Instr->operand1.value=="strlen") {
                       if(param = malloc(sizeof(Tparam))==0)
                        return INTERNAL_ERROR;
                     param->free = 1;
                     param->funkce->paramCount=1;
                     param->funkce->name="strlen";
                     garbage_add(param,&garbage_default_erase);
                     if (push(funcStack, param)==INTERNAL_ERROR)
                        return INTERNAL_ERROR;
                     break;
                    }
                
                if(Instr->operand1.value=="get_substring") {
                     if(param = malloc(sizeof(Tparam))==0)
                        return INTERNAL_ERROR;
                     param->free = 3;
                     param->funkce->name="get_substring";
                     param->funkce->paramCount=3;
                     garbage_add(param,&garbage_default_erase);
                     if (push(funcStack, param)==INTERNAL_ERROR)
                        return INTERNAL_ERROR;
                     break;
                    }
                                            
                if(Instr->operand1.value=="find_string") {
                     if(param = malloc(sizeof(Tparam))==0)
                        return INTERNAL_ERROR;
                     param->free = 2;
                     param->funkce->name="find_string";
                     param->funkce->paramCount=2;
                     garbage_add(param,&garbage_default_erase);
                     if (push(funcStack, param)==INTERNAL_ERROR)
                        return INTERNAL_ERROR;
                     break;
                    }
                   
               if(Instr->operand1.value=="sort_string") {
                    if(param = malloc(sizeof(Tparam))==0)
                        return INTERNAL_ERROR;
                     param->free = 1;
                     param->funkce->name="sort_string";
                     param->funkce->paramCount=1;
                     garbage_add(param,&garbage_default_erase);
                     if (push(funcStack, param)==INTERNAL_ERROR)
                        return ERROR_INTER;
                     break;
                    }

                 if(param = malloc(sizeof(Tparam))==0)
                    return INTERNAL_ERROR;
                 param->free = Instr;
                 param->funkce=Instr->operand1;
                 param->funkce->paramCount=Instr->operand1->paramCount;
                 garbage_add(param,&garbage_default_erase);
                 if (push(funcStack, param)==INTERNAL_ERROR)
                   return ERROR_INTER;
                 RetValue->returadress = Instr->vysledek
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
                        
                if(funcStack->top->data->funkce->name == "get_string") {
                    Instr->vysledek->value=get_string();
                    StackHelpItem = pop_top(funcStack->top->data->paramstack);
                    break;
                }
                    
                if(funcStack->top->data->funkce->name == "put_string") {
                    StackHelpItem = pop_top(funcStack->top->data->paramstack);
                    break;
                }
                    
                if(funcStack->top->data->funkce->name == "strlen") {
                  if(funcStack->top->data->free == 0) {
                     StackHelpItem = funcStack->top->data->paramstack->top;
                     if(StackHelpItem->data->paramstack->top->data->type == S_STR)
                       Instr->vysledek->value=strlen(StackHelpItem->data->paramstack->top->data->value);
                     else
                       return SEM_OTHER_ERROR;
                   }
                  StackHelpItem = pop_top(funcStack->top->data->paramstack);
                  break;
                }
                           
             if(funcStack->top->data->funkce->name == "find_string") {
                 if(funcStack->top->data->free == 0) {
                     StackHelpItem = funcStack->top->data->paramstack->top;
                     pom1 = (pop_back(&StackHelpItem)->data);
                     pom2 = (pop_back(&StackHelpItem)->data);
                     pom3 = (pop_back(&StackHelpItem)->data);
                     if(pom1->type== S_STR && pom2->type==S_INT && pom3->type==S_INT)
                         Instr->vysledek->value=strlen(StackHelpItem->data->paramstack->top->data->value);
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
                     if(pom1->type== S_STR)
                         Instr->vysledek->value=sort_string(StackHelpItem->data->paramstack->top->data->value);
                     else return SEM_OTHER_ERROR;        //ostatni
                    }
                 else return SEM_MISSING_PARAMETER;      //chybi parametr
                 StackHelpItem = pop_top(funcStack->top->data->paramstack);
                 break;
                }
               
                    //kontrola poctu parametru
                 if(funcStack->top->data->free != 0)
                    return SEM_MISSING_PARAMETER;
                
                 //kopíruji tabulku symbolu
               if(push(tableStack, copyTable(tableStack->top))==INTERNAL_ERROR) //prida na vrchol zasobniku novou tabulku
                    return ERROR_INTER;
                    
                    //pepisu v ni hodnoty parametru
                for(int j=funcStack->top->data->funkce->paramCount; j>0; j--) {
                    pom1 = pop_top(funcStack->top->data->paramStack); 
                    aux = findVarST( pom1->name, tableStack->top->data);
                    aux->data->value = pom1->value;
                }
                returnStack->top->data->adress = i;
                i = (Instr->operand1-1);   //operand1 je dalsi instrukce, ale na zacatku cyklu se i inkrementuje
                                                 //proto -1
                
                StackHelpItem = pop_top(funcStack->top->data->paramstack);
                break;

            case STORE_PARAM:
                switch (funcStack->top->data->free) {
                    case -1:               //tisknou se parametry funkce put_string()
                        if(Instr->operand1.type==S_STR) printf(Instr->operand1.value);
                        else return SEM_OTHER_ERROR;
                        break;
                    case 0: 
                        break;      //pole s parametry je plne, prebytecne se zahazuji
                    default:
                        if(push(funcStack->top->data->paramStack, Instr->operand1)==INTERNAL_ERROR)
                            return INTERNAL_ERROR;
                        funcStack->top->data->free--;       //snizeni hodnoty pocitadla volnych mist pro parametry
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
