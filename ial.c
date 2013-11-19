#define LEFT 1
#define RIGHT 0
#include "ial.h"
#define FREE_VAR_NAME 0
#define FREE_FUNC_NAME 1

char* mystrdup1(const char* s)
{
    char* p = malloc(strlen(s)+1);
    if(p==NULL) fprintf(stderr, "Cannot alocate memory\n");
    if (p) strcpy(p, s);
    return p;
}

//funkce inicalizuje vsechny pointery korene na NULL
void varSTInit (T_ST_Vars *pointer)
 {
 	pointer->data = NULL;
 	pointer->left = NULL;
 	pointer->right = NULL;
 	pointer->parrent = NULL;
 }

// funkce projde strom a vrati enum dle  toho jestli se promena jiz nachazi ve stromu, nebo ne, nebo doslo k chybe pir vkladani
ReturnCodesST addVarNodeToST(T_ST_VarsItem *newNode, T_ST_Vars *table)
{
	int comp;
	newNode->value = NULL;

	// prvni vlozeni do tabulky
	if(table->data ==  NULL){
		table->data=newNode;
		table->parrent = table;
		return ADDING_SUCCESSFUL;
	}

	comp = strcmp( newNode->name, table->data->name);

	 T_ST_Vars *pointer;

	

	// prvek se stejnym nazvem jiz v tabulce existuje
	if(!comp) return ITEM_EXIST;

	//prvek ma vetsi hodnotu nez rodic jdeme doprava
	else if (comp > 0) {

		// pravy uzel na nic neukazuje, vytvorime novy uzel
		if (table->right == NULL) {
 			
 			if(!(pointer = malloc(sizeof(T_ST_Vars)))) return INTERNAL_ERROR;
 			varSTInit(pointer);
 			pointer->data = newNode;
 			pointer->parrent = table;

 			table->right = pointer;
 			return ADDING_SUCCESSFUL;
		}
		//pravy uzel ukazuje na neco, funkce rekurzivne zavola sama sebe ale tenktokrat je jako tabulka pravy uzel
		else return addVarNodeToST(newNode, table->right);
	}

	else{

		if (table->left == NULL) {
 			
 			if (!(pointer = malloc(sizeof(T_ST_Vars)))) return INTERNAL_ERROR;
 			varSTInit(pointer);
 			pointer->data = newNode;
 			pointer->parrent = table;

 			table->left = pointer;
 			return ADDING_SUCCESSFUL;
		}

		else return addVarNodeToST(newNode, table->left);

	}

	return INTERNAL_ERROR;
}

//funkce projde binarni strom a vrati uzel obsahujici danou promenou nebo NULL v pripade ze promena se v tabulce nenachazi
T_ST_Vars* findVarST( char *var, T_ST_Vars *table)
{	
	
	int comp;
	//strom je prazdny
	if(table->data == NULL) return NULL;

	//aktualni koren obsahuje promenou s danym nazvem
	if(!(comp = strcmp(var,table->data->name))){  return table;}

	//promena se nachazi v prave casti stromu
	if(comp > 0){
		if (table->right == NULL) return NULL;
		return findVarST(var, table->right);
	}

	else{
		if(table->left==NULL) return NULL;
		return findVarST(var, table->left);
	}

	return NULL;
}

//odstrani danou promenou z ST vraci true v pripade uspechu
bool removeVarST (char* var, T_ST_Vars *table)
{
	
	T_ST_Vars *node = findVarST(var, table);
	T_ST_Vars *tempNode;
	
	bool right = false;

	if (node == NULL) return false;
	// zjistime si jestli odstranovanz uzel je levy, nebo pravy potomek
	if(!(strcmp (node->parrent->right->data->name, var))) right = true;

	 if (node->left == NULL) { 
        	if (right) node ->parrent->right = node->right;
        	else node->parrent->left = node->right;
        	free(node);
        	return true;

    } else if (node->right == NULL) {
       		if (right) node ->parrent->right = node->left;
        	else node->parrent->left = node->left;
        	free(node);
        	return true;
    
    } else {
        // uzel ma dva potomky, nahradimeho jeho pravym potomkem potom hledame, nejlevejsi uzel praveho potomka, kde mu pridame leveho potomka ruseneho uzlu
        
        if(right) node->parrent->right = node->right;
        else node->parrent->left = node->right;

        //pripojime levy podstrom k pravemu podstormu
        tempNode = node->right;

		//podstrom pripojime na nejlevejsi uzel        
        while (tempNode->left != NULL) tempNode = tempNode->left;
        tempNode->left = node->left;

        free(node);
        return true;
    }

    return false;
}


//stejne funkce stejny princip jako vyse akorat je to binarni strom na jinaci strukturz... je to rychlejsi a jednodusi ty fce zkopirovat nez se otravovat s void pointery


//funkce inicalizuje vsechny pointery korene na NULL
void functionSTInit (T_ST_Funcs *pointer)
 {
 	pointer->data = NULL;
 	pointer->left = NULL;
 	pointer->right = NULL;
 	pointer->parrent = NULL;
 }

// funkce projde strom a vrati enum dle  toho jestli se promena jiz nachazi ve stromu, nebo ne, nebo doslo k chybe pir vkladani
ReturnCodesST addFuncNodeToST(T_ST_FuncsItem *newNode, T_ST_Funcs *table)
{
	int comp;

	// prvni vlozeni do tabulky
	if(table->data ==  NULL){
		table->data=newNode;
		table->parrent = table;
		return ADDING_SUCCESSFUL;
	}


	comp = strcmp(newNode->name, table->data->name);
	 T_ST_Funcs *pointer;

	// prvek se stejnym nazvem jiz v tabulce existuje
	if(!comp) return ITEM_EXIST;

	//prvek ma vetsi hodnotu nez rodic jdeme doprava
	else if (comp > 0) {

		// pravy uzel na nic neukazuje, vytvorime novy uzel
		if (table->right == NULL) {
 			
 			if(!(pointer = malloc(sizeof(T_ST_Funcs)))) return INTERNAL_ERROR;
 			functionSTInit(pointer);
 			pointer->data = newNode;
 			pointer->parrent = table;

 			table->right = pointer;
 			return ADDING_SUCCESSFUL;
		}
		//pravy uzel ukazuje na neco, funkce rekurzivne zavola sama sebe ale tenktokrat je jako tabulka pravy uzel
		else return addFuncNodeToST(newNode, table->right);
	}

	else{

		if (table->left == NULL) {
 			
 			if (!(pointer = malloc(sizeof(T_ST_Funcs)))) return INTERNAL_ERROR;
 			functionSTInit(pointer);
 			pointer->data = newNode;
 			pointer->parrent = table;

 			table->left = pointer;
 			return ADDING_SUCCESSFUL;
		}

		else return addFuncNodeToST(newNode, table->left);

	}

	return INTERNAL_ERROR;
}

//funkce projde binarni strom a vrati uzel obsahujici danou promenou nebo NULL v pripade ze promena se v tabulce nenachazi
T_ST_Funcs* findFunctionST( char *funcName, T_ST_Funcs *table)
{
	int comp;
	//strom je prazdny
	if(table->data == NULL) return NULL;
	
	//aktualni koren obsahuje promenou s danym nazvem
	if(!(comp = strcmp(funcName,table->data->name)))  return table;

	//promena se nachazi v prave casti stromu
	if(comp > 0){
		if (table->right == NULL) return NULL;
		return findFunctionST(funcName, table->right);
	}

	else{
		if(table->left==NULL) return NULL;
		return findFunctionST(funcName, table->left);
	}

	return NULL;
}

//odstrani danou promenou z ST vraci true v pripade uspechu
bool removeFunctionST (char* funcName, T_ST_Funcs *table)
{
	T_ST_Funcs *node = findFunctionST(funcName, table);
	T_ST_Funcs *tempNode;

	bool right = false;

	// zjistime si jestli odstranovanz uzel je levy, nebo pravy potomek
	if(!(strcmp (node->parrent->right->data->name, funcName))) right = true;

	 if (node->left == NULL) { 
        	if (right) node ->parrent->right = node->right;
        	else node->parrent->left = node->right;
        	free(node);
        	return true;

    } else if (node->right == NULL) {
       		if (right) node ->parrent->right = node->left;
        	else node->parrent->left = node->left;
        	free(node);
        	return true;
    
    } else {
        // uzel ma dva potomky, nahradimeho jeho pravym potomkem potom hledame, nejlevejsi uzel praveho potomka, kde mu pridame leveho potomka ruseneho uzlu
        
        if(right) node->parrent->right = node->right;
        else node->parrent->left = node->right;

        
        //pripojime levy podstrom k pravemu podstormu
        tempNode = node->right;

		//podstrom pripojime na nejlevejsi uzel        
        while (tempNode->left != NULL) tempNode = tempNode->left;
        tempNode->left = node->left;

        free(node);
        return true;
    }

    return false;
}

//pomocna funkce ktera vypise tabulku symbolu podle abecedy
void printTree(T_ST_Vars *table)
{
	if (table->left != NULL) printTree(table->left);

	printf("%d. prvek: %s \n",table->data->type, table->data->name);
	if (table->right != NULL) printTree(table->right);
	//printf("kde to sakra padam?\n");
	return;
}


//funkce pro garbage collector, uvolni cele stromy
bool freeVarST(void *tree)
{	
	//strom je prazdny
	if(((T_ST_Vars*)tree)->data == NULL) return true;

	//existuje pravy podstrom
	if(((T_ST_Vars*)tree)->right != NULL) freeVarSTpom(((T_ST_Vars*)tree)->right, RIGHT);
	//existuje levy podstrom
	if (((T_ST_Vars*)tree)->left != NULL) freeVarSTpom(((T_ST_Vars*)tree)->left, LEFT);

	free(((T_ST_Vars*)tree)->data);
	free ((T_ST_Vars*)tree);
	return true;
}

//pomocna funkce pro odstranovani ve stromu.. aby kdby nahodou se returnulo nekde v polovine funkce aby byly pointery v poradku
void freeVarSTpom(T_ST_Vars* tree, int smer){

	//existuje pravy podstrom
	if(tree->right != NULL) freeVarSTpom(tree->right, RIGHT);
	//existuje levy podstrom
	if (tree->left != NULL) freeVarSTpom(tree->left, LEFT);

	if(smer== RIGHT) tree->parrent->right = NULL;
	else tree->parrent->left = NULL;

	#if FREE_VAR_NAME
		free(tree->data->name);
	#endif
		
	free(tree->data);
	free(tree);

	return;
}

bool freeFuncST(void *tree)
{	

	//strom je prazdny
	if(((T_ST_Funcs*)tree)->data == NULL) return true;

	//existuje pravy podstrom
	if(((T_ST_Funcs*)tree)->right != NULL) freeFuncSTpom(((T_ST_Funcs*)tree)->right, RIGHT);
	//existuje levy podstrom
	if (((T_ST_Funcs*)tree)->left != NULL) freeFuncSTpom(((T_ST_Funcs*)tree)->left, LEFT);

	//uvolnime tabulku symbolu dane funkce
	if(((T_ST_Funcs*)tree)->data->symbolTable != NULL) freeVarST(((T_ST_Funcs*)tree)->data->symbolTable);
	free(((T_ST_Funcs*)tree)->data);
	free ((T_ST_Funcs*)tree);
	return true;
}

//pomocna funkce pro odstranovani ve stromu.. aby kdby nahodou se returnulo nekde v polovine funkce aby byly pointery v poradku
void freeFuncSTpom(T_ST_Funcs* tree, int smer){

	//existuje pravy podstrom
	if(tree->right != NULL) freeFuncSTpom(tree->right, RIGHT);
	//existuje levy podstrom
	if (tree->left != NULL) freeFuncSTpom(tree->left, LEFT);

	if(smer== RIGHT) tree->parrent->right = NULL;
	else tree->parrent->left = NULL;

	//uvolnime tabulku symbolu dane funkce
	if(tree->data->symbolTable != NULL)freeVarST((void*)tree->data->symbolTable);
	free(tree->data);
	free(tree);
	return;
}

T_ST_Vars* copyTable(T_ST_Vars* hd)
{
	T_ST_Vars* sb;
		
    if (hd != NULL)
    {
		sb = (T_ST_Vars*)malloc(sizeof(T_ST_Vars));
		varSTInit(sb);
		if(copyTableFill(hd,sb) != OK) return NULL;
    }
	
	return sb;

}

int copyTableFill(T_ST_Vars* hd, T_ST_Vars* sb)
{
	T_ST_VarsItem* item;
	int ret;
	
    if (hd != NULL)
    {
		if (( item = (T_ST_VarsItem *)malloc (sizeof(T_ST_VarsItem)))  == NULL) return INTERNAL_ERROR;
		item->name = mystrdup1(hd->data->name);	
		ret = addVarNodeToST(item ,sb);
		if(ret != OK) return ret;
		
        copyTableFill(hd->left,sb);
        copyTableFill(hd->right,sb);
    }
	
	return OK;
}


     typedef struct {
        int length;
        char *first;
    }TString;


//spojí dvě seřazené pole  do jednoho, 
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



//seřazení pole znaků
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


//vyhledávání podřetězce
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
