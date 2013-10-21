//xbucht18, Buchta David
#include "ast_tree.h"
#include <stdio.h>

extern int por;

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

void printAss(Tleaf* vetev )
{	

	if(vetev->op2 != NULL ) printAss( (T_Token*)(vetev->op1)->value );
	printf("\t\t\tToken %d, typ = %d \n",++por, (T_Token*)(vetev->op1)->type );
	if(vetev->op != NULL) printf("\t\t\tsign: %d\n", (T_Token*)(vetev->op)->type);
	if(vetev->op2 != NULL) printf("\t\t\top2: %d\n", (T_Token*)(vetev->op2)->type);
	if(vetev->op2 != NULL ) printAss( (T_Token*)(vetev->op2)->value );
	return;

}