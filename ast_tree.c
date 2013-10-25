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
	if(vetev->op2 != NULL ) printAss( (T_Token*)(vetev->op2)->value ,uroven);
	//dostali jsme se dospod v pravo

	printf("\t\tToken %d, op1: %d \n",vetev->por, (T_Token*)(vetev->op1)->type );
	if(vetev->op != NULL) printf("\t\t\tsign: %d\n", (T_Token*)(vetev->op)->type);
	if(vetev->op2 != NULL) printf("\t\t\top2: %d\n", (T_Token*)(vetev->op2)->type);
	if((T_Token*)(vetev->op1)->type  == S_E ) printAss( (T_Token*)(vetev->op1)->value ,uroven );
	return;

}