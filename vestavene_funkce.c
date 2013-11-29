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
{  char  *array;
   int size=40;
   int i=0;

   char c=getchar();
   if(c!=EOF&&c!='\0')
       {
        array=malloc(size*sizeof(char));
        if(array==0)
            return array;
        garbage_add(array, &garbage_default_erase);

        for(;i<size; i++)
            {c=getchar();
             if(c==EOF||c=='\0')
                return array;
             array[i]=c;
            }
        size *= 2;
        }
        while(c!=EOF&&c!='\0')
       {
        array=realloc(array, size * sizeof(char));
        if(array==0)
            return array;
        garbage_add(array, &garbage_default_erase);

        for(;i<size; i++)
            {c=getchar();
             if(c==EOF||c=='\0')
                return array;
             array[i]=c;
            }
        size *= 2;
        }
    return array;
}



/* stejne jako v ial.h

char *get_substring(char input[], int a, int b)
{   int c=0, length=strlen(input);
    int lenght=b-a+1;
        //testovani parametru
    if(a<c||b<c||a>b||a>=length||b>length)
        return 0;
        
    char *output=malloc((b-a+1)*sizeof(char));
    if(output==0)
        return output;
     garbage_add(output,&garbage_default_erase);
    for(int i=0; i<(lenght); i++)       //plneni vystupniho pole
        output[i]=input[a+i-1];

    return output;
    }




     typedef struct {
        int length;
        char *first;
    }TString;


char *merge(TString left, TString right)
{
    char *output=malloc((left.length+right.length)*sizeof(char));
    int i=0;

    while(left.length>0&& right.length>0)
        {
         if(left.first[0]<right.first[0])
              {output[i]=left.first[0];
               left.length--;
               left.first=&left.first[1];
              }
         else
              {output[i]= right.first[0];
               right.length--;
               right.first=&right.first[1];
              }
         i++;
        }

    while(left.length)
        {output[i]=left.first[0];
         left.length--;
         i++;
         left.first=&left.first[1];
        }
    while(right.length)
        {output[i]=right.first[0];
         right.length--;
         i++;
         right.first=&right.first[1];
        }

    return output;
}


char *sort_string(char *vstup)
{   if(vstup==0)
        return 0;

    TString left, right, input;

    input.first=vstup;
    input.length=strlen(input.first);

    if(input.length<=1)
        return input.first;

    left.length = input.length / 2;
    right.length = input.length - left.length;

    left.first = malloc(left.length*sizeof(char));
    if(left.first==0)
    return 0;
    right.first = malloc(right.length*sizeof(char));
    if(right.first==0)
    return 0;

    int i = 0;
    for(; i<left.length; i++)
        {left.first [i] = input.first [i];
         }

    for(int j=0; j<right.length; j++)
        {right.first [j] = input.first [i+j];
         }


    left.first = sort_string(left.first);
    if(left.first==0)
        return 0;

    right.first = sort_string(right.first);
    if(right.first==0)
        return 0;


    return merge(left, right);
}

*/
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

    
    return pozice;
}


double StrToDouble(char *input)
{
    double output=0;
    unsigned int i=0, length=strlen(input);
    double increment =1;
    double  exponent=0;
    double expsign=0;
    double sign=1;

    if(input[i]=='-')
        {sign = -1;
        i++;
        }
    while(input[i]>='0' && input[i]<='9'&&i<length)
        {output *= 10;
        output += input[i] - '0';
        i++;
        }
    if(input[i]=='.')
        {i++;
        while(input[i]>='0' && input[i]<='9'&&i<length)
            {increment *= 10;
            output += (input[i] - '0') /increment;
            i++;
            }
          }
    if(input[i]=='e')
        {
            i++;
            if(input[i]=='-')
                expsign=-1;
            else if(input[i]=='+')
                        expsign=1;
                    else return NAN;
            while(input[i]>='0' && input[i]<='9'&&i<length)
                 {exponent*=10;
                 exponent += (input[i] - '0') ;
                 i++;
                }
                if(expsign!=0)
                    exponent *= expsign;
        }
        if(i!=length)
            return NAN;
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
    int sign=1;

    while((input[i]<'0' || input[i]>'9')&&input[i]!='-'&&i<length)
        i++;
        
    if(input[i]=='-')
        {sign = -1;
        i++;
        }
    while(input[i]>='0' && input[i]<='9'&&i<length)
        {output *= 10;
        output += input[i] - '0';
        i++;
        }
        return (sign * output );
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
    int i=0;
    int cislice;
    for(; input>=10; i++)
        {
            input/=10;
            i++;
        }
    char *output=malloc(sizeof (char)*i+1);
    garbage_add(output, &garbage_default_erase);
    for(int j=0; i>j;j++ )
    {
        cislice =input/1+'0';
        output[j]=input+ '0';
        input-=cislice;
        input*=10;
    }
    return output;
}

char *DoubleToStr(double input)
{   int i=0;
    int cislice;
    for(; input>=10; i++)
        {
            input/=10;
            i++;
        }
    char *output=malloc(sizeof (char)*i+1);
    garbage_add(output, &garbage_default_erase);
    for(int j=0; i>j;j++ )
    {
        cislice =input/1+'0';
        output[j]=cislice;
        input-=cislice-'0';
        input*=10;
    }
    while(input !=0)
        {input *=10;
        cislice = input/1 +'0';
        output=realloc(output, sizeof(output)+sizeof(char));
        output[i]=cislice;
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

