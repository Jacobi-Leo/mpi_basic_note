#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

int main(int argc, char **argv) {
    int rank0, rank1, size0, size1, color, size, rank;
    MPI_Comm parent, inter_comm, intra_comm;

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &size0 );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank0 );
    MPI_Comm_get_parent( &parent );

    if ( parent == MPI_COMM_NULL ) {
	char *color1[2] = {"1", NULL};
	color = 0;
	MPI_Comm_spawn( "merge", color1, size0, MPI_INFO_NULL,
		0, MPI_COMM_WORLD, &inter_comm, MPI_ERRCODES_IGNORE );
	/*
	 *MPI_Comm_size( inter_comm, &size1 );
	 *MPI_Comm_rank( inter_comm, &rank1 );
	 *printf( "I'm in group %d.\n \
	 *        My first rank is %d in %d processes.\n \
	 *        My second rank is %d in %d processes.\n\n",
	 *        color, rank0, size0, rank1, size1 );
	 */
	MPI_Intercomm_merge( inter_comm, color, &intra_comm );
	MPI_Comm_size( intra_comm, &size );
	MPI_Comm_rank( intra_comm, &rank );
	printf( "%d in %d\n", rank, size );
    }
    else {
	sscanf( argv[1], "%d", &color );
	if ( color == 1 ) {
	    /*
	     *MPI_Comm_size( parent, &size1 );
	     *MPI_Comm_rank( parent, &rank1 );
	     *printf( "I'm in group %d.\n \
	     *        My first rank is %d in %d processes.\n \
	     *        My second rank is %d in %d processes.\n\n",
	     *        color, rank0, size0, rank1, size1 );
	     */
	    MPI_Comm_get_parent( &inter_comm );
	    MPI_Intercomm_merge( inter_comm, color, &intra_comm );
	    MPI_Comm_size( intra_comm, &size );
	    MPI_Comm_rank( intra_comm, &rank );
	    printf( "%d in %d\n", rank, size );
	}
    }

    MPI_Finalize();
    return 0;
}

