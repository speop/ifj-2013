//Implementace interpretu imperativního jazyka IFJ13.

#include "stdlib.h"
#include <stdio.h>
#include "garbage_collector.h"
#define debug 1

FILE *pSource_File;
TGarbageList trash;

bool test(void * data);


int main(int argc, char **argv)
{
	#if debug
		printf("\x1B[1;40m================================================\nAhoj! Ja jsem \x1B[1;31m MAX \x1B[37m a jsem spustena v debug modu \n================================================\n\x1B[0;40m");
		printf("\nBylo zadano: %d parametru\n\n",argc-1);
	#endif
	/*int result; */

	if(argc!=2)
	{
		printf("Spatne parametry\n");
		return 1;
	}
	if((pSource_File=fopen(argv[1], "r"))==NULL)
	{
		printf("Soubor se nepodarilo otevrit.\n");
		return 1;
	}

	garbage_init();
	void *data;

	garbage_add(data,&test);
	garbage_add(data,&test);
	garbage_empty();

	#if debug
		printf("Volam lexikalni analyzator:\n");

	#endif

	#if debug 
		printf("Volam syntakci analyzator: \n");
	#endif

	#if debug 
		printf("Volam interpret: \n");
	#endif
	

	
}

bool test(void * data){
	printf("hello world!\n");
	return true;
}