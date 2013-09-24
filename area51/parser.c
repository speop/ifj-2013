//Implementace interpretu imperativního jazyka IFJ12.
// xradva00, Radvansky Matej
// xrajca00, Rajca Tomas
// xpokor57, Pokorny Oto
#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "parser.h"
#include "list.h"
#include "ial.h"
#include "scanner.h"
#include "vnitrni_kod.h"

#define LEFT 1
#define RIGHT 2
#define N 3
#define NDEBUG 0				//zobrazeni komentaru kdyz NDEBUG 1


T_Token_struct token, pom_token;
struct node*n,*o,*st;	//pomocne ukazatele na uzly stromu
FILE *f;
tList L;	//seznam ukazatelu na pocatky stromu
T_bt_root symbol_table,*gts,*active;	//globalni a lokalni tabulka symbolu
T_bt_node *pom;	//pomocna


struct node* newnode(T_Token_struct data) {
	//vytvori novy uzel
	struct node* node = malloc(sizeof(struct node));
	node->data = data;
	node->left = NULL;
	node->right = NULL;
	return node;
}

void insert(struct node*child,struct node*parent,int dir)
{	//s dir==N vlozi do leva pokud je tam misto, jinak doprava pokud je tam misto
	//vklada child do parent
	if(dir==LEFT) parent->left = child;
	if(dir==RIGHT) parent->right = child;
	if(dir==N)
	{
	  if(parent->left==NULL) parent->left = child;
	  else if(parent->right==NULL) parent->right = child;
	}
}
void addtree()
{	//prida do seznamu stromu pocatek stromu
  	n=newnode(token);
	PostInsert(&L,n);
	Succ(&L);
}

void freetree (struct node*tree) {
	//uvolni strom
	if(tree!=NULL)
	{ //dokud je kam se posouvat
		if(tree->left!=NULL)
			freetree(tree->left);  //maz vlevo
		if(tree->right!=NULL)
			freetree(tree->right);  //maz vpravo
		free(tree);
  }
  return;
}

int prog()
{	//pocatek rek. sestupu
	int result=PROG_OK;

	if((getNextToken(&token)) == LEX_ERROR)
		return LEX_ERROR;

	#if NDEBUG==1
		printf("Jsem v progu %d\n", token.type);
	#endif
	switch (token.type)
	{
		case S_EOL:
		case FUNCTION:
		case IF:
		case WHILE:
		case RETURN:
		case ID:
		case S_EOF:
			result=StList();
			break;

		default:
			result=SYNTAX_ERROR;
			break;
	}

	return result;
}

int StList()
{
	#if NDEBUG==1
	printf("Jsem v StList %d\n", token.type);
	#endif
	int result=PROG_OK;
	switch(token.type)
	{
		case S_EOL:
		case FUNCTION:
		case IF:
		case WHILE:
		case RETURN:
		case ID:
			#if NDEBUG==1
			printf("\t Jsem v ? %d\n",token.type);
			#endif
			result=Stat();						//pravidlo 2: StList->Stat eol StList
			if(result!=PROG_OK){
				return result;}
			if(token.type!=S_EOL){
				return SYNTAX_ERROR;}
			if((getNextToken(&token)) == LEX_ERROR)
				return LEX_ERROR;
			result=StList();
			break;

		case S_EOF:
			#if NDEBUG==1
			printf("\t Jsem v EOF\n");
			#endif
			result=PROG_OK;
			break;

		case W_RESERVED:
			result=LEX_ERROR;
			break;

		default:
			result=SYNTAX_ERROR;
			break;
	}

	return result;
}

int Stat()
{

	#if NDEBUG==1
	printf("Jsem v Stat %d\n", token.type);
	#endif
	int result;
	char *name;

	switch(token.type)
	{
		case S_EOL:										//pravidlo 10: Stat->Eps
			#if NDEBUG==1
			printf("\t Jsem v S_EOL\n");
			#endif
			addtree();
			result=PROG_OK;
			break;
		case FUNCTION:									//pravidlo 4: Stat->function id ( Params ) S_EOL St_list S_END S_EOL
			#if NDEBUG==1
			printf("\t Jsem v function\n");
			#endif
			st=newnode(token);
			o=st;
			if(getNextToken(&token) == LEX_ERROR)			// id
				return LEX_ERROR;
			if(token.type != ID)
				return SYNTAX_ERROR;
			pom_token=token;
			n=newnode(token);		//pridani do ass
			insert(n,st,N);
 			pom=Tree_Find_node(gts,(char *)pom_token.value);
			active=pom->data->plts;
			if(getNextToken(&token) == LEX_ERROR)			// (
				return LEX_ERROR;
			if(token.type != S_LBRA)
				return SYNTAX_ERROR;

			result = Params(1);									// parametry )
			if (result != PROG_OK)
				return result;

			if(getNextToken(&token) == LEX_ERROR)			// EOL
				return LEX_ERROR;
			if(token.type != S_EOL)
				return SYNTAX_ERROR;
			PostInsert(&L,st);
			Succ(&L);
			if(getNextToken(&token) == LEX_ERROR)			// prikazy
				return LEX_ERROR;
			result = StList();

			if(token.type != S_END )
				return result;
			else
				result=PROG_OK;
			addtree();
			active=gts->CUk->data->plts;
			if(getNextToken(&token) == LEX_ERROR)			// EOL
				return LEX_ERROR;
			if(token.type != S_EOL)
				return SYNTAX_ERROR;
			break;

		case ID:												//pravidlo 5: Stat->id = Assign
			#if NDEBUG==1
			printf("\t Jsem v ID\n");
			#endif
			name=token.value;
			n=newnode(token);
			#if NDEBUG==1
			printf("ID value token:%s\n",(char*)token.value);
			printf("ID value node:%s\n",(char*)n->data.value);
			#endif

			if (getNextToken(&token) == LEX_ERROR)			// =
				return LEX_ERROR;
			if (token.type != S_IS)
				return SYNTAX_ERROR;

			o=newnode(token);			//pridani do ass
			insert(n,o,N);
			PostInsert(&L,o);
			Succ(&L);
			#if NDEBUG==1
			printf("ID value list:%s\n",(char*)L.Act->data->data.value);
			#endif
			result=Assign();
			if(Tree_insert_new(active,name,0)==-1)
				return INTER_ERROR;
			if(result!=PROG_OK){
				return result;}
			if (token.type != S_EOL)
				return SYNTAX_ERROR;
			break;

		case IF:												//pravidlo 6: Stat->if Expr S_EOL St_list else S_EOL StList S_END S_EOL
			#if NDEBUG==1
			printf("\t Jsem v IF\n");
			#endif
     		addtree();
			if (getNextToken(&token) == LEX_ERROR)
				return LEX_ERROR;

			result=Expr(&token, &L,active);											// expr
			if (result != PROG_OK)
				return result;

			if (token.type != S_EOL)							// EOL
				return SYNTAX_ERROR;
			if (getNextToken(&token) == LEX_ERROR)
				return LEX_ERROR;
			result = StList();									// prikazy
			if(token.type != ELSE )
				return result;
			else
				result=PROG_OK;
			addtree();
			if (getNextToken(&token) == LEX_ERROR)			// EOL
				return LEX_ERROR;
			if (token.type != S_EOL)
				return SYNTAX_ERROR;
			if (getNextToken(&token) == LEX_ERROR)
				return LEX_ERROR;
			result = StList();									// prikazy
			if(token.type != S_END )
				return result;
			else
				result=PROG_OK;
     	 	addtree();
			if(getNextToken(&token) == LEX_ERROR)			// EOL
				return LEX_ERROR;
			if(token.type != S_EOL)
				return SYNTAX_ERROR;
			break;

		case WHILE:											//pravidlo 7: Stat->while Expr S_EOL St_list S_END S_EOL
			#if NDEBUG==1
			printf("\t Jsem v WHILE\n");
			#endif
			addtree();
			if (getNextToken(&token) == LEX_ERROR)
				return LEX_ERROR;
			result=Expr(&token, &L,active);												// expr
			if (result != PROG_OK)
				return result;

			if (token.type != S_EOL)
				return SYNTAX_ERROR;
			if((getNextToken(&token)) == LEX_ERROR)			// prikazy
				return LEX_ERROR;
			result = StList();
     		addtree();
			if(token.type != S_END )
				return result;
			else
				result=PROG_OK;
			if((getNextToken(&token)) == LEX_ERROR)			// EOL
				return LEX_ERROR;
			if( token.type != S_EOL)
				return SYNTAX_ERROR;
			break;

		case RETURN:													//pravidlo 8: Stat->return Expr S_EOL
			#if NDEBUG==1
			printf("\t Jsem v RETURN\n");
			#endif
			addtree();
			if (getNextToken(&token) == LEX_ERROR)
				return LEX_ERROR;
			result=Expr(&token, &L,active);												// expr
			if (result != PROG_OK)
				return result;
			if (token.type != S_EOL)
				return SYNTAX_ERROR;
			break;

		default:
			#if NDEBUG==1
			printf("\t SYNTAX_ERROR\n");
			#endif
			result=SYNTAX_ERROR;
			break;
	}
	return result;
}

int Params(int insert_ts)
{
	//parametry fci
	#if NDEBUG==1
	printf("Jsem v Params %d\n", token.type);
	#endif
	int result;

	if (getNextToken(&token) == LEX_ERROR)
		return LEX_ERROR;
	#if NDEBUG==1
	printf("Jsem v Params %d\n", token.type);
	#endif
	switch (token.type)
	{
		case S_LITERAL:
		case S_STR:
		case ID:													//11: Params->id Params_n
			n=newnode(token);
			insert(n,o,RIGHT);
			o=n;
			if(token.type==ID)
			{
				if(insert_ts==0)
				{
					if(Tree_Find_node(active,token.value)==NULL)	//prohleda aktivni tabulku symbolu
						return SEM_ERROR_UVAR;
				}
				else
				{
					if(Tree_insert_new(active,token.value,0)==-1)
						return INTER_ERROR;
				}
			}
			if((getNextToken(&token)) == LEX_ERROR)
				return LEX_ERROR;
			result = Params_n(insert_ts);
			break;

		case S_RBRA:	// )
			result=PROG_OK;
			break;

		default:
			result=SYNTAX_ERROR;
			break;
	}
	return result;
}

int Params_n(int insert_ts)
{	//nasledujici parametry fci
	#if NDEBUG==1
	printf("Jsem v Params_n %d\n", token.type);
	#endif
	int result;

	switch(token.type)
	{
		case S_RBRA:										//14: Params_n->Eps
			result=PROG_OK;
			break;

		case S_COMMA:										//pravidlo 13: Params_n->, id Params_n
			if(getNextToken(&token)==LEX_ERROR)
				return LEX_ERROR;
			if(token.type!=ID&&token.type!=S_LITERAL&&token.type!=S_STR)
			  return SYNTAX_ERROR;
			if(token.type==ID)
			{
				if(insert_ts==0)
				{
					if(Tree_Find_node(active,token.value)==NULL)
						return SEM_ERROR_UVAR;
				}
				else
				{
					insert_ts++;
					if(Tree_insert_new(active,token.value,0)==-1)
						return INTER_ERROR;
				}
			}
			n=newnode(token);		//pridani do ass
			insert(n,o,RIGHT);
			o=n;

			if(getNextToken(&token)==LEX_ERROR)				//nacteni dalsiho znaku pred zavolanim rekurze
				return LEX_ERROR;
			result=Params_n(insert_ts);
			break;

		default:
			result=SYNTAX_ERROR;
			break;
	}
	return result;
}

int Term()
{
	#if NDEBUG==1
	printf("Jsem v Termu %d\n", token.type);
	#endif
	switch(token.type)
	{
		case ID:							//19: Term->id
		case S_FALSE:					//23: Term->false
		case S_LITERAL:				//22: Term->literal
		case S_STR:						//21: Term->““
		case S_NIL:						//20: Term->nil
			return PROG_OK;
			break;

		default:
			return SYNTAX_ERROR;
			break;
	}
	return SYNTAX_ERROR;
}

int Opr()
{	//kontrola korektnosti operaci
	#if NDEBUG==1
	printf("Jsem v Opr %d\n", token.type);
	#endif
	switch(token.type)
	{
		case S_PLUS:
		case S_MINUS:
		case S_MUL:
		case S_DIV:
		case S_POW:
		case S_LEQ:
		case S_LST:
		case S_GEQ:
		case S_GRT:
		case S_NEQ:
		case S_EQ:
			return PROG_OK;
			break;

		default:
			return SYNTAX_ERROR;
			break;
	}
	return SYNTAX_ERROR;
}

int Func()
{	//vestavene fce
	switch(token.type)
	{
		case SORT:
		case FIND:
		case LEN:
		case TYPEOF:
		case PRINT:
		case NUMERIC:
		case INPUT:
			return PROG_OK;
	}
	return SYNTAX_ERROR;
}

int Assign()
{	//prirazeni id=..
	#if NDEBUG==1
	printf("Jsem v Assign %d\n", token.type);
	#endif
	if (getNextToken(&token) == LEX_ERROR)
		return LEX_ERROR;


	#if NDEBUG==1
		printf("Jsem v Assign %d\n", token.type);
	#endif
	int result;
	switch(token.type)
	{
		case S_EOL: //nechal sem to tak jak sem to mel predtim
		case ID:
		case S_PLUS:
		case S_MINUS:
		case S_MUL:
		case S_DIV:
		case S_POW:
		case S_LEQ:
		case S_LST:
		case S_GEQ:
		case S_GRT:
		case S_NEQ:
		case S_EQ:
		case S_FALSE:
		case S_LITERAL:
		case S_NIL:
		case S_LBRA:
			#if NDEBUG==1
			printf("\tznaky\n");
			#endif

			if(token.type==ID)
			{
				n=newnode(token);		//pridani do ass
				insert(n,o,N);
				o=o->right;
				T_bt_node *back_var=Tree_Find_node(active, token.value), *back_func=Tree_Find_node(gts, token.value);	//prohledani tabulek
				if(back_var==NULL&&back_func==NULL)
				{
					getNextToken(&token);		//z duvodu ze muze nasledovat i nejaka tecka
					if(token.type==S_LBRA)
						result=SEM_ERROR_UFUNC;	//nedefinovana fce
					else
						result=SEM_ERROR_UVAR;	//nedefinovana promenna
				}

				if(back_var!=NULL&&back_func==NULL)
					result=Expr(&token, &L,active);	//pokracuj do expr
				if(back_func!=NULL)
				{	//parametry
					getNextToken(&token);
					result=Params(0);
					if(getNextToken(&token) == LEX_ERROR)
						return LEX_ERROR;
				}
			}
			else
				result=Expr(&token, &L,active);	//pokracuj do expr

			break;

		case S_STR:									//36: Assign->str [ Term : Term ]
			#if NDEBUG==1
			printf("\tretezec\n");
			#endif
			pom_token=token;		//pridani do ass
			n=newnode(token);
			insert(n,o,N);
			if(getNextToken(&token) == LEX_ERROR) //retezec
				return LEX_ERROR;


			if(Opr()==PROG_OK||token.type==S_EOL)
			{	//pokracu jdo expr
			  result=Expr(&token, &L,active);
			}
			else if(token.type == S_LSBRA)
			{	// [
				if(getNextToken(&token) == LEX_ERROR)
					return LEX_ERROR;

				if(Term()==PROG_OK) //od
				{
					n=newnode(token);
					n->d_type=CUT_L;
					insert(n,o->right,LEFT); //vlozi doleva k retezci hodnotu od
					result=PROG_OK;
					if(getNextToken(&token) == LEX_ERROR)
						return LEX_ERROR;
				}

				if(token.type != S_COLON) //:
					return SYNTAX_ERROR;

				if(getNextToken(&token) == LEX_ERROR)
					return LEX_ERROR;

				if(Term()==PROG_OK) //do
				{
					n=newnode(token);
					n->d_type=CUT_R;
					insert(n,o->right,RIGHT); //vlozi doleva k retezci hodnotu do
					result=PROG_OK;
					if(getNextToken(&token) == LEX_ERROR)
						return LEX_ERROR;
				}

				if(token.type != S_RSBRA)	// ]
					return SYNTAX_ERROR;
				if(getNextToken(&token) == LEX_ERROR)
					return LEX_ERROR;
			}
			else
				return SYNTAX_ERROR;

			break;

		case SORT:									//9: Assign->Func ( Params )
		case FIND:
		case LEN:
		case TYPEOF:
		case PRINT:
		case NUMERIC:
		case INPUT:
			#if NDEBUG==1
			printf("\tfunkce\n");
			#endif
			n=newnode(token); 		//pridani do ass
			insert(n,o,N);
			o=n;
			if(getNextToken(&token) == LEX_ERROR)
				return LEX_ERROR;
			if(token.type != S_LBRA)
				return SYNTAX_ERROR;
			result=Params(0);	//parametry
			if (result != PROG_OK)
				return result;
			if(getNextToken(&token) == LEX_ERROR)
				return LEX_ERROR;
			break;

		default:
			result=SYNTAX_ERROR;
			break;
	}
	return result;
}

int parser()
{
	gts=malloc(sizeof(T_bt_root));	//globalni tabulka symbolu
	struct node*n=NULL;
	InitList(&L);
	n=newnode(token); //hlavicka
	InsertFirst(&L,n);
	First(&L);

	Tree_insert_new(gts,"@main",1);	//vlozeni halvni fce do tabulky symbolu
	active=gts->CUk->data->plts;
	char *func_name;
	while((func_name=first_pass())!='\0')	//prvni pruchod pro semantiku funkci
		Tree_insert_new(gts,func_name,1);

	int result=prog();	//rekurzivni sestup
	#if NDEBUG==1
	printf("symbol table:\n");
	if(gts->CUk!=NULL)
		Tree_show(gts->CUk);
	printf("vypis gts->plts\n");
	#endif
	tElemPtr pom;
	pom=L.First;
	L.First=L.First->ptr; //zruseni hlavicky


	if(result>PROG_OK)
	{
		//pokud se preklad bude ukoncovat s chybou, uvolni strom
		free(pom);
		DisposeList(&L);
		return result;
	}

	result=vnitrni(&L,active);		//pokracovani na generator vnitrniho kodu
	free(pom);
	DisposeList(&L);

	return result;	//vrat navratovou hodnotu
}
