//xhudzi01, Hudziec Tomas
#include "ast_tree.h"

Tleaf* makeLeaf(T_Token E, Tleaf Lop, Tleaf Rop)
{
	Tleaf leaf;
	if((leaf = malloc(sizeof(Tleaf)) == NULL)) return NULL;
	leaf.E = E;		//da token do listu
	leaf.Lchild = Lop;		//pod sebou nebude mit nic, zadne sve potomky
	leaf.Rchild = Rop;
	return *leaf;			//vrati ukazatel na vytvoreny list
}

Tleaf* makeTree(T_Token sign, T_Token op1, T_Token op2)
{
	Tleaf Lop, Rop, Operation;		//pravy a levy operand, operace
	//alokace mista v pameti
	if((Lop = malloc(sizeof(Tleaf)) == NULL)) return NULL;
	if((Rop = malloc(sizeof(Tleaf)) == NULL)) return NULL;
	if((Operation = malloc(sizeof(Tleaf)) == NULL)) return NULL;

	if((Lop = makeLeaf(op1, NULL, NULL) == NULL)) return NULL;
	if((Rop = makeLeaf(op2, NULL, NULL) == NULL)) return NULL;

	return makeLeaf(Operation, Lop, Rop);		//vrati ukazatel na list s operaci nebo NULL
}