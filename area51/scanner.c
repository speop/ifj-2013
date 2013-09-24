//Implementace interpretu imperativního jazyka IFJ12.
// xbucht18, Buchta David

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define NDEBUG 0				//zobrazeni komentaru kdyz NDEBUG 1

#include "global.h"
#include "scanner.h"

char *IFJ_KEY_WORDS[] = {"function","if", "else", "while","return","sort","find","len","typeOf","print","numeric","input","end","true","false","nil"};
char *IFJ_RESERVED_WORDS[] = {"as","def","directive","export","from","import","launch", "load", "macro"};

const char *LEX_ECODEMSG[] =
{//vyznamy chybovych stavu
  [EOK] = "Vse v poradku.\n",
  [EHexWrong] = "spatne zapsany hexadecimalni kod znaku\n",
  [EExpWrong]="chybne zapsany exponent\n",
  [ENumWrong]="chybne zapsane cislo\n",
  [EComToEof]="komentar do konce souboru\n",
  [EExclMarkWrong]="chybny znak abecedy. Nemysleli jste \"!=\"?\n",
  [ENotAlph] = "neplatny symbol\n",
  [EStrEof] = "spatne napsany retezec\n",
};

const int ascii_nula=48;
const int ascii_A_to_hex=55;
const int ascii_a_to_hex=87;

extern int row;
extern FILE *pSource_File;

char *strdup (const char *s) {
    char *d = malloc (strlen (s) + 1);   // Allocate memory
    if (d != NULL)
        strcpy (d,s);                    // Copy string if okay
    return d;                            // Return new memory
}



int getNextToken(T_Token_struct *token)
{
	#if NDEBUG==1
	printf("Ahoj ja jsem funkce getNextToken\n");
	#endif
	char scanned, real_scanned,c;
	int token_ready,pom;
	fpos_t pos;
//     if(token->value != NULL ){free(token->value); token->value = NULL;}   //uvolnime predchozi zpravu
	token_ready = 0;
	if(token->type==S_EOL)
		row++;

		if( token->state == 1) scanned = token->previous;
		else scanned = fgetc (pSource_File);
		token->state = 0;
			#if NDEBUG==1
			printf("Znak ktery jsem nacetla je: %c\n",scanned);
			#endif

		do {
			if (scanned == EOF) { token->type= S_EOF; return PROG_OK;}
			else if (scanned >= '0' && scanned <= '9') 	{ 	//DIGIT
															real_scanned = scanned;
															scanned = 'd';
													}
			else if ((scanned >= 'a' && scanned <= 'z') || (scanned >= 'A' && scanned <= 'Z')) 	{ 	//LETTER
																									real_scanned = scanned;
																									scanned = 'l';
																								}
			switch (scanned)
				{
					case '+': {	token->type = S_PLUS;
								token_ready = 1;
								break; }

					case '-': {	token->type= S_MINUS;
								token_ready = 1;
								break;}

					case '/': {	c = fgetc(pSource_File);

								if (c=='/') {//komentar do konce radku
											do{ c = fgetc(pSource_File);
											if( c== EOF)
											{   token->type= S_EOF;
                                                return PROG_OK;}}

											while(c!='\n');
											token_ready = 1;
											token->type= S_EOL;
// 											row++;
								}

								else if (c=='*') 	{//blokovy komentar
													pom = 1;
													do {	c = fgetc(pSource_File);
                                                            if(c=='\n'){ row++;} // zvetsime pocet radku jelikoz komentar je pres vice radku
															if (c=='*'){	c = fgetc(pSource_File);
																			if(c=='/') pom=0;}

															else if (c == EOF)	{	token->type = L_PARSE_ERROR;    // komentar do konce souboru mozna error tak vratime error
                                                                                        token->value=strdup(LEX_ECODEMSG[EComToEof]);
                                                                                        return LEX_ERROR;}
                                                        }
													while(pom);
													}

								else {	token->state = 1;
										token->previous = c;
										token->type = S_DIV;
										token_ready = 1;}
                                        break;}

					case '*': {	c=fgetc(pSource_File);
								if (c=='*') token->type = S_POW;
								else {	token->state= 1;
										token->previous = c;
										token->type = S_MUL;}
								token_ready = 1;
								break;}



					case '<': {	c=fgetc(pSource_File);
								if (c=='=') token->type= S_LEQ;
								else {	token->state= 1;
										token->previous = c;
										token->type=S_LST;}
								token_ready = 1;
								break;}

					case '>': {	c=fgetc(pSource_File);
								if (c=='=') token->type=S_GEQ;
								else {	token->state= 1;
										token->previous = c;
										token->type = S_GRT;}
								token_ready = 1;
								break;}

					case '!': {	c=fgetc(pSource_File);
								if (c=='=') token->type = S_NEQ;
								else {	token->state= 1;
										token->previous = c;
                                        token->type = L_PARSE_ERROR;
                                        token->value=strdup(LEX_ECODEMSG[EExclMarkWrong]);
                                        return LEX_ERROR;}
								token_ready = 1;
								break;}

					case '=': {	c=fgetc(pSource_File);
								if (c=='=') token->type = S_EQ;
								else {	token->state= 1;
										token->previous = c;
										token->type = S_IS;}
								token_ready = 1;
								break;}



					case '(': {	token->type= S_LBRA;
								token_ready = 1;
								break;}

					case ')': {	token->type= S_RBRA;
								token_ready = 1;
								break;}

					case '[': {	token->type= S_LSBRA;
								token_ready = 1;
								break;}

					case ']': {	token->type= S_RSBRA;
								token_ready = 1;
								break;}



					case ':': {	token->type = S_COLON;
								token_ready = 1;
								break;}

					case '"': { if(get_string(token)) return LEX_ERROR;
								token_ready = 1;
								break;}

					case ',': {	token->type = S_COMMA;
								token_ready = 1;
								break;}


					/* odkaz retezce*/
					case '_': 	// promena muze zacinat i podrzitkem, jestli to je klicove nebo vyhrazene slovo se ted neresi, nejdriv se to bere jako promena, pak az se docte cele tak se zkontroluje na zaklade tabulky

					case 'l': { // zpracovani promenych a klicovych slov...
								// funkce proveri delku retezce abychom mohli na tolik alokovat pamet

								fgetpos (pSource_File,&pos); // ulozime pozici ukazatele v souboru
								pom = string_length (real_scanned,token);


								if( pom == 0 )
									{
										token->type = S_BOOL;
										if((token->value =  (void*) malloc(sizeof(int))) == NULL) {token->type = L_INTER_ERROR; return INTER_ERROR;}
										*(int*)token->value = 0;
										token_ready = 1;
									}
								else if (pom == -1)
									{
										token->type = S_BOOL;
										if((token->value =  (void*) malloc(sizeof(int))) == NULL) {token->type = L_INTER_ERROR; return INTER_ERROR;}
										*(int*)token->value = 1;
										token_ready = 1;
									}

								else if (pom == 1)
								{
										token->type = ID;
										if((token->value =  (void*) malloc(sizeof(char)+1)) == NULL) {token->type = L_INTER_ERROR; return INTER_ERROR;}
										((char*)token->value)[0] = real_scanned;
										((char*)token->value)[1]='\0';
										token_ready = 1;
								}

								else
								{	//alokujem pamet na pocet +1 z duvodu \0; a ulozime cteny retezec
									pom++;
									if((token->value = (void*) malloc(pom*sizeof(char)+1)) == NULL) {token->type = L_INTER_ERROR; return INTER_ERROR;}

									int index = 1;
									((char*)token->value)[0] = real_scanned;
									//musime vratit ukazatel v souboru zpet
									fsetpos (pSource_File,&pos);

									//smycka ktera cte cte vsup po znacich dokud neposklada cely vstupni retezec
									do{	scanned = fgetc(pSource_File);

										if(index>pom){free(token->value); token->type = L_INTER_ERROR; return LEX_ERROR; }

										//pridame do token->value znak
										if ((scanned >= 'a' && scanned <= 'z') || (scanned >= 'A' && scanned <= 'Z') || (scanned >= '0' && scanned <= '9') || scanned == '_')
										{
											((char*)token->value)[index]= scanned;
											index++;
										}
										else {	token->state = 1;
												token->previous = scanned;
												((char*)token->value)[index]='\0';
												token_ready = 1;} }
									while(!token_ready);

									int word_position, loop_end=0;
                                    // prohledame tabulky jestli se nejedna o rezerevovane nebo klicove slovo
									word_position = 0;
									do{
										if(!strcmp(token->value, IFJ_KEY_WORDS[word_position])) {token->type=(word_position + 1); loop_end=1;
										#if NDEBUG==1
										printf("word_position je: %d a hodnota kterou jsem ulozila je %d",word_position,token->type);
										#endif
										}
									    word_position++;

									}
									while(!loop_end && word_position < 16);
									if(token->type==14)token->type=S_TRUE;
									else if(token->type==14)token->type=S_FALSE;
									else if(token->type==14)token->type=S_NIL;

									if(!loop_end)
									{
									   word_position = 0;
                                        do{
                                            if(!strcmp(token->value, IFJ_RESERVED_WORDS[word_position])) {token->type= W_RESERVED; loop_end=1;}
                                            word_position++;

                                            }
                                        while(!loop_end && word_position < 9);
									}

									if(!loop_end) token->type = ID;
                           }
									break;}

					case 'd':{  if (!get_number(real_scanned, token)) token->type = S_LITERAL;
                                else return LEX_ERROR;
								token_ready = 1;
								break;}



					case '\n': {	token->type = S_EOL;
									token_ready = 1;
// 									row++;
									break;}

					case ' ': break; // space
					case '	': break; // tabulator

					default: {  token->type = L_PARSE_ERROR;    // komentar do konce souboru mozna error tak vratime error
                                token->value=strdup(LEX_ECODEMSG[ENotAlph]);
                                return LEX_ERROR;}


				}
				if(!token_ready) {scanned = fgetc (pSource_File);
			#if NDEBUG==1
			printf("dalsi v poradi co jsem nacetla je: %c\n",scanned);
			#endif
				}
		}
		while(!token_ready);

		#if NDEBUG==1
		printf("Zavrete oci odchazim  - getNextToken\n");
		#endif
		return PROG_OK;

}


int string_length (char first, T_Token_struct *token)
{
	// funkce zkouma retezec v vraci 0 v pripade ze bylo slovo FALSE, -1 v pripade TRUE, nebo delku retezce
	#if NDEBUG==1
	printf("Ahoj ja jsem funkce length\n");
	#endif
	int count=1,pom =1;
	char c;

	if (first == 't' )									// nejakou smycku nebo neco, tak ne, np. i=0;pom[]="true"; while(i<4&&c=fgetc(pSource_File)){if(c!=pom[i]){neco=false;break;}}}
	{	c = fgetc(pSource_File);
		count++;
		if(c=='r')
		{	c = fgetc(pSource_File);
			count++;
			if(c=='u')
			{	c = fgetc(pSource_File);
				count++;
				if(c=='e')
				{	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') count++;
					else
					{	token->state=1;
						token->previous= c;
						return -1;}}
			}
		}
	}

	else if (first=='f')									// nejakou smycku nebo neco, tak ne, np. i=0;pom[]="false"; while(i<4&&c=fgetc(pSource_File)){if(c!=pom[i]){neco=false;break;}}}
	{
		c = fgetc(pSource_File);
		count++;
		if(c=='a')
		{	c = fgetc(pSource_File);
			count++;
			if(c=='l')
			{	c = fgetc(pSource_File);
				count++;
				if(c=='s')
				{	c = fgetc(pSource_File);
					count++;
					if(c=='e')
					{	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') count++;
						else
						{	token->state=1;
							token->previous= c;
							return 0;}}
				}
			}
		}
	}

	do
	{
		c = fgetc(pSource_File);
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') count++;
		else pom=0;
	}
	while (pom);


	if (count==1)
	{ 	token->state=1;
		token->previous = c;
	}

	#if NDEBUG==1
	printf("Odchazim a pritom vracim delku: %d\n",count);
	#endif
	return count;
}

int get_number(char first_digit, T_Token_struct *token) /*Funkce vezme prvni cislici zapisu cisla a pokracuje ve cteni dokud neni cislo prectene, nebo nenastala chyba*/
{

	#if NDEBUG==1
	printf("Ahoj ja jsem Bolkova super funkce na nacitani cisel \n");
	#endif
  double number=first_digit-ascii_nula;
  int exponent=0;
  int exponent_sign=1;
  double decimal=0.1;
   int byla_nactena_cislice = 0;
  char c;

  if((token->value = (void*)malloc(sizeof(double)))== NULL) {token->type = L_INTER_ERROR; return INTER_ERROR;}


  while ((c=fgetc(pSource_File))!=10||c!=' ') /*Ve finale zmenit getchar na fgetc*/
  {
    if (c>='0'&&c<='9') number=number*10+c-ascii_nula;
    else break;
  }
  if (c=='.')
  {

    while (1) /*Nacitani desetinne casti*/
    {
      c=fgetc(pSource_File);

      if (c>='0'&&c<='9')
      {
      number=number+(c-ascii_nula)*decimal;
      decimal/=10;
      byla_nactena_cislice = 1;
      }
      else
      {
        if(c=='e')
        {
          c=fgetc(pSource_File);
            if (c=='+');
            else if (c=='-') exponent_sign=-1;
            else if (c>='0'&&c<='9') {exponent=exponent*10+c-ascii_nula; byla_nactena_cislice = 1;}
            else { //spatny tvar exponentu
                    token->state=1;
                    token->previous=c;
                    token->type = L_PARSE_ERROR;
                    token->value=strdup(LEX_ECODEMSG[EExpWrong]);
                    return 2;
                 }



         while ((c=fgetc(pSource_File))>='0'&&c<='9') {exponent=exponent*10+c-ascii_nula; /*Nacitani exponentu*/ byla_nactena_cislice = 1;}

            if(byla_nactena_cislice)
            {
                exponent*=exponent_sign;
                number=number*pow(10,exponent);
                token->state=1;
                token->previous=c;
                token->type =S_LITERAL;
                *(double*)token->value=number;
                return PROG_OK;
            }

             else { //spatny tvar cisla
                    token->state=1;
                    token->previous=c;
                    token->type = L_PARSE_ERROR;
                    token->value=strdup(LEX_ECODEMSG[ENumWrong]);
                    return 2;
                    }

        }
        else
        {
            if(byla_nactena_cislice)
            {
                token->state=1;
                token->previous=c;
                token->type =S_LITERAL;
                *(double*)token->value=number;
                return PROG_OK;
            }

            else { //spatny tvar cisla
                    token->state=1;
                    token->previous=c;
                    token->type = L_PARSE_ERROR;
                    token->value=strdup(LEX_ECODEMSG[ENumWrong]);
                    return 2;
                    }
        }

      }
    }
  }
  else
  {
   if(c=='e')
        {
          c=fgetc(pSource_File);
            if (c=='+');
            else if (c=='-') exponent_sign=-1;
            else if (c>='0'&&c<='9') {exponent=exponent*10+c-ascii_nula; byla_nactena_cislice = 1;}
             else { //spatny tvar exponentu
                    token->state=1;
                    token->previous=c;
                    token->type = L_PARSE_ERROR;
                    token->value=strdup(LEX_ECODEMSG[EExpWrong]);
                    return 2;
                 }



         while ((c=fgetc(pSource_File))>='0'&&c<='9') {exponent=exponent*10+c-ascii_nula; /*Nacitani exponentu*/ byla_nactena_cislice = 1;}

           if(byla_nactena_cislice)
            {
                exponent*=exponent_sign;
                number=number*pow(10,exponent);
                token->state=1;
                token->previous=c;
                token->type =S_LITERAL;
                *(double*)token->value=number;
                return PROG_OK;
            }

             else { //spatny tvar cisla
                    token->state=1;
                    token->previous=c;
                    token->type = L_PARSE_ERROR;
                    token->value=strdup(LEX_ECODEMSG[ENumWrong]);
                    return 2;
                    }

        }
    else { //spatny tvar cisla
            token->state=1;
            token->previous=c;
            token->type = L_PARSE_ERROR;
            token->value=strdup(LEX_ECODEMSG[ENumWrong]);
            return 2;
        }


  }

  return PROG_OK;
}

int get_string(T_Token_struct *token) /*Funkce je zavolana po nacteni uvozovek a cte dokud nenarazi na uvozovky. Vrati token typu string*/
{
  //T_struct_token string_token;
  char c;
  char *retezec;
  int x, cislo=0, pomocna, i=0, velikost = 16;

  if((retezec = (char*) malloc(velikost *sizeof(char))) == NULL ) {token->type = L_INTER_ERROR; return INTER_ERROR;}

  while ( (c=fgetc(pSource_File))!='"'&& c!=EOF)
  {



	  #if NDEBUG==1
    printf("Ahoj nacetla jsem znak\'%c\'\n",c);
    #endif
    if(i>(velikost-4))          // overime velikost bufferu v pripade problemu realokujeme  a v pripade chyby reallocu vse uvolnime a vratime chybu
    {   velikost*=2;
        int pom128;
        pom128=velikost *sizeof(char);
        //printf("velikost alokace je: %d",pom128);
        if((retezec = (char*) realloc(retezec, pom128)) == NULL ) {token->type = L_INTER_ERROR; free(retezec); return 1;}
    }

    if(c=='\\')
    {
        c=fgetc(pSource_File);
        if(c=='\\' )        retezec[i++]='\\';
        else if( c=='"')    retezec[i++]='"';
        else if( c=='n')    retezec[i++]='\n';
        else if (c=='t')    retezec[i++]='\t';
        else if (c!='x')
        {
                    token->type = L_PARSE_ERROR;
                    token->value=strdup(LEX_ECODEMSG[EHexWrong]);
                    free(retezec);
                    return 2;
        }

        else
        {
             for (x=0;x<2;x++)
             {
                 c=fgetc(pSource_File);
                 if(c>='0'&&c<='9') pomocna = c-ascii_nula;
                 else if (c>='A'&&c<='F') pomocna = c-ascii_A_to_hex;
                 else if (c>='a'&&c<='f') pomocna = c-ascii_a_to_hex;
                 else  //nepovoleny znak hexadecimalniho cisla nebo nejsou  dve cislice
                 {
                    while ( (c=fgetc(pSource_File))!='"'&& c!=EOF);  // docteme do konce retezce a ignorujeme zbytek
                    token->type = L_PARSE_ERROR;
                    token->value=strdup(LEX_ECODEMSG[EHexWrong]);
                    free(retezec);
                    return 2;
                 }


                cislo*=16;
                cislo+=pomocna;
                if(!cislo)
                {
                    while ( (c=fgetc(pSource_File))!='"'&& c!=EOF);  // docteme do konce retezce a ignorujeme zbytek
                    token->type = L_PARSE_ERROR;
                    token->value=strdup(LEX_ECODEMSG[EHexWrong]);
                    free(retezec);
                    return 2;
                }
             }

            //printf("%d",cislo);
            retezec[i++] = (char)cislo; // prevedeme hexakod na dany znak
        }

    }

    else retezec[i++]=c;
  }

  if(c==EOF)
  {
	  token->type = L_PARSE_ERROR;
     token->value=strdup(LEX_ECODEMSG[EStrEof]);
     free(retezec);
     return LEX_ERROR;
	}
  retezec[i]='\0';
  #if NDEBUG==1
  printf("\"\n%s\n\"",retezec);
  #endif
  token->type = S_STR ;

  token->value=strdup(retezec);
  free(retezec);


 return PROG_OK;
}

char* first_pass()
{
	char *func_name,function[]="function";
	int c=0,i=0,pom,pom_2,len=0;
	while((c=fgetc(pSource_File))!=EOF){
		if(c=='/')
		{
			if((c=fgetc(pSource_File))==EOF)return '\0';
			if(c=='/')while((c=fgetc(pSource_File))!=EOF&&c!='\n');
			else if(c=='*')
			{
				pom='*';
				pom_2=0;
				while((c=fgetc(pSource_File))!=EOF){
					if(pom=='*')
						pom_2=1;
					else if(pom_2==1&&c=='/')
						break;
					pom_2=0;
				}
			}
		}
		else if(c=='"')
			while((c=fgetc(pSource_File))!=EOF&&c!='"');
		else if(i==9)
		{
			if((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9'))
			{
				if(len==0)
					func_name=malloc(len*sizeof(char)+2);
				else
					func_name=realloc(func_name, len*sizeof(char)+2);
				func_name[len]=c;
				len++;
				continue;
			}
			func_name[len]='\0';
			return func_name;

		}
		else if(i==8)
		{
			if(isspace(c))
			{
				i++;
				continue;
			}
		}
		else if(function[i]==c)
		{
			i++;
			continue;
		}

		i=0;
	}
	rewind(pSource_File);
	return '\0';
}
