#ifndef AST_TREE
#define AST_TREE

#include "types.h"
#include <stdbool.h>
#include <stdlib.h>


typedef struct Tleaf
{
  T_Token *op1;    //operand1
  T_Token *op;     //operator
  T_Token *op2;    //operand2
  int por;
} Tleaf;




//Tleaf* makeTree(T_Token *, Tleaf*  , Tleaf* );
Tleaf* makeLeaf(T_Token *, T_Token *, T_Token*);
bool freeAss( Tleaf* );
void printAss(Tleaf*, int );

#endif