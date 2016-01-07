/**
  * Gestisce il paziente tramite coda di messaggi con priorita' per x secondi e poi con coda di
  * messaggi FIFO per y secondi. Eroga prestazione del paziente
  */

#include "dottore.h"

int main(int argc, char** argv) {
	struct my_msg msg;
	char* sintomo;
	char* pidPaz;
	int tmpTime;

	signal(SIGQUIT, terminazione);

	boolean msgRicevuto = false;

	signal(SIGALRM, setQuequeType);  // gestisce SIGALRM chiamando setQuequeType'
	alarm(sec);   // dopo sec secondi manda segnale SIGALRM

	int numDottore = atoi(argv[1]);

	key_t key = ftok("reparto.c", numDottore);   // recupero chiave per coda di reparto
	if (key == -1) {
		perror("Dottore: errore creazione chiave\n");
		exit(EXIT_FAILURE);
	}


	int msgid = msgget(key, 0666);   // accedo coda di messaggi reparto
	if (msgid == -1) {
		perror("Dottore: errore creazione coda\n");
		exit(EXIT_FAILURE);
	}

	/* Ciclo infinito per leggere su coda di priorita' o FIFO */
	while (esc == false) {   // si blocca con SIGQUIT
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
			if(msgRicevuto){
			  printf("\n<- Dottore %d : ricevuto paziente in coda ** PRIORITA' ** con priorità: * %lu *\n",numDottore,(11-msg.mtype));
			}
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
			if(msgRicevuto){
				printf("\n<- Dottore %d : ricevuto paziente in coda ## FIFO ## con priorità: # %lu #\n",numDottore,(11-msg.mtype));
			}
		}
		if(msgRicevuto){
			sintomo = getSintomo(msg.mtext);   // recupero sintomo dal messaggio
			pidPaz = getPazientePid(msg.mtext);   // recupero PID paziente dal messaggio
			srand(time(NULL)); //funzione di randomizzazione
			tmpTime = ((rand() % 4)+7);   // attendo tempo casuale per erogazione prestazione tra 2 e 5 secondi
			while((tmpTime = sleep(tmpTime)) != 0 && esc == false); // se la sleep viene interrotta da un segnale riprende da dove aveva terminato fino ad arrivare a 0
			if(esc == false){
				printf("\n<---- Dottore %d: paziente con PID %s e sintomo %s curato ---->\n",numDottore, pidPaz,sintomo);
				int pidPazNum = atoi(pidPaz);
				if(kill(pidPazNum,SIGUSR1) == -1){   // invio messaggio SIGUSR1 a paziente
					perror("Dottore: errore invio segnale SIGUSR1 a paziente\n");
					exit(EXIT_FAILURE);
				}
				free(sintomo);
				free(pidPaz);
			}
		}
		fflush(stdout);
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

	alarm(sec);
}

/**
 * Estrae dalla stringa str il sintomo del paziente
 */
char* getSintomo(char* str) {
	char* sintomo;
	sintomo = (char*) malloc(sizeof(char) * SIZE);
	int i;
	for (i = 0; str[i] != ';'; i++) {
		sintomo[i] = str[i];
	}
	sintomo[i] = '\0';

	return sintomo;
}

/**
 * Estrae dalla stringa str il PID del paziente
 */
char* getPazientePid(char* str) {
	int i;
	char* pid;
	pid = (char*) malloc(sizeof(char) * SIZE);
	for (i = 0; str[i] != ';'; i++);

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
