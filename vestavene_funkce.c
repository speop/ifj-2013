#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "garbage_collector.h"
#include "types.h"
#include "ial.h"
#include "scaner.h"
#include <math.h>


//#define NAN (0.0/0.0)

//const int ASCII_ZERO=48;
//přečte jednenřádek ze vstupu
//pokud se nepovede některá alokace, vracím nulu
//HOTOVO

char *get_string()
{  char  *str, *more_str;
   int alokovano = 0;
   int prirustek = 32;
   int iterator =0;



   char c=getchar();
   str = (char *)malloc(prirustek*sizeof(char));
   alokovano += prirustek;

  
   while(c!= EOF && c!='\n')
   {
        // v pripade potreby zvetsime pamet na retezec
        if(iterator >= alokovano) {
            alokovano  += prirustek;

            more_str = (char*) realloc (str, alokovano * sizeof(int));
            if(more_str == NULL){
                free(str);
                return NULL;
            }
            else str = more_str;
        }

        str[iterator++] = c;//ulozime si znak   
        c = getchar();
   }

   //nahrajeme koncovy znak
   if(iterator >= alokovano) {
        alokovano  += 1;
        more_str = (char*) realloc (str, alokovano * sizeof(int));
        
        if(more_str == NULL){
            free(str);
            return NULL;
        }
        else str = more_str;
    }

    str[iterator] = '\0';
    return str;
}



// hotové, funguje
int find_string(char *string1, char *string2)
{
    int length1 = strlen(string1);  //
    int length2 = strlen(string2), pozice=0;
    int shoda=0;

    if(length1==0)
        return -1;
    if((length2)==0)
        return 0;
        //hledá string2 uvnitř string1
    while(pozice<length1 && shoda == 0)
        {
         for(int i=length2-1; i>=0; i--)
            {if(string2[i]==string1[pozice+length2-1])
                {shoda=1;
                 break;
                }
             }
           
         if(shoda)
            {for(int j=0; j<length2; j++)
                {if(string2[j]!=string1[pozice+j])
                    {shoda = 0;
                     pozice += j+1;
                     break;
                    }
                }
            }
         else
            pozice += length2;
        }

    if(!shoda) pozice = -1;
    return pozice;
}


double StrToDouble(int *ret, char *input)
{
    //double output = 0;
    unsigned int i = 0, length = strlen(input);
    //double increment = 1;
    //double  exponent = 0;
    //double expsign = 1;
    double sign = 1;
    //char c;
    const int ASCII_ZERO=48;
    //int a = 0, b = 0, c = 0;          //pocitadla cislic

    int firstTime = 1;
    int isDecimal = 0;
    int decimal = 10;                                       //Pocitadlo desetinneho mista
    int exp = 0;                                            //hodnota exponentu
    int expS = 0;                                           //stav exponentu
    int expM = 1;                                           //multiplikator exponentu (+-1)
    int n=0;                                  //PREDANY PARAMETR PRVNIHO PRECTENEHO (PODLE KTEREHO VIME, ZE JE TO CISLO)
    double num = 0;   

    // preskocime bile znaky (tabulator, mezera, konec radku)    
    while((input[i] == ' ' || input[i] == ' ' || input[i] == '\n')&&i<length)
        i++;
   
    if(input[i]=='-')
        {sign = -1;
         i++;
        }

    n = input[i++];
    if(!(n!='\0' && ((n >= '0' && n <= '9') || (n == '+' && expS == 1) || (n == '-' && expS == 1) || ((n == 'e' || n == 'E') && (expS == 0)) || ((n == '.') && (isDecimal == 0) && (expS == 0))))) {fprintf(stderr, "Problem pri pretypovanina double\n" ); return SEM_RETYPE;}
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
            } 
            else if (n == 'e' || n == 'E') {
                expS = 1;                                          //a mame tu exponent
            }
            else {
                if (!firstTime) {
                    fprintf(stderr, "Lexikalni chyba u doubleval \n");
                    *ret =  SEM_RETYPE;                                  //a tady neco falesneho
                    return 0;
                }
            }
        }

        firstTime = 0;

        n =  input[i++];
    }while(n!='\0' && ((n >= '0' && n <= '9') || (n == '+' && expS == 1) || (n == '-' && expS == 1) || ((n == 'e' || n == 'E') && (expS == 0)) || ((n == '.') && (isDecimal == 0) && (expS == 0))));
    
    if (expS >= 1) {
        exp *= expM;
        num = num * pow(10.0, exp);
    }


    if ((expS == 1) || (expS == 2)) {
        fprintf(stderr, "Lexikalni chyba u doubleval \n");
        *ret =  SEM_RETYPE;                                  //a tady neco falesneho
        return 0;      //Bud bylo nacteno jen e/E nebo e/E a znamenko
    }

    if (isDecimal == 1) {
        fprintf(stderr, "Lexikalni chyba u doubleval \n");
        *ret =  SEM_RETYPE;                                  //a tady neco falesneho
        return 0;      //Byla nactena tecka, avsak ne des. cast
    }

    return (sign * num );
}



double IntToDouble(int input)
{
    return input;
}

double BoolToDouble(bool input)
{
    if(input)
        return 1;
    else
        return 0;
}
int DoubleToInt(double input)
{
    return input;
}

bool StrToBool(char *input)
{
    if(strlen(input))
        return true;
    else return false;
}
int StrToInt(char *input)
{
    int output=0;
    unsigned int i=0, length=strlen(input);


    // preskocime bile znaky (tabulator, mezera, konec radku)    
    while((input[i] == ' ' || input[i] == ' ' || input[i] == '\n')&&i<length)
        i++;
   
    while(input[i]>='0' && input[i]<='9'&&i<length)
        {output *= 10;
        output += input[i] - '0';
        i++;
        }
        return output;
}

int BoolToInt(bool input)
{
    return input;
}

bool IntToBool(int input)
{
    if(input==0)
        return false;
    else return true;
}
bool DoubleToBool(double input)
{
    if(input==0.0)
        return false;
    else return true;
}
bool StringToBool(char *input)
{
    if(strlen(input)==0)
            return false;
    else return true;
}

char *BoolToStr(bool input)
{if(input)
    return "1";
else return "";
}

char *IntToStr(int input)
{
   
    char *output= (char *)malloc(sizeof (char)*256);
    if(sprintf(output, "%d", input) == 0)
        return NULL;
    /*int i, j;
    int cislice;
    double source = input;
    for(i = 1; input>=10; i++)   //zjistime pocet cislic
        {
            input /= 10;
            source /= 10;
        }
    
    char *output= (char *)malloc(sizeof (char)*i+1);
    
    for(j = 0; i>j;j++ )
    {
        cislice = source;
        output[j] = source + '0';
        source -= cislice;
        source *= 10;
    }
    output[j] = '\0'; */
    return output;
}


char *DoubleToStr(double input)
{
    //int k, length;
    char *output = malloc(512 * sizeof(char));
    if(sprintf(output, "%g", input) == 0)
        return NULL;
    
    /*
    length = strlen(output);
    for(int j = 0; j<length; j++)
        if(output[j] != '0')        //nalezneme posledni nenulu
            k = j+1;
            
    output[k] = '\0';
    //printf("%d\n", k);      //pokud je posledni znak '.', tak ji oddelame
    if(output[k-1] =='.')
        output[k-1] = '\0'; */
    return output;
}

bool newboolval(T_ST_VarsItem input)
{   
   switch (input.type){
    case S_INT: return IntToBool(*(int *)input.value);

    case S_DOUB: return DoubleToBool(*(double*)(input).value);

    case S_STR: return StrToBool((char*)(input).value);

    case S_BOOL: return *(int*)input.value; // vnitrne je bool reprezentovan jako int
                  
    case S_NULL: return false;
    }

    return false;
}

bool boolval(T_Token input)
{   
   switch (input.type){
    case S_INT: return IntToBool(*(int *)input.value);

    case S_DOUB: return DoubleToBool(*(double*)(input).value);

    case S_STR: return StrToBool((char*)(input).value);

    case S_BOOL: return *(int*)input.value; // vnitrne je bool reprezentovan jako int
                  
    case S_NULL: return false;
    }

    return false;
}

double doubleval(int *ret, T_ST_VarsItem input)
{   
   switch (input.type){
    case S_INT: return IntToDouble(*(int *)input.value);

    case S_DOUB: return *(double*)(input).value;

    case S_STR: return StrToDouble(ret, (char*)(input).value);

    case S_BOOL: return BoolToDouble(*(int*)input.value); // vnitrne je bool reprezentovan jako int
                  
    case S_NULL: return false;
    }

    return false;
}

int intval(T_ST_VarsItem input)
{   
   switch (input.type){
    case S_INT: return *(int *)input.value;

    case S_DOUB: return DoubleToInt(*(double*)(input).value);

    case S_STR: return StrToInt((char*)(input).value);

    case S_BOOL: return *(int*)input.value; // vnitrne je bool reprezentovan jako int
                  
    case S_NULL: return 0;
    }

    return false;
}

char *strval(T_ST_VarsItem input)
{   
   switch (input.type){
    case S_INT: return IntToStr(*(int *)input.value);

    case S_DOUB: return DoubleToStr(*(double*)(input).value);

    case S_STR:  return mystrdup((char*)(input).value);

    case S_BOOL: return BoolToStr(*(int*)input.value); // vnitrne je bool reprezentovan jako int          
    case S_NULL: return "";
    }

    return NULL;
}

