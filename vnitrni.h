#ifndef VNITRNI_H
#define VNITRNI_H

#include "types.h"
#include "ast_tree.h"

typedef struct TAC{
  int operator; /*Operator urceny podle tabulky terminalu.*/
  T_Token operand1;
  T_Token operand2;
  T_Token vysledek;

  int jmp;
}TAC;

typedef enum {
	LEFT = 0,
	RIGHT = 1
}Smery;


typedef struct {
 	T_Token *rightVar, *leftVar;
 	int ret;
 } tExpr;


tExpr exprGC(Tleaf *, Smery);
int funGC(Tleaf *);
int generate(int, T_Token* , T_Token* , T_Token*);
int addJump();
int generateTempVar(T_Token *);

// seznam instrukci
/*

	S_FUNC - jedna se o volani funkce operand1 obsahuje token s funkci, vysledek kam se ulozi return, pokud funkce nedela return tak se vraci NULL
	STORE_PARAM - muze se nachazet jen po predchozi instrukci, znamena uloz hodnotu operandu1 do vysledku
	CALL - vem si int adresu z operandu 1 tam zacina funkce
	
*/

#endif

