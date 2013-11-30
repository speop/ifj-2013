//Implementace interpretu jazyka IFJ13.
//xbucht18, Buchta David

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

int generateCode();
tExpr exprGC(Tleaf *, Smery);
int funGC(Tleaf *);
int generate(int, T_Token* , T_Token* , T_Token*);
int addJump();
int generateTempVar(T_Token *);
bool destroyPaska(void *);
// seznam instrukci
/*

	nepracujete s ukazateli na tokeny ale se statickymi token tak jejich existenci nemuzete testova na NULL, ale pokud neexistuje tak token.type = NOT_EXIST 

	S_FUNC - jedna se o volani funkce operand1 obsahuje token s funkci, vysledek kam se ulozi return, pokud funkce nedela return tak se vraci NULL
	STORE_PARAM - muze se nachazet jen po predchozi instrukci, znamena uloz hodnotu operandu1 do vysledku
	CALL - vem si int adresu z operandu 1 tam zacina funkce
	JMP_NOT - operand1 - promena ktera se prevede na bool a pokud neni true tak se provede skok na adresu ktera je vysledek.type
	JMP - operand1.type obsahuje skok kam skocit
	
	FUNCTION - jedna se o definici funkce, posle se v operandu1 nazev funkce, na toto misto pak vlezete callem, takze vam je to mozna k tomu aby ste si udelali seznam funkci a jejich miste na pasce
	RETURN - bud bez parametru, nebo je op1 kde je ulozeny vysledek vypoctu pro vraceni

	
	
*/

#endif

