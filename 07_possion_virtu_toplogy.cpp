#include <iostream>
#include <mpi.h>
#include <algorithm>
#define ITERMAX 1

#define numprocs size
using namespace std;

int main(int argc,char **argv)
{
	int rank,size;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm MPI_Grid_comm;

	int spat_dim[3],proc_dim[3];
	int  pbc_check[3];
	if(rank == 0)
	{
		for(int i = 0;i<3;i++)
		{
			spat_dim[i] = 301*i + 11 +i*i;//111*i+i*i+4;
			proc_dim[i] = 3;
			pbc_check[i] = 	/*1;*/i%2;
		}

	}
	MPI_Bcast(spat_dim,3,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(proc_dim,3,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(pbc_check,3,MPI_INT,0,MPI_COMM_WORLD);

	MPI_Dims_create(numprocs,3,proc_dim);
	if(rank == 0)
	{
		cout<<"("<<proc_dim[0]<<","<<proc_dim[1]<<","<<proc_dim[2]<<")"<<endl;
	}
	bool reorders[3];
	// for(int i = 0;i<3;i++)
	// {
	// 	if(pbc_check[i] == 0x1)
	// 	{
	// 		reorders[i] = true;
	// 	}else{
	// 		reorders[i] = false;
	// 	}
	// }
		
	MPI_Cart_create(MPI_COMM_WORLD,3,proc_dim,pbc_check,true,&MPI_Grid_comm);
	int my_gridId,numP_grid;
	MPI_Comm_rank(MPI_Grid_comm,&my_gridId);
	MPI_Comm_size(MPI_Grid_comm,&numP_grid);

	int mycoord[3];
	int loca_dim[3];
	int check_dim[3];
	MPI_Cart_coords(MPI_Grid_comm,my_gridId,3,mycoord);

	for(int i =0;i<3;i++)
	{
		loca_dim[i] = spat_dim[i]/proc_dim[i];
		check_dim[i] = loca_dim[i];
		if(mycoord[i] < spat_dim[i] % proc_dim[i])
		{
			loca_dim[i] += 1;
		}
		int a = mycoord[i] - 1;
		if(a < 0)
			a = proc_dim[i] - 1;
		if(a < spat_dim[i] % proc_dim[i])
		{
			check_dim[i] +=1;
		}
	}


	for(int i = 0;i<size;i++)
	{
		if(my_gridId == i)
			{
				cout<<my_gridId<<"=>("<<mycoord[0]<<","<<mycoord[1]<<","<<mycoord[2]<<")"<<endl;
				cout<<"size = ("<<loca_dim[0]<<","<<loca_dim[1]<<","<<loca_dim[2]<<")"<<endl;
			}
	}

	int iStart = 0;int iEnd = loca_dim[2] + 1;
	int jStart = 0;int jEnd = loca_dim[1] + 1;
	int kStart = 0;int kEnd = loca_dim[0] + 1;

	int * v1 = new int[(loca_dim[2] + 2)*(loca_dim[1] + 2)*(loca_dim[0] + 2)];
	int **v2 = new int*[(loca_dim[0] + 2)*(loca_dim[1] + 2)];
	for(int i = 0;i<(loca_dim[0] + 2)*(loca_dim[1] + 2);i++)
	{
		v2[i] = v1 + i*(loca_dim[2] + 2);
	}
	int ***v3 = new int **[loca_dim[0] + 2];
	for(int i =0;i<loca_dim[0] + 2;i++)
	{
		v3[i] = v2 + i*(loca_dim[1] + 2);
	}

	int check[3] = {1,100,10000};
	for(int i = 0;i<loca_dim[0]+2;i++)
	{
		for(int j = 0;j<loca_dim[1]+2;j++)
		{
			for(int k = 0;k<loca_dim[2]+2;k++)
			{
				v3[i][j][k] = i*check[0] + j*check[1]+ k*check[2];
				if(rank == 0)	
				{
			//		cout<<i<<"*"<<j<<"*"<<k<<"="<<i*j*k<<endl;
				}
			}
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	int totmsgsize[3];
	int MaxBufLen = -1;
	int plane_index[3][2];
	//	逻辑上	(k,j,i)
	//			 大间断 小间断 连续  存储
	//  存储索引(arr[0],arr[1],arr[2]);
	//  进程索引(cod[0],cod[1],cod[2]);


	//j-k plane
	totmsgsize[2] = loca_dim[1]*loca_dim[0];
	plane_index[2][0] = 0;
	plane_index[2][1] = 1;
	//i-k plane
	totmsgsize[1] = loca_dim[0]*loca_dim[2];
	plane_index[1][0] = 0;
	plane_index[1][1] = 2;
	//i-j plane
	totmsgsize[0] = loca_dim[1]*loca_dim[2];
	plane_index[0][0] = 1;
	plane_index[0][1] = 2;

	for(int i = 0;i<3;i++)
	{
		MaxBufLen = max(MaxBufLen,totmsgsize[i]);
	}
	int * fieldSend = new int[MaxBufLen];
	int * fieldRecv = new int[MaxBufLen];
///
	MPI_Request request;
	MPI_Status  status;
	int tag = 0;
	int source,dest;

	//测试进程拓扑是否满足
	for(int disp = -1;disp <= 1;disp +=2)
	{
		for(int dir = 0;dir < 3 ;dir++)
		{
			int test,test1;
			MPI_Cart_shift(MPI_Grid_comm,dir,disp,&source,&dest);
			if(source !=MPI_PROC_NULL)	
				MPI_Irecv(&test,1,MPI_INT,source,tag,MPI_Grid_comm,&request);
			if(dest != MPI_PROC_NULL)
				MPI_Send(&test1,1,MPI_INT,dest,tag,MPI_Grid_comm);
			if(source != MPI_PROC_NULL)
				{
					MPI_Wait(&request,&status);
				}
		}
	}
	for(int iter  = 0;iter<ITERMAX;iter++)
	{
		for(int disp = -1;disp <= 1;disp +=2)
		{
			for(int  dir = 0;dir < 3;dir++)
			{
					//ls -al
				MPI_Cart_shift(MPI_Grid_comm,dir,disp,&source,&dest);
			//	cout<<"source = "<<source<<"dest = "<<dest<<endl;
				if(source != MPI_PROC_NULL)
				{
					MPI_Irecv(fieldRecv,totmsgsize[dir],MPI_INT,source,tag,MPI_Grid_comm,&request);
				}else
				{
					//边界条件处理
					if(mycoord[2] == 0 || mycoord[2] == proc_dim[2]-1)
					{
						//i 方向边界
					}
					if(mycoord[1] == 0 || mycoord[1] == proc_dim[1]-1)
					{
						//j方向边界
					}
					if(mycoord[0] ==0  || mycoord[0] == proc_dim[0] - 1)
					{
						//k 方向边界
					}
				}
				if(dest != MPI_PROC_NULL)
				{
					int vec[3] = {0,0,0};
					if(disp >0 )
						vec[dir] = loca_dim[dir];
					else
						vec[dir] = 1;
					for(int i = 0;i<loca_dim[plane_index[dir][0]];i++)
					{
						vec[plane_index[dir][0]] = i+1;
						for(int j = 0;j<loca_dim[plane_index[dir][1]];j++)
						{
							vec[plane_index[dir][1]] = j+1;
							fieldSend[i*loca_dim[plane_index[dir][1]] + j] = v3[vec[0]][vec[1]][vec[2]];
							//v3[vec[0]][vec[1]][vec[2]]= 0.0;
						}
					}
					MPI_Send(fieldSend,totmsgsize[dir],MPI_INT,dest,tag,MPI_Grid_comm);
				}
				if(source != MPI_PROC_NULL)
				{
					MPI_Wait(&request,&status);
					int vec[3];
					int temp;
					if(disp > 0){
						temp = check_dim[dir];
						vec[dir] = 0;
					}
					else{
						temp = 1;
						vec[dir] = loca_dim[dir]+1;
					}

					for(int i = 0;i<loca_dim[plane_index[dir][0]];i++)
					{
						vec[plane_index[dir][0]] = i+1;
						for(int j = 0;j<loca_dim[plane_index[dir][1]];j++)
						{
							vec[plane_index[dir][1]] = j+1;
						    v3[vec[0]][vec[1]][vec[2]] = fieldRecv[i*loca_dim[plane_index[dir][1]] + j];
						    if(v3[vec[0]][vec[1]][vec[2]] != /*(int)vec[plane_index[dir][0]]*vec[plane_index[dir][1]]*temp */check[dir]*temp+check[plane_index[dir][0]]*vec[plane_index[dir][0]]
						    	+ check[plane_index[dir][1]]*vec[plane_index[dir][1]])
						    {
						    	cout<<"at:"<<vec[0]<<" "<<vec[1]<<" "<<vec[2]<<" direction "<<dir<<" shift "<<disp<<" "<<v3[vec[0]][vec[1]][vec[2]] <<"!= "<<(int)vec[plane_index[dir][0]]*vec[plane_index[dir][1]]*temp<<endl;
						    }
						}
					}
				 }
				//内存拷贝
			}
		}
	}



	MPI_Finalize();
	return 0;
}