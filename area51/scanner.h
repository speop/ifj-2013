//Implementace interpretu imperativního jazyka IFJ12.
// xbucht18, Buchta David
// xsidlo02, Sidlo Boleslav

#ifndef SCANNER_H
#define SCANNER_H

#include "global.h"

enum lex_tecodes
{//chybove stavy
  EOK = 0,     //vse je ok
  EHexWrong,     //chybne zapsany hex kod znaku
  EExpWrong,  //chybne zapsany exponent
  ENumWrong,  //chybne zapsane cislo
  EComToEof,//blok. komentar nekonci pred EOF
  EExclMarkWrong,//samostanty vykricnik
  ENotAlph,    //znak neni v abecede
  EStrEof,
};

int getNextToken(T_Token_struct *token); // main funkce scanneru

int string_length (char first, T_Token_struct *token);

int get_number(char first_digit, T_Token_struct *token);

int get_string(T_Token_struct *token);

char* first_pass();
#endif
