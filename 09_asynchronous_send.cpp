#include <iostream>
#include <time.h>
#include <unistd.h>
#include <mpi.h>

using namespace std;
int main(int argc,char **argv)
{
    int rank,size;
    int data_size = 1<<20;
    char data[data_size];
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Request req;
    MPI_Status status;

    if(rank == 0)
    {
        MPI_Send(&data,data_size,MPI_CHAR,size-1,0,MPI_COMM_WORLD);
        cout<<"send finish"<<endl;
    }else if(rank == size-1){
        sleep(1);
        //MPI_Recv(&data,128,MPI_CHAR,0,0,MPI_COMM_WORLD,&status);
        cout<<"recv finish"<<endl;
    }

    MPI_Finalize();

    return 0;
}
