g++ main.cpp -fopenmp
./a.out

mpic++ XOR_mpi.cpp -o XOR_mpi
mpirun -np 8 --display-map ./XOR_mpi