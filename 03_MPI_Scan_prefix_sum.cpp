#include <iostream>
#include <mpi.h>
#include <random>

using namespace std;
int nprocs,my_rank,size = 10000;
int* vec;
int *vec1;
MPI_Op Iph_op;

int main(int argc,char **argv)
{

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
	

	vec = new int[size];
		vec1 = new int[size];
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 600000);

	for(int i = 0;i<size;i++)
	{
		vec[i] = dist6(rng);
		vec[i] = 1;
	}

	MPI_Scan(vec,vec1,size,MPI_INT,MPI_SUM,MPI_COMM_WORLD);

	if(vec1[0] != my_rank+1){
		cout<<"error"<<endl;
	}else{
		cout<<"right"<<endl;
	}

	

	MPI_Finalize();
	return 0;
}