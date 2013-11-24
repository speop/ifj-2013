#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "ial.h"
#include "types.h"
#include "vnitrni.h"
#include "parser.h"
#include <stdbool.h>

//#define true 1
//#define false 0

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
    //místo čísel používat enumy!!!!
    //kontrolovat jenom typovost proměnných!!
    //Instr.type je bud promenna nebo prima hodnota

    while(1)
    {   
        Instr = &(paska[i++]);         //nacitame z pasky a posouvame se po ni
        switch (Instr->operator){
            case  FUNCTION: //DefineFunction(Instr->operand1);
                            break;
            case  RETURN:
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
        
            /*case S_FUNC:    //volani vestavene funkce
                           if(Instr->operand1.value=="get_string");
                                {*(Instr->vysledek) = get_string();
                                 break;
                                }
                           
                           if(Instr->operand1.value=="put_string")
                                {while(Instr=paska[i++]==STORE_PARAM)
                                    if(Instr->operand1.type==S_STR)
                                        printf("%s", Instr->operand1.value);
                                    else return 13; //asi ostatni chyby
                                 break;   
                                }
                                    
                            if(Instr->operand1.value=="strlen")
                                {
                                 if(Instr->operand1.value==S_STR)
                                     *(Instr->vysledek) = strlen(strval(Instr->operand1.value));
                                  else return 13;
                                  
                                 break;
                                }
                            
                            if(Instr->operand1.value=="get_substring")
                                {
                                  if(array=malloc(3 * sizeof(void))==NULL) return  INTERNAL_ERROR;
                                  garbage_add(array, &garbage_default_erase);
                                  if(Instr->operand1.value==S_STR)
                                    array[0]=Instr->operand1.value;
                                  else return 13;
                                  
                                  for(int j=1; j<3; j++)
                                     {
                                      if(Instr->operand1.value==S_INT)
                                        array[j]=paska[i+j]->operand1->value;      //předávání parametru, hodnota i zustava nezmenena
                                      else return 13;
                                     }
                                  paska[i]->vysledek = get_substring(strval(array[0]), intval(array[1]), intval(array[2]));
                                  break;
                                }
                            
                            if(Instr->operand1.value=="find_string")
                            {
                                if(Instr->operand1.type==S_STR && paska[i+1]->operand1->type==S_STR)
                                    &(Instr->vysledek.value) = find_string(Instr->operand1.value, paska[i+1]->operand1->value);
                                else return 13;
                                break;
                            }
                        
                            if(Instr->operand1.value=="sort_string")
                                {
                                    if(Instr->operand1.value==S_STR)
                                        &(Instr->vysledek.value) = sort_string(Instr->operand1.value);
                                    else return 13;
                                     break;
                                }
            */                
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
            case CALL: //dalsi Instrukce = FindVar(Instr->operand1);
                                break;
            case STORE_PARAM:
                //vysledek=FindVar(Instr->operand1);
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
