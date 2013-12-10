//Implementace interpretu imperativního jazyka IFJ13.
//xbucht18, Buchta David

//binarni strom
// 1,2,4,3 razeni
#include "stdlib.h"
#include <stdio.h>
#include "garbage_collector.h"
#include "types.h"
#include "parser.h"
#define debug 0

FILE *pSource_File;
TGarbageList trash;
T_Token *prevToken;
int row;

bool test(void * data);


int main(int argc, char **argv)
{
	int result = OK;
	#if debug
		printf("\x1B[1;40m================================================\nAhoj! Ja jsem \x1B[1;31m MAX \x1B[37m a jsem spustena v debug modu \n================================================\n\x1B[0;40m");
		printf("\nBylo zadano: %d parametru\n\n",argc-1);
	#endif
	/*int result; */

	if(argc!=2)
	{
		fprintf(stderr,"Spatne parametry\n");
		return 1;
	}
	if((pSource_File=fopen(argv[1], "r"))==NULL)
	{
		fprintf(stderr,"Soubor se nepodarilo otevrit.\n");
		return 1;
	}

	garbage_init();
	
	
	#if debug 
		printf("Volam syntakci analyzator: \n\n");		
	#endif
	result = parser();

	
	garbage_empty();
	fclose(stdout);
	fclose(pSource_File);
	return result;
	
}

