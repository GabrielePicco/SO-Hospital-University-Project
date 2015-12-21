app: paziente triage
	gcc -o app processGenerator.c
paziente: paziente.c 
	gcc -o paziente paziente.c
triage: triage.c
	gcc -o triage triage.c
clean: 
	rm -f app
	rm -f paziente
	rm -f triage
run:
	./app
