#ifndef GARBAGE_COLLECTOR
#define GARBAGE_COLLECTOR

#include <stdbool.h>

typedef struct  TGarbage {	int primaryKey;
					void *data; // obalka dat ktere byly dynamicky alokovany
					bool (*function)(void*); //ukazatel na funkci ktera dealokuje data
					struct TGarbage* next;
					struct TGarbage* prev;} TGarbage;

typedef struct {TGarbage* act;
				TGarbage* first;
				TGarbage* last;} TGarbageList;

bool garbage_init();
bool garbage_add(void *data, bool (*function)(void*));
bool garbage_empty(); // dealokuje vsechnz prvky v garbage collectoru
bool garbage_default_erase(void *data);
#endif
