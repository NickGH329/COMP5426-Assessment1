#include "task1.h"

int main(int argc, char *argv[])
{
    int np,id,M,N;
    float *board,*sub_board,*temp_board,*result1,*result2;
    int pairs,rows,max_iters;
    int i,j;
    double BEGIN,STOP,r1,r2;

     /* --initial MPI environment and get the total number of processes and process id.-- */
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&np);
    MPI_Comm_rank(MPI_COMM_WORLD,&id);

    if(id == 0){
        get_input(&N,&M);
        board = (float*)malloc(N*M*sizeof(float));
        init(board,N,M);
        //print_board(board,N,M);
        if(np == 1){
            result1 = (float*)malloc(N*(N-1)*sizeof(float));
            for(i = 0; i < N-1; i++)
                for(j = i+1; j < N; j++){
                    result1[i*N+j] = dot(&board[i*M],&board[j*M],M);
                }

            printf("\nnp = 1, do sequential computation, the result is:\n");
            print_result(result1,N);

            free(board);
            free(result1);
            MPI_Finalize();
            exit(0);
        }
    }

    /* -- Broadcast the parameter and scatter taskt to processes -- */
    MPI_Bcast(&N,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&M,1,MPI_INT,0,MPI_COMM_WORLD);
    rows = N/np;
    max_iters = (np-1)/2 + (np-1)%2;
    int total = rows*(rows-1)/2 + max_iters*rows*rows;
    float *sub_result;
    int *index;
    int iter = 0;

    sub_board = (float*)malloc(M*rows*sizeof(float));
    temp_board = (float*)malloc(M*rows*sizeof(float));
    index = (int*)malloc(total*sizeof(int));
    sub_result = (float*)malloc(total*sizeof(float));

    MPI_Scatter(board, M*rows, MPI_FLOAT,sub_board, M*rows, MPI_FLOAT, 0, MPI_COMM_WORLD);

    /* -- do the parallel computation -- */
    int count = 0;
    int TAG = 0;
    int source;
    MPI_Request req;
    MPI_Status status;
    BEGIN = MPI_Wtime();
    for(i = 0; i < rows-1; i++){
        for(j = i+1; j < rows;j++){
            sub_result[count] = dot(&sub_board[i*M],&sub_board[j*M],M);
            index[count] = (id*rows+i)*N+(j+id*rows);
            count++;
        }
    }
   
    while(iter < max_iters){
        source = (np+id-1-iter)%np;
        MPI_Isend(sub_board, rows*M, MPI_FLOAT,(id+1+iter)%np, TAG+iter, MPI_COMM_WORLD, &req);
        MPI_Irecv(temp_board, rows*M, MPI_FLOAT,source,TAG+iter, MPI_COMM_WORLD, &req);
        MPI_Wait(&req, &status);

        for(i = 0; i < rows; i++){
            for(j = 0; j < rows; j++){
                sub_result[count] = dot(&sub_board[i*M],&temp_board[j*M],M);
                if(source > id){
                    index[count] = (id*rows+i)*N+(j+source*rows);
                }
                else{
                    index[count] = (source*rows+j)*N +(i+id*rows);
                }
                count++;
            }
        }
        iter++;
    }
    STOP = MPI_Wtime();
    r1 = STOP-BEGIN;
    float *g_result;
    int *g_index;
    g_result = (float*)malloc(total*np*sizeof(float));
    g_index = (int*)malloc(total*np*sizeof(int));

    MPI_Gather(sub_result,total, MPI_FLOAT,g_result, total, MPI_FLOAT,0, MPI_COMM_WORLD);
    MPI_Gather(index,total, MPI_INT,g_index, total, MPI_INT,0, MPI_COMM_WORLD);
    

    /* -- print the result and do a self check -- */
    if(id ==0){
        result1 = (float*)malloc(N*(N-1)*sizeof(float));
        for(i = 0; i < np*total; i ++){
            result1[g_index[i]] = g_result[i];
        }
        printf("\nparallel computation finish, the result is:\n");
        print_result(result1,N);

        /* -- do the sequential computation -- */
        BEGIN = MPI_Wtime();
        result2 = (float*)malloc(N*(N-1)*sizeof(float));
        for(i = 0; i < N-1; i++)
            for(j = i+1; j < N; j++){
                result2[i*N+j] = dot(&board[i*M],&board[j*M],M);
            }
        printf("\nsequentive computation finish, the result is:\n");
        print_result(result2,N);
        STOP = MPI_Wtime();
        r2 = STOP - BEGIN;

        
        /* -- do a self-check, check if the two result are same -- */
        count = 0;
        for(i = 0; i < N-1; i ++)
            for(j = i +1; j < N; j++)
                if(result1[i*N+j] == result2[i*N+j])
                    count ++;
        if(count == (N-1)*N/2)
            printf("\nThe Parrallel computation and Sequential computation have same result.\n");
        else    
            printf("\nThe Parrallel computation and Sequential computation have different result.\n");

        printf("The running time of Parrallel computation is %f\n", r1);
        printf("The running time of Sequential computation is %f\n",r2);
    }

    if(id == 0){
        free(board);
        free(result1);
        free(result2);
        free(g_index);
        free(g_result);
    }
    free(sub_board);
    free(sub_result);
    free(index);
    free(temp_board);

    MPI_Finalize();
    return 0;
}
