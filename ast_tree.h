#ifndef AST_TREE
#define AST_TREE


typedef struct Tleaf
{
  T_Token *op1;    //operand1
  T_Token *op;     //operator
  T_Token *op2;    //operand2
  struct Tleaf *Lleave;
  struct Tleaf *Rleave;
} Tleaf;

typedef struct strom
{
  Tleaf root;
}Ttree;


Tleaf* makeTree(T_Token *, Tleaf*  , Tleaf* );
Tleaf* makeLeaf(T_Token *, T_Token *, T_Token*);
bool freeAss( Tleaf* );

#endif