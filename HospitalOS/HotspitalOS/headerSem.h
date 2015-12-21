#include <sys/sem.h>
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
#else
	union semun {
	 int val;// value for SETVAL
	 struct semid_ds* buf;// buffer for IPC_STAT, IPC_SET
	 unsigned short* array;// array for GETALL, SETALL
	#if defined(__linux__)// Linux specific part
	 struct seminfo* __buf;// buffer for IPC_INFO
	#endif
	};
#endif

