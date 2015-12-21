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
typedef enum {false = 0,true = 1}boolean;

/* STRUTTURA DELLA CODA DI MESSAGGI */
struct my_msg {
	long mytype; //type message
	char mtext[N]; // corpo del messaggio
};

int split_str(char* str);

int main(int argc, char** argv) {
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
	char* esempio = "Febbre Alta;123456789;6";
	int priorita = split_str(esempio);
	printf("La priorita' e': %d",priorita);
	msg.mytype = (10-priorita);

	while (1){
		if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) { // va in errore se mtext > maxmsgsz
					perror("Reparto: errore scrittura messaggio\n");
					exit(EXIT_FAILURE);
				}
	}

	printf("Messaggio scritto: ");
	return 0;
}


int split_str(char* str){
	int i;
	char* priorita;
	priorita = (char*)malloc(sizeof(char)*M);
	boolean step = false;
	for (i = 0; str[i] == ';' && !step; i++){
		step = true;
	}
	step = false;
	for (; str[i] == ';' && !step; i++){
			int j = 0;
			strcpy(priorita[j],str[i+1]);
			if(str[i+2]!= '\0'){
				j++;
				strcpy(priorita[j],str[i+2]);
			}

			step = true;
			str[i] = '\0';
			i = atoi(priorita);
		}
	return  i;

}
