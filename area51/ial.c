//Implementace interpretu imperativního jazyka IFJ12.
//xbucht18, Buchta David

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ial.h"
#include "scanner.h"

#define max(a, b) ((a < b) ? b : a)
// TABULKA SYMBOLU

void Tree_Init(T_bt_root *tree) // dostanu adresu korene stromu obecne to nemusi bzt jen koren ale kaydz nove vztvoreny prvek
{
   tree->CUk = NULL;
}

void Tree_Insert(T_bt_root *tree, T_bt_item *item)
{
    T_bt_node *ptr_node;

    ptr_node = malloc(sizeof (T_bt_node));
    ptr_node->data = item;
    ptr_node->LUk=NULL;
    ptr_node->PUk=NULL;

    if (tree->CUk==NULL) tree->CUk=ptr_node; // vkladame do prazdeho stromu

    else Tree_insert_node(tree->CUk, ptr_node);

    return;

}

//rekurzivne se porchazi stroom tak dlouho dokud se nenajde volne misto ke vlozeni
void Tree_insert_node (T_bt_node *node, T_bt_node *new_node)
{
	int compar;
	compar=strcmp(node->data->nazev, new_node->data->nazev);

	if(compar > 0)
	{
		if(node->LUk == NULL)
			node->LUk = new_node;
		else Tree_insert_node(node->LUk, new_node);
	}
	else if(compar < 0)
	{
		if(node->PUk == NULL)
			node->PUk = new_node;
		else
			Tree_insert_node(node->PUk, new_node);
	}

	return;
}

//pokud neexistuje tak vytvori a vlozi novy item, 0 podarilo se vlozit, 1 dany prvek uz existuje, -1 interni chyba
int Tree_insert_new(T_bt_root *tree, char *value,int gorl)
{
	T_bt_item *ptr_item;

// 	printf("2fsdsdfafsd\n");
	if((Tree_Find_node_pom(tree->CUk, value))!=NULL) return 1;
// 	printf("2fsdsdfafsd\n");
	if((ptr_item=malloc(sizeof(T_bt_item)))==NULL) return -1;

	ptr_item->nazev =value;
	if(gorl==1)
		ptr_item->plts=malloc(sizeof(T_bt_root*));
	Tree_Insert(tree, ptr_item);

	return 0;
}

T_bt_node* Tree_Find_node_pom (T_bt_node *node, char *value)
{
	if(node==NULL)
		return NULL;

	int compar=0;
// 	if(node->data->nazev!=NULL)
		compar=strcmp((char *)node->data->nazev, value);

	// vratime pointer na uzel obsahujici danou hodnotu
	if(!compar)return node;

	//divame se vlevo
	else if(compar >0)
	{
		if(node->LUk==NULL)
			return NULL; // dana hodnota neni v  bitevnim tanku
		else
			return Tree_Find_node_pom(node->LUk,value);
	}
	//divame se vpravo
	else if(compar <0)
	{
		if(node->PUk==NULL)
			return NULL; // dana hodnota neni v  bitevnim tanku
		else
			return Tree_Find_node_pom(node->PUk,value);
	}

	return NULL;
}

//pokud nalezeno vraci pointer na node jinac NULL
T_bt_node* Tree_Find_node (T_bt_root *tree, char *value)
{
	return  Tree_Find_node_pom(tree->CUk,value);
}

void Tree_Dispose (T_bt_root *tree)
{
    if (tree->CUk==NULL) return; //strom je prazdny
    else {Tree_Dispose_pom(tree->CUk); free(tree->CUk); tree->CUk=NULL;}
    return;
}

//rozhodujeme se vpravo pokud nejde prehazim doleva pokud jsme na konci vetve mazeme
void Tree_Dispose_pom (T_bt_node *node)
{
   if(node->PUk!=NULL) Tree_Dispose_pom(node->PUk);
   else if (node->LUk!=NULL) Tree_Dispose_pom(node->LUk);

   if(node->PUk==NULL && node->LUk==NULL ) return; //jsme uplne nakoonci potrebujeme o jednu uroven vyse

   if(node->PUk!=NULL) {free(node->PUk); node->PUk= NULL;Tree_Dispose_pom(node->LUk);} // mazu pravy prvek jdu do leve casti stromu
   else if(node->LUk!=NULL) {free(node->LUk); node->LUk= NULL;} // mazu levy prvek

   return;
}

void Tree_Delete_node(T_bt_node *node, T_bt_root *tree)
{
    T_bt_node *parent_node, *ptr_node;
    int smer;
    smer = strcmp(tree->CUk->data->nazev, node->data->nazev);
    //jediny prvek seznamu
    if(tree->CUk == node) {free(tree->CUk);tree->CUk=NULL;}
    else
    {

        // prvek ma jen pravou stranu
        if(node->LUk==NULL) {   parent_node=Tree_find_parent(node,tree->CUk,smer);
                                parent_node->PUk =node->PUk;
                                free(node);}


        //prvek ma jen levou stranu
        else if(node->PUk==NULL) {  parent_node=Tree_find_parent(node,tree->CUk,smer);
                                    parent_node->LUk =node->LUk;
                                    free(node);}

        //bohuzel ma prvek obe strany

        else {  parent_node=Tree_find_parent(node,tree->CUk,smer);
                smer = strcmp(parent_node->data->nazev, node->data->nazev);

                //prvek je vlevo
                if(smer > 0)        {   parent_node->LUk = node->PUk;
                                        ptr_node = parent_node->PUk;
                                        while(1)
                                        {
                                            if(ptr_node->LUk!=NULL) ptr_node=ptr_node->LUk;
                                            else {ptr_node->LUk=node->LUk;break;}
                                        }
                                        free(node);
                                    }

                //prvek je vpravo
                else if (smer < 0 ){    parent_node->PUk = node->LUk;
                                        ptr_node = parent_node->LUk;
                                        while(1)
                                        {
                                            if(ptr_node->PUk!=NULL) ptr_node=ptr_node->PUk;
                                            else {ptr_node->PUk=node->PUk;break;}
                                        }
                                        free(node);
                                    }
                }
    }

    return;

}

T_bt_node *Tree_find_parent(T_bt_node *node,T_bt_node *node_parent,int smer)
{
    int compar;

    //prvek lezi v leve casti
    if(smer >  0) {     compar = strcmp(node_parent->LUk->data->nazev, node->data->nazev);
                        if(compar > 0 ) Tree_find_parent(node,node_parent->LUk,1);
                        else if(compar < 0 ) Tree_find_parent(node,node_parent->LUk,-1);
                        else return node_parent;
                        }

    //prvek lezi v prave casti
    else if(smer <  0) {    compar = strcmp(node_parent->PUk->data->nazev, node->data->nazev);
                            if(compar > 0 ) Tree_find_parent(node,node_parent->PUk,1);
                            else if(compar < 0 ) Tree_find_parent(node,node_parent->PUk,-1);
                            else return node_parent;
                        }
     return NULL;
}

char* bi_sort(char *str)
{
    int i,size;
    char tmp;
    size=strlen(str);

    for (i=size/2-1;i>=0;i--) bi_sort_repair_top(str, size-1,i);

    for (i = size - 1; i > 0; i--) {    tmp = str[i];
                                        str[i] = str[0];
                                        str[0] = tmp;

                                        bi_sort_repair_top(str, i - 1, 0);}

    return str;
}

void bi_sort_repair_top(char *str,int right, int left)
{
    int succ;
    char tmp;

    tmp = str[left];
    succ = left*2 + 1;
    if (succ < right && str[succ] < str[succ+1]) succ++;

    while (succ <= right && tmp < str[succ]) {
        str[left] = str[succ];
        left = succ;
        succ = succ*2 + 1;
        if (succ < right && str[succ] < str[succ+1]) succ++;}

    str[left] = tmp;
    return;
}


int bi_find (char *str, char *pat)
{
    int pat_len = strlen(pat);
    int str_len =  strlen(str);

    int i;
    int delta1[255];
    int *delta2 = malloc(pat_len * sizeof(int));
    bi_find_delta1(delta1, pat, pat_len);
    bi_find_delta2(delta2, pat, pat_len);

    i = pat_len-1;

    while (i < str_len)
    {
        int j = pat_len-1;
        while (j >= 0 && (str[i] == pat[j])) {j--;i--;}

        if (j < 0)
        {
            free(delta2);
            return ++i;
        }

        i += max(delta1[(int)str[i]], delta2[j]);
    }
    free(delta2);
    return -1;

}

void bi_find_delta1(int *delta1, char *pat, int pat_len)
{
    int i;
    for (i=0; i < 255; i++)  delta1[i] = pat_len;
    for (i=0; i < pat_len-1; i++) delta1[(int)pat[i]] = pat_len-1 - i;
}

int bi_find_prefix(char *word, int word_len, int pos)
{
    int i;
    int suffix_len = word_len - pos;

    for (i = 0; i < suffix_len; i++)
    {
        if (word[i] != word[pos+i]) return 0;
    }
    return 1;
}

int bi_find_suffix_length(char *word, int word_len, int pos)
{
    int i;

    // smycka se opakuje tak dlouhu dokud jsou znaky sterjne
    for (i = 0; (word[pos-i] == word[word_len-1-i]) && (i < pos); i++);
    return i;
}

void bi_find_delta2(int *delta2, char *pat, int pat_len)
{
    int p;
    int last_prefix_index = pat_len-1;


    for (p=pat_len-1; p>=0; p--)
    {
        if (bi_find_prefix(pat, pat_len, p+1)) last_prefix_index = p+1;
        delta2[p] = last_prefix_index + (pat_len-1 - p);
    }


    for (p=0; p < pat_len-1; p++)
    {
        int slen = bi_find_suffix_length(pat, pat_len, p);
        if (pat[p - slen] != pat[pat_len-1 - slen]) delta2[pat_len-1 - slen] = pat_len-1 - p + slen;
    }

    return;
}
