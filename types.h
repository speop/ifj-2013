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
	S_STR = 33,	// retezec
	S_BOOL = 34,
	S_NULL = 35,	
	S_DOLAR = 36,
	S_LST = 37,  // <
	S_GRT = 38, // >
	S_LEQ = 39, // <=
	S_GEQ = 40, // >=
	S_EQ = 41, // ===
	S_NEQ = 42, // !==
	//konec, u tohle bloku co byl se nesmi zmenit cisla

	
	S_TRUE=43,
	CUT_L=44,
	CUT_R=45,
	S_PHP = 46,
	S_SEM = 47, // ;
	S_BLOCK_START = 48,  // {
  	S_BLOCK_END = 49,  // }



  	S_EOF = 50,	
	
	
	
	
	
	
	W_RESERVED = 51, // ale tohle netreba posilat, tohle je lexikalni chyba 
	TYPS_OEF=52,

	S_E = 53, //neterminial E pro precedenci tabulku  
	
	S_NUM = 54,
	S_EXP = 55,
  	    	
  	NOT_EXIST = 60,
  	CALL = 61,
  	STORE_PARAM = 62,
  	INTER_RETURN = 63,
  	JMP = 64,
  	JMP_NOT = 65,
  	THE_END = 66,
  	WHILE_BLOCK_END = 67,
	FUNCTION_BLOCK_START = 68,
	FUNCTION_BLOCK_END = 69,
	

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
