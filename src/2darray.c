#include<stdlib.h>

//build the 2D-array
int **build_array(int row,int column){
	int **temp = (int**)malloc(row*sizeof(int*));
	temp[0] = (int*)calloc(row*column,sizeof(int));
	int i;
	for(i = 1; i < row; i++)
		temp[i] = temp[i-1] + column;
	return temp;
}

//free the dynamic array
void free_malloc(int **board){
	free(board[0]);
	free(board);
}
