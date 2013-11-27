//Implementace interpretu jazyka IFJ13.
//xbucht18, Buchta David
#include "ast_tree.h"
#include <stdio.h>

extern int por;

Tleaf* makeLeaf(T_Token* sign, T_Token* op1, T_Token* op2)
{
	
	Tleaf* newLeaf;

	newLeaf = (Tleaf*) malloc(sizeof(Tleaf));

	if (newLeaf == NULL ) return NULL;

	newLeaf->op1 = op1;
	newLeaf->op2 = op2;
	newLeaf->op = sign;
	newLeaf->por = ++por;
	

	return newLeaf;

}

bool freeAss( Tleaf* vetev){
	return true;
}

void printAss(Tleaf* vetev, int uroven )
{	
	printf("\turoven %d: \n",uroven++ );


}