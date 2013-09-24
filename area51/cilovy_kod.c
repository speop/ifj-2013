//Implementace interpretu imperativního jazyka IFJ12.
//xbucht18, Buchta David
//xrajca00, Rajca Tomas

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cilovy_kod.h"
#include "built_in.h"
#include "global.h"
#include "ial.h"
#include "vnitrni_kod.h"

#define T_LIT   1
#define T_NUM   2
#define T_BOOL  3
#define STOP    100
#define T_NIL   4

T_bt_root *active;

char *strduplicit (char **str){
	char *new_str;
	if ((new_str=malloc(strlen (*str) + 1)) != NULL)
		strcpy (new_str,*str);
	return new_str;
}

void free_all(int pst_function_index, stack_calls *pst_function )
{
	while (pst_function_index)
	{
		Tree_Dispose (active);

		active = pst_function[pst_function_index].tabulka;
		pst_function[pst_function_index].jmp = -1;
	}
	if(!pst_function_index) free(pst_function);
}

int cilovy_kod (TAC pole[],int recipe_size)
{
	int i = 0;  //index pole triadresneho kodu
	TAC tac;
	T_bt_node *node,*dest_node,*dest_node_pom, *op1_node, *op2_node;
	T_bt_root *main_table;
	stack_calls *pst_function;

	char str_true[]="true";
	char str_false[]="false";
	char str_nil[]="nil";
	char *dta_op1 = NULL, *dta_op2 = NULL;
	char dummy[1];

	void *op1, *op2;

	double a;

	int op1_type,op2_type;
	int kolik=0,pom,l_op1,l_op2,pom2,pom3,kde_skok;
	int pst_function_index=0,stack_max=5,siz;

	active=malloc(sizeof(T_bt_root*));
	active->CUk=NULL;
	if((pst_function=malloc(sizeof(stack_calls)*stack_max)) == NULL) return INTER_ERROR;

	pst_function[pst_function_index].jmp=-1;
	pst_function[pst_function_index].tabulka = active;

	while(1)
	{
		if(recipe_size<=i)
			break;
		tac = pole[i++];

		if  (tac.operator->type == S_IS) // upravime hodnotu promene v tabulce symbolu
		{   //hodnoty jsou ulozene v tabulce zadne pointery na hodnoty

			if(tac.operand1==NULL&&tac.operand2==NULL&&tac.operator->type==S_IS)				//preskoc parametry
				continue;
			if(tac.jmp>0)
			{
				pom=i;
				kde_skok=tac.jmp;
				main_table = active;
				active=malloc(sizeof(T_bt_root));
				active->CUk=NULL;
						// zpracovani parametru
				do{
					tac = pole[i];
					if(tac.operand1!=NULL||tac.operand2!=NULL||tac.operator->type!=S_IS)
						break;
					Tree_insert_new(active,(char *)(pole[kde_skok]).operator->value,0);
					dest_node = Tree_Find_node(active, (char *)(pole[kde_skok]).operator->value);
					dest_node_pom = Tree_Find_node(main_table, (char *)tac.operator->value);

					if(dest_node_pom->data->type_of == S_STR)
						dest_node->data->value=strduplicit((char **)(&(dest_node_pom->data->value)));
					else if(dest_node_pom->data->type_of == S_LITERAL)
					{
						if((dest_node->data->value = (void*)malloc(sizeof(double)))== NULL) return INTER_ERROR;
						*((double *) (dest_node->data->value))= (*((double *)dest_node_pom->data->value));
					}
					else if (dest_node_pom->data->type_of == S_TRUE || dest_node_pom->data->type_of == S_FALSE  || dest_node_pom->data->type_of == S_NIL)
						dest_node->data->value= NULL;
					dest_node->data->type=ID;
					dest_node->data->type_of=dest_node_pom->data->type_of;
					i++;
				}while (tac.jmp != -1);

				pst_function_index++;
				pst_function[pst_function_index].jmp = pom;
				pst_function[pst_function_index].tabulka = main_table;
				if(pst_function_index==stack_max)
				{
					stack_max=stack_max+8;
					pst_function = realloc (pst_function, (sizeof(stack_calls)*stack_max));
				}
				i=kde_skok;
				continue;
			}
			if((dest_node = Tree_Find_node(active, (char *)tac.operator->value))== NULL)
			{
				Tree_insert_new(active,tac.operator->value,0);
				dest_node = Tree_Find_node(active, (char *)tac.operator->value);
				dest_node->data->type = ID;
			}

			if(tac.operand2->type==S_STR)
			{
				dest_node->data->type_of = S_STR;
				dest_node->data->value=strduplicit((char**)(&(tac.operand2->value)));
			}
			else if(tac.operand2->type == S_LITERAL)
			{
				dest_node->data->type_of = S_LITERAL;
				if((dest_node->data->value = (void*)malloc(sizeof(double)))== NULL) return INTER_ERROR;
				*((double *) (dest_node->data->value))= (*((double *)tac.operand2->value));
			}

			else if (tac.operand2->type == S_TRUE
                     ||  tac.operand2->type == S_FALSE
                     || tac.operand2->type == S_NIL
                     ) {dest_node->data->type_of = tac.operand2->type; dest_node->data->value=NULL;}

			else
			{
				dest_node_pom = Tree_Find_node(active, (char *)tac.operand2->value);
				if(dest_node_pom==NULL)
					return OTHER_ERROR;
				if(dest_node_pom->data->type_of == S_STR)
				{
					dest_node->data->type = ID;
					dest_node->data->type_of = S_STR;
					dest_node->data->value=strduplicit((char**)(&dest_node_pom->data->value));
				}
				else if(dest_node_pom->data->type_of == S_LITERAL)
				{
					dest_node->data->type = ID;
					dest_node->data->type_of = S_LITERAL;
					if((dest_node->data->value = (void*)malloc(sizeof(double)))== NULL) return INTER_ERROR;
					*((double *) (dest_node->data->value))= (*((double *)dest_node_pom->data->value));
				}

				else if (dest_node_pom->data->type_of == S_TRUE)dest_node->data->type_of = S_TRUE;
				else if (dest_node_pom->data->type_of == S_FALSE)dest_node->data->type_of = S_FALSE;
            else if (dest_node_pom->data->type_of == S_NIL)dest_node->data->type_of = S_NIL;

			}
		}

		else
		{
			if(tac.operand1!=NULL)
			{
				if( tac.operand1->type == ID)
				{
					op1_node = Tree_Find_node(active, (char *)tac.operand1->value);
					if(op1_node==NULL)
						return OTHER_ERROR;
					op1 = op1_node->data->value;
					op1_type=op1_node->data->type_of;
				}
				else
				{
					op1 = tac.operand1->value;
					op1_type= tac.operand1->type;
				}
			}

			if(tac.operand2!=NULL)
			{
				if( tac.operand2->type == ID)
				{
					op2_node = Tree_Find_node(active, (char *)tac.operand2->value);
					if(op2_node==NULL)
						return OTHER_ERROR;
					op2 = op2_node->data->value;
					op2_type=op2_node->data->type_of;
				}
				else
				{
					op2 = tac.operand2->value;
					op2_type= tac.operand2->type;
				}
			}

			switch(tac.operator->type)
			{
				case S_PLUS:
					Tree_insert_new(active,tac.operator->value,0);
					dest_node=Tree_Find_node(active, (char *)tac.operator->value);
					dest_node->data->type = ID;
					if (op1_type == S_LITERAL && op2_type == S_LITERAL)
					{
						if((dest_node->data->value = (void*)malloc(sizeof(double)))== NULL) return INTER_ERROR;
						*((double *) (dest_node->data->value))=(*((double *)op1))+(*((double *)op2));
						dest_node->data->type_of = S_LITERAL;
					}
					else if(op1_type == S_STR&&(op1_type == S_STR||op2_type == S_LITERAL))
					{
						kolik=1;
						dta_op1 = op1;

						if(op2_type == S_LITERAL)
						{
							siz = snprintf(dummy, sizeof dummy, "%g", (*((double *)op2)));
							dta_op2 = malloc(siz + 1);
							sprintf(dta_op2, "%g", (*((double *)op2)));
							kolik+= 2;
						}
						else if(op2_type == S_STR)  dta_op2 = op2;

						l_op1 = strlen(dta_op1);
						l_op2 = strlen(dta_op2);

						if((dest_node->data->value = (void*)malloc( (l_op1 + l_op2 +1)*sizeof(char)))== NULL) return INTER_ERROR;

						pom2=0;
						for (pom=0; pom<l_op1; pom++, pom2++) ((char*)(dest_node->data->value))[pom] = dta_op1[pom];
						for (pom=0; pom<l_op2; pom++) ((char*)(dest_node->data->value))[pom2 + pom] = dta_op2[pom];

						((char*)(dest_node->data->value))[pom2 + pom]='\0';

						if(kolik==1) free(dta_op1);
						if(kolik==2) free(dta_op2);
						if(kolik==3) {free(dta_op1); free(dta_op2);}

						dest_node->data->type_of = S_STR;
					}
					else return COMP_ERROR;
					break;

					case S_MINUS:
						if(op1_type != S_LITERAL || op2_type != S_LITERAL)
							return COMP_ERROR;

						Tree_insert_new(active,tac.operator->value,0);
						dest_node=Tree_Find_node(active, (char *)tac.operator->value);
						dest_node->data->type_of = S_LITERAL;
						dest_node->data->type = ID;

						if((dest_node->data->value = (void*)malloc(sizeof(double)))== NULL) return INTER_ERROR;
						*((double *) (dest_node->data->value))=(*((double *)op1))-(*((double *)op2));

						break;

					case CUT_L:
					case CUT_R:
						l_op1=0;
						l_op2=0;
						if(tac.operator->type==CUT_L&&tac.operator->type!=CUT_R)
						{
							l_op1=*((double *)(tac.operator->value));
							tac = pole[i++];
						}
						if(tac.operator->type==CUT_R)
						{
							l_op2=*((double *)(tac.operator->value));
							tac = pole[i++];
							if(l_op2>pom3){pom3=0.0;l_op1=0.0;}
						}
						pom3=strlen(tac.operand2->value);
						if(l_op2>pom3||l_op1<0.0)
							{pom3=0;l_op2=0;l_op1=0;}
						else
						{
							pom3=(int)(l_op2-l_op1);
							if(pom3<0.0)pom3=0.0;
						}

						if((dest_node = Tree_Find_node(active, (char *)tac.operator->value))== NULL)
						{
							Tree_insert_new(active,tac.operator->value,0);
							dest_node = Tree_Find_node(active, (char *)tac.operator->value);
							dest_node->data->type = ID;
						}
						dest_node->data->type_of = S_STR;
						dest_node->data->type = ID;

						dta_op1=strduplicit((char **)(&(tac.operand2->value)));
						if((dest_node->data->value = (void*)realloc(dest_node->data->value,sizeof(char)*(pom3+1)))== NULL) return INTER_ERROR;
						for(pom=0,pom2=l_op1;pom<l_op2;pom++,pom2++)
							((char*)(dest_node->data->value))[pom]= ((char*)(dta_op1))[pom2];
						((char*)(dest_node->data->value))[pom]='\0';
						break;

					case S_MUL:
						Tree_insert_new(active,tac.operator->value,0);
						dest_node=Tree_Find_node(active, (char *)tac.operator->value);
						dest_node->data->type = ID;
						if (op1_type == S_LITERAL && op2_type == S_LITERAL)
						{
							if((dest_node->data->value = (void*)malloc(sizeof(double)))== NULL) return INTER_ERROR;
							*((double *) (dest_node->data->value))=(*((double *)op1))*(*((double *)op2));
							dest_node->data->type_of = S_LITERAL;
						}
						else if(op1_type==S_STR&&op2_type==S_LITERAL)
						{
							kolik=0;
							l_op1 = strlen(op1);
							l_op2 = (int)*((double *)op2);
							if((dest_node->data->value = (void*)malloc( (l_op1 * l_op2 +1)*sizeof(char)))== NULL) return INTER_ERROR;
							for (pom=0,pom3=0;pom<l_op2;pom++)
								for(pom2=0;pom2<l_op1;pom2++,pom3++)
									((char*)(dest_node->data->value))[pom3] = ((char*)(op1))[pom2];

							((char*)(dest_node->data->value))[pom3] = '\0';
							dest_node->data->type_of = S_STR;
						}
						else
							return COMP_ERROR;
						break;

					case S_DIV:
						if(op1_type != S_LITERAL || op2_type != S_LITERAL)
							return COMP_ERROR;
						Tree_insert_new(active,tac.operator->value,0);
						dest_node=Tree_Find_node(active, (char *)tac.operator->value);
						dest_node->data->type_of = S_LITERAL;
						dest_node->data->type = ID;

						if(*((double *)op2)==0.0)
							return DIV_NUL_ERROR;
						if((dest_node->data->value = (void*)malloc(sizeof(double)))== NULL) return INTER_ERROR;
						*((double *) (dest_node->data->value))=(*((double *)op1))/(*((double *)op2));
						break;

					case S_POW:
						if(op1_type != S_LITERAL || op2_type != S_LITERAL)
							return COMP_ERROR;
						Tree_insert_new(active,tac.operator->value,0);
						dest_node=Tree_Find_node(active, (char *)tac.operator->value);
						dest_node->data->type_of = S_LITERAL;
						dest_node->data->type = ID;

						if((dest_node->data->value = (void*)malloc(sizeof(double)))== NULL) return INTER_ERROR;

						*((double *) (dest_node->data->value)) = *(double *)op1;
						a=(*(double *)op1);
						for (pom=1;pom< *(double *)op2;pom++)
							*(double *)(dest_node->data->value) = *((double *) (dest_node->data->value)) *  a;
						break;

					case INPUT:
						if((dest_node = Tree_Find_node(active, (char *)tac.operator->value))== NULL)
						{
							Tree_insert_new(active,tac.operator->value,0);
							dest_node = Tree_Find_node(active, (char *)tac.operator->value);
							dest_node->data->type = ID;
						}

						bi_input((char**)(&(dest_node->data->value)));
						dest_node->data->type_of = S_STR;
						break;

					case NUMERIC:
						if((dest_node = Tree_Find_node(active, (char *)tac.operator->value))== NULL)
						{
							Tree_insert_new(active,tac.operator->value,0);
							dest_node = Tree_Find_node(active, (char *)tac.operator->value);
							dest_node->data->type = ID;
						}
						if(op1_type==S_TRUE||op1_type==S_FALSE||op1_type==S_NIL)
							return NUMERIC_ERROR;

						pom=bi_numeric(((char *) (op1)), &a);						//nedela desetinnou cast spravne
						if(pom>PROG_OK)
							return NUMERIC_ERROR;
						if((dest_node->data->value = (void*)malloc(sizeof(double)))== NULL) return INTER_ERROR;
						*((double *) (dest_node->data->value)) = a;
						dest_node->data->type_of= S_LITERAL;
						break;

					case PRINT:
						if((dest_node = Tree_Find_node(active, (char *)tac.operator->value))== NULL)
						{
							Tree_insert_new(active,tac.operator->value,0);
							dest_node = Tree_Find_node(active, (char *)tac.operator->value);
							dest_node->data->type = ID;
						}

						switch(tac.operand1->type)
						{
							case S_LITERAL: printf("%g\n",*((double *)op1)); break;
							case S_STR: printf("%s",((char *)op1)); break;
							case S_TRUE: printf("%s",str_true); break;
							case S_FALSE: printf("%s",str_false); break;
							case S_NIL: printf("%s",str_nil); break;
							case ID:
								node = Tree_Find_node(active, (char *)tac.operand1->value);
								switch (node->data->type_of)
								{
									case S_LITERAL: printf("%g",*((double *)op1)); break;
									case S_STR: printf("%s",((char *)op1)); break;
									case S_TRUE: printf("%s",str_true); break;
									case S_FALSE: printf("%s",str_false); break;
									case S_NIL: printf("%s",str_nil); break;
								}
								break;
						}
						dest_node->data->type_of = S_NIL;
						break;

					case TYPEOF:
						if((dest_node = Tree_Find_node(active, (char *)tac.operator->value))== NULL)
						{
							Tree_insert_new(active,tac.operator->value,0);
							dest_node = Tree_Find_node(active, (char *)tac.operator->value);
							dest_node->data->type = ID;
						}

						if((dest_node->data->value = (void*)malloc(sizeof(double)))== NULL) return INTER_ERROR;
						switch(tac.operand1->type)
						{
							case S_STR: *((double *) (dest_node->data->value))=8.0; break;
							case S_LITERAL: *((double *) (dest_node->data->value))=3.0; break;
							case S_TRUE: *((double *) (dest_node->data->value))=1.0; break;
							case S_FALSE: *((double *) (dest_node->data->value))=1.0; break;
							case S_NIL: *((double *) (dest_node->data->value))=0.0; break;
							case ID: node = Tree_Find_node(active, (char *)tac.operand1->value);
								if(node == NULL) *((double *) (dest_node->data->value))=6.0;
								else
								{
									switch(node->data->type_of)
									{
										case S_STR: *((double *) (dest_node->data->value))=8.0; break;
										case S_LITERAL: *((double *) (dest_node->data->value))=3.0; break;
										case S_TRUE: *((double *) (dest_node->data->value))=1.0; break;
										case S_FALSE: *((double *) (dest_node->data->value))=1.0; break;
										case S_NIL: *((double *) (dest_node->data->value))=0.0; break;
									}
								}
								break;
						}
						dest_node->data->type_of = S_LITERAL;
						break;

					case LEN:
						if((dest_node = Tree_Find_node(active, (char *)tac.operator->value))== NULL)
						{
							Tree_insert_new(active,tac.operator->value,0);
							dest_node = Tree_Find_node(active, (char *)tac.operator->value);
							dest_node->data->type = ID;
						}

						if((dest_node->data->value = (void*)malloc(sizeof(double)))== NULL) return INTER_ERROR;
						dest_node->data->type_of=S_LITERAL;
						if(tac.operand1->type==S_STR) *(double*)dest_node->data->value = strlen(op1);
						else if(tac.operand1->type == ID)
						{
							if(op1 != NULL && op1_node->data->type_of == S_STR) {*(double*)dest_node->data->value= strlen(op1);}
							else *(double*)dest_node->data->value = 0.0;
						}
						else *(double*)dest_node->data->value = 0.0;
						break;

					case FIND:
						if(op1_type != S_STR || op2_type != S_STR)
							return COMP_ERROR;
						if((dest_node = Tree_Find_node(active, (char *)tac.operator->value))== NULL)
						{
							Tree_insert_new(active,tac.operator->value,0);
							dest_node = Tree_Find_node(active, (char *)tac.operator->value);
							dest_node->data->type = ID;
						}

						if((dest_node->data->value = (double*)malloc(sizeof(double)))== NULL) return INTER_ERROR;
						*(double*)dest_node->data->value =(double) bi_find (op1, op2);
						dest_node->data->type_of=S_LITERAL;
						break;

					case SORT:
						if(op1_type == S_STR)
							return COMP_ERROR;
						if((dest_node = Tree_Find_node(active, (char *)tac.operator->value))== NULL)
						{
							Tree_insert_new(active,tac.operator->value,0);
							dest_node = Tree_Find_node(active, (char *)tac.operator->value);
							dest_node->data->type = ID;
						}

						dest_node->data->value = bi_sort(op1);
						dest_node->data->type_of = S_STR;
						break;

					case S_LEQ:
					case S_LST:
					case S_GEQ:
					case S_GRT:
					case S_EQ:
					case S_NEQ:
						Tree_insert_new(active,tac.operator->value,0);
						dest_node = Tree_Find_node(active, (char *)tac.operator->value);
						dest_node->data->type=ID;

						if(op1_type == S_LITERAL && op2_type == S_LITERAL)
						{
							switch(tac.operator->type)
							{
								case S_LST:
									if(*(double *)op1<*(double *)op2) dest_node->data->type_of = S_TRUE;
									else dest_node->data->type_of = S_FALSE;
									break;
								case S_LEQ:
									if(*(double *)op1<=*(double *)op2) dest_node->data->type_of = S_TRUE;
									else dest_node->data->type_of = S_FALSE;
									break;
								case S_GEQ:
									if(*(double *)op1>=*(double *)op2) dest_node->data->type_of = S_TRUE;
									else dest_node->data->type_of = S_FALSE;
									break;
								case S_GRT:
									if(*(double *)op1>*(double *)op2) dest_node->data->type_of = S_TRUE;
									else dest_node->data->type_of = S_FALSE;
									break;
								case S_EQ:
									if(*(double *)op1==*(double *)op2) dest_node->data->type_of = S_TRUE;
									else dest_node->data->type_of = S_FALSE;
									break;
								case S_NEQ:
									if(*(double *)op1!=*(double *)op2) dest_node->data->type_of = S_TRUE;
									else dest_node->data->type_of = S_FALSE;
									break;
							}
						}
						else if(op1_type == S_STR && op2_type == S_STR)
						{
							l_op1 = strlen(op1);
							l_op2 = strlen(op2);

							if (l_op1<l_op2) pom = l_op1;
							else pom =l_op2;

							pom2=0;
							pom3=0;
							while(pom2<=pom)
							{
								switch(tac.operator->type)
								{
									case S_LST:
										if(((char*)(op1))[pom2] > ((char*)(op2))[pom2] ) {dest_node->data->type_of = S_FALSE; pom3=1;}
										else if(((char*)(op1))[pom2] < ((char*)(op2))[pom2] ) {dest_node->data->type_of = S_TRUE; pom3=1;}
										else if(pom2==pom)
										{
											if(l_op1<l_op2) dest_node->data->type_of = S_TRUE;
											else {dest_node->data->type_of = S_FALSE; pom3=1;}
										}
										break;
									case S_LEQ:
										if(((char*)(op1))[pom2] > ((char*)(op2))[pom2] ) {dest_node->data->type_of = S_FALSE; pom3=1;}
										else if(((char*)(op1))[pom2] < ((char*)(op2))[pom2] ) {dest_node->data->type_of = S_TRUE; pom3=1;}
										else if(pom2==pom)
										{
											if(l_op1<=l_op2) dest_node->data->type_of = S_TRUE;
											else {dest_node->data->type_of = S_FALSE; pom3=1;}
										}
										break;
									case S_GEQ:
										if(((char*)(op1))[pom2] < ((char*)(op2))[pom2] ) {dest_node->data->type_of = S_FALSE; pom3=1;}
										else if(((char*)(op1))[pom2] > ((char*)(op2))[pom2] ) {dest_node->data->type_of = S_TRUE; pom3=1;}
										else if(pom2==pom)
										{
										    if(l_op1>=l_op2) dest_node->data->type_of = S_TRUE;
										    else {dest_node->data->type_of = S_FALSE; pom3=1;}
										}
										break;
									case S_GRT:
										if(((char*)(op1))[pom2] < ((char*)(op2))[pom2] ) {dest_node->data->type_of = S_FALSE; pom3=1;}
										else if(((char*)(op1))[pom2] > ((char*)(op2))[pom2] ) {dest_node->data->type_of = S_TRUE; pom3=1;}
										else if(pom2==pom)
										{
											if(l_op1>l_op2) dest_node->data->type_of = S_TRUE;
											else {dest_node->data->type_of = S_FALSE; pom3=1;}
										}
										break;
									case S_EQ:
										if(((char*)(op1))[pom2] != ((char*)(op2))[pom2] ) {dest_node->data->type_of = S_FALSE; pom3=1;}
										else if(pom2==pom)
										   {dest_node->data->type_of = S_TRUE; pom3=1;}
										break;
									case S_NEQ:
										if(((char*)(op1))[pom2] != ((char*)(op2))[pom2] ) {dest_node->data->type_of = S_TRUE; pom3=1;}
										else if(pom2==pom)
										    {dest_node->data->type_of = S_FALSE; pom3=1;}
										break;
								}
								pom2++;
								if(pom3>0)
									break;
							}
						}
						else
						{
							free_all(pst_function_index, pst_function );
							return COMP_ERROR;
						}
						break;

					case FUNCTION:
						i=tac.jmp;
						break;

					case S_END:
						if(tac.jmp!=-1)
							i=tac.jmp;
						else
						{
							Tree_Dispose(active);
							i=pst_function[pst_function_index].jmp;
							pst_function_index--;
							active=pst_function[pst_function_index].tabulka;
							if((dest_node = Tree_Find_node(active, (char *)((pole[i-1]).operator->value)))== NULL)
							{
								Tree_insert_new(active,(char *)((pole[i-1]).operator->value),0);
								dest_node = Tree_Find_node(active, (char *)((pole[i-1]).operator->value));
								dest_node->data->type = ID;
							}
							dest_node->data->type_of=S_NIL;
							dest_node->data->value=NULL;
						}
						break;

					case RETURN:
						i = pst_function[pst_function_index].jmp;
						pst_function[pst_function_index].jmp=-1;
						dest_node_pom=Tree_Find_node(active,(char *)tac.operand1->value);
						if(dest_node_pom==NULL)
							return OTHER_ERROR;
						pst_function_index--;
						if(pst_function_index<0)
							return PROG_OK;
						active=pst_function[pst_function_index+1].tabulka;

						if((dest_node=Tree_Find_node(active,(pole[i-1]).operator->value))== NULL)
						{
							Tree_insert_new(active,(char *)((pole[i-1]).operator->value),0);
							dest_node=Tree_Find_node(active,(pole[i-1]).operator->value);
							dest_node->data->type = ID;
						}

						if(dest_node_pom->data->type_of==S_STR)
						{
							dest_node->data->value=strduplicit((char **)(&(dest_node_pom->data->value)));
							dest_node->data->type_of = S_STR;
						}
						else if(dest_node_pom->data->type_of==S_LITERAL)
						{
							if((dest_node->data->value = (void*)malloc(sizeof(double)))== NULL) return INTER_ERROR;
							*((double *) (dest_node->data->value))= *((double *)dest_node_pom->data->value);
							dest_node->data->type_of = S_LITERAL;
						}
						else if (dest_node_pom->data->type_of == S_TRUE || dest_node_pom->data->type_of == S_FALSE  || dest_node_pom->data->type_of == S_NIL)
						{
							dest_node->data->type_of=dest_node_pom->data->type_of;
							dest_node->data->value= NULL;
						}
						Tree_Dispose (active);

						break;

					case WHILE:
					case IF:
						dest_node=Tree_Find_node(active, (char *)tac.operator->value);
						if(dest_node->data->type==S_FALSE||dest_node->data->type==S_NIL||
							(dest_node->data->type==S_LITERAL&&*((double*)dest_node->data->value)==0.0)||
							(dest_node->data->type==S_STR&&(((char*)(dest_node->data->value))[0])=='\0'))
							i=tac.jmp;
						else if(dest_node->data->type==ID)
							if(dest_node->data->type_of==S_FALSE||dest_node->data->type_of==S_NIL||
								(dest_node->data->type_of==S_LITERAL&&*((double*)dest_node->data->value)==0.0)||
								(dest_node->data->type_of==S_STR&&(((char*)(dest_node->data->value))[0])=='\0'))
								i=tac.jmp;

						break;

					case ELSE:
						i=tac.jmp;
						break;


					case S_EOF:
						return PROG_OK;
						break;
			}
		}
	} // konec else vetve, operand je neco jineho nez S_IS

	return PROG_OK;
}




