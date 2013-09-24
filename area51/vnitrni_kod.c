//Implementace interpretu imperativního jazyka IFJ12.
// xradva00, Radvansky Matej
// xrajca00, Rajca Tomas

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "global.h"
#include "list.h"
#include "ial.h"
#include "vnitrni_kod.h"
#include "cilovy_kod.h"

#define NDEBUG 0						//zobrazeni komentaru kdyz NDEBUG 1

T_Token_struct operator,operand1,operand2,*temp;
int first=0,temp_var=0;
extern T_bt_root *gts;

TAC *recipe;
int i=0,recipe_size=16;
char newname[3];

void generate(T_Token_struct *operator,T_Token_struct *operand1,T_Token_struct *operand2, int jmp)
{
	if(i+1>recipe_size)
	{
		recipe_size=recipe_size+16;
		recipe=realloc(recipe,recipe_size*sizeof(TAC));
	}
	recipe[i].operator=operator;
	if(operator->type==S_IS)
	{
		recipe[i].operand1=NULL;
		recipe[i].operand2=operand2;
	}
	else
	{
		recipe[i].operand1=operand1;
		recipe[i].operand2=operand2;
	}
	recipe[i].jmp=jmp;
	i++;
}

char *gen_temp_var()
{
	char *name;
	int lenght=7,pom=temp_var;
	while(pom>9){
		pom=pom/10;
		lenght++;
	}
	if((name=malloc(sizeof(char)*lenght))==NULL)
		return NULL;
	if(sprintf(name,"=temp%d",temp_var)==-1)
		return NULL;
	temp_var++;
	return name;
}

void inorder(struct node*root,T_bt_root *ts)
{
	if(root->left!=NULL&&root->left->data.type!=ID&&root->left->data.type!=S_LITERAL&&root->left->data.type!=S_STR&&!(root->left->data.type>=RETURN&&root->left->data.type<=INPUT))
		inorder(root->left,ts);

	if(root->right!=NULL&&root->right->data.type!=ID&&root->right->data.type!=S_LITERAL&&root->right->data.type!=S_STR&&!(root->left->data.type>=RETURN&&root->left->data.type<=INPUT))
		inorder(root->right,ts);
	if(root->data.type!=ID&&root->data.type!=S_STR&&root->data.type!=S_LITERAL&&!(root->data.type>=RETURN&&root->data.type<=INPUT))
	{
		root->data.value=gen_temp_var();
		generate(&(root->data),&(root->left->data),&(root->right->data),0);
	}
}


int vnitrni(tList *L,T_bt_root *ts)
{
	int result=0,pom_i,exp=0;
	tElemPtr tree;	//pro list
	struct node*pom;
	recipe=malloc(recipe_size*sizeof(TAC));
	tree=L->First;
	L->Act=L->First;
	while(tree!=NULL)
	{
		if(tree->data->data.type!=S_EOL)
		{
			if(tree->data->right!=NULL)
				inorder(tree->data->right,ts);

			if(tree->data->data.type==S_IS)
			{
				if(tree->data->right->data.type==INPUT)
				{
					tree->data->right->data.value=tree->data->left->data.value;
					generate(&(tree->data->right->data),NULL,NULL,0);
				}
				else if(tree->data->right->data.type==NUMERIC||tree->data->right->data.type==TYPEOF||tree->data->right->data.type==LEN||tree->data->right->data.type==SORT)
				{
					tree->data->right->data.value=tree->data->left->data.value;
					generate(&(tree->data->right->data),&(tree->data->right->right->data),NULL,0);
				}
				else if(tree->data->right->data.type==PRINT)
				{
					tree->data->right->data.value=tree->data->left->data.value;
					pom=tree->data->right->right;
					while(pom!=NULL){
						generate(&(tree->data->right->data),&(pom->data),NULL,0);
						pom=pom->right;
					}
				}
				else if(tree->data->right->data.type==FIND)
				{
					tree->data->right->data.value=tree->data->left->data.value;
					generate(&(tree->data->right->data),&(tree->data->right->right->data),&(tree->data->right->right->right->data),0);
				}
				else
				{
					if(Tree_Find_node(gts,tree->data->right->data.value)!=NULL)
					{
						generate(&(tree->data->data),&(tree->data->left->data),&(tree->data->right->data),-2);
						pom_i=i-2;
						while(pom_i>=0){
							if(recipe[pom_i].operator->type==FUNCTION)
							{
								if(!strcmp((char *)recipe[pom_i].operator->value,(char *)tree->data->right->data.value))
								{
									recipe[i-1].jmp=pom_i+1;
									break;
								}
							}
							pom_i--;
						}
						tree->data=tree->data->right;
						while(tree->data->right!=NULL){
							tree->data=tree->data->right;
							tree->data->data.type=S_IS;
							generate(&(tree->data->data),NULL,NULL,0);
						}
						recipe[i-1].jmp=-1;
					}
					else
					{
						if((tree->data->right->right!=NULL&&tree->data->right->right->d_type==CUT_R)||
							(tree->data->right->left!=NULL&&tree->data->right->left->d_type==CUT_L))
						{
							if(tree->data->right->left!=NULL&&tree->data->right->left->d_type==CUT_L)
							{
								tree->data->right->left->data.type=CUT_L;
								generate(&(tree->data->right->left->data),NULL,NULL,0);
							}
							if(tree->data->right->right!=NULL&&tree->data->right->right->d_type==CUT_R)
							{
								tree->data->right->right->data.type=CUT_R;
								generate(&(tree->data->right->right->data),NULL,NULL,0);
							}
							tree->data->left->data.type=S_IS;
							generate(&(tree->data->left->data),NULL,&(tree->data->right->data),0);
						}
						else
							generate(&(tree->data->data),&(tree->data->left->data),&(tree->data->right->data),0);
					}
				}

		//typeof operator- kam typeof, operand1 parametr
		//
			}
			else if(tree->data->data.type==S_END)
			{
				generate(&(tree->data->data),NULL,NULL,0);
				pom_i=i-2;
				while(pom_i>=0){
					if(recipe[pom_i].operator->type==S_END)
						exp++;
					else if(recipe[pom_i].operator->type==IF||recipe[pom_i].operator->type==WHILE)
						exp--;
					if(exp>0)
					{
						pom_i--;
						continue;
					}
					if(recipe[pom_i].jmp==-1&&(recipe[pom_i].operator->type==ELSE||recipe[pom_i].operator->type==WHILE||recipe[pom_i].operator->type==FUNCTION))
					{
						recipe[pom_i].jmp=i;
						if(recipe[pom_i].operator->type==WHILE)
						{
							while(pom_i>0&&recipe[pom_i].operand1==NULL)
								pom_i--;
							recipe[i-1].jmp=pom_i;
						}
						else if(recipe[pom_i].operator->type==FUNCTION)
							recipe[i-1].jmp=-1;
						else
							recipe[i-1].jmp=i;
						break;
					}
					pom_i--;
				}
				if(pom_i<=0)
					recipe[i-1].jmp=-1;
				exp=0;
			}
			else if(tree->data->data.type==ELSE)
			{
				generate(&(tree->data->data),NULL,NULL,-1);
				pom_i=i-1;
				while(pom_i>=0){
					if(recipe[pom_i].operator->type==S_END)
						exp++;
					else if(recipe[pom_i].operator->type==IF||recipe[pom_i].operator->type==WHILE)
						exp--;
					if(exp>0)
					{
						pom_i--;
						continue;
					}
					if(recipe[pom_i].jmp==-1&&recipe[pom_i].operator->type==IF)
					{
						recipe[pom_i].jmp=i;
						break;
					}
					pom_i--;
				}
				exp=0;
			}
			else if(tree->data->data.type==FUNCTION)
			{
				pom_i=i-1;
				while(pom_i>=0){
					if(recipe[pom_i].jmp==-2)
					{
						if(!strcmp((char *)recipe[pom_i].operand2->value,(char *)tree->data->left->data.value))
							recipe[pom_i].jmp=i+1;
					}
					pom_i--;
				}
				tree->data->data.value=tree->data->left->data.value;
				generate(&(tree->data->data),NULL,NULL,-1);
				while(tree->data->right!=NULL){
					tree->data=tree->data->right;
					tree->data->data.type=S_IS;
					generate(&(tree->data->data),NULL,NULL,0);
				}
			}
			else if(tree->data->data.type==RETURN)
			{
				generate(&(tree->data->data),&(tree->data->right->data),NULL,0);
			}
			else
			{
				tree->data->data.value=recipe[i-1].operator->value;
				generate(&(tree->data->data),NULL,NULL,-1);
			}
		}
		Succ(L);
		tree=L->Act;
	}
	#if NDEBUG==1
	for(int y=0;y<i;y++){
		if(recipe[y].operand1!=NULL)
		{
			if(recipe[y].operator->type==NUMERIC)
				printf("%d.numeric %s %s\n", y, (char *)recipe[y].operator->value, (char *)recipe[y].operand1->value);
			else if(recipe[y].operator->type==PRINT)
				printf("%d.print %s %s\n", y, (char *)recipe[y].operator->value, (char *)recipe[y].operand1->value);
			else if(recipe[y].operator->type==TYPEOF)
				printf("%d.typeOf %s %s\n", y, (char *)recipe[y].operator->value, (char *)recipe[y].operand1->value);
			else if(recipe[y].operator->type==LEN)
				printf("%d.len %s %s\n", y, (char *)recipe[y].operator->value, (char *)recipe[y].operand1->value);
			else if(recipe[y].operator->type==SORT)
				printf("%d.sort %s %s\n", y, (char *)recipe[y].operator->value, (char *)recipe[y].operand1->value);
			else if(recipe[y].operator->type==RETURN)
				printf("%d.return %s\n", y, (char *)recipe[y].operand1->value);
			else if(recipe[y].operator->type==FIND)
				printf("%d.find %s %s %s\n", y, (char *)recipe[y].operator->value, (char *)recipe[y].operand1->value,(char *)recipe[y].operand2->value);
			else
				printf("%d.\t%s %s %d %s\n", y,(char *)recipe[y].operator->value, (char *)recipe[y].operand1->value, recipe[y].operator->type, (char *)recipe[y].operand2->value);
		}
		else
			if(recipe[y].operator->type==IF||recipe[y].operator->type==WHILE)
				printf("%d.skoky %d %s %d\n", y, recipe[y].operator->type, (char *)recipe[y].operator->value, recipe[y].jmp);
			else if(recipe[y].operator->type==S_END)
				printf("%d.end %d\n", y, recipe[y].jmp);
			else if(recipe[y].operator->type==ELSE)
				printf("%d.else %d\n", y, recipe[y].jmp);
			else if(recipe[y].operator->type==FUNCTION)
				printf("%d.function %s %d\n", y, (char *)recipe[y].operator->value,recipe[y].jmp);
			else if(recipe[y].operator->type==INPUT)
				printf("%d.input %s\n", y, (char *)recipe[y].operator->value);
			else if(recipe[y].operator->type==CUT_L||recipe[y].operator->type==CUT_R){
				printf("%d.odseknuti %d\n", y, recipe[y].operator->type);}
			else if(recipe[y].operator->type==S_IS&&recipe[y].operand2==NULL)
				printf("%d.parametr %d %s %d\n", y, recipe[y].operator->type,(char *)recipe[y].operator->value,recipe[y].jmp);
			else
				if(recipe[y].operand2->type==S_LITERAL)
					printf("%d.prirazeni %s %d %g %d\n", y, (char *)recipe[y].operator->value, recipe[y].operator->type, *((double *)recipe[y].operand2->value),recipe[y].jmp);
				else
					printf("%d.prirazeni %s %d %s %d\n", y, (char *)recipe[y].operator->value, recipe[y].operator->type, (char *)recipe[y].operand2->value,recipe[y].jmp);
		}
	#endif

	if(result==PROG_OK)
		result=cilovy_kod(recipe, i);


	return result;
}
