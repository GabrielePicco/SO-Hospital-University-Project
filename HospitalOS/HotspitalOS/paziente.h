#include "headerMsg.h"
#include "headerSem.h"

/* VARIABILI GLOBALI */
boolean esc = false;

/* PROTOTIPI */
char* lettura_sintomo(FILE*file_sintomi, int sin);
int conta_righe(FILE* file_sintomi);
int getIdSemaforo();
void prenotaSemaforo(int semid);
void rilasciaSemaforo(int semid);
void uscita();
void terminazione();