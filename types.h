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

#define preTableOffset = 20;

typedef enum {
	//KLICOVA SLOVA 
	ELSE = 0,
	FUNCTION = 1,
	IF = 2,
	RETURN = 3,
	WHILE = 4,
  	ELSEIF = 5, // elseif 
	

	//KONSTANTY
	C_FALSE = 6,
	C_NULL = 7,
	C_TRUE = 8,

	//SYMBOLY
	S_LITERAL = 9,
		
	
	S_LSBRA = 12,	// [
	S_RSBRA = 13,	// ]
	S_EOL = 14,
	
	S_FALSE = 17,
	S_STR = 18,	// retezec
	S_COLON = 19,	// :
	
	//nasledujici symboly jsou serazenz tak abz jima po odecteni ofsetu se dala adresovat precedencni tabulka
	S_PLUS = 20,	// +
	S_MUL = 21,		// *
	S_LBRA = 22,	// left bracket - (
	S_RBRA = 23,	// )
	S_IS = 24,	// =
	S_CONCATENATE = 25, // .
	S_DIV = 26,	// /
	S_MINUS = 27,	// -
	S_COMMA = 28,	// ,
	S_FUNC = 29, // funkce
	S_ID = 30,
	S_INT = 31,
	S_DOUB = 32,
	S_BOOL = 33,
	S_NULL = 34,
	S_DOLAR = 35, 	
	//konec, u tohle bloku co byl se nesmi zmenit cisla

	TYPS_OEF=36,
	S_TRUE=37,
	CUT_L=38,
	CUT_R=39,
	S_PHP = 40,
	S_SEM = 41, // ;
	S_BLOCK_START = 42,  // {
  	S_BLOCK_END = 43,  // }

  	S_EOF = 44,	
	S_LEQ = 45, // <=
	S_LST = 46, // <
	S_GEQ = 47, // >=
	S_GRT = 48, // >
	S_NEQ = 49, // !==
	S_EQ = 50, // ===
	W_RESERVED = 51, // ale tohle netreba posilat, tohle je lexikalni chyba   	
  	    	
  
	L_PARSE_ERROR = 70,
	L_FATAL_ERROR = 71,
	L_INTER_ERROR = 72

	}TERMINALY;

typedef enum {	NEXT_READ = 0,
				CONTINUE_READ = 1,
}FUn;

typedef struct { int type;
                 void *value;
} T_Token;




#endif
