#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<mpi.h>

int main(int argc, char ** argv) {
    MPI_Comm intra_gcomm, inter_lcomm, inter_rcomm, intra_lcomm, intra_rcomm;
    MPI_Status status;
    int id_key, id_key1;
    int rank, size;
    char message[100];
    
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    
    if ( rank > 3*(size/3) - 1 ) {
	id_key = MPI_UNDEFINED;
	id_key1 = id_key;
    }
    else {
	id_key = rank % 3;
	id_key1 = 1;
    }

    MPI_Comm_split( MPI_COMM_WORLD, id_key, rank, &intra_gcomm );

    if ( id_key == 0 ) {
	MPI_Intercomm_create( intra_gcomm, 0, MPI_COMM_WORLD, 1, 1, &inter_lcomm );
	MPI_Intercomm_create( intra_gcomm, 0, MPI_COMM_WORLD, 2, 2, &inter_rcomm );
	MPI_Intercomm_merge( inter_lcomm, id_key, &intra_lcomm );
	MPI_Intercomm_merge( inter_rcomm, id_key, &intra_rcomm );
    }
    else if ( id_key == 1 ) {
	MPI_Intercomm_create( intra_gcomm, 0, MPI_COMM_WORLD, 0, 1, &inter_rcomm );
	MPI_Intercomm_create( intra_gcomm, 0, MPI_COMM_WORLD, 2, 3, &inter_lcomm );
	MPI_Intercomm_merge( inter_rcomm, id_key, &intra_rcomm );
	MPI_Intercomm_merge( inter_lcomm, id_key, &intra_lcomm );
    }
    else if (id_key == 2 ) {
	MPI_Intercomm_create( intra_gcomm, 0, MPI_COMM_WORLD, 0, 2, &inter_lcomm );
	MPI_Intercomm_create( intra_gcomm, 0, MPI_COMM_WORLD, 1, 3, &inter_rcomm );
	MPI_Intercomm_merge( inter_lcomm, id_key, &intra_lcomm );
	MPI_Intercomm_merge( inter_rcomm, id_key, &intra_rcomm );
    }

    int rrank, lrank, rsize, lsize, gsize, grank;
    if ( id_key1 != 1 ) {
	printf("I'm process %d with id_key %d.\n\n", rank, id_key);
    }
    else {
	MPI_Comm_size( intra_lcomm, &lsize );
	MPI_Comm_rank( intra_lcomm, &lrank );
	MPI_Comm_size( intra_rcomm, &rsize );
	MPI_Comm_rank( intra_rcomm, &rrank );
	MPI_Comm_size( intra_gcomm, &gsize );
	MPI_Comm_rank( intra_gcomm, &grank );
	if ( rank == 0 ) {
	    printf( "color rank size lrank lsize rrank rsize grank gsize\n" );
	    printf( "%5d %4d %4d %5d %5d %5d %5d %5d %5d\n",
		    id_key, rank, size, lrank, lsize, rrank, rsize, grank, gsize );
	    for ( int i = 1; i < 3*(size/3); i++) {
		MPI_Recv( message, 100, MPI_CHAR, i, 10, MPI_COMM_WORLD, &status );
		printf( "%s\n", message );
	    }
	} 
	else {
	    sprintf( message, "%5d %4d %4d %5d %5d %5d %5d %5d %5d",
		    id_key, rank, size, lrank, lsize, rrank, rsize, grank, gsize );
	    MPI_Send( message, strlen(message)+1, MPI_CHAR, 0, 10, MPI_COMM_WORLD );
	}
    }

    int ndims=2, cart_rank;	
    int dims[2], periods[2], coords[2];
    MPI_Comm comm_cart, comm_new;
    MPI_Comm_split( MPI_COMM_WORLD, id_key1, rank, &comm_new );
    if ( id_key1 == 1) {
	dims[0] = 3; 
	dims[1] = size / 3;
	periods[0] = false; 
	periods[1] = false;
	MPI_Cart_create( comm_new, ndims, dims, periods, false, &comm_cart );
	if ( rank == 0 ) {
	    for ( int i = 0; i < 3; i++) {
		for(int j = 0; j < size/3; j++) {
		    coords[0] = i;
		    coords[1] = j;
		    MPI_Cart_rank( comm_cart, coords, &cart_rank );
		    printf( "[%d, %d] rank=%d\n", coords[0], coords[1], cart_rank );
		}
	    }
	}
	MPI_Comm_free( &inter_lcomm );
	MPI_Comm_free( &inter_rcomm );
	MPI_Comm_free( &intra_lcomm );
	MPI_Comm_free( &intra_rcomm );
	MPI_Comm_free( &intra_gcomm );
	MPI_Comm_free( &comm_cart );
	MPI_Comm_free( &comm_new );
    }
    MPI_Finalize();
}

