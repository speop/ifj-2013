
#include "garbage_collector.h"
#include "stdio.h"
#include "stdlib.h"

#define debug 1
#define POCET_POKUSU 3 // kolikrat ma zkouset garbage collector odstranit prvek nez vyhlasi chybu

extern TGarbageList trash; //z main.c

bool garbage_init()
{
	#if debug 
		printf("Inicializuji garbage collector\n");
	#endif

	trash.act = NULL;
	trash.first = NULL;
	trash.last = NULL;
	return true;
}

bool garbage_add(void *data, bool (*function)(void*)) // pridam prvek do garbage collectoru
{
	#if debug 
		printf("Pidavam prvek do garbage collectoru\n");
	#endif

	TGarbage *newtrash;
	newtrash = malloc(sizeof(struct TGarbage));
	if (newtrash==NULL){
		#if debug 
			printf("Nepodarilo se alokovat novy prvek\n");
		#endif
		return false;
	}

	newtrash->data = data;
	newtrash->function = function;

	if(trash.act == NULL){ //seznam je prazdny

		#if debug 
			printf("Pidavam do prazdneho seznamu\n");
		#endif

		newtrash->primaryKey = 1;
		newtrash->prev = NULL;
		newtrash->next = NULL;

		trash.act = newtrash;
		trash.first = newtrash;
		trash.last = newtrash;

	}

	else{ //pridavame jiz do existujiciho seznamu
		
		#if debug 
			printf("Pidavam na posledni misto v seznamu\n");
		#endif

		newtrash->primaryKey = trash.last->primaryKey + 1;
		newtrash->prev =trash.last;
		newtrash->next = NULL;
		
		trash.last->next =  newtrash;
		trash.last = newtrash;

	}

	return true;

}

bool garbage_empty() // vyprazdni garbage_collector
{
	#if debug 
		printf("Vysypavam kos\n");
	#endif
	int i =0;
	bool ret;

	trash.act = trash.last;

	while (trash.act)
	{
		trash.act = trash.last->prev;

		#if debug 
			printf("Mazu prvek cislo: %d \n",trash.last->primaryKey);
		#endif

		do
		{
			ret = (*trash.last->function)(trash.last->data); //uvolnim pamet
			i++;

		} while (!ret && i < POCET_POKUSU);

		if (i>= POCET_POKUSU){
			#if debug 
				printf("Prekrocen pocet pokusu\n");
			#endif
			return false;
		}

		free(trash.last);
		trash.last = trash.act;
		i=0;
	}

	#if debug 
		printf("Kos je prazdny\n");
	#endif

	return true;
}