/*
 * reparto.c
 *
 *  Created on: 21 dic 2015
 *      Author: davide
 */

#include "reparto.h"

int main(int argc, char** argv) {
	struct my_msg msg;
	
	signal(SIGQUIT, terminazione);

	int numReparto = atoi(argv[1]);
	int keyFIFO = getFIFOReparto(numReparto);
	//printf("\n\n<-- Inizio reparto %d\n",numReparto);
	char* BUFFER = (char*)malloc(sizeof(char)*N);
	//char esempio[N];
	//char* esempio = "Febbre Alta;123456789;6";
	int priorita;
	key_t key;
	key = ftok("reparto.c",numReparto);
	if (key == -1) {
		perror("Reparto: errore creazione chiave\n");
		exit(EXIT_FAILURE);
	}
	int msgid = msgget(key, IPC_CREAT | 0666); // creo la coda di messaggi
	if (msgid == -1) {
		perror("Reparto: errore creazione coda\n");
		exit(EXIT_FAILURE);
	}

	avviaDottore(numReparto);

	while (esc == false) {
		if(read(keyFIFO, BUFFER, N) > 0){
			//printf("\n<--Reparto: %d Stringa: %s--->\n",numReparto,esempio);
			//printf("Reparto: esempio prima dello split e': %s\n", esempio);
			BUFFER = split_str(BUFFER, &priorita);
			//printf("La priorita' e': %d\n", priorita);
			//printf("esempio e': %s\n", esempio);


			msg.mtype = (11 - priorita);
			strcpy(msg.mtext,BUFFER); // sostituire esempio con dati da FIFO
			if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) { // va in errore se mtext > maxmsgsz
				perror("Reparto: errore scrittura messaggio");
				exit(EXIT_FAILURE);
			}
			printf("\n\n<--- Reparto %d: Messaggio scritto: %s--->\n\n",numReparto,BUFFER);
			free(BUFFER);
		}
		sleep(CONTROL_TIME); // Controlla la fifo ogni due secondi
	}

	//elimina coda messaggi
	if (msgctl(msgid, IPC_RMID, NULL) == -1) {
		fprintf(stderr, "Coda messaggi non può essere elminata.\n");
		exit(EXIT_FAILURE);
	}

	//printf("\n## Reparto %d : coda eliminata ##\n",numReparto);

	return EXIT_SUCCESS;
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
	free(prio);
	return sintomo;

}

int getFIFOReparto(int num){
	int key;
	char name[N];
	strcpy(name,"fifoReparto");
	char* snum = intToString(num);
	strcat(name,snum);
	mkfifo(name,0666);
	if((key = open(name,O_RDONLY)) == -1){
		perror("Reparto: errore apertura FIFO\n");
		exit(EXIT_FAILURE);
	}
	free(snum);
	return key;
}

char* intToString(int num){
	char* snum;
	snum = (char*)malloc(sizeof(char)*3);
	sprintf(snum, "%d",num);
	return snum;
}

void avviaDottore(int num){
	pid_t pid;
	pid = fork();
	if(pid == 0){ /* dopo la fork controllo se sono il figlio e avvio il processo dottore*/
		char *argv[] = {"./dottore" ,intToString(num), NULL }; // Il primo elemento (argv[0]) deve contenere il nome del programma da invocare, il secondo contiene il numero di reparto.
		execv("dottore",argv);
		perror("\n<--- errore nella exve del dottore --->\n"); // la execve non ritorna, se lo fa è un errore
		exit(EXIT_FAILURE);
	}
}

void terminazione(){
	esc = true;
}
