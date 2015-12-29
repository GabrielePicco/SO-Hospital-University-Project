#include "headerMsg.h"

#define FIFOTIME 2
#define PRIORTIME 4

/* VARIABILI GLOBALI */
boolean fifo = false;    // variabile per coda di messaggi
int sec = PRIORTIME;     // tempo per il prossimo SIGALRM
boolean esc = false;

/* PROTOTIPI */
void setQuequeType();
char* getSintomo(char* str);
char* getPazientePid(char* str);
void terminazione();
