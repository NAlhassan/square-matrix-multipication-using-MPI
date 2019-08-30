#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

#define MASTER 0

float *A, *B, *R, *r;
int n;


void prnt(float *a){
  for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			printf("%f ",*(a + i*n + j));
		}
		printf("\n");
	}

  printf("************************\n\n");

}

void filling(float *a){
	srand(time(0));
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
			*(a + i*n + j) =/* 3*/ rand();
		}
	}
}




int main(int argc, char *argv[]){

	MPI_Init(&argc, &argv);

	n = atoi(argv[argc - 1]);
//	n = 3;
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	if (size != n) {
		printf("the number of processes must equal the number of rows in the squre matrix\n");
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}


	A = (float *)malloc(n*n*sizeof(float));
	B = (float *)malloc(n*n*sizeof(float));
	r = (float *)malloc(n*sizeof(float));


	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	filling(A);
	filling(B);

	clock_t start, end;
	double total_time = 0;
																				
	start = clock();

	MPI_Bcast(A, n*n, MPI_FLOAT, MASTER, MPI_COMM_WORLD);
	MPI_Bcast(B, n*n, MPI_FLOAT, MASTER, MPI_COMM_WORLD);

	if(rank == MASTER){
		R = (float *)malloc(n*n*sizeof(float));

		for(int i = 1; i < n; i++)
			MPI_Send(&i, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

		float s = 0;
		for (int j = 0; j < n; j++){
			for(int k = 0; k < n; k++){
				s += *(A + j) * *(B + j);
			}
			*(r + j) = s;
			s = 0;
		}

		MPI_Barrier(MPI_COMM_WORLD);

		MPI_Gather(r, n, MPI_FLOAT, R, n, MPI_FLOAT, MASTER, MPI_COMM_WORLD);
//		prnt(R);
		free(R);
		end = clock();
																				
		total_time = (double) (end - start) / CLOCKS_PER_SEC; //Total time
		printf("%f\n\n", total_time);

	}

	else {
		int i;
		MPI_Recv(&i, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, NULL);
//The multitpication btween A & B
		float s = 0;

		for(int j = 0; j < n; j++){
			for(int k = 0; k < n; k++){
				s += *(A + i*n + j) * *(B + i*n + j);
			}
			*(r + j) = s;
			s = 0;
		}
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Gather(r, n, MPI_FLOAT, NULL, 0, MPI_FLOAT, MASTER, MPI_COMM_WORLD);

	}


	MPI_Finalize();


	free(A);
	free(B);
	free(r);

	return 0;
}
