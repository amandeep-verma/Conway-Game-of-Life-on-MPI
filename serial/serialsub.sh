#!/bin/sh

#To run this script, please use "sbatch -N<numberofnodes> <path>/sub.sh"

# setting the max wallclock time after which the job will be killed; 
# note: the format is hr:min:sec (default set here to 10 mins)
#SBATCH --time=00:10:00


#MPI helloworld example - this line is a comment
#"np" stands for number of processes.
#this command will run the job on 8 processes.
mpirun -np 1 ~/gameOfLife/Serialgol/gol 128 5
mpirun -np 1 ~/gameOfLife/Serialgol/gol 256 5
mpirun -np 1 ~/gameOfLife/Serialgol/gol 512 5
mpirun -np 1 ~/gameOfLife/Serialgol/gol 1024 5
mpirun -np 1 ~/gameOfLife/Serialgol/gol 2048 5
mpirun -np 1 ~/gameOfLife/Serialgol/gol 4096 5
mpirun -np 1 ~/gameOfLife/Serialgol/gol 8192 5
