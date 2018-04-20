#include"RBcomputation.h"

int main(int argc, char **argv){
	int **board;
	int **pro_board;
	int *tile_num,*sendcount,*displs;
	int n,t,c,max_iters,numprocs,myid,k,i,j,tile,NP;
	int finish = 0;
	int n_iters = 0;
	double BEGIN,STOP,dif1,dif2;
	MPI_Status status;
	MPI_Request req1,req2;
	MPI_Comm NEW_COMM;

	//initial MPI environment and get the total number of processes and process id.
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	NP = numprocs;
        MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Group world_group,group;
	MPI_Comm_group(MPI_COMM_WORLD, &world_group);
	
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
			tile_num = (int*)malloc(numprocs*sizeof(int));
			displs = (int*)malloc(numprocs*sizeof(int));
			sendcount = (int*)malloc(numprocs*sizeof(int));
			tile_num = alloc_procs(numprocs,t);
			k = tile_num[0]*tile;
			int sum = 0;

			for(i = 0; i < NP; i++){
				if(tile_num[i] == 0)
					numprocs--;
				sendcount[i] = tile_num[i]*tile*n;
				displs[i] = sum;
				sum += sendcount[i];
			}
			for(i = 1; i < NP; i++){
				//printf("Process[%d] has %d tiles\n",i,tile_num[i]);
				int k_temp = tile_num[i]*tile;
				MPI_Send(&para,4,MPI_INT,i,1,MPI_COMM_WORLD);
				MPI_Send(&k_temp,1,MPI_INT,i,2,MPI_COMM_WORLD);
				MPI_Send(&numprocs,1,MPI_INT,i,4,MPI_COMM_WORLD);
			}
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
		MPI_Recv(&numprocs,1,MPI_INT,0,4,MPI_COMM_WORLD,&status);
	}

	int TAG = 5;
	//Using scatterv to allocate the board
	pro_board = build_array(k+2,n);
	MPI_Scatterv(&board[0][0], sendcount, displs,MPI_INT,&pro_board[1][0],k*tile*n,MPI_INT,0,MPI_COMM_WORLD);

	//Build a new comm, and terminate the process which has no job
	int *ranks =(int*)calloc(numprocs,sizeof(int));
	for(i = 0; i < NP; i++){
		ranks[i] = i;
		}
	MPI_Group_incl(world_group, numprocs, ranks, &group);
	MPI_Comm_create(MPI_COMM_WORLD, group, &NEW_COMM);
	free(ranks);
	if(myid >= numprocs){
		MPI_Finalize();
		exit(0);
	}

	//Begin the parellel iterative computation
	BEGIN = MPI_Wtime();
	while(n_iters < max_iters && finish == 0){
		if(myid == 0){
			MPI_Isend(&pro_board[1][0],n,MPI_INT,numprocs-1,TAG,NEW_COMM,&req1);
		}else{
			MPI_Isend(&pro_board[1][0],n,MPI_INT,myid-1,TAG,NEW_COMM,&req1);
		}
		if(myid == numprocs -1){
			MPI_Irecv(&pro_board[k+1][0],n,MPI_INT,0,TAG, NEW_COMM,&req1);
			MPI_Isend(&pro_board[k][0],n,MPI_INT,0,TAG+1, NEW_COMM,&req2);
		}else{
			MPI_Irecv(&pro_board[k+1][0],n,MPI_INT,myid+1,TAG, NEW_COMM,&req1);
			MPI_Isend(&pro_board[k][0],n,MPI_INT,myid+1,TAG+1, NEW_COMM,&req2);
		}
		if(myid == 0){
			MPI_Irecv(&pro_board[0][0],n,MPI_INT,numprocs-1,TAG+1,NEW_COMM,&req2);
		}else{
			MPI_Irecv(&pro_board[0][0],n,MPI_INT,myid-1,TAG+1,NEW_COMM,&req2);
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
		MPI_Gather(&finish,1,MPI_INT,p_finish,1,MPI_INT,0,NEW_COMM);
		if(myid == 0)
			for(i = 0; i < numprocs; i++)
				if(p_finish[i] == 1)
					finish = 1;
		MPI_Bcast(&finish,1,MPI_INT,0,NEW_COMM);
	}
	STOP = MPI_Wtime();
	dif1 = STOP - BEGIN;
	if(n_iters == max_iters && myid == 0)
		printf("\nParallel computation finished.Run out of max_iters and no tile has the colored squares more than %d%% one color (blue or red).\n",c);
	if(myid == 0)
		printf("dif1 = %f\n",dif1);

	//Do the sequential iterative computation and compare the result
	BEGIN = MPI_Wtime();
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
		printf("dif2 = %f\n",dif2);
		printf("\nParallel Computation time is:%f\n",dif1);
		printf("Sequential Computation time is:%f\n",dif2);
		printf("dif2 - dif1 = %f\n", dif2-dif1);
		free_malloc(board);
		free(tile_num);
		free(sendcount);
		free(displs);
	}
	free_malloc(pro_board);
	MPI_Finalize();
	return 0;
}
