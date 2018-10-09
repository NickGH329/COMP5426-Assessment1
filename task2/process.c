#include<stdlib.h>
#include<stdio.h>
#include<time.h>

void init(float *board,int row, int column){
    srand(time(NULL));
    int i,j;

    for(i = 0; i < row; i++)
        for(j = 0; j < column; j++)
            board[i*column+j] = (float)rand()/(float)RAND_MAX;
}

float dot(float *a, float *b, int M){
    float result = 0.0;
    int i;
    for(i = 0; i < M; i++){
        result += a[i]*b[i];
    }
    return result;
}