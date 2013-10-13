//Implementace interpretu imperativního jazyka IFJ13.
//xbucht18, Buchta David

//binarni strom
// 1,2,4,3 razeni
#include "stdlib.h"
#include <stdio.h>
#include "garbage_collector.h"
#include "types.h"
#include "parser.h"
#define debug 1

FILE *pSource_File;
TGarbageList trash;
T_Token prevToken;
int row = 0;

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

	data = malloc(sizeof (int));
	(*(int*)data) = 20;
	
	//garbage_add(data,&garbage_default_erase);
	garbage_add(data,&test);

	#if debug 
		printf("Volam syntakci analyzator: \n");		
	#endif
	parser();

	
	

	garbage_empty();
	fclose(stdout);
	fclose(pSource_File);
	return 0;
	
}

bool test(void * data){
	printf("Mazu cislo %d\n",*(int*)data);
	free(data);

	return true;
}