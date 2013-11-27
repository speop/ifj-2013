//Implementace interpretu jazyka IFJ13.
//xbucht18, Buchta David
#include "garbage_collector.h"
#include "stdio.h"
#include "stdlib.h"

#define debug 0
#define POCET_POKUSU 3 // kolikrat ma zkouset garbage collector odstranit prvek nez vyhlasi chybu

extern TGarbageList trash; //z main.c

bool garbage_init()
{
	//inicializuji ponteru pro posovani v GC
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
		printf("Pridavam prvek do garbage collectoru\n");
	#endif

	TGarbage *newtrash;	// vytvorim novy prvek seznamu			
	newtrash = malloc(sizeof(struct TGarbage));
	if (newtrash==NULL){
		#if debug 
			printf("\tNepodarilo se alokovat novy prvek\n");
		#endif
		return false;
	}

	newtrash->data = data;			
	newtrash->function = function;

	if(trash.act == NULL){ //seznam je prazdny

		#if debug 
			printf("\tPridavam do prazdneho seznamu\n");
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
			printf("\tPridavam na posledni misto v seznamu\n");
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
		//nastavim jako aktivni predposledni prvek
		trash.act = trash.last->prev;

		#if debug 
			printf("\tMazu prvek cislo: %d \n",trash.last->primaryKey);
		#endif

		do
		{
			ret = (*trash.last->function)(trash.last->data); //uvolnim pamet
			i++;

		} while (!ret && i < POCET_POKUSU);

		if (i>= POCET_POKUSU){
			#if debug 
				printf("\t\tPrekrocen pocet pokusu\n");
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


bool garbage_default_erase(void *data)
{	
	//defaultni funkce pro uvolneni jednoduche pametove struktury
	

	return true;
}

// jestliye je dealloc parametr FREE_DATA, funkce vyhleda v seznamu pointer ktery ma stejnou hodnotu jako zadana data, tento prvek odstrani ze seznamu a uvolni pamet
// pokud ma parametr hodnotu REMOVE_DATA pouze odstrani prvek ze seznamu
bool garbage_delete_by_pointer(void *ptr, EGarbage dealloc)
{	
	TGarbageList pom;
	trash.act = trash.first;
	
	do{
		//nalezli jsme prvek k odstraneni
		if (trash.act == NULL) return false; // prazdny seznam
		if(((trash.act)->data) == ptr){ 
			
			//mame uvolnit pamet
			if(dealloc == FREE_DATA) (*trash.act->function)(trash.act->data);
			pom.act = trash.act;
			// seznam ma jediny prvek
			if(trash.first == trash.last){
				trash.first = NULL;
				trash.last = NULL;
				trash.act = NULL;


			}

			//mazeme prvni prvek
			else if(trash.act == trash.first){
					(trash.act)->next->prev = NULL;
					trash.act = (trash.act)->next;
					trash.first = trash.act;

			}

			//mazeme posledni prvek
			else if (trash.act==trash.last){
					(trash.act)->prev->next = NULL;
					trash.act = (trash.act)->prev;
					trash.last = trash.act;
			}


			//mazeme nekde uprostred seznamu
			else{
				(trash.act)->prev->next = (trash.act)->next;
				(trash.act)->next->prev = (trash.act)->prev;
				trash.act = (trash.act)->next;
			}

			//smazeme uzel
			free(pom.act);
			return true;
		}
		trash.act = (trash.act)->next;

	}while(trash.act != trash.last);

	//jelikoz neodzkousime posledni prvek
	if(((trash.act)->data)== ptr){
			
			//mame uvolnit pamet
			if(dealloc == FREE_DATA) (*trash.act->function)(trash.act->data);
			
			// nastavime poslednimu prvku ze jiz na nic neukazuje
			(trash.act)->prev->next= NULL;
			pom.act = trash.act;
			trash.act = (trash.act)->prev;
			//opravime konec fronty
			trash.last = trash.act;
			

			free(pom.act);
			return true;
	}

	
	return false;
}

// jestlize je dealloc parametr FREE_DATA, funkce vyhleda v seznamu pointer ktery ma stejnou hodnotu jako zadana data, tento prvek odstrani ze seznamu a uvolni pamet
// pokud ma parametr hodnotu REMOVE_DATA pouze odstrani prvek ze seznamu
bool garbage_delete_by_id(int primaryKey, EGarbage dealloc)
{	
	TGarbageList pom;
	trash.act = trash.first;
	
	do{
		//nalezli jsme prvek k odstraneni
		if (trash.act == NULL) return false; // prazdny seznam
		if(((trash.act)->primaryKey) == primaryKey){ 
			//mame uvolnit pamet
			if(dealloc == FREE_DATA) (*trash.act->function)(trash.act->data);
			pom.act = trash.act;
			// seznam ma jediny prvek
			if(trash.first == trash.last){
				trash.first = NULL;
				trash.last = NULL;
				trash.act = NULL;

			}

			//mazeme prvni prvek
			else if(trash.act == trash.first){
					(trash.act)->next->prev = NULL;
					trash.act = (trash.act)->next;
					trash.first = trash.act;

			}

			//mazeme posledni prvek
			else if (trash.act==trash.last){
					(trash.act)->prev->next = NULL;
					trash.act = (trash.act)->prev;
					trash.last = trash.act;
			}


			//mazeme nekde uprostred seznamu
			else{
				(trash.act)->prev->next = (trash.act)->next;
				(trash.act)->next->prev = (trash.act)->prev;
				trash.act = (trash.act)->next;
			}

			//smazeme uzel
			free(pom.act);
			return true;

		}
		trash.act = (trash.act)->next;

	}while(trash.act != trash.last);

	//jelikoz neodzkousime posledni prvek
	if(((trash.act)->primaryKey)== primaryKey){
			
			//mame uvolnit pamet
			if(dealloc == FREE_DATA) (*trash.act->function)(trash.act->data);
			
			// nastavime poslednimu prvku ze jiz na nic neukazuje
			(trash.act)->prev->next= NULL;
			pom.act = trash.act;
			trash.act = (trash.act)->prev;
			//opravime konec fronty
			trash.last = trash.act;
			

			free(pom.act);
			return true;
	}

	
	return false;
}