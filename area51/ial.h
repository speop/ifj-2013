//Implementace interpretu imperativního jazyka IFJ12.
//xbucht18, Buchta David

#ifndef IAL_H
#define IAL_H

#include "global.h"

typedef struct T_bt_root T_bt_root;

typedef struct {
	char *nazev;
	int type;
	int type_of;
	void *value;
	T_bt_root*plts;
}T_bt_item;

typedef struct T_bt_node{
	T_bt_item *data;
	struct T_bt_node* LUk;
	struct T_bt_node* PUk;
}T_bt_node;

struct T_bt_root{
	T_bt_node* CUk;
};


void Tree_Init(T_bt_root *);
void Tree_insert_node (T_bt_node *, T_bt_node *);
T_bt_node* Tree_Find_node_pom(T_bt_node *, char * );
int Tree_insert_new(T_bt_root *, char *,int)	;
T_bt_node* Tree_Find_node(T_bt_root *, char * );
void Tree_Dispose (T_bt_root *);
void Tree_Dispose_pom (T_bt_node *);
void Tree_Delete_node(T_bt_node *, T_bt_root *);
T_bt_node *Tree_find_parent(T_bt_node *,T_bt_node *,int );


//built in fundtions
char *bi_sort(char *);
int bi_find (char *, char *);

//pomocne built in
void bi_sort_repair_top(char *,int , int );
void bi_find_delta1(int *, char *, int );
int bi_find_prefix(char *, int , int );
int bi_find_suffix_length(char *, int , int );
void bi_find_delta2(int *, char *, int );


#endif // IAL_H_INCLUDED
