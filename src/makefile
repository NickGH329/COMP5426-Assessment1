run: main.o cell.o 2darray.o init.o io.o 
	mpicc -o run main.o cell.o 2darray.o init.o io.o
main.o: main.c RBcomputation.h
	mpicc -c main.c
2darray.o: 2darray.c
	gcc -c 2darray.c
cell.o: cell.c
	gcc -c cell.c
init.o: init.c
	gcc -c init.c
io.o: io.c
	gcc -c io.c
clean: 
	rm run main.o 2darray.o cell.o init.o io.o
