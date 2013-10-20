//xhudzi01, Hudziec Tomas
#include "ast_tree.h"

Tleaf* makeTree(T_Token *E, Tleaf *Lop, Tleaf *Rop)
{
	Tleaf leaf;
	if((leaf = (Tleaf)malloc(sizeof(struct leaf)) == NULL)) return NULL;
	leaf.E = E;		//da token do listu
	leaf.Lchild = Lop;		//pod sebou nebude mit nic, zadne sve potomky
	leaf.Rchild = Rop;
	return *leaf;			//vrati ukazatel na vytvoreny list
}

Tleaf* makeLeaf(T_Token* sign, T_Token* op1, T_Token* op2)
{
	Tleaf Lop, Rop, Operation;		//pravy a levy operand, operace
	//alokace mista v pameti
	if((Lop = (Tleaf)malloc(sizeof(struct leaf)) == NULL)) return NULL;
	if((Rop = (Tleaf)malloc(sizeof(struct leaf)) == NULL)) return NULL;
	if((Operation = (Tleaf)malloc(sizeof(struct leaf)) == NULL)) return NULL;

	if((Lop = makeLeaf(op1, NULL, NULL) == NULL)) return NULL;
	if((Rop = makeLeaf(op2, NULL, NULL) == NULL)) return NULL;

	return makeLeaf(Operation, Lop, Rop);		//vrati ukazatel na list s operaci nebo NULL
}

bool freeAss( Tleaf* vetev){
	return true;
}