#include <stdio.h> 
#include <string.h> 
#include <stdbool.h>
#include <mpi.h> 
int main(int argc, char**argv) { 
	int rank, size;   
	int source, dest, tag=50; 
	char message[100]; 
	MPI_Status status; 

	int version, subversion;  
	MPI_Comm parent, child1, child2; 

	MPI_Get_version(&version, &subversion);  

	MPI_Init(&argc, &argv); 
	//查看进程的运行环境信息 
	MPI_Comm_size(MPI_COMM_WORLD, &size); 
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
	MPI_Comm_get_parent(&parent); 
	if ( version>=3 && rank==0 && parent==MPI_COMM_NULL ) { 
		char key[MPI_MAX_INFO_KEY], key_val[MPI_MAX_INFO_VAL], *next_key; 
		char keys[]="command argv maxprocs soft host arch wdir file thread_level"; 
		int  flag; 
		printf("version=%d subversion=%d\n", version, subversion); 
		printf("values for predefined keys of MPI_INFO_ENV:\n"); 
		next_key = keys; 
		for(int i=0; i<9; i++) { 
			sscanf(next_key, "%s", key); 
			next_key = strstr(keys, key) + strlen(key); 
			MPI_Info_get(MPI_INFO_ENV, key, MPI_MAX_INFO_VAL, key_val, &flag); 
			printf(" %s:%s:%d\n", key, key_val, flag ); 
		} 
	} 
	//繁殖进程，构造进程组的“环” 
	int color, lrank, lsize, rrank, rsize, gsize, grank; 
	MPI_Comm intra_lcomm, intra_rcomm, inter_lcomm, inter_rcomm,  
			 intra_gcomm, inter_gcomm; 
	if ( parent==MPI_COMM_NULL ) { 
		char *color1[2] = {"1", NULL}; 
		char *color2[2] = {"2", NULL}; 
		color = 0; 
		MPI_Comm_spawn("process", color1, size, MPI_INFO_NULL, 0,  
				MPI_COMM_WORLD, &inter_rcomm, MPI_ERRCODES_IGNORE); 
		MPI_Intercomm_merge(inter_rcomm, color, &intra_rcomm); 
		MPI_Comm_spawn("process", color2, size, MPI_INFO_NULL, 0,  
				intra_rcomm, &inter_gcomm,MPI_ERRCODES_IGNORE); 
		MPI_Intercomm_merge(inter_gcomm, color, &intra_gcomm); 

		MPI_Intercomm_create(MPI_COMM_WORLD, 0, intra_gcomm, size*2, 50, 
				&inter_lcomm); 
		MPI_Intercomm_merge(inter_lcomm, color, &intra_lcomm); 
	} else { 
		char *color2[2] = {"2", NULL}; 
		sscanf(argv[1], "%d", &color); 
		if (color==1) { 
			MPI_Comm_get_parent(&inter_lcomm); 
			MPI_Intercomm_merge(inter_lcomm, color, &intra_lcomm); 
			MPI_Comm_spawn("process", color2, size, MPI_INFO_NULL, 0,  
					intra_lcomm, &inter_gcomm,MPI_ERRCODES_IGNORE); 
			MPI_Intercomm_merge(inter_gcomm, color, &intra_gcomm); 

			MPI_Intercomm_create(MPI_COMM_WORLD, 0, intra_gcomm, size*2, 100,  
					&inter_rcomm); 
			MPI_Intercomm_merge(inter_rcomm, color, &intra_rcomm); 
		} else { 
			MPI_Comm_get_parent(&inter_gcomm); 
			MPI_Intercomm_merge(inter_gcomm, color, &intra_gcomm); 

			MPI_Intercomm_create(MPI_COMM_WORLD, 0, intra_gcomm, 0, 50,  
					&inter_rcomm); 
			MPI_Intercomm_merge(inter_rcomm, color, &intra_rcomm); 
			MPI_Intercomm_create(MPI_COMM_WORLD, 0, intra_gcomm, size, 100,  
					&inter_lcomm); 
			MPI_Intercomm_merge(inter_lcomm, color, &intra_lcomm); 
		} 
	} 

	MPI_Comm_size(intra_lcomm, &lsize); 
	MPI_Comm_rank(intra_lcomm, &lrank); 
	MPI_Comm_size(intra_rcomm, &rsize); 
	MPI_Comm_rank(intra_rcomm, &rrank); 
	MPI_Comm_size(intra_gcomm, &gsize); 
	MPI_Comm_rank(intra_gcomm, &grank); 
	if (grank==0) { 
		printf("color rank size lrank lsize rrank rsize grankd gsize\n"); 
		printf("%5d %4d %4d %5d %5d %5d %5d %5d %5d\n",  
				color, rank, size, lrank, lsize, rrank, rsize, grank, gsize); 
		for(int i=1; i<gsize; i++) { 
			MPI_Recv(message, 100, MPI_CHAR, i, 10, intra_gcomm, &status); 
			printf("%s\n", message); 
		} 
	} else { 
		sprintf(message, "%5d %4d %4d %5d %5d %5d %5d %5d %5d",  
				color, rank, size, lrank, lsize, rrank, rsize, grank, gsize); 
		MPI_Send(message, strlen(message)+1, MPI_CHAR, 0, 10, intra_gcomm); 
	} 

	//创建附加了进程组拓扑结构的通信子  
	int ndims=2, cart_rank; 
	int dims[2], periods[2], coords[2]; 
	MPI_Comm comm_cart; 
	dims[0]=3; dims[1]=size; 
	periods[0]=false; periods[1]=false; 
	MPI_Cart_create(intra_gcomm, ndims, dims, periods, false, &comm_cart); 
	if(grank==0) for(int i=0; i<3; i++) for(int j=0; j<size; j++) { 
		coords[0] = i; 
		coords[1] = j; 
		MPI_Cart_rank(comm_cart, coords, &cart_rank); 
		printf("[%d, %d] rank=%d\n", coords[0], coords[1], cart_rank); 
	}  
	//释放所创建的通信子 
	MPI_Comm_free(&comm_cart); 
	MPI_Comm_free(&inter_lcomm); 
	MPI_Comm_free(&intra_lcomm); 
	MPI_Comm_free(&inter_rcomm); 
	MPI_Comm_free(&intra_rcomm); 
	MPI_Comm_free(&inter_gcomm); 
	MPI_Comm_free(&intra_gcomm); 

	MPI_Finalize(); 
	return 0;
} 

