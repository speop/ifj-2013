#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "garbage_collector.h"
#include "types.h"
#include "ial.h"

#define NAN (0.0/0.0)

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


double StrToDouble(char *input)
{
    double output = 0;
    unsigned int i = 0, length = strlen(input);
    double increment = 1;
    double  exponent = 0;
    double expsign = 1;
    double sign = 1;

    // preskocime bile znaky (tabulator, mezera, konec radku)    
    while((input[i] == ' ' || input[i] == ' ' || input[i] == '\n')&&i<length)
        i++;
   
    if(input[i]=='-')
        {sign = -1;
         i++;
        }
    while(input[i]>='0' && input[i]<='9' && i<length)
        {output *= 10;
         output += input[i] - '0';
         i++;
        }
    if(input[i]=='.')
        {i++;
         while(input[i]>='0' && input[i]<='9' && i<length)
            {increment *= 10;
            output += (input[i] - '0') / increment;
            i++;
            }
        }
        
    if(input[i] == 'e' || input[i] == 'E')
        {
         i++;
         if(i<length && input[i] == '-' )
            {expsign = -1;
            i++;
            }
         else if(input[i] == '+')
                    {expsign = 1;
                    i++;
                    }
         while(i<length && input[i]>='0' && input[i]<='9')
             {exponent*=10;
             exponent += (input[i] - '0') ;
             i++;
            }
         exponent *= expsign;
        }
        if(exponent>0)
            for(; exponent>0; exponent--)
                output*=10;
        if(exponent<0)
            for(;exponent<0; exponent++)
                output/=10;
    return (sign * output );
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
    int i=1;
    int cislice;
    double source = input;
    for(; input>=10; i++)   //zjistime pocet cislic
        {
            input /= 10;
            source /= 10;
        }
    
    char *output=malloc(sizeof (char)*i+1);
    garbage_add(output, &garbage_default_erase);
    
    for(int j=0; i>j;j++ )
    {
        cislice = source;
        output[j] = source + '0';
        source -= cislice;
        source *= 10;
    }
    return output;
}


char *DoubleToStr(double input)
{   int i=0, j = 0; //i= pocet cislis pred des. teckou, j= pocet cislic
    int cislice;
    int abc = 0;
    while(input>=10)
        {
            input/=10;
            i++;
        }
        
    while((abc-input) != 0)
        {
         input *= 10;
         abc = input;
         j++;
        }
         
    char *output=malloc(sizeof (char)*(j+2));
    garbage_add(output, &garbage_default_erase);
    
        
    for(; i>j;j--)
    {	
        cislice = abc % 10;
        output[j+1] = cislice + '0';
        abc -= cislice;
    }
    
    output[j] ='.';
    
    for(; j>=0; j--)
        {
         cislice = abc % 10;
         output[j] = cislice + '0';
         abc -= cislice;
         abc /= 10;
        }
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

double doubleval(T_ST_VarsItem input)
{   
   switch (input.type){
    case S_INT: return IntToDouble(*(int *)input.value);

    case S_DOUB: return *(double*)(input).value;

    case S_STR: return StrToDouble((char*)(input).value);

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

    case S_STR: return (char*)(input).value;

    case S_BOOL: return BoolToStr(*(int*)input.value); // vnitrne je bool reprezentovan jako int
                  
    case S_NULL: return "";
    }

    return false;
}

