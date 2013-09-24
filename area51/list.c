//Implementace interpretu imperativního jazyka IFJ12.
// xradva00, Radvansky Matej

#include "global.h"
#include "list.h"
#include "parser.h"

void InitList (tList *L)	{
  L->Act=NULL;
  L->First=NULL;
}

void DisposeList (tList *L)	{
  tElemPtr pom;
  while(L->First!=NULL)
  {//dokud existuje dalsi prvek seznamu
    pom=L->First->ptr;
    freetree(L->First->data);
    free(L->First);
    L->First=pom;
  }
  L->Act=NULL;
  L->First=NULL;
}

void InsertFirst (tList *L, struct node*val)	{
  tElemPtr item;
  item=malloc(sizeof(struct tElem)); //alokace pameti pro novy prvek seznamu
												//bude muset vracet int, a cislo pripadne chyby
  item->data=val;
  item->ptr=L->First; //vlozeni pred first
  L->First=item;//nastaveni noveho first
}

void First (tList *L)		{
  L->Act=L->First;
}

void PostInsert (tList *L, struct node *val)	{
  tElemPtr item;
  if(L->Act!=NULL)
  {
    item=malloc(sizeof(struct tElem)); //alokace pameti pro novy prvek
											//bude muset vracet int, a cislo pripadne chyby
    item->data=val;
    item->ptr=L->Act->ptr;//novy prvek ukazuje na prvek za Act
    L->Act->ptr=item;//Act ukazuje na novy prvek
  }
}

void Succ (tList *L)	{
  if(L->Act!=NULL)
  {	//posune aktivitu na dalsi prvek seznamu
    L->Act=L->Act->ptr;
  }
  else
  {
    L->Act=L->First;
  }
}

