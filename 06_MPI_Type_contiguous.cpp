#include <iostream>
#include <mpi.h>
#include <random>

using namespace std;
int nprocs,my_rank;
MPI_Status status;
int size = 10000;
int *vec;
int *vec1;


void Get_input()


int main(int argc,char **argv)
{

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
	vec = new int[size];
	vec1 = new int[size];
	for(int i = 0;i<size;i++)
	{
		vec1[i] = vec[i] = 1;
	}


	MPI_Finalize();
	return 0;
}
