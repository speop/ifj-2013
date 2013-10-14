#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "scaner.h"
#include "types.h"
//#include "string.h"

#define NDEBUG 0

void putToken( T_Token *token)
{
    prevToken = token; 
}

FILE *pSource_File; //vstupni soubor
static in state; //soucasny stav automatu
extern T_Token *prevToken; 
extern int row; // z main.c


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
int next( int newst )
{
    state = newst;
    return fgetc(file);
}


/** Hlavni fce lexikalniho analyzatoru */
int getToken(T_Token *token)
{
 
   /* int ch = next(0); // soucasny znak na vstupu
    strClear(S); // vyprazdneni retezce
    for( ; ; )
    {
        switch(state) {
            case 0: // stav 0 - startovaci stav
                if(ch == '\n') // konec radku 
                {
                    return S_END_OF_LINE;
                }*/

//==========================================================================================================
  if(prevToken != NULL){ 
      token = prevToken; 
      prevToken = NULL; 
      return token; 
  }


/*
if (nextToken != null) {
  temp = nextToken;
  nextToken = null
  return temp;
}*/

  char scanned;
  int result;   

  while((scanned = fgetc(pSource_File)) != EOF){

    result = OK;

    // slozitejsi podminky se vezmou pres if tam kde zalezi jen na jednom znaku se pouzije switch
    if (scanned >= '0' && scanned <= '9'){
        result = readNumber(&token);
        return result;
    }
    else if ((scanned >= 'A' && scanned <= 'Z') || (scanned >= 'a' && scanned <= 'z') || scanned == '_'){
        result = readWord(&token);
        return result;
    }

    else{

      switch(scanned){

        case '$': 
            result = readVariable(&token);
            return result;

        case  '"':
            result = readString(&token);
            return result;

        case '.':
            token->type = S_CONCATENATE;  
            return OK;

        case ';':
            token->type = S_SEM;
            return OK;

        case '/': 
              scanned = fgetc(pSource_File)

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
                fseek(pFile, -1,SEEK_CUR); // nacetli jsme znak ktery pozdeji muzeme potrebovat
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
            token->type = S_BLOCK_START;                                //   {
            return OK;

        case  '}':
            token->type = S_BLOCK_END;
            return OK;

        case '<':
            scanned = fgetc(pSource_File);

            //jedna se o startujici terminal <?php
            if (scanned == '?'){
              //zkontrolujeme jestli se pred startujicim terminalem nenachazi zadne znaky
               fseek(pFile, -2,SEEK_CUR);
               if(tell(pFile) != SEEK_SET ){
                  fprintf(stderr, "Uexpected symbols at the begining of file.\n");
                  return ERROR_SYN;
               }

               fseek(pFile, 2,SEEK_CUR);
               if (fgetc(pSource_File) != 'p') return ERROR_LEX;
               if (fgetc(pSource_File) != 'h') return ERROR_LEX;
               if (fgetc(pSource_File) != 'p') return ERROR_LEX;

               scanned = fgetc(pSource_File);
               
               if(scanned!= '\n' || scanned != '\t' || scanned != '\v' || scanned !=' ') return ERROR_LEX;

               token->type = S_PHP;
               return OK;

            }

            //   <=
            else if( scanned == '='){
                token->type = S_LEQ;
                return OK;
            }

            token->type = S_LST;
            fseek(pFile, -1,SEEK_CUR); // nacetli jsme znak ktery pozdeji muzeme potrebovat
            return OK;

        case '>':
              
              scanned = fgetc(pSource_File);
              if(scanned == '='){
                token->type = S_GEQ;
                return OK; 
              }

              token->type = S_GRT;
              fseek(pFile, -1,SEEK_CUR); // nacetli jsme znak ktery pozdeji muzeme potrebovat
              return OK;
    
        case '\t': //tabulator
        case '\v': //vertical space
        case ' ': //obycejna mezera
        case '\n': 
            row++;
            continue;

        default: return ERROR_LEX; 
      }
    }

  }

  token->type = EOF;
  return OK;
}

//tu je konec funkce get token ten zblitek predelat do switche podle prikladu vyse

//  if (inSTring == 1) readString(&token);



}
else if (scanned == '=') {
  scanned == //CTENI DALSIHO ZNAKU
  if (scanned == '=') {
    scanned == //CTENI DALSIHO ZNAKU
    if (scanned == '=') {
      token.type = S_EQ;                                  //   ===
    }
    else {
      //ULOZENI SCANNED DO STRUKTURY K PREVIOUSLY READ
      token.type = //CHYBA;
    }
  }
  else {
    //ULOZENI SCANNED DO STRUKTURY K PREVIOUSLY READ
    token.type = S_IS;                                   //   =
  }
}
else if (scanned == '!') {
  scanned = //CTENI DLASIHO ZNAKU
  if (scanned == '=') {
    scanned == //CTENI DALSIHO ZNAKU
    if (scanned == '=') {
      token.type = S_NEQ;                              //   !==
    }
    //tuhle cast jsem neopravil nerozumim ji
    else {
      //ULOZENI SCANNED DO STRUKTURY K PREVIOUSLY READ
      return //CHYBA;
    }
  }
  else {
    //ULOZENI SCANNED DO STRUKTURY K PREVIOUSLY READ
    return //CHYBA;
  }
}
else return ERROR_LEX;

return token;
}

//==========================================================================================================


// je super ze mi vracite ze se jedna o retezec ale jaksik potrebuju vedet jeho hodnotu tzn.
// token.type = S_STR;
// token.value = malloc(sizeof(char * DELKA));
// samozrejme budete potrebovat nahrat ten retezec a jelikoz to je void pointer tak musite pri kazde praci pretypovat ((char*)token->value)
int readString(T_Token *token) {
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

//==========================================================================================================
//info viz string
int readNumber(T_Token *token) {
  decimal = 10;
  exp = 0;
  expM = 1;
  n = firstNum;                                       //PREDANY PARAMETR PRVNIHO PRECTENEHO (PODLE KTEREHO VIME, ZE JE TO CISLO)
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
    else if (n != /*BILY ZNAK*/) {
      return //CHYBA;
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
      return //CHYBA;
    }
  }
  return S_NUMBER;                                                //DOPLNIT ROZDELENI NA INT, DOUBLE
}
