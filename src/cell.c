#include<stdlib.h>
//move red cells and blue cells
//check tile

//move red cells
void RedMove(int **pro_board,int row,int column){
	int i,j,move;
	for(i = 0; i < row; i++){
		if(pro_board[i][0] == 0)
			move = 1;
		else
			move = 0;
		for(j = 0; j < column; j++){
			if(j == column-1){
				if(pro_board[i][j] == 1 && move == 1){
					pro_board[i][j] = 0;
					pro_board[i][0] = 1;
				}
				else if(pro_board[i][j] == 3)
					pro_board[i][j] = 1;	
			}
			else if(pro_board[i][j] == 1 && pro_board[i][j+1] == 0){
				pro_board[i][j] = 0;
				pro_board[i][j+1] = 3;
			}
			else if(pro_board[i][j] == 3)
				pro_board[i][j] = 1;
		}
	}
}

//move blue cells
void BlueMove(int **pro_board,int row,int column){
	int i,j,move;
	for(j = 0; j < column; j++){
		if(pro_board[0][j] == 0)
			move = 1;
		else
			move = 0;
		for(i = 0; i < row; i++){
			if(i == row-1){
				if(pro_board[i][j] == 2 && move == 1){
					pro_board[i][j] = 0;
					pro_board[0][j] = 2;
				}
				else if(pro_board[i][j] == 3)
					pro_board[i][j] = 2;	
			}
			else if(pro_board[i][j] == 2 && pro_board[i+1][j] == 0){
				pro_board[i][j] = 0;
				pro_board[i+1][j] = 3;
			}
			else if(pro_board[i][j] == 3)
				pro_board[i][j] = 2;
		}
	}
}

//check if if tile’s colored cells are more than c% one color (blue or red)
void *check_c(int **pro_board,int k,int n,int t, int c){
	int tile = n/t;
	int bluenum = 0;
	int rednum = 0;
	int i,j,x,y;
	int *p;
	int cells = tile*tile;
	p = (int*)malloc(sizeof(int)*(k/tile*n/tile));
	for(i = 0; i < k/tile; i++){
		for(j = 0; j < n/tile; j++){
			for(x = tile*i; x < tile*(i+1); x++){
				for(y = tile*j; y < tile*(j+1); y++){
					if(pro_board[x][y] == 1)
						rednum++;
					else if(pro_board[x][y] == 2)
						bluenum++;
				}
			}
			if((float)rednum/(float)cells > (float)c/100)
				p[i*t+j] = 1;
			else if((float)bluenum/(float)cells > (float)c/100)
				p[i*t+j] = 2;
			else
				p[i*t+j] = 0;

			//printf("%d%d: rnum=%d bnum=%d\n",i,j,rednum,bluenum);
			rednum = 0;
			bluenum = 0;
		}
	}
	/*for(i = 0; i < k/tile*n/tile;i++)
		printf("p[%d]:%d ", i,p[i]);
		*/
	return p;
}
