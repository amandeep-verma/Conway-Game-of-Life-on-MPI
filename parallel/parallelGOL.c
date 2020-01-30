#include <stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include <sys/time.h>
#include <mpi.h>
#include <assert.h>

#define alive 1
#define dead 0

//prototype functions
void DisplayGoL(int **local_Matrix,int g);
void Simulate(int **local_Matrix);
int DetermineState(int i,int j,int **local_Matrix);
void Filling_Local_Matrix(int **local_Matrix);
void GenerateInitialGoL(int **local_Matrix);

double compT, tComm; // compT=computation time and tComm;= communication time
struct timeval start, end; // time var for compT
struct timeval t1, t2; // time var for Tcomm

int rank;
int p;              // number of processor
int n;              // size of side of matrix
int BIGPRIME=68111; // big prime number
int NW,NE,SW,SE;    //northwest northeast southwest southeast- for neighbours of corner elements
MPI_Status status;

void DisplayGoL(int **local_Matrix,int g)
{
    int a=n*n/p;
    
    if(rank!=0)
    {
        int *sending_Matrix=(int*)malloc(sizeof(int)*(n*(n/p)));
        int i,j;
        for(i=0;i<n/p;i++)
        {
            for(j=0;j<n;j++)
            {
                sending_Matrix[i*n+j]=local_Matrix[i+1][j];
            }
        }
        
        MPI_Send(sending_Matrix,a,MPI_INT,0,0,MPI_COMM_WORLD);
        free(sending_Matrix);
    }
    
    if(rank==0)
    {
        int full_Matrix[n*n];
        int i,j;
        for(i=0;i<n/p;i++)
        {
            for(j=0;j<n;j++)
            {
                full_Matrix[i*n+j]=local_Matrix[i+1][j];
            }
        }
        int order=n*(n/p); //point at full_Matrix
        int processor;
        for(processor=1;processor<p;processor++)
        {
            int *recieving_matrix=(int*)malloc(sizeof(int)*n*(n/p));
            MPI_Recv(recieving_matrix,a,MPI_INT,processor,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            for(i=0;i<a;i++)
            {
                full_Matrix[processor*order+i]=recieving_matrix[i];
            }
            
            free(recieving_matrix);
            //printf("am here22\n");
        }
        printf("\n\nfor %d gen\n\n",g);
        for(i=0;i<n;i++)
        {
            for(j=0;j<n;j++)
            {
                printf("%d",full_Matrix[i*n+j]);
            }
            printf("\n");
        }
    }
    
}

void Simulate(int **local_Matrix)
{
    int i,j;
    gettimeofday(&start, NULL);
    for(i=1;i<n/p;i++)
    {
        for(j=0;j<n;j++)
        {
            int state=DetermineState(i,j,local_Matrix);
            local_Matrix[i][j]=state;
        }
    }
    gettimeofday(&end, NULL);
    compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
}

int DetermineState(int i,int j,int **local_Matrix)
{
    int sum=0; //holds value of number of alive neighbours for each element
    
    if(!(rank==0)&&!(rank==p-1))
    {
        if(!(j==0)&&!(j==n-1))
        {
            sum=local_Matrix[i-1][j-1]+local_Matrix[i-1][j]+local_Matrix[i-1][j+1]+
            local_Matrix[i][j-1]+local_Matrix[i][j+1]+
            local_Matrix[i+1][j-1]+local_Matrix[i+1][j]+local_Matrix[i+1][j+1];
        }
        else if(j==0)
        {
            sum=local_Matrix[i-1][n-1]+local_Matrix[i-1][j]+local_Matrix[i-1][j+1]+
            local_Matrix[i][n-1]+local_Matrix[i][j+1]+
            local_Matrix[i+1][n-1]+local_Matrix[i+1][j]+local_Matrix[i+1][j+1];
        }
        else if(j==n-1)
        {
            sum=local_Matrix[i-1][j-1]+local_Matrix[i-1][j]+local_Matrix[i-1][0]+
            local_Matrix[i][j-1]+local_Matrix[i][0]+
            local_Matrix[i+1][j-1]+local_Matrix[i+1][j]+local_Matrix[i+1][0];
        }
    }
    
    if(rank==0)
    {
        if((!(i==1)&&!(j==0))||(!(i==1)&&!(j==n-1)))
        {
            if(!(j==0)&&!(j==n-1))
            {
                sum=local_Matrix[i-1][j-1]+local_Matrix[i-1][j]+local_Matrix[i-1][j+1]+
                local_Matrix[i][j-1]+local_Matrix[i][j+1]+
                local_Matrix[i+1][j-1]+local_Matrix[i+1][j]+local_Matrix[i+1][j+1];
            }
            else if(j==0)
            {
                sum=local_Matrix[i-1][n-1]+local_Matrix[i-1][j]+local_Matrix[i-1][j+1]+
                local_Matrix[i][n-1]+local_Matrix[i][j+1]+
                local_Matrix[i+1][n-1]+local_Matrix[i+1][j]+local_Matrix[i+1][j+1];
            }
            else if(j==n-1)
            {
                sum=local_Matrix[i-1][j-1]+local_Matrix[i-1][j]+local_Matrix[i-1][0]+
                local_Matrix[i][j-1]+local_Matrix[i][0]+
                local_Matrix[i+1][j-1]+local_Matrix[i+1][j]+local_Matrix[i+1][0];
            }
        }
        else if((i==1)&&(j==0))
        {
            sum=NW+local_Matrix[i-1][j]+local_Matrix[i-1][j+1]+
            local_Matrix[i][n-1]+local_Matrix[i][j+1]+
            local_Matrix[i+1][n-1]+local_Matrix[i+1][j]+local_Matrix[i+1][j+1];
            
        }
        else if((i==1) && (j==n-1))
        {
            sum=local_Matrix[i-1][j-1]+local_Matrix[i-1][j]+NE+
            local_Matrix[i][j-1]+local_Matrix[i][0]+
            local_Matrix[i+1][j-1]+local_Matrix[i+1][j]+local_Matrix[i+1][0];
        }
    }
    
    if (rank==p-1)
    {
        if((!(i==n/p)&&!(j==0))||(!(i==n/p)&&!(j==n-1)))
        {
            if(!(j==0)&&!(j==n-1))
            {
                sum=local_Matrix[i-1][j-1]+local_Matrix[i-1][j]+local_Matrix[i-1][j+1]+
                local_Matrix[i][j-1]+local_Matrix[i][j+1]+
                local_Matrix[i+1][j-1]+local_Matrix[i+1][j]+local_Matrix[i+1][j+1];
            }
            else if(j==0)
            {
                sum=local_Matrix[i-1][n-1]+local_Matrix[i-1][j]+local_Matrix[i-1][j+1]+
                local_Matrix[i][n-1]+local_Matrix[i][j+1]+
                local_Matrix[i+1][n-1]+local_Matrix[i+1][j]+local_Matrix[i+1][j+1];
            }
            else if(j==n-1)
            {
                sum=local_Matrix[i-1][j-1]+local_Matrix[i-1][j]+local_Matrix[i-1][0]+
                local_Matrix[i][j-1]+local_Matrix[i][0]+
                local_Matrix[i+1][j-1]+local_Matrix[i+1][j]+local_Matrix[i+1][0];
            }
        }
        else if((i==n/p)&&(j==0))
        {
            sum=local_Matrix[i-1][n-1]+local_Matrix[i-1][j]+local_Matrix[i-1][j+1]+
            +local_Matrix[i][n-1]+local_Matrix[i][j+1]+
            SW+local_Matrix[i+1][j]+local_Matrix[i+1][j+1];
        }
        else if((i==n/p)&&(j==n-1))
        {
            sum=local_Matrix[i-1][j-1]+local_Matrix[i-1][j]+local_Matrix[i-1][0]+
            local_Matrix[i][j-1]+local_Matrix[i][0]+
            local_Matrix[i-1][j-1]+local_Matrix[i-1][j]+SE;
        }
    }
    
    //returning the state of cell
    if(((sum<3||sum>5)&&(local_Matrix[i][j]=0)) || ((local_Matrix[i][j]=1)&&(sum<3||sum>5)))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


void Filling_Local_Matrix(int **local_Matrix)
{
    //fill local_Matrix top row and lower row for further generation
    if(rank==p-1)
    {
        int i;
        int *b=(int*)malloc(sizeof(int)*n);
        gettimeofday(&start, NULL);
        for(i=0;i<n;i++)
        {
            b[i]=local_Matrix[n/p][i];
        }
        gettimeofday(&end, NULL);
        compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
        gettimeofday(&t1, NULL);
        MPI_Send(b,n,MPI_INT,0,0, MPI_COMM_WORLD);
        gettimeofday(&t2, NULL);
        tComm=tComm+ ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec));
        free(b);
        
        int *c=(int*)malloc(sizeof(int)*n);
        gettimeofday(&start, NULL);
        for(i=0;i<n;i++)
        {
            c[i]=local_Matrix[1][i];
        }
        gettimeofday(&end, NULL);
        compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
        gettimeofday(&t1, NULL);
        MPI_Send(c,n,MPI_INT,p-2,0, MPI_COMM_WORLD);
        gettimeofday(&t2, NULL);
        tComm=tComm+ ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec));
        free(c);
    }
    
    if(rank==0)
    {
        int i;
        int *b=(int*)malloc(sizeof(int)*n);
        gettimeofday(&start, NULL);
        for(i=0;i<n;i++)
        {
            b[i]=local_Matrix[n/p][i];
        }
        gettimeofday(&end, NULL);
        compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
        gettimeofday(&t1, NULL);
        MPI_Send(b,n,MPI_INT,1,0, MPI_COMM_WORLD);
        gettimeofday(&t2, NULL);
        tComm=tComm+ ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec));
        free(b);
        
        int *c=(int*)malloc(sizeof(int)*n);
        gettimeofday(&start, NULL);
        for(i=0;i<n;i++)
        {
            c[i]=local_Matrix[1][i];
        }
        gettimeofday(&end, NULL);
        compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
        gettimeofday(&t1, NULL);
        MPI_Send(c,n,MPI_INT,p-1,0, MPI_COMM_WORLD);
        gettimeofday(&t2, NULL);
        tComm=tComm+ ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec));
        free(c);
    }
    
    if(!(rank==0)&&!(rank==p-1))
    {
        
        int i;
        int *b=(int*)malloc(sizeof(int)*n);
        gettimeofday(&start, NULL);
        for(i=0;i<n;i++)
        {
            b[i]=local_Matrix[n/p][i];
        }
        gettimeofday(&end, NULL);
        compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
        gettimeofday(&t1, NULL);
        MPI_Send(b,n,MPI_INT,rank+1,0, MPI_COMM_WORLD);
        gettimeofday(&t2, NULL);
        tComm=tComm+ ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec));
        free(b);
        
        int *c=(int*)malloc(sizeof(int)*n);
        gettimeofday(&start, NULL);
        for(i=0;i<n;i++)
        {
            c[i]=local_Matrix[1][i];
        }
        gettimeofday(&end, NULL);
        compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
        gettimeofday(&t1, NULL);
        MPI_Send(c,n,MPI_INT,rank-1,0, MPI_COMM_WORLD);
        gettimeofday(&t2, NULL);
        tComm=tComm+ ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec));
        free(c);
    }
    
    if(rank==0)
    {
        int *a=(int*)malloc(sizeof(int)*n);
        gettimeofday(&t1, NULL);
        MPI_Recv(a,n,MPI_INT,p-1,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        gettimeofday(&t2, NULL);
        tComm=tComm+ ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec));
        
        int i;
        gettimeofday(&start, NULL);
        for(i=0;i<n;i++)
        {
            local_Matrix[0][i]=a[i];
        }
        gettimeofday(&end, NULL);
        compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
        free(a);
        
        int *b=(int*)malloc(sizeof(int)*n);
        gettimeofday(&t1, NULL);
        MPI_Recv(b,n,MPI_INT,1,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        gettimeofday(&t2, NULL);
        tComm=tComm+ ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec));
        gettimeofday(&start, NULL);
        for(i=0;i<n;i++)
        {
            local_Matrix[n/p+1][i]=b[i];
        }
        gettimeofday(&end, NULL);
        compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
        free(b);
        
        // assingning SE and SW
        SW=local_Matrix[1][n-1];
        SE=local_Matrix[1][0];
    }
    
    if(rank==p-1)
    {
        int *a=(int*)malloc(sizeof(int)*n);
        gettimeofday(&t1, NULL);
        MPI_Recv(a,n,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        gettimeofday(&t2, NULL);
        tComm=tComm+ ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec));
        int i;
        gettimeofday(&start, NULL);
        for(i=0;i<n;i++)
        {
            local_Matrix[n/p+1][i]=a[i];
        }
        gettimeofday(&end, NULL);
        compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
        free(a);
        
        int *b=(int*)malloc(sizeof(int)*n);
        gettimeofday(&t1, NULL);
        MPI_Recv(b,n,MPI_INT,p-2,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        gettimeofday(&t2, NULL);
        tComm=tComm+ ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec));
        gettimeofday(&start, NULL);
        for(i=0;i<n;i++)
        {
            local_Matrix[0][i]=b[i];
        }
        gettimeofday(&end, NULL);
        compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
        free(b);
        
        // assingning NE and NW
        NW=local_Matrix[n/p][n-1];
        NE=local_Matrix[n/p][0];
    }
    
    if(!(rank==0)&&!(rank==p-1))
    {
        int *a=(int*)malloc(sizeof(int)*n);
        MPI_Recv(a,n,MPI_INT,rank+1,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        int i;
        gettimeofday(&start, NULL);
        for(i=0;i<n;i++)
        {
            local_Matrix[n/p+1][i]=a[i];
        }
        gettimeofday(&end, NULL);
        compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
        free(a);
        
        int *b=(int*)malloc(sizeof(int)*n);
        MPI_Recv(b,n,MPI_INT,rank-1,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        gettimeofday(&start, NULL);
        for(i=0;i<n;i++)
        {
            local_Matrix[0][i]=b[i];
        }
        gettimeofday(&end, NULL);
        compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
        free(b);
    }
}

void GenerateInitialGoL(int **local_Matrix)
{
    
    int recieved_seed;                   //seed for each matrix to generate random values declaring it here so as we can use it again
    int l_row,l_col;
    int i;
    if (rank == 0)
    {
        int random_number_seed[p];       // as we can have maximum number of processor to be 64
        srand((unsigned int)time(0));
        for(i = 0; i < p ; i++)
        {
            gettimeofday(&start, NULL);
            random_number_seed[i] = (rand() % BIGPRIME);
            gettimeofday(&end, NULL);
            compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
            if(i==0)
            {
                recieved_seed=random_number_seed[i];
                continue;
            }
            int k=random_number_seed[i]; // the random number is k which is being send to each processor except 0th ranked
            gettimeofday(&t1, NULL);
            MPI_Send(&k,1,MPI_INT,i,0, MPI_COMM_WORLD);
            gettimeofday(&t2, NULL);
            tComm=tComm+ ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec));
        }
    }
    
    if(rank==0)
    {
        srand(recieved_seed);
        gettimeofday(&start, NULL);
        for(l_row=1;l_row<n/p+1;l_row++)
        {
            for(l_col=0;l_col<n;l_col++)
            {
                local_Matrix[l_row][l_col]=(rand()%2);      //%2 for filling the cell in condition of dead/alive
            }
        }
        gettimeofday(&end, NULL);
        compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    }
    if(rank!=0)
    {
        gettimeofday(&t1, NULL);
        MPI_Recv(&recieved_seed,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        gettimeofday(&t2, NULL);
        tComm=tComm+ ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec));
        srand(recieved_seed);
        gettimeofday(&start, NULL);
        for(l_row=1;l_row<n/p+1;l_row++)
        {
            for(l_col=0;l_col<n;l_col++)
            {
                local_Matrix[l_row][l_col]=(rand()%2);      //%2 for filling the cell in condition of dead/alive
            }
        }
        gettimeofday(&end, NULL);
        compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
    }
    
}


int main(int argc, char *argv[]) {
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&p);
    assert(p>=2);
    int G;
    n = atoi(argv[1]);      //User input - size of side of matrix
    G = atoi(argv[2]);      //User input - number of generations
    if(!(p==2||p==4||p==8||p==16||p==32||p==64))
    {
        printf("processors not in order of 2\n");
    }
    
    if((n%p==0) && (n>p))
    {
        //allocates the memory for local matrixes
        int** local_Matrix;         // this is local matrix which will be generated in each processor
        int h=(n/p)+2;                // for height of local matrix which will be 2 row extra with 1-top and 1-bottom for neighbours
        local_Matrix=(int*)malloc(sizeof *local_Matrix *h);
        if(local_Matrix)
        {
            int i=0;
            for(i=0;i<h;i++)
            {
                local_Matrix[i]=malloc(sizeof *local_Matrix[i]*n);
            }
        }
        GenerateInitialGoL(local_Matrix);
        int i,j,k;
        i=0;
//        DisplayGoL(local_Matrix,i);
        int dis=2; //display after how many iteration
        for(i=1;i<=G;i++)
        {
            Filling_Local_Matrix(local_Matrix);
            MPI_Barrier(MPI_COMM_WORLD);
            Simulate(local_Matrix);
            MPI_Barrier(MPI_COMM_WORLD);
            if(i%dis==0)
            {
//                DisplayGoL(local_Matrix,i);
            }
        }
        free(local_Matrix);
        if(rank==0)
        {
            if(n==128)
            {
                printf("for p==%d\n",p);
                printf("for n==128\n");
                printf("computation time =%f microsec\n",compT);
                printf("communication time =%f microsec\n",tComm);
                printf("total time =%f microsec\n",compT+tComm);
                printf("speed up %f\n",2769/(compT+tComm));
                printf("efficiency %f\n",(2769/(compT+tComm))/p);
            }
            else if(n==256)
            {
                printf("for p==%d\n",p);
                printf("for n==256\n");
                printf("computation time =%f microsec\n",compT);
                printf("communication time =%f microsec\n",tComm);
                printf("total time =%f microsec\n",compT+tComm);
                printf("speed up %f\n",10993/(compT+tComm));
                printf("efficiency %f\n",(10993/(compT+tComm))/p);
            }
            else if(n==512)
            {
                printf("for p==%d\n",p);
                printf("for n==512\n");
                printf("computation time =%f microsec\n",compT);
                printf("communication time =%f microsec\n",tComm);
                printf("total time =%f microsec\n",compT+tComm);
                printf("speed up %f\n",43809/(compT+tComm));
                printf("efficiency %f\n",(43809/(compT+tComm))/p);
            }
            else if(n==1024)
            {
                printf("for p==%d\n",p);
                printf("for n==1024\n");
                printf("computation time =%f microsec\n",compT);
                printf("communication time =%f microsec\n",tComm);
                printf("total time =%f microsec\n",compT+tComm);
                printf("speed up %f\n",175079/(compT+tComm));
                printf("efficiency %f\n",(175079/(compT+tComm))/p);
            }
            else if(n==2048)
            {
                printf("for p==%d\n",p);
                printf("for n==2048\n");
                printf("computation time =%f microsec\n",compT);
                printf("communication time =%f microsec\n",tComm);
                printf("total time =%f microsec\n",compT+tComm);
                printf("speed up %f\n",709266/(compT+tComm));
                printf("efficiency %f\n",(709266/(compT+tComm))/p);
            }
            else if(n==4096)
            {
                printf("for p==%d\n",p);
                printf("for n==4096\n");
                printf("computation time =%f microsec\n",compT);
                printf("communication time =%f microsec\n",tComm);
                printf("total time =%f microsec\n",compT+tComm);
                printf("speed up %f\n",2855154/(compT+tComm));
                printf("efficiency %f\n",(2855154/(compT+tComm))/p);
            }
            else if(n==8192)
            {
                printf("for p==%d\n",p);
                printf("for n==8192\n");
                printf("computation time =%f microsec\n",compT);
                printf("communication time =%f microsec\n",tComm);
                printf("total time =%f microsec\n",compT+tComm);
                printf("speed up %f\n",11430512/(compT+tComm));
                printf("efficiency %f\n",(11430512/(compT+tComm))/p);
            }
        }
    }
    else
    {
        printf("the size you have entered is either not multiple of p or less than p\n");
    }
    MPI_Finalize();
}



