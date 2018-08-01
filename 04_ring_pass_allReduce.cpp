#include <iostream>
#include <mpi.h>
#include <random>

using namespace std;
int nprocs,my_rank;
MPI_Status status;
int size = 10000;
int *vec;
int *vec1;

void IPH_all_reduce()
{
	int prev,next;
	prev = my_rank - 1;
	next = my_rank + 1;
	if(prev < 0) prev = nprocs - 1;
	if(next >= nprocs) next = 0;

	for(int i = 0;i<nprocs-1;i++)
	{
		MPI_Sendrecv_replace(vec,size,MPI_INT,
							 next,0,
							 prev,0,
							 MPI_COMM_WORLD,&status);
		for(int j = 0;j<size;j++)
		{
			vec[j] += vec1[j];
		}
	}
	if(vec[0] != nprocs){
		cout<<"error"<<endl;
	}

}

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

	 IPH_all_reduce();

	MPI_Finalize();
	return 0;
}
