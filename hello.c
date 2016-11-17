#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
	int my_rank;
	int p;
	int source;
	int dest;
	int tag = 50;
	int a = 2;
	int *pa;
	char message[100];

	/*printf("%d\n", argc);*/
	/*printf("%s\n", *argv);*/
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	a =  my_rank;
	if ( my_rank == 0 ) {
	    /*pa = new int[3];*/
	    pa = (int *)calloc(3, sizeof(int));
	    pa[0] = 0;
	    pa[1] = 1;
	    pa[2] = 2;
	}
	printf("pointer in %d process is: \n %d\t%d\t%d\t%d\n", 
		my_rank, pa[0], pa[1], pa[2], pa[3]);
	if ( my_rank != 0 ) {
		sprintf( message, "greetings from process %d! a is %d.", 
			my_rank, a );
		dest = 0;
		MPI_Send( message, strlen(message)+1, MPI_CHAR, 
			dest, tag, MPI_COMM_WORLD );
	}
	else if (my_rank == 0) {
		for ( source=1; source<p; source++ ) {
			MPI_Recv( message, 100, MPI_CHAR, 
				source, tag, MPI_COMM_WORLD, &status );
			printf("%s\n", message);
		}
		printf("There are %d piece(s) of messages.\n", 
			MPI_MAX_INFO_KEY);
		printf("And each piece is at most %d characters long.\n", 
			MPI_MAX_INFO_VAL);
	}
	MPI_Finalize();
	return 0;
}
