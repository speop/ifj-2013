//Implementace interpretu jazyka IFJ13.
//xbucht18, Buchta David

#ifndef PARSER_H
#define PARSER_H

int parser();

int functionHeaders();
int functionList();
int if_extra();
int st_list();
int program();

int expr();

typedef enum PREC_SYMBOLY{
	L = 100, // < nacitame na zasobnik
	H = 101, // > redujujeme
	X = 300, // pravidlo neexistuje syntakticka chyba
	EQ = 400
}PREC_SYMBOLY;
#endif