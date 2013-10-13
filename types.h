//xbucht18, Buchta David
#ifndef TYPES_H
#define TYPES_H

typedef enum{
	OK = 0,
	ERROR_LEX = 1,
	ERROR_SYN = 2,
	
	SEM_DEF_ERROR = 3,
	SEM_MISSING_PARAMETER = 4,
	SEM_UNDECLARED_PARAMETER = 5,
	SEM_ZERRO_DIVISION = 10,	
	SEM_RETYPE = 11,
	SEM_TYPE_ERROR = 12,
	SEM_OTHER_ERROR = 13,
	ERROR_INTER = 99,
	
}RETURN_VALUES;

typedef enum {
	//KLICOVA SLOVA 
	ELSE = 1,
	FUNCTION = 2,
	IF = 3,
	RETURN = 4,
	WHILE = 5,

	//KONSTANTY
	C_FALSE = 6,
	C_NULL = 7,
	C_TRUE = 8,

	//SYMBOLY
	S_LITERAL = 9,
	S_LBRA = 10,	// left bracket - (
	S_RBRA = 11,	// )
	S_LSBRA = 12,	// [
	S_RSBRA = 13,	// ]
	S_EOL = 14,
	S_DOLAR = 15, //nevim jestli to neni zbytecne
	S_COMMA = 16,	// ,
	S_FALSE = 17,
	S_STR = 18,	// retezec
	S_COLON = 19,	// :
	S_PLUS = 20,	// +
	S_MINUS = 21,	// -
	S_DIV = 22,	// /
	S_MUL = 23,	// *
	S_POW = 24,	// mocnina
	S_EOF = 25,
	S_IS = 26,	// =
	S_LEQ = 27, // <=
	S_LST = 28, // <
	S_GEQ = 29, // >=
	S_GRT = 30, // >
	S_NEQ = 31, // !==
	S_EQ = 32, // ===
	S_BOOL = 33,
	S_ID = 34,
	W_RESERVED = 35,
	TYPS_OEF=36,
	S_TRUE=37,
	CUT_L=38,
	CUT_R=39,
	S_PHP = 40,
	S_SEM = 41, // ;
	S_BLOCK_START = 42,  // {
  	S_BLOCK_END = 43,  // }
  	S_EOF = 44, // konec souboru
  	S_CONCATENATE = 45, // .
  	
  	 //KOMENTARE
  	S_BLOCK_COMMENT = 46, // /*
  	S_END_BLOCK_COMMENT = 47, // */
  	S_LINE_COMMENT = 48, // //
  	
  	S_NUMBER = 49, // cislo
  	S_ELSE = 50, // else
  	S_ELSEIF = 51, // elseif 
	
	L_PARSE_ERROR = 70,
	L_FATAL_ERROR = 71,
	L_INTER_ERROR = 72

	}TERMINALY;

typedef struct { int type;
                 void *value;
} T_Token;




#endif
