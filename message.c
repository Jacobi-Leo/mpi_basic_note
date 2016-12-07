#include<stdio.h>
#include<mpi.h>

#define N 10000

int main ( int argc, char ** argv ) {
    int a[N], b[N], c[N];
    int rank, size;

    for ( int i=0; i < N; i++ ) {
	a[i] = 2;
	b[i] = i;
    }
     
    MPI_Status status;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    MPI_Send( &a[0], 1, MPI_INT, (rank+1)%size, 1, MPI_COMM_WORLD );
    MPI_Recv( &c[0], 1, MPI_INT, (rank-1+size)%size, 1, MPI_COMM_WORLD, &status );

    for ( int i=0; i < N; i++ ) {
	MPI_Recv( &c[i], 1, MPI_INT, (rank-1+size)%size, i, MPI_COMM_WORLD, &status );
    }
    for ( int i=0; i < N; i++ ) {
	MPI_Send( &a[i], 1, MPI_INT, (rank+1)%size, i, MPI_COMM_WORLD );
	printf("%d\n", i);
    }

    MPI_Finalize();
    return 0;
}

