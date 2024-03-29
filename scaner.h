//Implementace interpretu jazyka IFJ13.
//xbucht18, Buchta David
//xsalom04, Salomon Vit
//xbulin02, Bulin Martin

#ifndef SCANNER_H
#define SCANNER_H

#include "types.h"

enum lex_tecodes
{//chybove stavy
  EOK = 0, //vse je ok//EALLOC = 99, // interni chyba (napr. Alokace)
  EHexWrong, //chybne zapsany hex kod znaku
  EExpWrong, //chybne zapsany exponent
  ENumWrong, //chybne zapsane cislo
  EComToEof,//blok. komentar nekonci pred EOF
  EExclMarkWrong,//samostanty vykricnik
  ENotAlph, //znak neni v abecede
  EStrEof,
};


//funkce
void soubor(FILE *f); // nastaveni vstupniho souboru
void putToken( T_Token *token);
int next(int newst);   // prechod do noveho stavu

int getToken(T_Token *token); /* hlavni fce lexikalniho an. */
int readNumber(T_Token * token, char); 
int readString(T_Token *token);

int getFunctionHeader(T_Token* , FUn);
char* mystrdup(const char* );

int getTokenReal(T_Token *token);
#endif
