//Implementace interpretu imperativního jazyka IFJ12.
// xradva00, Radvansky Matej

#ifndef LIST_H
#define LIST_H

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

#define TRUE 1
#define FALSE 0

typedef struct tElem {
    struct tElem *ptr;
    struct node *data;
} *tElemPtr;

typedef struct {
    tElemPtr Act;
    tElemPtr First;
} tList;

void InitList (tList *);
void DisposeList (tList *);
void InsertFirst (tList *, struct node*);
void First (tList *);
void PostInsert (tList *, struct node*);
void Succ (tList *);

#endif
