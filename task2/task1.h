#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

void get_input(int *N,int *M);
void init(float *board,int row, int column);
void print_board(float *board, int row,int column);
float dot(float *a, float *b, int M);
void print_result(float *result,int N);