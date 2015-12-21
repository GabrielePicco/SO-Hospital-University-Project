app: processGenerator.o paziente.o triage.o
	gcc -o app processGenerator.o paziente.o triage.o
processGenerator.o: processGenerator.c 
	gcc -c processGenerator.c
paziente.o: paziente.c 
	gcc -c paziente.c
triage.o: triage.c
	gcc -c triage.c
clean: 
	rm -f *.o
run:
	./app
