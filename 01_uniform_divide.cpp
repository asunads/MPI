#include <iostream>
#include <mpi.h>
using namespace std;
int main(int argc,char **argv)
{
    MPI_Init(&argc,&argv);
    int n_size = 3333333;
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    int temp = n_size % size;
    int block_size = n_size / size;
    int start = block_size*rank;
    if(rank < temp){
          start += rank;
          block_size++;
    }
    if(rank == temp) start += rank;
    int end = start + block_size;

    for(int i = 0;i<size;i++)
    {
        if(rank == i)
        {
            cout<<start<<" "<<end<<endl;
        }
    }

    MPI_Finalize();
    return 0;

}
