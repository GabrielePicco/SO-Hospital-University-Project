#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>

#define N 50
char* lettura_sintomo(FILE*file_sintomi, int sin);
void conta_righe(FILE* file_sintomi, int* conta_righe);

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
	FILE*file_sintomi; 	//Apertura file sintomi
	file_sintomi = fopen("Sintomi.txt", "r");
	if (file_sintomi == NULL) {
		perror("Errore in apertura del file\n");
		exit(1);
	}
	int righe_file = 0;
	conta_righe(file_sintomi, &righe_file);
	rewind(file_sintomi);
	int sin = rand() % righe_file; //Generazione di un numero random per la scelta del sintomo
	printf("Sintomo scelto: %d\n", sin);
	char* sintomo;
	sintomo = lettura_sintomo(file_sintomi, sin);
	printf("Il sintomo %d corrisponde al sintomo %s\n", sin, sintomo);

	//Accesso al semaforo da fare

	int msgid = msgget(IPC_PRIVATE, IPC_CREAT | 0666); // creo la coda di messaggi
	if (msgid == -1) {
		perror("Paziente: errore creazione coda\n");
		exit(EXIT_FAILURE);
	}
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
	sprintf(msg.mtext, "%s; %d", sintomo, msgid); // scrive sulla mtext il sintomo e la chiave della coda del paziente
	if (msgsnd(triage_msgid, &msg, sizeof(msg), 0) == -1) { // scrive sull coda di messaggi
		perror("Paziente: errore scrittura nella coda triage\n");
		exit(EXIT_FAILURE);
	}

	//riliascia semaforo

	return 0;
}

/*lettura_sintomo prende come parametri: un puntatore a FILE che � indirizzato ad un file aperto
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
	rewind(file_sintomi);
	return sintomo;

}

void conta_righe(FILE* file_sintomi, int* size) {
	int i = 1;
	char c;
	while ((c = fgetc(file_sintomi)) != EOF) {
		if (c == '\n')
			i++;
	}
	*size = i;
}
