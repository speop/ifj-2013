#ifndef PARSER_H
#define PARSER_H

int parser();

int functionHeaders();
int functionList();
int if_extra();
int cond();
int st_list();
int program();

int expr();

typedef enum PREC_SYMBOLY{
	L = 0, // < nacitame na zasobnik
	H = 1, // > redujujeme
	X = 3, // pravidlo neexistuje syntakticka chyba
	EQ = 4
}PREC_SYMBOLY;
#endif