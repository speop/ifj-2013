#ifndef STRING_H
#define STRING_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<malloc.h>

#include "types.h"

typedef struct
{
  char* str;		// misto pro dany retezec ukonceny znakem '\0'
  int strLength;		// opravdova delka retezce
  int strAllocSize;	// velikost alokovane pameti
} string;

int strMalloc(string **S) //alokuje retezec
int strInit(string *S); //vytvori novej retezec
void strFree(string *S); //fce uvolneno retezce
void strClear(string *S); //fce pro vymazani noveho retezce
int strAdd(string *S, char c); //fce ktera prida znak do retezce
int strCopy(string *S1, string *S2); // kopiruje retezec S1 do S2
int strEqual(string *S1, string *S2) //porovnavani obou retezcu, pak se vrati vysledek porovnavani
char *strGet(string *S); //vraceni retezce
int strLenght(string *S); //vrati delku retezce

#endif
