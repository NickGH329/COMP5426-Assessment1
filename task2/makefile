run: main.o io.o process.o
	mpicc -o run main.o io.o process.o
main.o: main.c task1.h
	mpicc -c main.c
io.o: io.c
	gcc -c io.c
process.o: process.c
	gcc -c process.c
clean: 
	rm run main.o io.o process.o
