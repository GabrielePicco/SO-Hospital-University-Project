/**
 * Paziente
 */

#include "paziente.h"

int main(int argc, char** argv) {
	struct my_msg msg;
	int semid;

	signal(SIGQUIT, terminazione);
	signal(SIGUSR1, uscita); // gestisce SIGUSR1 liberando il semaforo e deallocandosi. SIGUSR1 arriva
							 // da dottore una volta che il paziente � curato

	FILE* file_sintomi; 	//Apertura file sintomi
	file_sintomi = fopen("Sintomi.txt", "r");
	if (file_sintomi == NULL) {
		perror("Paziente: Errore nell'apertura del file\n");
		exit(1);
	}

	int righe_file = conta_righe(file_sintomi);

	rewind(file_sintomi);

	srand(time(NULL)); //funzione di randomizzazione
	int sin = rand() % righe_file; //Generazione di un numero random per la scelta del sintomo

	//Accesso al semaforo
	semid = getIdSemaforo();
	prenotaSemaforo(semid);

	char* sintomo = lettura_sintomo(file_sintomi, sin);
	printf("\n<---- Sono il paziente appena entrato nell'ospedale, di PID (%d), con il sintomo \"%s\" ---->\n", getpid(),
			sintomo);

	// Recupero chiave coda msg triage
	key_t key = ftok("triage.c", '(');
	if (key == -1) {
		perror("Paziente: errore chiave da triage\n");
		exit(EXIT_FAILURE);
	}
	// Mi collego alla coda msg di triage
	int triage_msgid = msgget(key, 0666);
	if (triage_msgid == -1) {
		perror("Paziente: errore collegamento coda triage\n");
		exit(EXIT_FAILURE);
	}
	msg.mtype = 1;
	// scrive sulla mtext il sintomo e la chiave della coda del paziente
	sprintf(msg.mtext, "%s;%d", sintomo, getpid());
	if (msgsnd(triage_msgid, &msg, sizeof(msg), 0) == -1) { // scrive sull coda di messaggi
		perror("Paziente: errore scrittura nella coda triage\n");
		exit(EXIT_FAILURE);
	}
	free(sintomo);

	while (esc == false); // Paziente rimane nell'ospedale finche non viene curato

	return EXIT_SUCCESS;
}

void terminazione() {
	esc = true;
}

void uscita() {
	if (esc == false) {
		printf("\n\n\t\t |||  Uscita paziente %d ||| \n\n", getpid());
		rilasciaSemaforo(getIdSemaforo());
		esc = true;
	}
}

/**
 * Lettura_sintomo prende come parametri: un puntatore a FILE che e' indirizzato ad un file aperto
 * e un intero che contiene il numero della riga in cui si trova il sintomo da restituire in output
 */
char* lettura_sintomo(FILE* file_sintomi, int sin) {
	char* sintomo = (char*) malloc(sizeof(char));
	int i = 0;
	char c;
	while (i < sin) {  // cerco il sintomo
		fgets(sintomo, 200, file_sintomi);
		i++;
	}
	for (i = 0; (c = fgetc(file_sintomi)) != ';'; i++) { // una volta trovato lo inserisco in sintomo
		sintomo[i] = c;
	}
	sintomo[i] = '\0';

	fclose(file_sintomi);

	return sintomo;

}

int conta_righe(FILE* file_sintomi) {
	int i;
	char s[SIZE];

	for (i = 0; fgets(s, 200, file_sintomi) != NULL; i++);

	return i;
}

//Restituisce l'id del semaforo di ingresso creato da processGenerator.c
int getIdSemaforo() {
	key_t key;
	int semid;
	//Genero la chiave per il semaforo
	if ((key = ftok("processGenerator.c", ']')) == -1) {
		perror("Paziente: Errore nella creazione delle chiave del semaforo pazienti con ftok");
		exit(EXIT_FAILURE);
	}

	//Ottengo semaforo
	if ((semid = semget(key, 1, 0666)) == -1) {
		if (esc == false) {
			perror("Paziente: Errore nel trovare semaforo pazienti");
			exit(EXIT_FAILURE);
		}
	}
	return semid;
}

//prenota il semaforo di ingresso
void prenotaSemaforo(int semid) {
	if (esc == false) {
		struct sembuf sop;
		sop.sem_num = 0; // specifica il primo semaforo nel set
		sop.sem_op = -1; // decrementa di 1 il semaforo
		sop.sem_flg = 0;
		if (semop(semid, &sop, 1) == -1) {
			perror("Paziente: Errore sulla prenotazione del semaforo di ingresso");
			exit(EXIT_FAILURE);
		}
		printf("\n ----- Ingresso di un paziente nell'ospedale. \tI posti rimanenti sono: %d -----\n",
				semctl(semid, 0, GETVAL, NULL));
	}
}

//rilascia il semaforo di ingresso
void rilasciaSemaforo(int semid) {
	if (esc == false) {
		struct sembuf sop;
		sop.sem_num = 0; // specifica il primo semaforo nel set
		sop.sem_op = 1; // incrementa di 1 il semaforo
		sop.sem_flg = 0;
		if (semop(semid, &sop, 1) == -1) {
			if (esc == false) {
				perror("Paziente: Errore sul rilascio del semaforo di ingresso");
				exit(EXIT_FAILURE);
			}
		}
		printf("\n***** Un paziente e' stato curato e lascia l'ospedale. \tI posti liberi sono: %d *****\n",
				semctl(semid, 0, GETVAL, NULL));
	}
}

