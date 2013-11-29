////Implementace interpretu jazyka IFJ13.
//xbucht18, Buchta David

// interpret se nezavola nejakym magickym ritualem o pulnoci kdy musi byt uplnek a je treba obetovat 12 panen... Je to mnohem snazsi staci udelat jeden obycejny hlavickovy soubor.
//Hlavickovy soubor co to je? To je vec ktera slouzi jako rozhrani, zde se pisou strukturzy a nazvy funkci s parametry, aby ten co chce pouzit funkce interpretu si je mohl pouzit.
// A to nejen aby vedel  jak je vubec zavolat, ale aby i prekladac vedel , kde je najde a jak vypadaji.. 

// text napsany nastvanym vedoucim jak musel zpetne tento hlavickovy soubor vytvarer.

#ifndef INTERPRET_H
#define INTERPRET_H

typedef struct TCallStack{
  int adress;         //sem se vraci po vykonani funkce
  T_ST_Vars *symbolTable;
}TCallStack;

typedef struct Tparam{
  int paramCount;
  char *funcName;
  char *returnvalue;  
  bool BIf;
  int BIfPointer;
  T_ST_Vars *symbolTable;
}Tparam;

int interpret();

#endif 
// INTERPRET_H