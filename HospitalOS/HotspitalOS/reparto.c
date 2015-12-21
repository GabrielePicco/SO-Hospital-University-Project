/*
 * reparto.c
 *
 *  Created on: 21 dic 2015
 *      Author: davide
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#define N 50
#define M 5
typedef enum {
	false = 0, true = 1
} boolean;

/* STRUTTURA DELLA CODA DI MESSAGGI */
struct my_msg {
	long mytype; //type message
	char mtext[N]; // corpo del messaggio
};

char* split_str(char* str, int* priorita);

int main(int argc, char** argv) {
	char* esempio = "Febbre Alta;123456789;6";
	int priorita;
	printf("esempio prima dello split e': %s\n", esempio);
	esempio = split_str(esempio, &priorita);
	printf("La priorita' e': %d\n", priorita);
	printf("esempio e': %s\n", esempio);
	struct my_msg msg;
	key_t key;
	key = ftok("reparto.c", '#');
	if (key == -1) {
		perror("Reparto: errore creazione chiave\n");
		exit(EXIT_FAILURE);
	}
	int msgid = msgget(key, IPC_CREAT | 0666); // creo la coda di messaggi
	if (msgid == -1) {
		perror("Reparto: errore creazione coda\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		msg.mytype = (10 - priorita);
		//strcpy(msg.mtext,esempio); // sostituire esempio con dati da FIFO
		if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) { // va in errore se mtext > maxmsgsz
			perror("Reparto: errore scrittura messaggio\n");
			exit(EXIT_FAILURE);
		}
	}

	printf("Messaggio scritto: ");

return 0;
}

/* riceve in input come parametri la stringa in formato: SINTOMO;PID;PRIORITA  e un puntatore ad intero
 * in cui verra' salvato il valore della priorita' e restituisce un puntatore a carattere
 * La funzione ha l'obiettivo di separare il sintomo;pid con la priorita' restituendo
 * SINTOMO;PID come risultato e salvando la PRIORITA nel puntatore ad interi
 */
char* split_str(char* str, int* priorita) {
int i;
char* sintomo;
char* prio;
sintomo = (char*) malloc(sizeof(char) * N);
prio = (char*) malloc(sizeof(char) * M);
for (i = 0; str[i] != ';'; i++) {
	sintomo[i] = str[i];
}
sintomo[i] = str[i]; // inserisce il primo punto e virgola nella coppia sintomo , pid
for (i = i + 1; str[i] != ';'; i++) {
	sintomo[i] = str[i];
}
//sintomo[i] = str[i]; // inserisce il secondo punto e virgola nella coppia sintomo , pid (DA SCEGLIERE IMPLEMENTAZIONE)
sintomo[i] = '\0';
prio[0] = str[i + 1];
if (str[i + 2] != '\0') {
	prio[1] = str[i + 2];
}

*priorita = atoi(prio);
return sintomo;

}
