//xbucht18, Buchta David
#include "ast_tree.h"


/*
Tleaf* makeTree(T_Token *E, Tleaf *Lop, Tleaf *Rop)
{
	Tleaf leaf;
	if((leaf = (Tleaf)malloc(sizeof(struct leaf)) == NULL)) return NULL;
	leaf.E = E;		//da token do listu
	leaf.Lchild = Lop;		//pod sebou nebude mit nic, zadne sve potomky
	leaf.Rchild = Rop;
	return *leaf;			//vrati ukazatel na vytvoreny list
}
*/
Tleaf* makeLeaf(T_Token* sign, T_Token* op1, T_Token* op2)
{
	
	Tleaf* newLeaf;

	newLeaf = (Tleaf*) malloc(sizeof(Tleaf));

	if (newLeaf == NULL ) return NULL;

	newLeaf->op1 = op1;
	newLeaf->op2 = op2;
	newLeaf->op = sign;

	return newLeaf;

}

bool freeAss( Tleaf* vetev){
	return true;
}