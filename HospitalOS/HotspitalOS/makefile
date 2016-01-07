app: paziente triage reparto dottore processGenerator.c processGenerator.h headerSem.h my_header.h
	gcc -o app processGenerator.c
	gcc  -Wall -std=gnu99 -o app processGenerator.c
	gcc -pedantic -std=gnu99 -o app processGenerator.c
paziente: paziente.c paziente.h headerMsg.h headerSem.h my_header.h
	gcc -o paziente paziente.c
	gcc -Wall -std=gnu99 -o paziente paziente.c
	gcc -pedantic -std=gnu99 -o paziente paziente.c
triage: triage.c triage.h headerMsg.h headerSem.h my_header.h
	gcc -o triage triage.c
	gcc -Wall -std=gnu99 -o triage triage.c
	gcc -pedantic -std=gnu99 -o triage triage.c
reparto: reparto.c reparto.h headerMsg.h my_header.h
	gcc -o reparto reparto.c
	gcc -Wall -std=gnu99 -o reparto reparto.c
	gcc -pedantic -std=gnu99 -o reparto reparto.c
dottore: dottore.c dottore.h headerMsg.h my_header.h
	gcc -o dottore dottore.c
	gcc -Wall -std=gnu99 -o dottore dottore.c
	gcc -pedantic -std=gnu99 -o dottore dottore.c
clean: 
	rm -f app
	rm -f paziente
	rm -f triage
	rm -f reparto
	rm -f dottore
run:
	./app
file:
	./app > log.txt
both:
	./app | tee log.txt
