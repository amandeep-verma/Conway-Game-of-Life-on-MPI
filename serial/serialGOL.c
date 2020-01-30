#include <stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include <sys/time.h>
#include <assert.h>
int DetermineState(int i,int j,int **local_Matrix);

int n; //side of main matrix
double compT;
void DisplayGoL(int **Matrix)
{
    int i,j;
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            printf("%d",Matrix[i][j]);
        }
        printf("\n");
    }
}

void Simulate(int **Matrix)
{
    int i,j;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            int state=DetermineState(i,j,Matrix);
            Matrix[i][j]=state;
        }
    }
    gettimeofday(&end, NULL);
    compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)
                  - (start.tv_sec * 1000000 + start.tv_usec));
}

int DetermineState(int i,int j,int **Matrix)
{
    int sum=0; //holds value of number of alive neighbours for each element
    if((!(i==0)&&!(j==0))&&(!(i==n-1)&&!(j==n-1)))
    {
        sum= Matrix[i-1][j-1] + Matrix[i-1][j] + Matrix[i-1][j+1] +
        Matrix[i][j-1]+Matrix[i][j+1] +
        Matrix[i+1][j-1]+ Matrix[i+1][j] + Matrix[i+1][j+1] ;
    }
    else if(i==0)
    {
        if(j==0)
        {
            sum= Matrix[n-1][n-1] + Matrix[n-1][0] + Matrix[n-1][1] +
            Matrix[i][n-1]+Matrix[i][j+1] +
            Matrix[i+1][n-1]+ Matrix[i+1][j] + Matrix[i+1][j+1] ;
        }
        else if(j==n-1)
        {
            sum= Matrix[n-1][j-1] + Matrix[n-1][j] + Matrix[n-1][0] +
            Matrix[i][j-1]+Matrix[i][0] +
            Matrix[i+1][j-1]+ Matrix[i+1][j] + Matrix[i+1][0] ;
        }
        else
        {
            sum= Matrix[n-1][j-1] + Matrix[n-1][j] + Matrix[n-1][j+1] +
            Matrix[i][j-1]+Matrix[i][j+1] +
            Matrix[i+1][j-1]+ Matrix[i+1][j] + Matrix[i+1][j+1] ;
        }
    }
    else if(i==n-1)
    {
        if(j==0)
        {
            sum= Matrix[i-1][n-1] + Matrix[i-1][j] + Matrix[i-1][j+1] +
            Matrix[i][n-1]+Matrix[i][j+1] +
            Matrix[0][n-1]+ Matrix[0][j] + Matrix[0][j+1] ;
        }
        else if(j==n-1)
        {
            sum= Matrix[i-1][j-1] + Matrix[i-1][j] + Matrix[i-1][0] +
            Matrix[i][j-1]+Matrix[i][0] +
            Matrix[0][j-1]+ Matrix[0][j] + Matrix[0][0] ;
        }
        else
        {
            sum= Matrix[i-1][j-1] + Matrix[i-1][j] + Matrix[i-1][j+1] +
            Matrix[i][j-1]+Matrix[i][j+1] +
            Matrix[0][j-1] + Matrix[0][j] + Matrix[0][j+1];
        }
    }
    else if(j==0)
    {
        sum= Matrix[i-1][n-1] + Matrix[i-1][j] + Matrix[i-1][j+1] +
        Matrix[i][n-1]+Matrix[i][j+1] +
        Matrix[i+1][n-1] + Matrix[i+1][j] + Matrix[i+1][j+1];
    }
    else if(j==n-1)
    {
        sum= Matrix[i-1][j-1] + Matrix[i-1][j] + Matrix[i-1][0] +
        Matrix[i][j-1]+Matrix[i][0] +
        Matrix[i+1][j-1]+ Matrix[i+1][j] + Matrix[i+1][0] ;
    }
    
    if(((sum<3||sum>5)&&(Matrix[i][j]=0)) || ((Matrix[i][j]=1)&&(sum<3||sum>5)))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


void GenerateInitialGoL(int **Matrix)
{
    srand((unsigned int)time(0));
    int i,j;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            Matrix[i][j]=(rand()%2);
        }
    }
    gettimeofday(&end, NULL);
    compT=compT+ ((end.tv_sec * 1000000 + end.tv_usec)
                  - (start.tv_sec * 1000000 + start.tv_usec));
}

int main(int argc, char *argv[])
{
    int G; // number of generations
    n = atoi(argv[1]);      //User input - size of side of matrix
    G = atoi(argv[2]);      //User input - number of generations
    int** Matrix;
    Matrix=(int*)malloc(sizeof *Matrix *n);
    if(Matrix)
    {
        int i=0;
        for(i=0;i<n;i++)
        {
            Matrix[i]=malloc(sizeof *Matrix[i]*n);
        }
    }
    GenerateInitialGoL(Matrix);
  //  printf("\n\n1st initial generation is \n\n");
 //   DisplayGoL(Matrix);
    int i;
    int a=2; //after how many generation display is required
    for(i=1;i<G;i++)
    {
        Simulate(Matrix);
        if(i%a==0)
        {
 //           printf("\n\n%d generation is \n\n",i+1);
 //           DisplayGoL(Matrix);
        }
    }
    printf("comp time =%f microsec\n",compT);
    return 0;
    
}

