#include "string.h"

#define STR_LENGHT 8 // velikost pocatecni alokace pameti

//naalokujem strukturu pro string
int strMalloc(string **S)
{
  if ((*S = (string*) malloc(sizeof(struct string))) == NULL)
  {
    return EALLOC;
  }
  (*S)->str = NULL;
  (*S)->strLength = 0;
  (*S)->strAllocSize = 0;
  return EOK;
}

// fce vztvoreni noveho retezce
int strInit(string *S)
{
  S->str = (char*) malloc(sizeof(char) * STR_LENGHT);
  if (S->str == NULL)
  {
   return EALLOC;
  }

  S->strAllocSize = STR_LENGHT;
  S->str[0] = '\0';
  S->strLenght = 0;
  return EOK;
}

//fce uvolneni retezce a nulovani parametru
void strFree(string *S)
{
    free(S->str);

    S->str = NULL;
    S->strAllocSize = 0;
    S->strLength = 0;
}

// vymaze retezec
void strClear(string *S)
{
    S->str[0] = '\0';
    S->strLength = 0;
}

//prida znak do retzece
int strAdd(string *S, char c)
{
  if (S->strLenght + 1 >= S->strAllocSize)
  { // nedostatecna pamet -> realokace
    if ((S->str = (char*) realloc(S->str, S->strLenght + (sizeof(char) * STR_LENGHT))) == NULL)
      return EALLOC;
    S-> strAllocSize = S->strLenght + STR_LENGHT;
  }

  S->str[S->strLenght] = c;
  S->strLenght++;
  S->[S->strLenght] = '\0';
  return EOK;
}

//kopiruje retezec S2 do S1
int strCopy(string *S1, string *S2)
{
  int newLenght = S2->strLenght;
  if (newLenght >= S1->strAllocSize)
  {
    if ((S1->str = (char*) realloc(S1->str, newLenght + 1)) == NULL)
      return EALLOC;
    S1->strAllocSize = newLenght + 1;
  }
  strcpy(S1->str, S2->str);
  S1->strLenght = newLenght;
  return EOK;
}

// porovnavani obou retezcu, pak se vrati vysledek porovnavani
int strEqual(string *S1, string *S2)
{
  return strcmp(S1->str, S2->str);
}
//vraceni  textu retezce
char *strGet(string *S)
{
  return S->str;
}

//delka stringu
int strLenght(string *S)
{
  return S->strLenght;
}
