//xnovot0m, Novotný Vojtěch
#ifndef VESTAVENE_FUNKCE_H
#define VESTAVENE_FUNKCE_H

//precte jeden radek textu
char *get_string();


char *get_substring(char[], int , int );	//vrati podretezec od pozice a do b
char *sort_string(char *);		  //serazeni retezce
int find_string(char *, char *);  //vyhledani podretezce


//prevody datovach typu
char *strval(T_ST_VarsItem input);
int intval(T_ST_VarsItem input);
double doubleval(T_ST_VarsItem input);
bool newboolval(T_ST_VarsItem input);
bool boolval(T_Token input);		//rozdil v datovych typech na vstupu

#endif
