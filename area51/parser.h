//Implementace interpretu imperativního jazyka IFJ12.
// xradva00, Radvansky Matej
// xrajca00, Rajca Tomas
// xpokor57, Pokorny Oto

#ifndef PARSER_H
#define PARSER_H

#include "global.h"

#define LEFT 1
#define RIGHT 2
#define N 3

struct node{
  T_Token_struct data;
  struct node* left;
  struct node* right;
  int d_type;
};

struct node* newnode(T_Token_struct data);
void insert(struct node*child,struct node*parent,int dir);
void printout(struct node * st);
void freetree (struct node*tree);


int StList();
int Stat();
int Expr();
int Params(int insert_ts);
int Params_n(int insert_ts);
int Term();
int Term_n();
int Opr();
int Func();
int Assign();
int parser();

#endif
