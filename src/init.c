#include<stdlib.h>
#include<stdio.h>
#include<time.h>
//white = 0, red = 1, blue =2
//red or blue move in = 3

void swap(int *a, int *b){
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

void board_init(int **board,int n){
	int rnum = (int)(n*n/3);
	int bnum = rnum;
	int wnum = n*n - rnum - bnum;
	printf("rnum = %d, bnum = %d, wnum = %d\n",rnum,bnum,wnum);
	srand(time(NULL));
	int count = 0;
	int i,j;
	for(i = 0; i < n; i++)
		for(j = 0; j < n; j++){
			if(count < wnum)
				board[i][j] = 0;
			else if(count >= rnum && count < rnum + wnum)
				board[i][j] = 1;
			else
				board[i][j] = 2;
			count++;
		}
	for(i = 0; i < n; i++)
		for(j = 0; j < n; j++)
			swap(&board[i][j],&board[rand()%n][rand()%n]);
}

int *alloc_procs(int numprocs, int t){
	int *p = (int *)malloc(numprocs*sizeof(int));
	int extra = t%numprocs;
	int i;
	for(i = 0; i < numprocs; i++){
		if(extra != 0){
			p[i] = t/numprocs + 1;
			extra--;
		}
		else
			p[i] = t/numprocs;
	}
	return p;
}
