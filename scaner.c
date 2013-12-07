//Implementace interpretu jazyka IFJ13.
//xbucht18, Buchta David
//xsalom04, Salomon Vit
//xbulin02, Bulin Martin
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "scaner.h"
#include "types.h"

#include <string.h>

#define debug 0

FILE *pSource_File; //vstupni soubor
extern T_Token *prevToken;
extern int row; // z main.c

bool extenVar = false;
bool readStr = false;
bool con = false;

void putToken( T_Token *token)
{

    prevToken = token;
}

//nastaveni vstupniho souboru
void soubor(FILE *f)
{
    pSource_File = f;
}

char *IFJ_KEY_WORDS[] = {"else","function", "if", "return","while","elseif"};

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


/** Hlavni fce lexikalniho analyzatoru */
//kvuli testum getToken budu volat pres tuto pomocnou funkci abych mohl jednoduse vypsat co mi vraci
int getToken(T_Token *token){
   
  if (token->value != NULL ){free(token->value); token->value = NULL;}
  int result = getTokenReal(token);



	#if debug 
		printf("================================================\nFunkce getToken vracim:\n\ttoken.type = %d\n",token->type);
      if(token->type ==  S_STR || token->type ==  S_FUNC || token->type ==  S_ID ) printf("\t\"%s\"\n",(char*)(token)->value);
       printf("\tnavratova hodnota: %d\n================================================\n",result);
	#endif
  return result;
}

int getTokenReal(T_Token *token)
{
//==========================================================================================================
  if(prevToken != NULL){
      token = prevToken;
      prevToken = NULL;
      return OK;
  }


  char scanned;
  int result;
  int pozice = 0;
  int alokovano = 32;
  char *str, *more_str;
  int pomRow;
  bool first = false;

  //kvuli rozisreni expanze promene, posleme tecku
  if(con){
    token->type = S_CONCATENATE;
    token->value = NULL;
    con = false;
    return OK;
  }

  if(extenVar || readStr) {
    result = readString(token);
    return result;
  }

 

  while((scanned = fgetc(pSource_File)) != EOF){
    //printf("nacteny znak je: %c\n",scanned);
    result = OK;
    first = false;

    // slozitejsi podminky se vezmou pres if tam kde zalezi jen na jednom znaku se pouzije switch
    if (scanned >= '0' && scanned <= '9'){
        
        result = OK;
        result = readNumber(token, scanned);
        return result;
      
    }
    else if ((scanned >= 'A' && scanned <= 'Z') || (scanned >= 'a' && scanned <= 'z') || scanned == '_'){
        
        pozice = 0;
        alokovano = 32;
        if ((str = (char*)malloc(alokovano * sizeof(char))) == NULL) return ERROR_INTER;
 
        //nacteme si cely retezec;
        do{
            // v pripade potreby zvetsime pamet na retezec
            if(pozice >= alokovano) {
              alokovano  = alokovano << 1;

              more_str = (char*) realloc (str, alokovano * sizeof(int));
              if(more_str == NULL){
                  free(str);
                  fprintf(stderr, "Spatna alokace pameti pri nacitani retezce.\n");
                  return ERROR_INTER;
              }
              else str = more_str;
            }

            str[pozice++] = scanned;//ulozime si znak   
            scanned = fgetc(pSource_File);

            

        }while((scanned >= 'A' && scanned <= 'Z') || (scanned >= 'a' && scanned <= 'z') || (scanned >= '0' && scanned <= '9') || scanned == '_');

        //vratime znak navic co jsme nacetli
        fseek(pSource_File, -1,SEEK_CUR);

        //ulozime konec retezce za poseldni znak, u ted se muzeme dostat za to co mame alokovano tak musim zkontrolovat
        if(pozice >= alokovano) {
              alokovano  = alokovano << 1;

              more_str = (char*) realloc (str, alokovano * sizeof(int));
              if(more_str == NULL){
                  free(str);
                  fprintf(stderr, "Spatna alokace pri ulozeni posledniho znaku .\n");
                  return ERROR_INTER;
              }
              else str = more_str;
        }

        str[pozice++] = '\0';

        //zkontrolujeme jestli to neni klicove slovo
        for(int i =0; i <6; i++){
          //nasli jsme klicove slovo
          if((strcmp (str,IFJ_KEY_WORDS[i])) ==  0) {
            token->type =i;
            return OK;
          }
        }

        //klicove slovo to neni porovname s konstantami
        if((strcmp (str,"false")) ==  0) {
            token->type = S_BOOL;
            if( (token->value = malloc(sizeof (int))) == NULL){
              free(str);
              fprintf(stderr, "Spatna alokace klicoveho slova false.\n");
              return ERROR_INTER;
            }
            *(int*)token->value = 0;
            return OK;
        }

        if((strcmp (str,"null")) ==  0) {
            token->type = S_NULL;
			if( (token->value = malloc(sizeof (int))) == NULL){
              free(str);
              fprintf(stderr, "Spatna alokace klicoveho slova null.\n");
              return ERROR_INTER;
            }

            *(int*)token->value = 0;
            return OK;
        }

        if((strcmp (str,"true")) ==  0) {
           token->type = S_BOOL;
            if( (token->value = malloc(sizeof (int))) == NULL){
              free(str);
              fprintf(stderr, "Spatna alokace klicoveho slova true.\n");
              return ERROR_INTER;
            }

            *(int*)token->value = 1;
            return OK;
           
        }

        //ani konstanty to nejsou takze se skutecne jedna o pojmenovani funkce
        token->type = S_FUNC;
        token->value = mystrdup(str);
        free(str);
        return OK;

    }

    else{

      switch(scanned){

        case '$':
              pozice = 0;
              alokovano = 32;

              if ((str = (char*)malloc(alokovano * sizeof(char))) == NULL) return ERROR_INTER;             
              do{
                // v pripade potreby zvetsime pamet na retezec
                if(pozice >= alokovano) {
                  alokovano  = alokovano << 1;

                  more_str = (char*) realloc (str, alokovano * sizeof(int));
                  if(more_str == NULL){
                      free(str);
                      fprintf(stderr, "Spatna alokace pri zvetosvani pameti pro retezec.\n");
                      return ERROR_INTER;
                  }
                  else str = more_str;
                }

              str[pozice++] = scanned;//ulozime si znak   
              scanned = fgetc(pSource_File);
              
              //prvni cteni muze byt jen pismeno nebo podrziko
              if(!first) { 
                if(!((scanned >= 'A' && scanned <= 'Z') || (scanned >= 'a' && scanned <= 'z') ||  scanned == '_')){
                  fprintf(stderr, "Row: %d, promena nesmi zacinat \"%c\"\n", row,scanned);
                  free(str);
                  return ERROR_LEX;
                }
                first = true;
              }

              

            }while((scanned >= 'A' && scanned <= 'Z') || (scanned >= 'a' && scanned <= 'z') || (scanned >= '0' && scanned <= '9') || scanned == '_');

            //nebyl zadan nazev promenne
            if(pozice<2){fprintf(stderr, "Row: %d, Musite zadat nazev promenne\n",row ); free(str); return ERROR_LEX;}

            //ulozime konec retezce za poseldni znak, i ted se muzeme dostat za to co mame alokovano tak musim zkontrolovat
            if(pozice >= alokovano) {
                alokovano  = alokovano << 1;

                more_str = (char*) realloc (str, alokovano * sizeof(int));
                if(more_str == NULL){
                  free(str);
                  fprintf(stderr, "Spatna alokace pri ulozeni posledniho znaku a kontrole .\n");
                  return ERROR_INTER;
                }
                else str = more_str;
            }

            str[pozice++] = '\0';



            //vratime znak navic co jsme nacetli
            fseek(pSource_File, -1,SEEK_CUR);
            token->type = S_ID;
            token->value = mystrdup(str);
            free(str);

            return OK;

        case '"':
            result = readString(token);
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
                while(scanned != '\n' && scanned != '\r' && scanned != EOF);
				row++;

                break;
              }

              //blokovy komentar
              else if (scanned == '*'){
                  do{   pomRow = row;
                        scanned = fgetc(pSource_File);
                        if(scanned == '\n') row++;
						
                        //mozny konec komentare
                        if (scanned == '*'){
                            
                            scanned = fgetc(pSource_File);
                            if(scanned == '/') break; // konec komentare ukoncime nekonecnou smycku
                        }
                }
                while(scanned!=EOF);

                if(scanned == EOF ){fprintf(stderr, "Row: %d, Zacatek neukonceneho komentare\n",pomRow ); return ERROR_LEX;}
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
                  fprintf(stderr, "Unexpected symbols at the begining of file.\n");
                  return ERROR_SYN;
               }
				

               fseek(pSource_File, 2,SEEK_SET);
              // scanned = fgetc(pSource_File); printf("znak: %c\n",scanned);
              // scanned = fgetc(pSource_File); printf("znak: %c\n",scanned);
               //scanned = fgetc(pSource_File); printf("znak: %c\n",scanned);

               if (fgetc(pSource_File) != 'p') return ERROR_LEX;
               if (fgetc(pSource_File) != 'h') return ERROR_LEX;
               if (fgetc(pSource_File) != 'p') return ERROR_LEX;
               scanned = fgetc(pSource_File);
                
               if(scanned!= '\n' && scanned != '\t' && scanned != '\v' && scanned !=' ' && scanned !='\r') {fprintf(stderr, "Code not start with \"<?php\"\n"); return ERROR_SYN; }
               if(scanned == '\n') row++;

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
        case ' ': //obycejna mezera 
          continue;
        
        case '\n':
            ++row;
            continue;

	      case '=':
		
		      scanned = fgetc(pSource_File);
      	  if(scanned == '='){
                 scanned = fgetc(pSource_File);
                if(scanned == '='){
				          token->type = S_EQ;
				          return OK;
			          }
				
			         else{
				          fprintf(stderr, "Lexikalni chyba porovnavani je ===.\n");
           		    return ERROR_LEX;
				       }
          }
          token->type = S_IS;
          fseek(pSource_File, -1,SEEK_CUR); // nacetli jsme znak ktery pozdeji muzeme potrebovat
          return OK;
		
	      case '!':
		
		      scanned = fgetc(pSource_File);
		      if(scanned == '='){
             scanned = fgetc(pSource_File);
            if(scanned == '='){
				        token->type = S_NEQ;
				        return OK;
			      }
				
			      else{
				        fprintf(stderr, "Lexikalni chyba nerovnani je !==.\n");
				        return ERROR_LEX;
				    }
          }            
	        fprintf(stderr, "Lexikalni chyba nerovnani je !==.\n");
	        return ERROR_LEX;

       
        default: 
        fprintf(stderr, "Lexikalni chyba nejakej jinej symbol.\n");
        return ERROR_LEX;
		  }//konec switche
    }//konec else
  }//konec while

  token->type = S_EOF;
return OK;
}


int getFunctionHeader(T_Token*  token, FUn what)
{ 
  // hledame klicove slovo slovo function
  char scanned;

  if(what ==NEXT_READ){
    do{
        scanned = fgetc(pSource_File);
		
		if(scanned == '/'){
		
		 scanned = fgetc(pSource_File);

              //komentar do konce radku
              if(scanned == '/'){
                do{ scanned = fgetc(pSource_File);}
                while(scanned != '\n' && scanned != '\r' && scanned != EOF);
				row++;
              }

              //blokovy komentar
              else if (scanned == '*'){
                  do{ 	
                        scanned = fgetc(pSource_File);
						if(scanned == '\n') row++;
                        //mozny konec komentare
                        if (scanned == '*'){
                            
                            scanned = fgetc(pSource_File);
                            if(scanned == '/' || scanned == EOF) break; // konec komentare ukoncime nekonecnou smycku
                        }
                }
                while(scanned!=EOF);
              }
		  scanned = fgetc(pSource_File);
		}
		
        if(scanned == 'f'){
           
           scanned = fgetc(pSource_File);
           if(scanned == 'u'){
              
              scanned = fgetc(pSource_File);
              if(scanned == 'n'){
                
                scanned = fgetc(pSource_File);
                if(scanned == 'c'){
                  
                  scanned = fgetc(pSource_File);
                  if(scanned == 't'){

                    scanned = fgetc(pSource_File);
                    if(scanned == 'i'){

                       scanned = fgetc(pSource_File);
                       if(scanned == 'o'){

                         scanned = fgetc(pSource_File);
                         if(scanned == 'n'){
                              token->type = FUNCTION;
                              return OK;
                         }
                       }
                     }
                   }
                 }
               }
             }
           }
           else if(scanned == EOF){ token->type = S_EOF; rewind(pSource_File); row = 1; return OK;}
		   
		   if(scanned == '\n') row++;
           //printf("scan: %c\n",scanned);
    }while(true);    
  }
  else  return getToken(token);
  fprintf(stderr, "Lexikalni chyba pri function.\n");
  return ERROR_LEX;
}

int readNumber(T_Token *token, char firstNum)
{
  token->type = S_INT;
  token->value = malloc(sizeof(int));
  
//  *(int*)token->value = 5; 
  int firstTime = 1;
  int isDecimal = 0;
  int decimal = 10;                                       //Pocitadlo desetinneho mista
  int exp = 0;                                            //hodnota exponentu
  int expS = 0;                                           //stav exponentu
  int expM = 1;                                           //multiplikator exponentu (+-1)
  int n = firstNum;                                       //PREDANY PARAMETR PRVNIHO PRECTENEHO (PODLE KTEREHO VIME, ZE JE TO CISLO)
  double num = n - ASCII_ZERO;                            //ctene cislo (numericka hodnota)
  n = fgetc(pSource_File);
  do {
    if (expS >= 1) {                                      //Predchozi bylo e/E
      if (n == '+' && expS == 1) {                        //cteme nepovinne znamenko a to poprve
        expM = 1;
        expS = 2;                                         //vickrat znamenko v exp. je chyba
      }
      else if (n == '-' && expS == 1) {
        expM = -1;
        expS = 2;
      }
      else if (n >= '0' && n <= '9') {                    //hodnota exponentu
        expS = 3;
        exp *= 10;
        exp += n - ASCII_ZERO;
      }
    }
    else {
      if (n >= '0' && n <= '9') {
        if (isDecimal >= 1) {
    	  isDecimal = 2;
          double tmp = n - ASCII_ZERO;
          tmp = tmp / decimal;
          num += tmp;
          decimal *= 10;
        }
        else {
          num *= 10;                                         //pridani cele casti cisla
          num += n - ASCII_ZERO;
        }
      }
      else if (n == '.') {
        isDecimal = 1;
        free(token->value);
        token->value = malloc(sizeof(double));
        token->type = S_DOUB;
      }
      else if (n == 'e' || n == 'E') {
        expS = 1;                                          //a mame tu exponent
        if (token->type == S_INT) {
          free(token->value);
          token->value = malloc(sizeof(double));
          token->type = S_DOUB;
        }
      }
      else {
        if (!firstTime) {
          fprintf(stderr, "Lexikalni chyba u funkce pro cisla \n");
          return ERROR_LEX;                                  //a tady neco falesneho
        }
        fseek(pSource_File, -1, SEEK_CUR);
      }
    }

    firstTime = 0;

    n = fgetc(pSource_File);

  }while((n >= '0' && n <= '9') || (n == '+' && expS == 1) || (n == '-' && expS == 1) || ((n == 'e' || n == 'E') && (expS == 0)) || ((n == '.') && (isDecimal == 0) && (expS == 0)));

    if ((expS == 1) || (expS == 2)) {
	fprintf(stderr, "Lexikalni chyba u funkce pro cisla \n");
	return ERROR_LEX;		//Bud bylo nacteno jen e/E nebo e/E a znamenko
    }
    if (isDecimal == 1) {
	fprintf(stderr, "Lexikalni chyba u funkce pro cisla \n");
	return ERROR_LEX;		//Byla nactena tecka, avsak ne des. cast
    }


  fseek(pSource_File, -1,SEEK_CUR);
  
  if (expS >= 1) {
    exp *= expM;
    num = num * pow(10.0, exp);
  }
  
//  printf("\n\n\t\tnahraju cislo %f\n\n", num);
  if (token->type == S_INT) {
    *(int*)token->value = num; 
  }
  else {
    *(double*)token->value = num;
  }
  return OK;
}

int readString(T_Token *token){

 // int inString = 1;
  int pozice = 0;
  int alokovano = 32;
  int nextChar,i;
  char *string, *more_str,s1, zaloha, zaloha2;
  bool zalohovat = false;
  char scanned = fgetc(pSource_File);



  if ((string = (char*)malloc(alokovano * sizeof(char))) == NULL) return ERROR_INTER;

  if (extenVar)
  {     extenVar =false;
        do { 
          //kvuli znaku konce retezce
          if(pozice >= alokovano+1) {                                                    //Realokace, kdy nemame misto
            alokovano  = alokovano << 1;

            more_str = (char*) realloc (string, alokovano * sizeof(int));
            if(more_str == NULL){
              free(string);
              fprintf(stderr, "Spatna reallokace stringu.\n");
              return ERROR_INTER;
            }
            else string = more_str;
        }
        string[pozice++] = scanned;
        scanned = fgetc(pSource_File);
      }
      while((scanned >= 'A' && scanned <= 'Z') || (scanned >= 'a' && scanned <= 'z') || (scanned >= '0' && scanned <= '9') || scanned == '_');

    string[pozice] ='\0';
    readStr =true;
    con = true;
    token->type = S_ID;
    token->value = string;
  
    //vratime znak navic co jsme nacetli
    fseek(pSource_File, -1,SEEK_CUR);

    return OK;

  }

 
  else{ 

    readStr = false;
    

    do { 
      zalohovat = false;
      //z vrchu uvozovky zavolame si get token.. jinac by to delalo       
      if(scanned == '"'){break;}
      if(pozice >= alokovano) {                                                    //Realokace, kdy nemame misto
        alokovano  = alokovano << 1;

        more_str = (char*) realloc (string, alokovano * sizeof(int));
        if(more_str == NULL){
          free(string);
          fprintf(stderr, "Spatna reallokace stringu.\n");
          return ERROR_INTER;
        }
        else string = more_str;
      }
      

      //konec souboru
      if(scanned == EOF){
        fprintf(stderr, "Row: %d, Chybi konec retezce\n",row);
        return ERROR_LEX;
      }

      //expanze promenych
      else if(scanned == '$'){
        extenVar = true;
        con = true;
        fseek(pSource_File, -1,SEEK_CUR);
        break;
      }

      //lexikalni chyba
      else if(scanned <=31){
        fprintf(stderr, "Row: %d, Byl nacten znak s ordinalni hodnotou mensi nez 32\n",row );
        return ERROR_LEX;
      }
      //nacitani retezce
      else{
        //neni to escape sekvence pridame to 
        if(scanned != '\\') string[pozice++] =scanned;
        else{
          
          //zalohujme si kdyz to neni znama escapse sekvence
          zaloha = scanned;

          scanned = fgetc(pSource_File);
          switch(scanned){
            case 'x':
                nextChar = 0;
                for (i = 0; i < 2; i++) {
                    nextChar *= 16;
                    s1 = fgetc(pSource_File);
                  
                    if (s1 >= 'A' && s1 <= 'F') {
                      nextChar += s1 - ASCII_A_TO_HEX;
                    }
                    else if (s1 >= 'a' && s1 <= 'f') {
                      nextChar += s1 - ASCII_a_TO_HEX;
                    }
                    else if (s1 >= '0' && s1 <= '9') {
                      nextChar += s1 - ASCII_ZERO;
                    }
                    else{fseek(pSource_File, -1,SEEK_CUR); break;}
                      
                     



                }

                if(!i){
                    string[pozice++] = zaloha;
                    string[pozice++] = scanned;
                }
                else {
                   string[pozice++] = nextChar;
                }

               
                break;
            case '$':   string[pozice++] = '$'; break;                                      //$
            case 'n':   string[pozice++] = '\n'; break;
            case 't':   string[pozice++] = '\t'; break;
            case '\\':  string[pozice++] = '\\'; break;
            case '"':   string[pozice++] = '"'; break;
            //neznama escapen sekvence
            default:
                string[pozice++] = zaloha;
                //if(zalohovat){
               //      string[pozice++] = scanned;
                //}
                //zalohovat se nastavi pri nacitani hexa cisla a tam se to samo vraci o 1
               
                fseek(pSource_File, -1,SEEK_CUR);
                break;
          }
        }
      }
      scanned = fgetc(pSource_File);

    } while(scanned != '"');
  }//konec  else if(scanned != '"') pro if(extenVar)

    
  //jeste je treba nahrat znak konce retezce
  if(pozice >= alokovano) {                                                    //Realokace, kdy nemame misto
      alokovano  = alokovano << 1;

      more_str = (char*) realloc (string, alokovano * sizeof(int));
      if(more_str == NULL){
          free(string);
          fprintf(stderr, "Spatna realokace pri stringu.\n");
          return ERROR_INTER;
      }
      else string = more_str;
    }
    string[pozice] ='\0';
  //fseek(pSource_File, -1,SEEK_CUR);

  token->type = S_STR;
  token->value = mystrdup(string);
  free(string);

  return OK;

    

    /*if (scanned == '"') {
      inString = 0;
      return T_STRING;
    }*/
    /*else if (scanned == '$') {
      nextToken = T_CONCATENATE;
      return T_STRING;
    }*/


     


     /* autor tohoto zakomentovaneho kodu: Vit Salamon, prasacky napsane a jeste ne zcela funkci 
    else if (scanned == '\\' ) {                                                 //special znak
      scanned = fgetc(pSource_File);
      if (scanned == 'x') {                                                      //hexa hodnota
        nextChar = 0;
        for (i = 0; i < 2; i++) {
          nextChar *= 16;
          s1 = fgetc(pSource_File);
          if (s1 >= 'A' && s1 <= 'F') {
            nextChar += s1 - ASCII_A_TO_HEX;
          }
          else if (s1 >= 'a' && s1 <= 'f') {
            nextChar += s1 - ASCII_a_TO_HEX;
          }
          else if (s1 >= '0' && s1 <= '9') {
            nextChar += s1 - ASCII_ZERO;
          }
        }

        string[pozice++] = nextChar;

      }
      else if (scanned == '$') {                                                 //$
        string[pozice++] = '$';
      }
      else if (scanned == 'n') {                                                  //\n
        string[pozice++] = '\n';
      }
      else if (scanned == 't') {                                                  //\t
        string[pozice++] = '\t';
      }
      else if (scanned == '\\') {                                                 // "\"
        string[pozice++] = '\\';
      }
      else if (scanned == '"') {                                                  //\"
        string[pozice++] = '"';
      }
    }
    else if (scanned == ' ') {
      string[pozice++] = ' ';
    }
    else if (scanned > 31) {
      string[pozice++] = scanned;
    }
    
    scanned = fgetc(pSource_File);
    if (scanned == EOF) {
    	fprintf(stderr, "Lexikalni chyba pri escape sekvencich.\n");
	return ERROR_LEX;
    } */
    
  
  
}


char* mystrdup(const char* s)
{
    if(s== NULL) return NULL;
    char* p = malloc(strlen(s)+1);
    if(p==NULL) fprintf(stderr, "Cannot alocate memory\n");
    if (p) strcpy(p, s);
    return p;
}
