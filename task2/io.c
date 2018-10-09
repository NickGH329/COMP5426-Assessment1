 #include<stdio.h>

void get_input(int *N,int *M){
	printf("Please input N(row number):\n");
    scanf("%d",N);
    printf("Please input M(the length of vector):\n");
    scanf("%d",M);
}

//print board
void print_board(float *board, int row,int column){
	int i;
	for(i=0; i<row*column; i++){
        printf("%f ",board[i]);
        if((i+1)%column == 0)
            printf("\n");
    }
}

void print_result(float *result,int N){
    int i,j;
    for(i = 0; i < N -1; i++){
        for(j = i + 1; j < N; j++){
            printf("%f ",result[i*N+j]);
            if(j+1 == N)
                printf("\n");
        }
    }
}
