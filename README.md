# mpi_basic_note

Learning note of MPI

## Command to compile

The following commands are all tested under OpneMPI implementation istead of
MPICH implementation, while `process.c` failes under MPICH.
```
mpicc hello.c -o hello ; mpiexec -n 4 hello
```
```
mpicc process.c -std=c99 -o process ; mpiexec -n 4 process
```
```
mpicc spawn.c -o spawn ; mpiexec -n 4 spawn
```
