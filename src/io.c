#include<stdio.h>

void get_input(int *n,int *t,int *c,int *max_iters){
	printf("Please input n:\n");
        scanf("%d",n);
        printf("Please input t:\n");
        scanf("%d",t);
        printf("Please input c:\n");
        scanf("%d",c);
        printf("Please input the max iteration number:\n");
        scanf("%d",max_iters);
}

//print board
void print_board(int **board, int row,int column){
	int i,j;
	for(i = 0; i < row; i++)
		for(j = 0; j < column; j++){
			printf("%d ",board[i][j]);
			if(j == column-1)
				printf("\n");
		}
}
void print_tile(int **board,int id, int n,int t){
	int tile = n/t;
	int i,j;
	for(i = id/t*tile; i < (id/t+1)*tile; i++)
		 for(j = id%t*tile; j < (id%t+1)*tile; j++){
			 printf("%d ", board[i][j]);
			 if(j == (id%t+1)*tile - 1)
				  printf("\n");
		 }
}

