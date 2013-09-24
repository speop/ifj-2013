//Implementace interpretu imperativního jazyka IFJ12.
//xrajca00,Rajca Tomas

#include <stdio.h>

#include "global.h"

int row=1;

const char *text_error[]=
{
	[LEX_ERROR]="Chyba v programu v ramci lexikalni analyzy",
	[SYNTAX_ERROR]="Chyba v programu v ramci syntakticke analyzy",
	[SEM_ERROR_UVAR]="Semanticka chyba v programu-nedefinovana promenna",
	[SEM_ERROR_UFUNC]="Semanticka chyba v programu-nedefinovana funkce",
	[SEM_ERROR]="Semanticka chyba",
	[DIV_NUL_ERROR]="Chyba deleni nulou",
	[COMP_ERROR]="Chyba nekompatibility typu",
	[NUMERIC_ERROR]="Chyba pri pretypovani na cislo",
	[OTHER_ERROR]="Jina chyba",
	[INTER_ERROR]="Iterni chyba interpretu"
};

void print_error(int result)
{
	fprintf(stderr, "%d: [%d]%s\n",row, result, text_error[result]);
}
