#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "scaner.h"
#include "types.h"
//#include "string.h"

#define NDEBUG 0

FILE *pSource_File; //vstupni soubor
//static int state; //soucasny stav automatu
extern T_Token *prevToken;
extern int row; // z main.c


void putToken( T_Token *token)
{
    prevToken = token;
}

//nastaveni vstupniho souboru
void soubor(FILE *f)
{
    pSource_File = f;
}

char *IFJ_KEY_WORDS[] = {"function", "if", "else", "return", "while", "true", "false", "null"};

const char *LEX_ECODEMSG[] =
{//vyznamy chybovych stavu
  [EOK] = "Vse v poradku.\n",
  [EHexWrong] = "Spatny zapis hexadecimalniho kodu znaku\n",
  [EExpWrong]="Chybny zapis exponentu\n",
  [ENumWrong]="Chybny zapis cisla\n",
  [EComToEof]="Komentar do konce souboru\n",
  [EExclMarkWrong]="Chybny znak abecedy.\n",
  [ENotAlph] = "neplatny symbol\n",
  [EStrEof] = "Spatny zapis retezce\n",
};

const int ASCII_ZERO=48;
const int ASCII_A_TO_HEX=55;
const int ASCII_a_TO_HEX=87;

//prechod do noveho stavu
/*int next( int newst )
{
    state = newst;
    return fgetc(file);
}*/


/** Hlavni fce lexikalniho analyzatoru */
int getToken(T_Token *token)
{
 
//==========================================================================================================
  if(prevToken != NULL){
      token = prevToken;
      prevToken = NULL;
      return token;
  }


  char scanned;
  int result;

  while((scanned = fgetc(pSource_File)) != EOF){

    result = OK;

    // slozitejsi podminky se vezmou pres if tam kde zalezi jen na jednom znaku se pouzije switch
    if (scanned >= '0' && scanned <= '9'){
        token->type = S_NUM;
                return OK;
        //result = readNumber(&token);
        //return result;
    }
    else if ((scanned >= 'A' && scanned <= 'Z') || (scanned >= 'a' && scanned <= 'z') || scanned == '_'){
        token->type = S_FUNC;
        return OK;
        /*result = readWord(&token);
        return result;*/
    }

    else{

      switch(scanned){

        case '$':
            token->type = S_ID;
            return OK;

        case '"':
            result = readString(&token);
            return result;

        case '.':
            token->type = S_CONCATENATE;
            return OK;

        case ';':
            token->type = S_SEM;
            return OK;

        case '/':
              scanned = fgetc(pSource_File);

              //komentar do konce radku
              if(scanned == '/'){
                do{ scanned = fgetc(pSource_File);}
                while(scanned != '\n' || scanned != EOF);

                break;
              }

              //blokovy komentar
              else if (scanned == '*'){
                  do{
                        scanned = fgetc(pSource_File);
                        
                        //mozny konec komentare
                        if (scanned == '*'){
                            
                            scanned = fgetc(pSource_File);
                            if(scanned == '/') break; // konec komentare ukoncime nekonecnou smycku
                        }
                }
                while(true);
                break;
              }

              else{
                fseek(pSource_File, -1,SEEK_CUR); // nacetli jsme znak ktery pozdeji muzeme potrebovat
                token->type = S_DIV;
                return OK;
              }

        case ',':
            token->type = S_COMMA;
            return OK;

        case '+':
            token->type = S_PLUS;
            return OK;

        case '-':
            token->type = S_MINUS;
            return OK;

        case '*':
            token->type = S_MUL;
            return OK;

        case '(':
            token->type = S_LBRA;
            return OK;

        case ')':
            token->type = S_RBRA;
            return OK;

        case '{':
            token->type = S_BLOCK_START; // {
            return OK;

        case '}':
            token->type = S_BLOCK_END;
            return OK;

        case '<':
            scanned = fgetc(pSource_File);

            //jedna se o startujici terminal <?php
            if (scanned == '?'){
              //zkontrolujeme jestli se pred startujicim terminalem nenachazi zadne znaky
               fseek(pSource_File, -2,SEEK_CUR);
               if(ftell(pSource_File) != SEEK_SET ){
                  fprintf(stderr, "Uexpected symbols at the begining of file.\n");
                  return ERROR_SYN;
               }

               fseek(pSource_File, 2,SEEK_CUR);
               if (fgetc(pSource_File) != 'p') return ERROR_LEX;
               if (fgetc(pSource_File) != 'h') return ERROR_LEX;
               if (fgetc(pSource_File) != 'p') return ERROR_LEX;

               scanned = fgetc(pSource_File);
               
               if(scanned!= '\n' || scanned != '\t' || scanned != '\v' || scanned !=' ') return ERROR_LEX;

               token->type = S_PHP;
               return OK;

            }

            // <=
            else if( scanned == '='){
                token->type = S_LEQ;
                return OK;
            }

            token->type = S_LST;
            fseek(pSource_File, -1,SEEK_CUR); // nacetli jsme znak ktery pozdeji muzeme potrebovat
            return OK;

        case '>':
              
              scanned = fgetc(pSource_File);
              if(scanned == '='){
                token->type = S_GEQ;
                return OK;
              }

              token->type = S_GRT;
              fseek(pSource_File, -1,SEEK_CUR); // nacetli jsme znak ktery pozdeji muzeme potrebovat
              return OK;
    
        case '\t': //tabulator
        case '\v': //vertical space
        // hazi to chybu 
        //case ' ': //obycejna mezera 
        
        case '\n':
            row++;
            continue;

	case '=':
		
		scanned = fgetc(pSource_File);
		if(scanned == '='){
                	if(scanned == '='){
				token->type = S_EQ;
				return OK;
			}
				
			else
				fprintf(stderr, "Lexikalni chyba porovnavani je ===.\n");
				return ERROR_LEX;
				
             	}
             	else
			fprintf(stderr, "Lexikalni chyba porovnavani je ===.\n");
			return ERROR_LEX;
			
            
	token->type = S_IS;
        fseek(pSource_File, -1,SEEK_CUR); // nacetli jsme znak ktery pozdeji muzeme potrebovat
        return OK;
		
	case '!':
		
		scanned = fgetc(pSource_File);
		if(scanned == '='){
                	if(scanned == '='){
				token->type = S_NEQ;
				return OK;
			}
				
			else
				fprintf(stderr, "Lexikalni chyba nerovnani je !==.\n");
				return ERROR_LEX;
				
             	}
             	else
				fprintf(stderr, "Lexikalni chyba nerovnani je !==.\n");
				return ERROR_LEX;
					
            
	fprintf(stderr, "Lexikalni chyba nerovnani je !==.\n");
	return ERROR_LEX;
		
		
    



case S_STR:
{
    scanned = fgetc(pSource_File);

      if(scanned == EOF){
        token->type = S_EOF;
        fprintf(stderr, "Lexikalni chyba v stringu \n");
        return ERROR_LEX;
      }

      //prazdny retezec
      else if (scanned == '"'){
        token->type = S_STR;
        return OK;
      }

      //pak asi jeste escape sekvence 
      //nepovoleny znaky
      else if (scanned < '1' || scanned > '255'){
        fprintf(stderr, "Lexikalni chyba nepovoleny znak ve stringu");
        return ERROR_LEX;
      }

     /* else if (scanned == '\\' ) {
        scanned = fgetc(pSource_File);
          if (scanned == 'x') {
            nextChar = 0;
            for (int i = 0; i < 2; i++) {
                scanned = fgetc(pSource_File);
                return OK;
               
               if (s1 >= 'A' && s1 <= 'F') {
                    nextChar += s1 - ASCII_A_TO_HEX;
                }

                else if (s1 >= 'a' && s1 <= 'f') {
                    nextChar += s1 - ASCII_a_TO_HEX;
                }*/
      


    break;
}

case S_ID:
{
    scanned = fgetc(pSource_File);

    //identifikator muze byt posloupnost cisel znaku a podtrzitka
    if((scanned >= '0' && scanned <= '9') || ((scanned >= 'A' && scanned <= 'Z') || (scanned >= 'a' && scanned <= 'z')) || scanned == '_'){
       token->type = S_ID;
       return OK;
     } 

     //neco jineho tak ykusime klicove slovo
    else 
        if((strcmp(scanned, "function")) == 0) return FUNCTION;
        else if(scanned = "function"){
          token->type = FUNCTION;
          return OK;
        }
        /*else if((strcmp(token->type, "if")) == 0) return IF;
        else if((strcmp(token->type, "else")) == 0) return ELSE;
        else if((strcmp(token->type, "return")) == 0) return RETURN;
        else if((strcmp(token->type, "while")) == 0) return WHILE;
        else if((strcmp(token->type, "true")) == 0) return C_TRUE;
        else if((strcmp(token->type, "false")) == 0) return C_FALSE;
        else if((strcmp(token->type, "null")) == 0) return C_NULL;*/
        else fprintf(stderr, "Lexikalni chyba v cisle.\n");
        return ERROR_LEX;       
    

    break;
}




case S_NUM:
{ 
    scanned = fgetc(pSource_File);

    // cele cislo
    if (scanned >= '0' && scanned <= '9'){
        token->type = S_INT;
        return OK;
    }

    else if(scanned == '.'){  // narazime na tecku == double
        token->type = S_DOUB;
        return OK;
    }

    else if((scanned == 'e' || scanned == 'E')){ // pokud narazime na e nebo E 
        token->type = S_EXP;                      // tak jde o celociselny exponent
        return OK;
    }

    //narazim na cokoliv jineho tak chyba
    else{
        fprintf(stderr, "Lexikalni chyba v cisle.\n");
        return ERROR_LEX;
    }
  break;
}

case S_DOUB:
{
  scanned = fgetc(pSource_File);

    // cele cislo co pokracuje za desetinou teckou
    if (scanned >= '0' && scanned <= '9'){
        token->type = S_INT;
        return OK;
    }

    else if((scanned == 'e' || scanned == 'E')){
        token->type = S_EXP;                      // tak jde o celociselny exponent
        return OK;
    }

    else{
        fprintf(stderr, "Lexikalni chyba v double cisle.\n");
        return ERROR_LEX;
    }
  break;

}

case S_EXP:
{
  scanned = fgetc(pSource_File);
    if ((scanned == '-' || scanned == '+'))
    {

      //musi nasledovat potom cele cislo 
      if (scanned >= '0' && scanned <= '9'){
        token->type = S_EXP;                      // tak jde o celociselny exponent
        return OK;
      }

      //kdyz neni tak chyba
      else{
        fprintf(stderr, "Lexikalni chyba nerovnani je s exponentem.\n");
        return ERROR_LEX; 
      }
    }

    //cislo
    else if (scanned >= '0' && scanned <= '9'){
      token->type = S_EXP;
      return OK;
    }

    //neco jineho == chyba
    else{
      fprintf(stderr, "Lexikalni chyba nerovnani je s exponentem.\n");
      return ERROR_LEX; 
    }
  
  break;

}

    default: return ERROR_LEX;
      }
    }

  }

  token->type = EOF;
  return OK;
}


/*
int getFunctionHeader(T_Token* , FUn)
{
  // zkouska
  scanned = fgetc(pSource_File);

}
*/

//==========================================================================================================


// je super ze mi vracite ze se jedna o retezec ale jaksik potrebuju vedet jeho hodnotu tzn.
// token.type = S_STR;
// token.value = malloc(sizeof(char * DELKA));
// samozrejme budete potrebovat nahrat ten retezec a jelikoz to je void pointer tak musite pri kazde praci pretypovat ((char*)token->value)
/*int readString(T_Token *token) {
  inString = 1;
  //ZACATEK SMYCKY
  scanned = //CTENI DALSIHO ZNAKU;
  if (scanned == '"') {
    inString = 0;
    return S_STR;
  }
  else if (scanned == '$') {
    nextToken = S_CONCATENATE;
    return S_STR;
  }
  else if (scanned == '\\' ) {
    scanned = //CTENI DLASIHO ZNAKU;
    if (scanned == 'x') {
      nextChar = 0;
      for (i = 0; i < 2; i++) {
        s = //CTENI DLASIHO ZNAKU;
        if (s1 >= 'A' && s1 <= 'F') {
          nextChar += s1 - ASCII_A_TO_HEX;
        }
        else if (s1 >= 'a' && s1 <= 'f') {
          nextChar += s1 - ASCII_a_TO_HEX;
        }
      }

      string //PRIDAT NEXTCHAR;

    }
    else if (scanned == '$') {
      string //PRIDAT $;
    }
    else if (scanned == 'n') {
      string //PRIDAT \n;
    }
    else if (scanned == 't') {
      string //PRIDAT \t;
    }
    else if (scanned == '\\') {
      string //PRIDAT \;
    }
    else if (scanned == '"') {
      string //PRIDAT ";
    }
  }
  else if (scanned == ' ') {
    string //PRIDAT MEZERU;
  }
  else if (scanned > 31) {
    string //PRIDAT scanned;
  }
  //SMYCKOVAT
  return S_STR;
}
*/
//==========================================================================================================
//info viz string


/*
int readNumber(T_Token *token) {
  int decimal = 10;
  int exp = 0;
  int expM = 1;
  n = firstNum; //PREDANY PARAMETR PRVNIHO PRECTENEHO (PODLE KTEREHO VIME, ZE JE TO CISLO)
  num = n - ASCII_ZERO;
  //ZACATEK SMYCKY
  n = //CTENI DALSIHO ZNAKU;
  if (exp == 1) {
    if (n == '+') {
      expM = 1;
    }
    else if (n == '-') {
      expM == -1;
    }
    else if (n >= '0' && n <= '9') {
      exp *= 10;
      exp += n - ASCII_ZERO;
    }
    else if (n != \n && n != \t && n != \v && n != ' ')  { //bile znaky
      return ERROR_LEX;
    }
    else {
      exp *= expM;
      //PRIDANI EXPONENTU
    }
  }
  else {
    if (n >= '0' && n <= '9') {
      num *= 10;
      num += n - ASCII_ZERO;
    }
    else if (n == '.') {
      n = n / decimal;
      num += n;
      decimal *= 10;
    }
    else if (n == 'e' || n == 'E') {
      exp = 1;
    }
    else {
      return ERROR_LEX;
    }
  }
  return S_NUMBER; //DOPLNIT ROZDELENI NA INT, DOUBLE
}
*/

char* mystrdup(const char* s)
{
    char* p = malloc(strlen(s)+1);
    if (p) strcpy(p, s);
    return p;
}
