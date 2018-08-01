#include <iostream>
#include <mpi.h>
#include <random>

using namespace std;
int nprocs,my_rank;
int * vec,*vec1;int vec_size;
int scala,temp;
int IPH_start,IPH_end;
int * loc_vec,block_size;
int *sendcounts = new int[nprocs];
int *displs = new int[nprocs];

void input()
{

	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 600000);


	vec_size = dist6(rng)/100;
	vec = new int[vec_size];
	vec1 = new int[vec_size];
	for(int i = 0;i<vec_size;i++)
	{
		vec[i] = dist6(rng);
    	//cout<<vec[i]<<endl;
	}
	scala = dist6(rng);
}
void  dispatch()
{
	MPI_Bcast(&scala,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&vec_size,1,MPI_INT,0,MPI_COMM_WORLD);

	temp = vec_size % nprocs;
	block_size = vec_size / nprocs;
	loc_vec = new int[block_size + 1];

	sendcounts = new int[nprocs];
	displs = new int[nprocs];
	for(int i = 0;i<nprocs;i++)
	{
		if(i < temp)
			sendcounts[i] = block_size+1;
		else
			sendcounts[i] = block_size;
		if(i <= temp)
			displs[i] = block_size*i + i;
		else
			displs[i] = block_size*i + temp;
	}
	IPH_start = displs[my_rank];
	IPH_end = IPH_start + sendcounts[my_rank];

    // for(int i = 0;i<nprocs;i++)
    // {
    // 	if(my_rank == i)
    // 	cout<<IPH_start<<" "<<IPH_end<<endl;
    // }
//	cout<<vec_size<<endl;
	if(my_rank == 0){
		MPI_Scatterv(vec,sendcounts,displs,MPI_INT,
			loc_vec,1000000,MPI_INT,
			0,MPI_COMM_WORLD);
	}else{
		MPI_Scatterv(vec,sendcounts,displs,MPI_INT,
			loc_vec,1000000,MPI_INT,
			0,MPI_COMM_WORLD);
	}
}

void Iph_calc()
{
	for(int i = 0;i<sendcounts[my_rank];i++)
	{
		loc_vec[i] = loc_vec[i]*scala;
	}
}

void Iph_gather()
{
	if(my_rank == 0){
		MPI_Gatherv(loc_vec,sendcounts[my_rank],MPI_INT,
			vec1 ,sendcounts,displs,MPI_INT,
			0,MPI_COMM_WORLD);
	}else{
		MPI_Gatherv(loc_vec,sendcounts[my_rank],MPI_INT,
			vec1 ,sendcounts,displs,MPI_INT,
			0,MPI_COMM_WORLD);
	}
	if(my_rank == 0)
		for(int i = 0;i<vec_size;i++)
		{
			if(vec[i]*scala != vec1[i])
			{
				cout<<"error"<<endl;
			}
		}
}

	int main(int argc,char **argv)
	{

		MPI_Init(&argc,&argv);
		MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
		MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

		if(my_rank == 0){
			input();
		}
		dispatch();
		Iph_calc();

		Iph_gather();
		MPI_Finalize();
		return 0;
	}
