#ifndef MY_HEADER_H_
#define MY_HEADER_H_
#include "my_header.h"
#endif /* MY_HEADER_H_ */

#include <sys/msg.h>

/* STRUCT PER CODA DI MESSAGGI */
struct my_msg {
	long mtype;    // tipo di messaggio
	char mtext[SIZE];  // corpo messaggio
};
