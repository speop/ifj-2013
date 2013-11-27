//Implementace interpretu jazyka IFJ13.
//xbucht18, Buchta David
#ifndef GARBAGE_COLLECTOR
#define GARBAGE_COLLECTOR

#include <stdbool.h>

typedef struct  TGarbage {	int primaryKey; // cisluje se od jednicky
					void *data; // obalka dat ktere byly dynamicky alokovany
					bool (*function)(void*); //ukazatel na funkci ktera dealokuje data
					struct TGarbage* next;
					struct TGarbage* prev;} TGarbage;

typedef struct {TGarbage* act;
				TGarbage* first;
				TGarbage* last;} TGarbageList;


typedef enum{ 	FREE_DATA = 0,
				REMOVE_DATA = 1} EGarbage;


bool garbage_init();
bool garbage_add(void *data, bool (*function)(void*));
bool garbage_empty(); // dealokuje vsechnz prvky v garbage collectoru
bool garbage_default_erase(void *data);
bool garbage_delete_by_pointer(void *, EGarbage);
bool garbage_delete_by_id(int, EGarbage);
#endif
