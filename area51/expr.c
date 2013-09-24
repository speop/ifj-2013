//Implementace interpretu imperativního jazyka IFJ12.
//xradva00, Radvansky Matej
//xrajca00, Rajca Tomas

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expr.h"
#include "parser.h"
#include "list.h"
#include "ial.h"
#include "scanner.h"

#define NDEBUG 0						//zobrazeni komentaru kdyz NDEBUG 1
#define FIRST_ARRAY_SIZE	20

int stack_size=FIRST_ARRAY_SIZE-1;

static int prtable [15][15] = {	//precedencni tabulka
/*			0			1				2			3			4			5			6			7			8			9			10			11			12			13			14
			==			<=				<			>=			>			!=			+			-			*			/			**			(			)			i			$	*/
/*0 == */ 	{PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRGRT,	PRLST,	PRGRT},
/*1 <= */	{PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRGRT,	PRLST,	PRGRT},
/*2 <  */	{PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRGRT,	PRLST,	PRGRT},
/*3 >= */	{PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRGRT,	PRLST,	PRGRT},
/*4 >  */	{PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRGRT,	PRLST,	PRGRT},
/*5 != */	{PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRGRT,	PRLST,	PRGRT},
/*6 +  */	{PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRLST,	PRLST,	PRLST,	PRLST,	PRGRT,	PRLST,	PRGRT},
/*7 -  */	{PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRLST,	PRLST,	PRLST,	PRLST,	PRGRT,	PRLST,	PRGRT},
/*8 *  */	{PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRLST,	PRLST,	PRGRT,	PRLST,	PRGRT},
/*9 /  */	{PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRLST,	PRLST,	PRGRT,	PRLST,	PRGRT},
/*0 ** */ 	{PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRLST,	PRGRT,	PRLST,	PRGRT},
/*1 (  */	{PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PREQ,		PRLST,	PRERR},
/*2 )  */	{PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRERR,	PRGRT,	PRERR,	PRGRT},
/*3 i  */	{PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRGRT,	PRERR,	PRGRT,	PRERR,	PRGRT},
/*4 $  */	{PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRLST,	PRERR,	PRLST,	PRERR},
};

int tr_type(int type)
{	//prevod pro stavy prec. tabulky
	switch(type)
	{
		case S_LBRA: return PRS_LBRA;
		case S_RBRA: return PRS_RBRA;
		case S_EOL: return PRS_EOL;
		case S_PLUS: return PRS_PLUS;
		case S_MINUS: return PRS_MINUS;
		case S_MUL: return PRS_MUL;
		case S_DIV: return PRS_DIV;
		case S_POW: return PRS_POW;
		case S_EQ: return PRS_SEQ;
		case S_LEQ: return PRS_LEQ;
		case S_LST: return PRS_LST;
		case S_GEQ: return PRS_GEQ;
		case S_GRT: return PRS_GRT;
		case S_NEQ: return PRS_NEQ;
		case S_LITERAL: return PRS_I;
		case ID: return PRS_I;
		case S_STR: return PRS_I;
		default: return PRERR;
	}
	return PRERR;
}

int rtr_type(int type)
{
	switch(type)
	{
		case PRS_LBRA: return S_LBRA;
		case PRS_RBRA: return S_RBRA;
		case PRS_EOL:	return S_EOL;
		case PRS_PLUS:	return S_PLUS;
		case PRS_MINUS:return S_MINUS;
		case PRS_MUL:	return S_MUL;
		case PRS_DIV:	return S_DIV;
		case PRS_POW:	return S_POW;
		case PRS_SEQ:	return S_EQ;
		case PRS_LEQ:	return S_LEQ;
		case PRS_LST:	return S_LST;
		case PRS_GEQ:	return S_GEQ;
		case PRS_GRT:	return S_GRT;
		case PRS_NEQ:	return S_NEQ;
		default: return PRERR;
	}
	return PRERR;
}

//fce pro zasobnik uzlu (struct node *)
void sinitPX(TstackPX *s)
{
	s->top=NULL;
}

void pushPX(TstackPX *s, telemptr elem)
{
	elem->ptr=s->top;
	s->top=elem;
}

void popPX(TstackPX *s)
{
	telemptr pom;
	if(s->top!=NULL)
	{
		pom=s->top;
		s->top=s->top->ptr;
		free(pom);
	}
}

telemptr topPX(TstackPX s)
{
	return s.top;
}

void freestackPX(TstackPX *s)
{
  while(s->top!=NULL)
  {
    popPX(s);
  }
}
//konec fci pro zasobnik uzlu

void push(PTstack stack, PRTABLEVALUE pom)
{
	if(stack->top==stack_size)
		stack->items=realloc(stack->items,stack_size*2);
	stack->items[++stack->top]=pom;
}

PRTABLEVALUE popt(PTstack stack)
{
	if(stack->top<0)
	{
		return 0;
	}
	else
		return stack->items[(stack->top)--];
}

void stack_destroy(PTstack stack)
{
	free(stack->items);
	free(stack);
}

void show(PTstack stack)
{
	for(int i=0;i<=stack->top;i++)
		printf("%d-", stack->items[i]);
	printf("\n");
}

int Expr(T_Token_struct *token, tList *L,T_bt_root *active)
{	//fce pro vyhodnocovani vyrazu
	#if NDEBUG==1
 	printf("----------exprrr---------\n");
	#endif
 	int first=TRUE;	//pro ID=a napr
 	int first_string=FALSE;
	int f=TRUE;
	int eco=PROG_OK;	//pomocny navratovy stav
	int stop=FALSE;	//ukonceni podminky pro puchod operaci
 	struct node*operation=NULL;	//vrchol stromu pro expr
	PTstack stack=(PTstack)malloc(sizeof(Tstack));
	if(stack==NULL)
		return INTER_ERROR;
	TstackPX s;
	sinitPX(&s);
	telemptr elem;	//pomocna pro alokaci uzlu
	stack->top=-1;
	stack->top_term=0;
	T_Token_struct undef;	//pro alokaci uzlu pro operaci
	undef.value="o";
	undef.type=888;
	if((stack->items=malloc(FIRST_ARRAY_SIZE*sizeof(PRTABLEVALUE)))==NULL)
		return INTER_ERROR;
	int result=PROG_OK,tr_token_type,pom,t;

	push(stack, PRS_EOL);

	if(token->type==ID)
		if(Tree_Find_node(active,(char*)token->value)==NULL)
			return SEM_ERROR_UVAR;

	if(L->Act->data->right==NULL || L->Act->data->right->data.type!=S_STR)
		tr_token_type=tr_type(token->type);
	else
	{
		tr_token_type=tr_type(L->Act->data->right->data.type);
		first_string=TRUE;
	}
	if(tr_token_type!=PRS_I&&tr_token_type!=PRS_LBRA)
		return SYNTAX_ERROR;

	do{
		switch(prtable[stack->items[stack->top_term]][tr_token_type])
		{
			case PREQ:

				push(stack,tr_token_type);
				stack->top_term=stack->top;
				if(first_string!=TRUE)
				{
					if((result=getNextToken(token))!=PROG_OK)
						return result;
					if(token->type==ID)
						if(Tree_Find_node(active,(char*)token->value)==NULL)
							return SEM_ERROR_UVAR;
				}
				first_string=FALSE;
				tr_token_type=tr_type(token->type);
				break;

			case PRLST:
				if(tr_token_type>=PRS_LBRA)
					push(stack,PRLST);
				else
				{
					pom=popt(stack);
					push(stack,PRLST);
					push(stack,pom);
				}

				push(stack,tr_token_type);
				stack->top_term=stack->top;
				t=token->type;	//ulozeni type(kvuli zasobnikum)
				if(first_string!=TRUE)
				{
					if((result=getNextToken(token))!=PROG_OK)
						return result;
					if(token->type==ID)
						if(Tree_Find_node(active,(char*)token->value)==NULL)
							return SEM_ERROR_UVAR;
				}
				first_string=FALSE;
				tr_token_type=tr_type(token->type);

				break;

			case PRGRT:
				if(stack->items[stack->top]<PRS_LBRA||stack->items[stack->top]==PRS_I)
				{
					pom=popt(stack);
					if(stack->items[stack->top]!=PRLST)
					{
						result=SYNTAX_ERROR;
						break;
					}
					#if NDEBUG==1
					printf("|%s-",(char*)token->value);		//tady vyhazuje promenne
					#endif

					elem=malloc(sizeof(struct telem));	//novy uzel pro promennou
					if(elem==NULL)
					{
						return INTER_ERROR;
					}
					elem->po=newnode(*token);
					if(L->Act->data->right!=NULL&&L->Act->data->right->data.type==S_STR&&f==TRUE)
					{	//kvuli retezcum kdyz sou prvn v poradi
						elem->po->data.type=S_STR;
						f=FALSE;
					}
					else
						elem->po->data.type=t;
					pushPX(&s,elem);	//vloz do zasobniku

					pom=popt(stack);
					stack->top_term=stack->top;
					push(stack,PREXPR);
				}
				else if(stack->items[stack->top_term]==PRS_RBRA)
				{
					pom=popt(stack);
					if(popt(stack)!=PREXPR ||
						popt(stack)!=PRS_LBRA ||
						popt(stack)!=PRLST){result=SYNTAX_ERROR;break;}
					stack->top_term=stack->top;
					push(stack,PREXPR);
				}
				else
				{

					#if NDEBUG==1
					printf("%d\n",rtr_type(stack->items[stack->top_term]));							//tady se vyhazuje operatory
					#endif
					first=FALSE;
					operation=newnode(undef);	//novy uzel pro operator
					operation->data.type=rtr_type(stack->items[stack->top_term]);
					elem=topPX(s);										//vytahne promennou ze zasobniku
					insert(elem->po,operation,RIGHT);		//vlozi ji do uzlu operace
					popPX(&s);
					elem=topPX(s);										//dalsi
        			insert(elem->po,operation,LEFT);
					popPX(&s);
					elem=malloc(sizeof(struct telem));	//pop ji dealokoval, je potreba vlozit operaci do zasobniku
					if(elem==NULL)
						return INTER_ERROR;
					elem->po=operation;
					pushPX(&s,elem);

			if(stop==FALSE)
			{	//porovnani semantiky pro chybu 5
				if(operation->left->data.type==ID||operation->right->data.type==ID)
				{	//pokud narazil na promennou, nelze v tomto stadiu vyhodnotit typ vyrazu
						eco=PROG_OK;
						stop=TRUE;
				}
				else if((operation->right->data.type>=S_PLUS&&operation->right->data.type<=S_POW)&&(operation->left->data.type>=S_PLUS&&operation->left->data.type<=S_POW))
				{	//oba detske uzly jsou operace
					eco=SEM_ERROR;
					switch(operation->data.type)
					{
						case S_EQ:
							eco=PROG_OK;
							break;

						case S_PLUS:
							if(operation->left->d_type==S_LITERAL&&operation->right->d_type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							if(operation->left->d_type==S_STR&&operation->right->d_type==S_STR)
							{
								eco=PROG_OK;
								operation->d_type=S_STR;
							}
							if(operation->left->d_type==S_STR&&operation->right->d_type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_STR;
							}
							break;

						case S_MUL:
							if(operation->left->d_type==S_STR&&operation->right->d_type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_STR;
							}
							if(operation->left->d_type==S_LITERAL&&operation->right->d_type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							break;

						case S_MINUS:
							if(operation->left->d_type==S_LITERAL&&operation->right->d_type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
						case S_DIV:
							if(operation->left->d_type==S_LITERAL&&operation->right->d_type==S_LITERAL )
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							break;

						case S_POW:
							if(operation->left->d_type==S_LITERAL&&operation->right->d_type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							break;

						default:
							if(operation->data.type>=S_LEQ &&operation->data.type<=S_GRT)
							{
								eco=SEM_ERROR;
								if(operation->left->d_type==S_LITERAL&&operation->right->d_type==S_LITERAL)
								{
									eco=PROG_OK;
									operation->d_type=S_LITERAL;
								}
								if(operation->left->d_type==S_STR&&operation->right->d_type==S_STR)
								{
									eco=PROG_OK;
									operation->d_type=S_STR;
								}
							}
							break;
					}
				}
				else	if(operation->left->data.type>=S_PLUS&&operation->left->data.type<=S_POW)
				{	//levy je operace
					eco=SEM_ERROR;
					switch(operation->data.type)
					{
						case S_EQ:
							eco=PROG_OK;
							break;

						case S_PLUS:
							if(operation->left->d_type==S_LITERAL&&operation->right->data.type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							if(operation->left->d_type==S_STR&&operation->right->data.type==S_STR)
							{
								eco=PROG_OK;
								operation->d_type=S_STR;
							}
							if(operation->left->d_type==S_STR&&operation->right->data.type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_STR;
							}
							break;

						case S_MUL:
							if(operation->left->d_type==S_STR&&operation->right->data.type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_STR;
							}
							if(operation->left->d_type==S_LITERAL&&operation->right->data.type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							break;

						case S_MINUS:
							if(operation->left->d_type==S_LITERAL&&operation->right->data.type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
						case S_DIV:
							if(operation->left->d_type==S_LITERAL&&operation->right->data.type==S_LITERAL )
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							break;

						case S_POW:
							if(operation->left->d_type==S_LITERAL&&operation->right->data.type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							break;

						default:
							if(operation->data.type>=S_LEQ &&operation->data.type<=S_GRT)
							{
								eco=SEM_ERROR;
								if(operation->left->d_type==S_LITERAL&&operation->right->data.type==S_LITERAL)
								{
									eco=PROG_OK;
									operation->d_type=S_LITERAL;
								}
								if(operation->left->d_type==S_STR&&operation->right->data.type==S_STR)
								{
									eco=PROG_OK;
									operation->d_type=S_STR;
								}
							}
							break;
					}
				}
				else if(operation->right->data.type>=S_PLUS&&operation->right->data.type<=S_POW)
				{	//pravy je operace
					eco=SEM_ERROR;
					switch(operation->data.type)
					{
						case S_EQ:
							eco=PROG_OK;
							break;

						case S_PLUS:
							if(operation->left->data.type==S_LITERAL&&operation->right->d_type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							if(operation->left->data.type==S_STR&&operation->right->d_type==S_STR)
							{
								eco=PROG_OK;
								operation->d_type=S_STR;
							}
							if(operation->left->data.type==S_STR&&operation->right->d_type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_STR;
							}
							break;

						case S_MUL:
							if(operation->left->data.type==S_STR&&operation->right->d_type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_STR;
							}
							if(operation->left->data.type==S_LITERAL&&operation->right->d_type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							break;

						case S_MINUS:
							if(operation->left->data.type==S_LITERAL&&operation->right->d_type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
						case S_DIV:
							if(operation->left->data.type==S_LITERAL&&operation->right->d_type==S_LITERAL )
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							break;

						case S_POW:
							if(operation->left->data.type==S_LITERAL&&operation->right->d_type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							break;

						default:
							if(operation->data.type>=S_LEQ &&operation->data.type<=S_GRT)
							{
								eco=SEM_ERROR;
								if(operation->left->data.type==S_LITERAL&&operation->right->d_type==S_LITERAL)
								{
									eco=PROG_OK;
									operation->d_type=S_LITERAL;
								}
								if(operation->left->data.type==S_STR&&operation->right->d_type==S_STR)
								{
									eco=PROG_OK;
									operation->d_type=S_STR;
								}
							}
							break;
					}
				}
				else
				{	//zadny neni operace
					eco=SEM_ERROR;
					switch(operation->data.type)
					{
						case S_EQ:
							eco=PROG_OK;
							break;

						case S_PLUS:
							if(operation->left->data.type==S_LITERAL&&operation->right->data.type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							if(operation->left->data.type==S_STR&&operation->right->data.type==S_STR)
							{
								eco=PROG_OK;
								operation->d_type=S_STR;
							}
							if(operation->left->data.type==S_STR&&operation->right->data.type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_STR;
							}
							break;

						case S_MUL:
							if(operation->left->data.type==S_STR&&operation->right->data.type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_STR;
							}
							if(operation->left->data.type==S_LITERAL&&operation->right->data.type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							break;

						case S_MINUS:
							if(operation->left->data.type==S_LITERAL&&operation->right->data.type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
						case S_DIV:
							if(operation->left->data.type==S_LITERAL&&operation->right->data.type==S_LITERAL )
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							break;

						case S_POW:
							if(operation->left->data.type==S_LITERAL&&operation->right->data.type==S_LITERAL)
							{
								eco=PROG_OK;
								operation->d_type=S_LITERAL;
							}
							break;

						default:
							if(operation->data.type>=S_LEQ &&operation->data.type<=S_GRT)
							{
								eco=SEM_ERROR;
								if(operation->left->data.type==S_LITERAL&&operation->right->data.type==S_LITERAL)
								{
									eco=PROG_OK;
									operation->d_type=S_LITERAL;
								}
								if(operation->left->data.type==S_STR&&operation->right->data.type==S_STR)
								{
									eco=PROG_OK;
									operation->d_type=S_STR;
								}
							}
							break;
					}
				}
				if(eco!=PROG_OK)
				{
					result=eco;
					break;
				}
			}


					#if NDEBUG==1
					printf("---------------JEDEN PRUCHOD------\n");
					#endif
					if(stack->items[stack->top]!=PREXPR)
					{
						result=SYNTAX_ERROR;
						break;
					}
					pom=popt(stack);
					pom=popt(stack);
					if(stack->items[stack->top]!=PREXPR)
					{
						result=SYNTAX_ERROR;
						break;
					}
					pom=popt(stack);
					pom=popt(stack);
					stack->top_term=stack->top;
					push(stack,PREXPR);

				}
				break;
			default:
				result=SYNTAX_ERROR;
				break;

		}

	if(result>PROG_OK)
		break;
	}while(tr_token_type!=PRS_EOL||stack->items[stack->top_term]!=PRS_EOL);	//dokud je s cim pracovat
    if(first==TRUE&&result==PROG_OK)
    { 	//	osetreni pro id=b napr
     	insert(elem->po,L->Act->data,RIGHT);
    }
    else
    	insert(operation,L->Act->data,RIGHT);
	stack_destroy(stack);
	freestackPX(&s);
	return result;
}
