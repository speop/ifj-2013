//Implementace interpretu imperativního jazyka IFJ12.
//xradva00, Radvansky Matej
//xrajca00, Rajca Tomas

#ifndef VNITRNI_KOD_H
#define VNITRNI_KOD_H

#include "list.h"

typedef struct TAC{ // 4 adresy asi tezko
  T_Token_struct *operator; /*Operator urceny podle tabulky terminalu.*/
  T_Token_struct *operand1;
  T_Token_struct *operand2;
  int jmp;
}TAC;

char *gen_temp_var();
void inorder(struct node*root,T_bt_root *ts);
int vnitrni(tList *L,T_bt_root *ts);

#endif
