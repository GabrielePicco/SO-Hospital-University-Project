/**
 * Gestisce il paziente tramite coda di messaggi con priorita' per x secondi e poi con coda di
 * messaggi FIFO per y secondi. Eroga prestazione del paziente
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <errno.h>
#include <time.h>

#define N 50

#define FIFOTIME 2
#define PRIORTIME 2

typedef enum {
	false = 0, true = 1
} boolean;

/* STRUCT PER CODA DI MESSAGGI */
struct my_msg {
	long mtype;    // tipo di messaggio
	char mtext[N];  // corpo messaggio
};

/* EXTERN VARIABLE */
boolean fifo = false;   // variabile per coda di messaggi
int sec = PRIORTIME;            // tempo per il prossimo SIGALRM

/* PROTOTIPI */
void setQuequeType();
char* getSintomo(char* str);
char* getPazientePid(char* str);
void terminazione();

boolean esc = false;

/* FUNCTION */
int main(int argc, char** argv) {

	struct my_msg msg;
	char* sintomo;
	char* pidPaz;

	signal(SIGQUIT, terminazione);

	boolean msgRicevuto = false;

	signal(SIGALRM, setQuequeType);  // gestisce SIGALRM chiamando setQuequeType'
	alarm(sec);   // dopo sec secondi manda segnale SIGALRM

	int numDottore = atoi(argv[1]);

	key_t key = ftok("reparto.c", numDottore);   // creo chiave per coda di reparto
	if (key == -1) {
		perror("Dottore: errore creazione chiave\n");
		exit(EXIT_FAILURE);
	}
	//printf("Dottore: la chiave per coda messaggi di reparto e' %d\n", key);

	//printf("Dottore: accedo alla coda di messaggi di reparto\n");
	int msgid = msgget(key, 0666);   // accedo coda di messaggi reparto
	if (msgid == -1) {
		perror("Dottore: errore creazione coda\n");
		exit(EXIT_FAILURE);
	}

	/* Ciclo infinito per leggere su coda di priorita' o FIFO */
	while (esc == false) {
		if (fifo == false) {   // leggo su coda priorita'
			msgRicevuto = true;
			if (msgrcv(msgid, &msg, sizeof(msg), -10, 0) == -1) { // va in errore se mtext > maxmsgsz
				if(errno != EINTR){ // se e' in attesa di messaggi e il segnale SIGALARM la interrompe non va in errore
					perror("Dottore: errore ricezione messaggio\n");
					exit(EXIT_FAILURE);
				}else{
					msgRicevuto = false;
				}
			}
			if(msgRicevuto)printf("\n<-- Dottore %d : ricevuto paziente in coda priorita' %s | priorita': %lu -->\n",numDottore, msg.mtext,(11-msg.mtype));
		} else {   // leggo su coda FIFO
			msgRicevuto = true;
			if (msgrcv(msgid, &msg, sizeof(msg), 0, 0) == -1) { // va in errore se mtext > maxmsgsz
				if(errno != EINTR){  // se e' in attesa di messaggi e il segnale SIGALARM la interrompe non va in errore
					perror("Dottore: errore ricezione messaggio\n");
					exit(EXIT_FAILURE);
				}else{
					msgRicevuto = false;
				}
			}
			if(msgRicevuto)printf("\n<-- Dottore %d : ricevuto paziente in coda FIFO %s | priorita': %lu -->\n",numDottore, msg.mtext,(11-msg.mtype));
		}
		if(msgRicevuto){
			sintomo = getSintomo(msg.mtext);
			pidPaz = getPazientePid(msg.mtext);

			srand(time(NULL)); //funzione di randomizzazione
			sleep((rand() % 4)+2);   // attendo tempo casuale erogazione prestazione tra 2 e 5 secondi


			printf("\n<-- Dottore %d: paziente con PID %s e sintomo %s curato -->\n",numDottore, pidPaz,sintomo);
			int pidPazNum = atoi(pidPaz);
			if(kill(pidPazNum,SIGUSR1) == -1){
				perror("Dottore: errore invio segnale SIGUSR1 a paziente\n");
				exit(EXIT_FAILURE);
			}
			free(sintomo);
			free(pidPaz);
		}
	}

	return EXIT_SUCCESS;
}

/**
 *  Funzione per gestire SIGALRM. Setta la variabile fifo in modo da gestire
 *  il paziente o con coda di messaggi per priorita' (fifo = false) o con FIFO (fifo = true).
 */
void setQuequeType() {
	if (fifo == false) {
		fifo = true;
		sec = FIFOTIME;
	} else {
		fifo = false;
		sec = PRIORTIME;
	}
	/*if(fifo){
		printf("\n<-- Dottore: segnale SIGALRM ricevuto, Coda in modalità fifo -->\n");
	}else{
		printf("\n<-- Dottore: segnale SIGALRM ricevuto, Coda in modalità priorita' -->\n");
	}*/

	alarm(sec);
}

char* getSintomo(char* str) {
	char* sintomo;
	sintomo = (char*) malloc(sizeof(char) * N);
	int i;
	for (i = 0; str[i] != ';'; i++) {
		sintomo[i] = str[i];
	}
	sintomo[i] = '\0';

	return sintomo;
}

char* getPazientePid(char* str) {
	int i;
	char* pid;
	pid = (char*) malloc(sizeof(char) * N);
	for (i = 0; str[i] != ';'; i++)
		;

	int j;
	i++;
	if (str[i] == ' ') {
		i++;
	}
	for (j = 0; str[i] != '\0'; i++, j++) {
		pid[j] = str[i];
	}
	pid[j] = '\0';

	return pid;
}

void terminazione(){
	esc = true;
}
