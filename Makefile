#Implementace interpretu imperativního jazyka IFJ13.
 
PROGS=main
CC=gcc
CFLAGS=-Wall -std=c99 -pedantic

all: $(PROGS)

garbage_collector.o: garbage_collector.c garbage_collector.h
	$(CC) $(CFLAGS) -c garbage_collector.c -o garbage_collector.o

ial.o: ial.c ial.h
	$(CC) $(CFLAGS) -c ial.c -o ial.o

scaner.o: scaner.c scaner.h types.h
	$(CC) $(CFLAGS) -c scaner.c -o scaner.o

#list.o: list.c list.h global.h parser.h
#	$(CC) $(CFLAGS) -c list.c -o list.o

#expr.o: expr.c expr.h parser.h list.h ial.h scanner.h
#	$(CC) $(CFLAGS) -c expr.c -o expr.o


parser.o: parser.c parser.h types.h ial.h garbage_collector.h scaner.h
	$(CC) $(CFLAGS) -c parser.c -o parser.o

#vnitrni_kod.o: vnitrni_kod.c parser.h scanner.h list.h
#	$(CC) $(CFLAGS) -c vnitrni_kod.c -o vnitrni_kod.o

#cilovy_kod.o: cilovy_kod.c cilovy_kod.h built_in.h global.h ial.h vnitrni_kod.h
#	$(CC) $(CFLAGS) -c cilovy_kod.c -o cilovy_kod.o

main.o:  main.c parser.h
	#global.h parser.h global.h
	$(CC) $(CFLAGS) -c main.c -o main.o

main: main.o garbage_collector.o ial.o parser.o scaner.o
#parser.o scanner.o list.o expr.o ial.o global.o vnitrni_kod.o cilovy_kod.o built_in.o
	$(CC) $(CFLAGS) main.o garbage_collector.o ial.o parser.o scaner.o -o main -lm
	#parser.o scanner.o list.o expr.o ial.o global.o vnitrni_kod.o cilovy_kod.o built_in.o -o main -lm

clean:
	rm -f *.o *.out *.zip $(PROGS)

pack:
	zip xbucht18.zip *.c *.h rozdeleni dokumentace.pdf Makefile
