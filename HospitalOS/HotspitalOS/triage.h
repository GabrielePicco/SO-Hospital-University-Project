#include "headerSem.h"
#include "headerMsg.h"

/* VARIABILI GLOBALI */
boolean esc = false;

/* PROTOTIPI */
char* getSintomo(char* str);
char* getPazientePid(char* str);
char* getPriorita(char* sintomo,FILE* fp);
int getNumReparti();
void creaReparti(int num);
char* intToString(int num);
int* creaFIFOReparti(int numReparti);
int getIdSemTriagePaziente();
void releaseSemaforoTriagePazienti(int semid);
void terminazione();