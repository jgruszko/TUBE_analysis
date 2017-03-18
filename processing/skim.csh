#!/bin/csh

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
cd $TUBEDIR/data/skim

$GATDIR/Apps/skim_tube_data 170 0 >& $TUBEDIR/analysis/processing/Logs/skim_170_0.log
