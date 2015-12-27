#ifndef MY_HEADER_H_
#define MY_HEADER_H_
#include "my_header.h"
#endif /* MY_HEADER_H_ */

#include "headerSem.h"

/* VARIABILI GLOBALI */
boolean generaProcessi = true;
boolean esc = false;

/* PROTOTIPI */
int creaSemPazienti();
int avviaTriage();
int getNumMaxPazienti();
void rimuoviSemaforo(int semid);
int initSemTriagePaziente();
int getIdSemTriagePaziente();
int sincronizzaTriage(int semid);
int getTimerChiusura();
void stopGeneration();
void terminazione();
