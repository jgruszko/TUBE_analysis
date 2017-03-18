#!/bin/csh
#Set runStart and runEnd and run: python -c 'for i in range(runStart, runEnd+1): print i' | xargs -l qsub gatify_files.csh

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
cd $TUBEDIR/data/gatified

$GATDIR/Apps/process_tube_data $TUBEDIR/data/built/OR_run$1.root $TUBEDIR/analysis/energyCal/calibrationMap.txt >& $TUBEDIR/analysis/processing/Logs/gatify_$1.log
