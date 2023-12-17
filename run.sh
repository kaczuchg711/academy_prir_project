# g++ main.cpp -fopenmp
# ./a.out

g++ XOR_sequential.cpp -o XOR_sequential
./XOR_sequential

g++ XOR_omp.cpp -fopenmp -o XOR_omp
./XOR_omp

mpic++ XOR_mpi.cpp -o XOR_mpi
mpirun -np 8 ./XOR_mpi