#include"RBcomputation.h"

int main(int argc, char **argv){
	int **board;
	int **pro_board;
	int n,t,c,max_iters,numprocs,myid,k,i,j,tile;
	int finish = 0;
	int n_iters = 0;
	double BEGIN,STOP,dif1,dif2;
	MPI_Status status;
	MPI_Request req1,req2;
	//MPI_Comm MPI_COMM_WORLD;

	//initial MPI environment and get the total number of processes and process id.
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
        MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	
	if(myid == 0){
		get_input(&n,&t,&c,&max_iters);	
		board = build_array(n,n);
		board_init(board,n);
		//print_board(board,n,n);
		int para[4] = {n,t,c,max_iters};
		tile = n/t;

		//if only one process created, do a sequential iterative computation
		if(numprocs == 1){
			while(n_iters < max_iters && finish == 0){
				RedMove(board,n,n);
				BlueMove(board,n,n);
				int *p =(int*)malloc(t*t*sizeof(int));
				p=check_c(board,n,n,t,c);
				for(i = 0; i < t*t; i++){
					if(p[i] != 0){
						finish = 1;
						printf("\n");
						printf("\nComputation finished, the number %d tile has colored cells than %d%%:\n",i,c);
						print_tile(board,i,n,t);
					}
				}
				n_iters++;
			}
			if(n_iters == max_iters)	
				printf("\nSequentive computation finished.Run out of max_iters and no tile has the colored squares more than %d%% one color (blue or red).\n",c);
			free_malloc(board);
			MPI_Finalize();
			exit(0);
		}
		//Process0 alloc task to others
		else{
			int *tile_num = (int*)malloc(numprocs*sizeof(int));
			tile_num = alloc_procs(numprocs,t);
			k = tile_num[0]*tile;
			int addk = k;
			pro_board = build_array(k+2,n);
			int count = numprocs;
			for(i = 0; i < k; i++)
				for(j = 0; j < n; j++)
					pro_board[i+1][j] = board[i][j];
						
			for(i = 1; i < numprocs; i++){
				printf("Process[%d] has %d tiles\n",i,tile_num[i]);
				int k_temp = tile_num[i]*tile;
				MPI_Send(&para,4,MPI_INT,i,1,MPI_COMM_WORLD);
				MPI_Send(&k_temp,1,MPI_INT,i,2,MPI_COMM_WORLD);
				int **temp_board;
			       	temp_board = build_array(k_temp,n);
				int x,y;
				for(x = 0; x < k_temp; x++){
					for(y = 0; y < n; y++){
						temp_board[x][y] = board[x+addk][y];
					}
				}
				MPI_Send(&temp_board[0][0],k_temp*n,MPI_INT,i,3,MPI_COMM_WORLD);
				free_malloc(temp_board);
				addk +=	k_temp;
			}
			free(tile_num);
		}
	}
	else{
		int para[4];
		MPI_Recv(&para,4,MPI_INT,0,1,MPI_COMM_WORLD,&status);
		MPI_Recv(&k,1,MPI_INT,0,2,MPI_COMM_WORLD,&status);
		n = para[0];
		t = para[1];
		c = para[2];
		tile = n/t;
		max_iters = para[3];
		pro_board = build_array(k+2,n);
		MPI_Recv(&pro_board[1][0],k*n,MPI_INT,0,3,MPI_COMM_WORLD,&status);
	}

	int TAG = 5;

	//Begin the parellel iterative computation
	BEGIN = MPI_Wtime();
	while(n_iters < max_iters && finish == 0){
		if(myid == 0){
			MPI_Isend(&pro_board[1][0],n,MPI_INT,numprocs-1,TAG,MPI_COMM_WORLD,&req1);
		}else{
			MPI_Isend(&pro_board[1][0],n,MPI_INT,myid-1,TAG,MPI_COMM_WORLD,&req1);
		}
		if(myid == numprocs -1){
			MPI_Irecv(&pro_board[k+1][0],n,MPI_INT,0,TAG, MPI_COMM_WORLD,&req1);
			MPI_Isend(&pro_board[k][0],n,MPI_INT,0,TAG+1, MPI_COMM_WORLD,&req2);
		}else{
			MPI_Irecv(&pro_board[k+1][0],n,MPI_INT,myid+1,TAG, MPI_COMM_WORLD,&req1);
			MPI_Isend(&pro_board[k][0],n,MPI_INT,myid+1,TAG+1, MPI_COMM_WORLD,&req2);
		}
		if(myid == 0){
			MPI_Irecv(&pro_board[0][0],n,MPI_INT,numprocs-1,TAG+1,MPI_COMM_WORLD,&req2);
		}else{
			MPI_Irecv(&pro_board[0][0],n,MPI_INT,myid-1,TAG+1,MPI_COMM_WORLD,&req2);
		}
		MPI_Wait(&req1,&status);
		MPI_Wait(&req2,&status);

		RedMove(pro_board,k+2,n);
		BlueMove(pro_board,k+2,n);

		int *p=(int*)calloc(k/tile*t,sizeof(int));
		p=check_c(&pro_board[1],k,n,t,c);
		for(i = 0; i < k/tile*t; i++){
			if(p[i] != 0){
				finish = 1;
				printf("\nIterative computation finished, the tile has colored cells than %d%% is:\n",c);
				print_tile(&pro_board[1],i,n,t);	
				printf("\n");
			}
		}
		free(p);
		TAG += 2;
		n_iters++;
		int *p_finish=(int*)calloc(numprocs,sizeof(int));
		MPI_Gather(&finish,1,MPI_INT,p_finish,1,MPI_INT,0,MPI_COMM_WORLD);
		if(myid == 0)
			for(i = 0; i < numprocs; i++)
				if(p_finish[i] == 1)
					finish = 1;
		MPI_Bcast(&finish,1,MPI_INT,0,MPI_COMM_WORLD);
	}
	STOP = MPI_Wtime();
	dif1 = STOP - BEGIN;
	if(n_iters == max_iters && myid == 0)
		printf("\nParallel computation finished.Run out of max_iters and no tile has the colored squares more than %d%% one color (blue or red).\n",c);
	if(myid == 0)
		printf("dif1 = %f\n",dif1);

	//Do the sequential iterative computation and compare the result
	BEGIN = MPI_Wtime();
	//print_board(board,n,n);
	if(myid == 0){
		n_iters = 0;
		finish = 0;
		while(n_iters < max_iters && finish == 0){
			RedMove(board,n,n);
			BlueMove(board,n,n);
			int *p =(int*)malloc(t*t*sizeof(int));
			p=check_c(board,n,n,t,c);
			for(i = 0; i < t*t; i++){
				if(p[i] != 0){
					finish = 1;
					printf("\nSequentive computation finished, the tile has colored cells than %d%% is:\n",c);
					print_tile(board,i,n,t);
				}
			}
			n_iters++;
		}
	}	
	if(n_iters == max_iters && myid == 0)
		printf("\nSequentive computation finished.Run out of max_iters and no tile has the colored squares more than %d%% one color (blue or red).\n",c);
	STOP = MPI_Wtime();
	dif2 = STOP - BEGIN;

	if(myid == 0){
		printf("\ndif2 = %f\n",dif2);
		free_malloc(board);
	}
	free_malloc(pro_board);
	MPI_Finalize();
	return 0;
}
