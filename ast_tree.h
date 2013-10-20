#ifndef AST_TREE
#define AST_TREE

typedef struct strom
{
  Tleave root;
}Ttree;

typedef struct leave
{
  T_Token *op1;    //operand1
  T_Token *op;     //operator
  T_Token *op2;    //operand2
  struct leave *Lleave;
  struct leave *Rleave;
} *Tleave;

Tleaf* makeTree(T_Token *, Tleaf*  , Tleaf* );
Tleaf* makeLeaf(T_Token *, T_Token *, T_Token*)

#endif