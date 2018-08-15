#include <iostream>
#include <malloc.h>
#include <mpi.h>
using namespace std;

int main(int argc,char **argv)
{
    int N = 10000;
    MPI_Init(&argc,&argv);
    int rank,size;
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    cout<<"lets staart"<<endl;
    int pack_size;
    MPI_Pack_size(N,MPI_INT,MPI_COMM_WORLD,&pack_size);
    pack_size =N*sizeof(int) +  MPI_BSEND_OVERHEAD;
    int *buf = (int *)malloc(pack_size);
    MPI_Buffer_attach(buf,pack_size);
    int * vec = new int [N];
    int data = 0;
    if(rank == 0)
    {
        MPI_Bsend(vec,N,MPI_INT,1,0,MPI_COMM_WORLD);
        MPI_Ssend(&data,1,MPI_INT,1,1,MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
    }else
    {
        MPI_Recv(&data,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
        MPI_Barrier(MPI_COMM_WORLD);
        
        MPI_Recv(vec,N,MPI_INT,0,0,MPI_COMM_WORLD,&status);
    }

    MPI_Finalize();
    return 0;
}
