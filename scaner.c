#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "scanner.h"
#include "types.h"
#include "string.h"

#define NDEBUG 0

void putToken( T_Token *token)
{
    prevToken = token; 
}

FILE *pSource_File; //vstupni soubor
static in state; //soucasny stav automatu
extern T_Token prevToken; 

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
int getToken(string *S)
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

if (inSTring == 1) {
  readString();
}


if (scanned == EOF) {
  return S_EOF;                                         //VRACI KONEC SOUBORU
}
else if (scanned >= '0' && scanned <= '9') {
  readNumber();                                         //FUNKCE NA CTENI CISLA, VRATI TOKEN
}
else if (scanned == '$') {
  readVariable();                                       //FUNKCE NA CTENI PROMENNE, VRATI TOKEN
}
else if (scanned == '"') {
  readString();                                         //FUNKCE NA CTENI RETEZCE
}
else if ((scanned >= 'A' && scanned <= 'Z') || (scanned >= 'a' && scanned <= 'z') || scanned == '_') {
  readWord();
}
else if (scanned == '.') {
  return S_CONCATENATE;                                 //   .
}
else if (scanned == ';') {
  return S_SEM;                                   //   ;
}
else if (scanned == ',') {
  return S_COMMA;                                       //   ,
}
else if (scanned == '+') {
  return S_PLUS;                                        //   +
}
else if (scanned == '-') {
  return S_MINUS;                                       //   -
}
else if (scanned == '*') {

  scanned = //CTENI DALSIHO ZNAKU;
  if (scanned == '/') {
    return S_END_BLOCK_COMMENT;                        //   */
  }
  else {
    //ULOZENI SCANNED DO STRUKTURY K PREVIOUSLY READ
    return S_MUL;                           //   *
  }
}
else if (scanned == '/') {

  scanned = //CTENI DALSIHO ZNAKU;
  if (scanned == '/') {                                //   //
    return S_LINE_COMMENT;
  }
  else if (scanned == '*') {
    return S_BLOCK_COMMENT;                            //   /*
  }
  else {
    //ULOZENI SCANNED DO STRUKTURY K PREVIOUSLY READ
    return S_DIV;                                 //   /
  }
}
else if (scanned == '(') {
  return S_LBRA;                              //   (
}
else if (scanned == ')') {
  return S_RBRA;                              //   )
}
else if (scanned == '{') {
  return S_BLOCK_START;                                //   {
}
else if (scanned == '}') {
  return S_BLOCK_END;                                  //   }
}
else if (scanned == '<') {

  scanned = //CTENI DALSIHO ZNAKU;
  if (scanned == '=') {
    return S_LEQ;                                    //   <=
  }
  else if (scanned == '?') {
    //ZKOUSKA PROTI PHP                                //   <?php
  }
  else {
    //ULOZENI SCANNED DO STRUKTURY K PREVIOUSLY READ
    return S_LST;                                     //   <
  }

}
else if (scanned == '>') {

  scanned = //CTENI DALSIHO ZNAKU;
  if (scanned == '=') {
    return S_GEQ;                                    //   >=

  else {
    //ULOZENI SCANNED DO STRUKTURY K PREVIOUSLY READ
    return S_GRT;                                      //   >
  }

}
else if (scanned == '=') {
  scanned == //CTENI DALSIHO ZNAKU
  if (scanned == '=') {
    scanned == //CTENI DALSIHO ZNAKU
    if (scanned == '=') {
      return S_EQ;                                  //   ===
    }
    else {
      //ULOZENI SCANNED DO STRUKTURY K PREVIOUSLY READ
      return //CHYBA;
    }
  }
  else {
    //ULOZENI SCANNED DO STRUKTURY K PREVIOUSLY READ
    return S_IS;                                   //   =
  }
}
else if (scanned == '!') {
  scanned = //CTENI DLASIHO ZNAKU
  if (scanned == '=') {
    scanned == //CTENI DALSIHO ZNAKU
    if (scanned == '=') {
      return S_NEQ;                              //   !==
    }
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
else {
  return //CHYBA;
}

//==========================================================================================================

readString() {
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

readNumber() {
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
