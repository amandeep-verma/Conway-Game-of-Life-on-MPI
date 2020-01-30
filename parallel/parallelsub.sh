#!/bin/sh

#To run this script, please use "sbatch -N<numberofnodes> <path>/sub.sh"

# setting the max wallclock time after which the job will be killed;
# note: the format is hr:min:sec (default set here to 10 mins)
#SBATCH --time=00:10:00


#MPI helloworld example - this line is a comment
#"np" stands for number of processes.
#this command will run the job on 8 processes.
STR="for 128"
echo $STR
mpirun -np 2 ~/gameOfLife/gol 128 5
mpirun -np 4 ~/gameOfLife/gol 128 5
mpirun -np 8 ~/gameOfLife/gol 128 5
mpirun -np 16 ~/gameOfLife/gol 128 5
mpirun -np 32 ~/gameOfLife/gol 128 5
mpirun -np 64 ~/gameOfLife/gol 128 5


STR="for 256"
echo $STR
mpirun -np 2 ~/gameOfLife/gol 256 5
mpirun -np 4 ~/gameOfLife/gol 256 5
mpirun -np 8 ~/gameOfLife/gol 256 5
mpirun -np 16 ~/gameOfLife/gol 256 5
mpirun -np 32 ~/gameOfLife/gol 256 5
mpirun -np 64 ~/gameOfLife/gol 256 5

STR="for 512"
echo $STR
mpirun -np 2 ~/gameOfLife/gol 512 5
mpirun -np 4 ~/gameOfLife/gol 512 5
mpirun -np 8 ~/gameOfLife/gol 512 5
mpirun -np 16 ~/gameOfLife/gol 512 5
mpirun -np 32 ~/gameOfLife/gol 512 5
mpirun -np 64 ~/gameOfLife/gol 512 5

STR="for 1024"
echo $STR
mpirun -np 2 ~/gameOfLife/gol 1024 5
mpirun -np 4 ~/gameOfLife/gol 1024 5
mpirun -np 8 ~/gameOfLife/gol 1024 5
mpirun -np 16 ~/gameOfLife/gol 1024 5
mpirun -np 32 ~/gameOfLife/gol 1024 5
mpirun -np 64 ~/gameOfLife/gol 1024 5

STR="for 2048"
echo $STR
mpirun -np 2 ~/gameOfLife/gol 2048 5
mpirun -np 4 ~/gameOfLife/gol 2048 5
mpirun -np 8 ~/gameOfLife/gol 2048 5
mpirun -np 16 ~/gameOfLife/gol 2048 5
mpirun -np 32 ~/gameOfLife/gol 2048 5
mpirun -np 64 ~/gameOfLife/gol 2048 5


STR="for 4096"
echo $STR
mpirun -np 2 ~/gameOfLife/gol 4096 5
mpirun -np 4 ~/gameOfLife/gol 4096 5
mpirun -np 8 ~/gameOfLife/gol 4096 5
mpirun -np 16 ~/gameOfLife/gol 4096 5
mpirun -np 32 ~/gameOfLife/gol 4096 5
mpirun -np 64 ~/gameOfLife/gol 4096 5

STR="for 8192"
echo $STR
mpirun -np 2 ~/gameOfLife/gol 8192 5
mpirun -np 4 ~/gameOfLife/gol 8192 5
mpirun -np 8 ~/gameOfLife/gol 8192 5
mpirun -np 16 ~/gameOfLife/gol 8192 5
mpirun -np 32 ~/gameOfLife/gol 8192 5
mpirun -np 64 ~/gameOfLife/gol 8192 5

