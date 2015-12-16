#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

/**
 * Genera ogni x secondi un nuovo paziente (processo), dove x è
 * un numero casuale tra 0 e 2.
 */
int main(int argc,char** argv){
	int waitTime = 2;
	pid_t pidGenerator;
	pidGenerator = getpid();
	while(1){ /* ciclo infinito per il processo generatore, termina con SIGQUIT */
		fork();
		if(pidGenerator == getpid()){ /* dopo la fork controllo se sono il padre o il filgio */
			sleep(waitTime);
			waitTime = rand() % 3; /* numero int casuale tra 0 e 2 */
		}else{
			char *argv[] = {"./paziente" , NULL }; // Il primo elemento (argv[0]) deve contenere il nome del programma da invocare.
			execv("paziente",argv);
			perror("\n<--- errore nella exve del paziente --->\n"); // la execve non ritorna, se lo fa è un errore
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_SUCCESS);
	return 0;
}
