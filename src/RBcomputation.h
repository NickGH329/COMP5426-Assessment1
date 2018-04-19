#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<mpi.h>

int **build_array(int row,int colum);
void free_malloc(int **board);

void get_input(int *n,int *t,int *c,int *max_iters);
void print_board(int **board, int row,int column);
void print_tile(int **board,int id, int n,int t);

void swap(int *a,int *b);
void board_init(int **board,int n);
int *alloc_procs(int numprocs, int t);

void RedMove(int **pro_board, int row, int column);
void BlueMove(int **pro_board, int row, int column);
void *check_c(int **pro_board,int k,int n,int t,int c);
