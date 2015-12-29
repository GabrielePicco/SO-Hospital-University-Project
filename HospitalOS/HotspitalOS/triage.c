#include "triage.h"

int main(int argc, char** argv) {

	int numReparti;
	char* sintomo;
	char* pid_paziente;
	char* priorita;
	struct my_msg msg;
	char* BUFFER;

	signal(SIGQUIT, terminazione);

	int semid = getIdSemTriagePaziente();

	BUFFER = (char*)malloc(sizeof(char)*SIZE);

	numReparti = getNumReparti();
	creaReparti(numReparti);
	int* keyFIFO = creaFIFOReparti(numReparti);

	key_t key = ftok("triage.c", '(');   // creo chiave per coda di triage
	if (key == -1) {
		perror("Triage: errore creazione chiave\n");
		exit(EXIT_FAILURE);
	}

	int msgid = msgget(key, IPC_CREAT | 0666);   // creo coda di messaggi
	if (msgid == -1) {
		perror("Triage: errore creazione coda\n");
		exit(EXIT_FAILURE);
	}

	releaseSemaforoTriagePazienti(semid); // subito dopo creato coda rilasciamo semaforo abilitiamo pazienti

	FILE* fpSintomi = fopen("Sintomi.txt", "r");  // apro il file dei sintomi
	if (fpSintomi  == NULL) {
		perror("Triage: errore apertura file\n");
		exit(EXIT_FAILURE);
	}

	/* Ciclo infinito per associare priorita' a sintomo letto da coda di messaggi */
	while (esc == false) {
		/* leggo il primo messaggio arrivato su coda */
		if (msgrcv(msgid, &msg, sizeof(msg), 0, 0) == -1) { // va in errore se mtext > maxmsgsz
			if(errno != EINTR){ // se e' in attesa di messaggi e il segnale SIGALARM la interrompe non va in errore
				perror("Triage: errore ricezione messaggio\n");
				exit(EXIT_FAILURE);
			}
		}

		if(errno != EINTR){
			printf("Triage: PID e sintomo del paziente: %s\n", msg.mtext);
			// recupero pid e sintomo da mtext
			 sintomo = getSintomo(msg.mtext);
			 pid_paziente = getPazientePid(msg.mtext);
			 priorita = getPriorita(sintomo,fpSintomi);

			 sprintf(BUFFER,"%s;%s;%s",sintomo,pid_paziente,priorita);

			 // scrivo su FIFO del reparto Sintomo, PID e priorita' del paziente
			 if(write(keyFIFO[atoi(pid_paziente)%numReparti],BUFFER,SIZE) == -1){
				perror("Triage: errore scrittura fifo reparto\n");
				exit(EXIT_FAILURE);
			 }

			 free(sintomo);
			 free(pid_paziente);
			 free(priorita);
		}
	}
	
	// Eliminazione FIFO reparto
	int k;
	char* snum;
	for(k=0;k < numReparti; k++){
		char* name = (char*) malloc(sizeof(char)*SIZE);
		strcpy(name,"fifoReparto");
		snum = intToString(k);
		strcat(name,snum);
		if(unlink(name) == -1){
			printf("fifo name: %s",name);
			perror("Fifo non puo' essere eliminata");
		}
		free(snum);
		free(name);
	}
  	free(BUFFER);
  	free(keyFIFO);

  	fclose(fpSintomi);

  	// Elimina coda messaggi
  	if (msgctl(msgid, IPC_RMID, NULL) == -1) {
		fprintf(stderr, "Coda messaggi non puo' essere elminata.\n");
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
/**
 * Estare dalla stringa str il sintomo del paziente
 */
char* getSintomo(char* str){
	char* sintomo;
	sintomo = (char*)malloc(sizeof(char)*SIZE);
	int i;
	for (i = 0; str[i] != ';'; i++){
		 sintomo[i] = str[i];
	 }
	 sintomo[i] = '\0';
	 return sintomo;
}

/**
 * Estare dalla stringa str il PID del paziente
 */
char* getPazientePid(char* str){
	int i,j;
	char* pid = (char*) malloc(sizeof(char)*SIZE);

	for (i = 0; str[i] != ';'; i++);

	i++;
	 if(str[i] == ' '){
		 i++;
	 }
	 for (j = 0; str[i] != '\0'; i++,j++){
		 pid[j] = str[i];
	 }
	 pid[j] = '\0';

	 return pid;
}

/**
 * Leggo dal file sintomi la priorita' associata al sintomo passato come parametro
 */
char* getPriorita(char* sintomo,FILE* fp){
	boolean trov = false;
	char* buf;
	buf = (char*)malloc(sizeof(char)*SIZE);
	while(!trov && fgets(buf,(SIZE-1),fp) != NULL){
		if(strcmp(getSintomo(buf),sintomo) == 0){
			strcpy(buf,getPazientePid(buf));
			trov = true;
		}
	}
	if(!trov){
		strcpy(buf,"-1");
	}
	rewind(fp);
	return buf;
}

/**
 * Legge dal file di configurazione il numero di reparti
 */
int getNumReparti(){
	int reparti = 0;
	FILE* fconf;
	fconf = fopen("config.ini","r");
	while(getc(fconf)!='\n');
	fscanf(fconf,"Numero Reparti = %d;",&reparti);
	fclose(fconf);
	return reparti;
}

/**
 * Crea reparti in base al numero letto dal file di configurazione
 */
void creaReparti(int num){
	pid_t pid;
	int i;
	for(i=0;i < num; i++){
		pid = fork();
		if(pid == 0){ /* dopo la fork controllo se sono il figlio e avvio il processo reparto*/
			printf("Creazione reparto: %d",(i+1));
			char *argv[] = {"./reparto" ,intToString(i), NULL }; // Il primo elemento (argv[0]) deve contenere il nome del processo da invocare.
			execv("reparto",argv);
			perror("\n<--- errore nella exve del reparto --->\n"); // la execve non ritorna, se lo fa e' un errore
			exit(EXIT_FAILURE);
		}
		printf("\n<-- Creato reparto %d -->\n",i);
	}
}

char* intToString(int num){
	char* snum = (char*) malloc(sizeof(char)*3);
	sprintf(snum, "%d",num);
	return snum;
}

int* creaFIFOReparti(int num){
	int* key = (int*) malloc(sizeof(int)*num);
	int i;
	char* snum;
	for(i=0;i < num; i++){
		char* name = (char*) malloc(sizeof(char)*SIZE);
		strcpy(name,"fifoReparto");
		snum = intToString(i);
		strcat(name,snum);
		mkfifo(name,0666);
		key[i] = open(name,O_WRONLY);  // apro FIFO i in sola scrittura
		if(key[i] == -1){
			perror("Triage: errore apertura FIFO\n");
			exit(EXIT_FAILURE);
		}
		printf("\n<--- Creata %s --->\n",name);
		free(snum);
		free(name);
	}
	return key;
}

/**
 * Restituisce l'ID del semaforo binario paziente - triage
 */
int getIdSemTriagePaziente(){
	key_t key;
	int semid;
	//Recupero la chiave per il semaforo
	if((key = ftok("triage.c",'+')) == -1){
		perror("Triage: Errore nel recupero delle chiave del semaforo sincronizzazione");
		exit(EXIT_FAILURE);
	}

	//Mi collego al semaforo
	if((semid = semget(key,1,0666)) == -1){
		perror("Errore nel collegamento al semaforo sincronizzazzione");
		exit(EXIT_FAILURE);
	}
	return semid;
}

/**
 * Incrementa semaforo binario paziente - triage di 1
 */
void releaseSemaforoTriagePazienti(int semid){
	struct sembuf sops;
	sops.sem_num = 0;
	sops.sem_op = 1;
	sops.sem_flg = 0;
	if(semop(semid, &sops, 1) == -1){
		perror("Errore release semaforo sincronizzazzione");
		exit(EXIT_FAILURE);
	}
}

void terminazione(){
	printf("\n---- Ricevuto SIGQUIT, eliminazione strutture in corso ----\n");
	esc = true;
}

