//Implementace interpretu jazyka IFJ13.
//xbucht18, Buchta David

#ifndef BUILT_IN_H
#define BUIILT_IN_H

/***********************************************************************
Pomocna knihovna pro vestavene funkce, ucelem teto knihovny je zajistit,
aby prekladac vedel jak volat vestavene funkce.

Tato knihovna nedefinuje zadne vestavene funkce, pouze funkce registruje.
Jelikoz vestavene funkce nejsou zapsany v jazyce ifj13, a ani je nijak 
programotor nevklada do kodu napsanych v tomto je jazyce, je ze 
syntaktickych a semantickych duvodu nutne, ulozit do tabulku funkci
nazvy techto funkci a vytvorim jim vzorove tabulky symbolu.
***********************************************************************/

#include "ial.h"

int registerBuiltIn(T_ST_Funcs *functionTable);


#endif