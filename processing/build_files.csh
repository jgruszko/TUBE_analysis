#!/bin/csh
#Set runStart and runEnd and run: python -c 'for i in range(runStart, runEnd+1): print i' | xargs -l qsub build_files.csh

# Start from current working directory
#$ -cwd
#
# Join stdout and stderr
#$ -j y
#
# and put them somewhere
#$ -o ./Logs
#
# Specify mj just to be sure
#$ -P majorana
#
# Resources: use projectio=1 as requested by Lisa Gerhart, even if there
# will be a large number of jobs accessing /project
#$ -l projectio=1 

source ~/.cshrc
cd $TUBEDIR/data/built

~/software/MJOR/majorcaroot --sis /projecta/projectdirs/majorana/data-projecta/TUBE/data/raw/Run$1 >& $TUBEDIR/analysis/processing/Logs/build_$1.log
