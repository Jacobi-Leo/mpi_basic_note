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
```
mpicc merge.c -o merge -std=c99 ; mpiexec -n 4 merge
```
```
mpicc create.c -o create -std=c99 ; mpiexec -n 23 create
```
```
mpicc message.c -o message -std=c99 ; mpiexec -n message
```

## 
