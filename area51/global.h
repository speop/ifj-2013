//Implementace interpretu imperativního jazyka IFJ12.
//xrajca00,Rajca Tomas

#ifndef GLOBAL_H
#define GLOBAL_H

typedef enum{
	PROG_OK=0,
	LEX_ERROR=1,
	SYNTAX_ERROR=2,
	SEM_ERROR_UVAR=3,
	SEM_ERROR_UFUNC=4,
	SEM_ERROR=5,
	DIV_NUL_ERROR=10,
	COMP_ERROR=11,
	NUMERIC_ERROR=12,
	OTHER_ERROR=13,
	INTER_ERROR=99,
}navratove_hodnoty;

typedef enum {
	FUNCTION = 1,
	IF = 2,
	ELSE = 3,
	WHILE = 4,
	RETURN = 5,
	SORT = 6,
	FIND = 7,
	LEN = 8,
	TYPEOF = 9,
	PRINT = 10,
	NUMERIC = 11,
	INPUT = 12,
	S_END = 13,
	ID = 14,
	S_LITERAL = 15,
	S_LBRA = 16,	// left bracket - (
	S_RBRA = 17,	// )
	S_LSBRA = 18,	// left square bracket (ne podprsenka velikosti 18 :D ) - [
	S_RSBRA = 19,	// ]
	S_EOL = 20,
	S_NIL = 21,
	S_DOLAR = 22,
	S_COMMA = 23,	// ,
	S_FALSE = 24,
	S_STR = 25,	// retezec
	S_COLON = 26,	// :
	S_PLUS = 27,	// +
	S_MINUS = 28,	// -
	S_MUL = 29,	// /
	S_DIV = 30,	// *
	S_POW = 31,	// mocnina
	S_EOF = 32,
	S_IS = 33,	// =
	S_LEQ = 34, // <=
	S_LST = 35, // <
	S_GEQ = 36, // >=
	S_GRT = 37, // >
	S_NEQ = 38, // !=
	S_EQ = 39, // ==
	S_BOOL = 40,
	S_VAR = 41,
	W_RESERVED = 42,
	TYPS_OEF=43,
	S_TRUE=44,
	CUT_L=45,
	CUT_R=46,
	L_PARSE_ERROR = 70,
	L_FATAL_ERROR = 71,
	L_INTER_ERROR = 72

	}TERMINALY;

typedef struct {int state;
                char previous;
                int type;
                void *value;
} T_Token_struct;

void print_error(int result);

#endif

