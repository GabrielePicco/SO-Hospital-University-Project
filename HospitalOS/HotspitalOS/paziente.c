#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include "headerSem.h"

#define N 50

typedef enum {
	false = 0, true = 1
} boolean;

char* lettura_sintomo(FILE*file_sintomi, int sin);
int conta_righe(FILE* file_sintomi);
int getIdSemaforo();
void prenotaSemaforo(int semid);
void rilasciaSemaforo(int semid);
void uscita();
void terminazione();

boolean esc = false;

/* STRUTTURA DELLA CODA DI MESSAGGI */
struct my_msg {
	long mytype; //type message
	char mtext[N]; // corpo del messaggio
};

/**
 * Paziente
 */
int main(int argc, char** argv) {
	struct my_msg msg;
	int semid;

	signal(SIGQUIT, terminazione);
	signal(SIGUSR1, uscita);  // gestisce SIGUSR1 liberando il semaforo e deallocandosi

	FILE*file_sintomi; 	//Apertura file sintomi
	file_sintomi = fopen("Sintomi.txt", "r");
	if (file_sintomi == NULL) {
		perror("Errore in apertura del file\n");
		exit(1);
	}
	int righe_file = 0;
	righe_file = conta_righe(file_sintomi);
	rewind(file_sintomi);
	srand(time(NULL)); //funzione di randomizzazione
	int sin = rand() % righe_file; //Generazione di un numero random per la scelta del sintomo

	//Accesso al semaforo
	semid = getIdSemaforo();
	prenotaSemaforo(semid);

	char* sintomo;
	sintomo = lettura_sintomo(file_sintomi, sin);

	printf("\n<-- Paziente (%d): sintomo %d, corrispondente a %s -->\n",getpid(), sin, sintomo);

	/*int msgid = msgget(IPC_PRIVATE, IPC_CREAT | 0666); // creo la coda di messaggi
	if (msgid == -1) {
		perror("Paziente: errore creazione coda\n");
		exit(EXIT_FAILURE);
	}*/
	key_t key = ftok("triage.c", '(');
	if (key == -1) {
		perror("Paziente: errore chiave da triage\n");
		exit(EXIT_FAILURE);
	}
	int triage_msgid = msgget(key, 0666);
	if (triage_msgid == -1) {
		perror("Paziente: errore collegamento coda triage\n");
		exit(EXIT_FAILURE);
	}
	msg.mytype = 1;
	sprintf(msg.mtext, "%s;%d", sintomo,getpid()); // scrive sulla mtext il sintomo e la chiave della coda del paziente
	if (msgsnd(triage_msgid, &msg, sizeof(msg), 0) == -1) { // scrive sull coda di messaggi
		perror("Paziente: errore scrittura nella coda triage\n");
		exit(EXIT_FAILURE);
	}

	while(esc == false); // Paziente rimane nell'ospedale finche non viene curato

	free(sintomo);

	return EXIT_SUCCESS;
}

void terminazione(){
	esc = true;
}

void uscita(){
	if(esc == false){
		printf("\n\n     |||  Uscita paziente %d  |||  \n\n",getpid());
		rilasciaSemaforo(getIdSemaforo());
		esc = true;
	}
}

/* lettura_sintomo prende come parametri: un puntatore a FILE che e' indirizzato ad un file aperto
 * e un intero che contiene il numero della riga in cui si trova il sintomo da restituire in output
 */
char* lettura_sintomo(FILE*file_sintomi, int sin) {
	char* sintomo;
	sintomo = (char*) malloc(sizeof(char));
	int i = 0;
	char c;
	while (i < sin) {
		fgets(sintomo, 200, file_sintomi);
		i++;
	}
	for (i = 0; (c = fgetc(file_sintomi)) != ';'; i++) {
		sintomo[i] = c;
	}
	sintomo[i] = '\0';
	fclose(file_sintomi);
	return sintomo;

}

int conta_righe(FILE* file_sintomi) {
	int i = 0;
	char c;
	while ((c = fgetc(file_sintomi)) != EOF) {
		if (c == '\n')
			i++;
	}
	return i;
}

//Restituisce l'id del semaforo di ingresso creato da processGenerator.c
int getIdSemaforo(){
	key_t key;
	int semid;
	//Genero la chiave per il semaforo
	if((key = ftok("processGenerator.c",']')) == -1){
		perror("Paziente: Errore nella creazione delle chiave del semaforo pazienti con ftok");
		exit(EXIT_FAILURE);
	}

	//Ottengo semaforo
	if((semid = semget(key,1,0666)) == -1){
		if(esc == false){
			perror("Paziente: Errore nel trovare semaforo pazienti");
			exit(EXIT_FAILURE);
		}
	}
	return semid;
}



//prenota il semaforo di ingresso
void prenotaSemaforo(int semid){
	if(esc == false){
		struct sembuf sop;
		sop.sem_num = 0; // specifica il primo semaforo nel set
		sop.sem_op = -1; // decrementa di 1 il semaforo
		sop.sem_flg = 0;
		if(semop(semid,&sop,1) == -1){
			perror("Paziente: Errore sulla prenotazione del semaforo di ingresso");
			exit(EXIT_FAILURE);
		}

		printf("\nS: <-- Valore Semaforo pazienti (prenotazione): %d -->\n",semctl(semid,0,GETVAL,NULL));
	}
}

//rilascia il semaforo di ingresso
void rilasciaSemaforo(int semid){
	if(esc == false){
		struct sembuf sop;
		sop.sem_num = 0; // specifica il primo semaforo nel set
		sop.sem_op = 1; // incrementa di 1 il semaforo
		sop.sem_flg = 0;
		if(semop(semid,&sop,1) == -1){
			if(esc == false){
				perror("Paziente: Errore sul rilascio del semaforo di ingresso");
				exit(EXIT_FAILURE);
			}
		}

		printf("\nS: <-- Valore Semaforo pazienti (rilascio): %d -->\n",semctl(semid,0,GETVAL,NULL));
	}
}
