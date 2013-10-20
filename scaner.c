#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "scaner.h"
#include "types.h"
//#include "string.h"

#define debug 1

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

//prechod do noveho stavu
/*int next( int newst )
{
    state = newst;
    return fgetc(file);
}*/


/** Hlavni fce lexikalniho analyzatoru */
//kvuli testum getToken budu volat pres tuto pomocnou funkci abych mohl jednoduse vypsat co mi vraci
int getToken(T_Token *token){
   
  int result = getTokenReal(token);

  //printf("================================================\nFunkce getToken vracim:\n\ttoken.type = %d\n",token->type);
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

  while((scanned = fgetc(pSource_File)) != EOF){
    //printf("nacteny znak je: %c\n",scanned);
    result = OK;

    // slozitejsi podminky se vezmou pres if tam kde zalezi jen na jednom znaku se pouzije switch
    if (scanned >= '0' && scanned <= '9'){
        
        result = OK;
        result = readNumber(token);
        return result;
        //result = readNumber(&token);
        //return result;
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
              return ERROR_INTER;
            }
            *(int*)token->value = 0;
            return OK;
        }

        if((strcmp (str,"null")) ==  0) {
            token->type = S_NULL;
            return OK;
        }

        if((strcmp (str,"true")) ==  0) {
           token->type = S_BOOL;
            if( (token->value = malloc(sizeof (int))) == NULL){
              free(str);
              return ERROR_INTER;
            }
            *(int*)token->value = 1;
            return OK;
        }

        //ani konstanty to nejsou takze se skutecne jedna o pojmenovani funkce
        token->type = S_FUNC;
        token->value = mystrdup(str);

        return OK;
        /*result = readWord(&token);
        return result;*/
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
                      return ERROR_INTER;
                  }
                  else str = more_str;
                }

                str[pozice++] = scanned;//ulozime si znak   
                scanned = fgetc(pSource_File);

            }while((scanned >= 'A' && scanned <= 'Z') || (scanned >= 'a' && scanned <= 'z') || (scanned >= '0' && scanned <= '9') || scanned == '_');

            //vratime znak navic co jsme nacetli
            fseek(pSource_File, -1,SEEK_CUR);
            token->type = S_ID;
            token->value = mystrdup(str);

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
               
               if(scanned!= '\n' && scanned != '\t' && scanned != '\v' && scanned !=' ') return ERROR_SYN;
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

       
        default: return ERROR_LEX;
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
 #if debug 
      printf("posilam hlavicky \n");    
  #endif
  if(what ==NEXT_READ){
    do{
        scanned = fgetc(pSource_File);
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
           //printf("%c",scanned);
    }while(true);    
  }
  else  return getToken(token);
  return ERROR_LEX;
}

int readNumber(T_Token *token)
{
  token->type = S_INT;
  token->value = malloc(sizeof(int));
  *(int*)token->value = 5;
  return OK;
}

int readString(T_Token *token){
  return OK;
}


char* mystrdup(const char* s)
{
    char* p = malloc(strlen(s)+1);
    if (p) strcpy(p, s);
    return p;
}
