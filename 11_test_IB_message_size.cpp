#include <iostream>
#include <mpi.h>
using namespace std;

int main(int argc,char **argv)
{
    MPI_Init(&argc,&argv);
    int rank,size;
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    MPI_Request * req;
    MPI_Status * status;
    char *buf;
    double time;
    int msg_size_total = 30;
    int size_all = 1<<30;

    int a = 20;
    int b = 5;

    if(rank == 0 || rank== size - 1)
    {
        buf= new char[1<<msg_size_total];
        req = new MPI_Request[1<<28];
        status = new MPI_Status[1<<26];
    }
    for(int a = 20;a<22;a++)
    {
        if(rank == 0)
        {
            int temp = 1<<a;
            int j = 0;
            double start = MPI_Wtime();
            for(int i = 0;i<size_all;i+=temp)
            {
                MPI_Isend(buf+i,temp,MPI_CHAR,size-1,0,MPI_COMM_WORLD,&req[j]);
                j++;
            }
            MPI_Waitall(j,req,status);
            double end = MPI_Wtime();
            cout<<"size = "<<a<<" time = "<<end - start<<endl;

        }
        if(rank == size - 1)
        {
            int temp = 1<<a;
            int j = 0;
            for(int i = 0;i<size_all;i+=temp)
            {
                MPI_Irecv(buf+i,temp,MPI_CHAR,0,0,MPI_COMM_WORLD,&req[j]);
                j++;
            }
            MPI_Waitall(j,req,status);
        }

    }

    MPI_Finalize();
    return 0;
}
