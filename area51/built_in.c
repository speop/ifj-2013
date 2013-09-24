//Implementace interpretu imperativního jazyka IFJ12.
//xbucht18, Buchta David

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "built_in.h"
#define max(a, b) ((a < b) ? b : a)


//funkce nacte znaky ze std input a vrati jako retezec
void bi_input(char** d)
{
    char *str,c;
    int delka=8,index = 0;

    str = malloc(delka * sizeof(char));

    c=getchar();
    if(c=='\n') {free(str); return;}

    str[index++]=c;

    while((c=getchar())!='\n')
    {
        if(index==(delka-1)) {delka*=2; str=realloc(str,delka*sizeof(char));}
        str[index++]=c;
    }

    str[index]='\0';

    *d =malloc (strlen (str) + 1);
    if (*d != NULL)
        strcpy (*d,str);

    free(str);
    //insert_free_char(d);
    return;

}

int bi_numeric(char *str, double *number)
{

	int asci_zero=48;
    int pozice=0;



  double cislo;//=str[pozice++]-asci_zero;

  int exponent=0;
  int exponent_sign=1;
  double decimal=0.1;
//   char c;

 // if((token->value = (void*)malloc(sizeof(double)))== NULL) {token->type = L_INTER_ERROR; return 1;} asi uy dostanu nejakz pointer na number ktere uy bude mit alokovanou pamet, pokud ne tak ji tu muzu alokovat al budeu muset vztvorit seznam ukazujici na doubly ktere potom uvolnim


  while (1)
  {
    if ((str[pozice]>='0')&&(str[pozice]<='9')) {cislo*=10;cislo+=str[pozice++]-asci_zero;}
    else break;
  }
  if (str[pozice]=='.')
  {
    pozice++;
    while (1) /*Nacitani desetinne casti*/
    {

      if ((str[pozice]>='0')&&(str[pozice]<='9')) {   cislo+=(str[pozice++]-asci_zero)*decimal;
                                                        decimal/=10;}
      else
      {
        if(str[pozice]=='e')
        {
            pozice++;
            if (str[pozice]=='+');
            else if (str[pozice]=='-') exponent_sign=-1;
            else if ((str[pozice]>='0')&&(str[pozice]<='9')) {exponent*= 10;exponent+=(str[pozice]-asci_zero);}
            else return 12;

            pozice++;

            while ((str[pozice]>='0')&&(str[pozice]<='9')) {exponent*= 10;exponent+=(str[pozice++]-asci_zero);} /*Nacitani exponentu*/

            exponent*=exponent_sign;
            cislo*=pow(10,exponent);
            *number =cislo;
            return 0;}

        else if(str[pozice]=='\0'){   *number =cislo;
                return 0;}
        else return 12;

      }
    }
  }
  else
  {
   if(str[pozice++]=='e')
        {
            if (str[pozice]=='+');
            else if (str[pozice]=='-') exponent_sign=-1;
            else if ((str[pozice]>='0')&&(str[pozice]<='9')){ exponent*= 10;exponent+=(str[pozice]-asci_zero);}
            else return 12;

            pozice++;
            while ((str[pozice]>='0')&&(str[pozice]<='9')) {exponent*= 10;exponent+=(str[pozice++]-asci_zero);}/*Nacitani exponentu*/

            exponent*=exponent_sign;
            cislo*=pow(10,exponent);
            *number =cislo;
            return 0;

        }
    else return 12; //spatny tvar cisla

    }

  return 0;
}
