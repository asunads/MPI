#include <iostream>
#include <time.h>
#include <unistd.h>
#include <mpi.h>

using namespace std;
int main(int argc,char **argv)
{
    int rank,size;
    int data_size = 1<<20;
  	int node_cpu_num = 20;
    char *data;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Request req[2];
    MPI_Status status;


    if(rank == 0)
    {
    	data = new char [data_size];
        MPI_Isend(data,data_size,MPI_CHAR,size-1,0,MPI_COMM_WORLD,&req[0]);
        cout<<"send finish"<<endl;
    }else if(rank == size-1){
    	data = new char [data_size];
        sleep(1);
        MPI_Irecv(data,data_size,MPI_CHAR,0,0,MPI_COMM_WORLD,&req[1]);
        MPI_Wait(&req[1],&status);
        //MPI_Recv(&data,128,MPI_CHAR,0,0,MPI_COMM_WORLD,&status);
        cout<<"recv finish"<<endl;
    }

    MPI_Finalize();

    return 0;
}
