//Implementace interpretu imperativního jazyka IFJ12.
//xradva00, Radvansky Matej
//xrajca00, Rajca Tomas

#ifndef EXPR_H_H
#define EXPR_H

typedef enum {
	PRS_SEQ=0,
	PRS_LEQ=1,
	PRS_LST=2,
	PRS_GEQ=3,
	PRS_GRT=4,
	PRS_NEQ=5,
	PRS_PLUS=6,
	PRS_MINUS=7,
	PRS_MUL=8,
	PRS_DIV=9,
	PRS_POW=10,
	PRS_LBRA=11,
	PRS_RBRA=12,
	PRS_I=13,
	PRS_EOL=14,

	PREQ=15,
	PRLST=16,
	PRGRT=17,
	PRERR=18,
	PREXPR=19,
}PRTABLEVALUE;

typedef struct {
	int top;
	int top_term;
	PRTABLEVALUE *items;
} Tstack, *PTstack;

typedef struct telem {
	struct telem *ptr;
	struct node*po;
} *telemptr;

typedef struct {
	telemptr top;
} TstackPX;

void sinitPX(TstackPX *s);
void pushPX(TstackPX *s, telemptr elem);
void popPX(TstackPX *s);
telemptr topPX(TstackPX s);
void showPX(TstackPX *s);
void freestackPX(TstackPX *s);

int tr_type(int type);
void push(PTstack stack, PRTABLEVALUE pom);
PRTABLEVALUE popt(PTstack stack);
void stack_destroy(PTstack stack);
void show(PTstack stack);
// int Expr(T_Token_struct *token, tList *L, T_bt_root **active)

#endif
