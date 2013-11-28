//xnovot0m, Novotný Vojtěch
#ifndef VESTAVENE_FUNKCE_H
#define VESTAVENE_FUNKCE_H

//precte jeden radek textu
char *get_string();


char *get_substring(char[], int , int );	//vrati podretezec od pozice a do b
char *sort_string(char *);		  //serazeni retezce
int find_string(char *, char *);  //vyhledani podretezce


//prevody datovach typu
void strval(T_Token input, T_Token *);
void intval(T_Token input, T_Token *);
void doublelval(T_Token input, T_Token *);
bool boolval(T_Token input);

#endif
