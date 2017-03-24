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

$GATDIR/Apps/skim_tube_data 100 0 >& $TUBEDIR/analysis/processing/Logs/skim_100_0.log
$GATDIR/Apps/skim_tube_data 110 0 >& $TUBEDIR/analysis/processing/Logs/skim_110_0.log
$GATDIR/Apps/skim_tube_data 120 0 >& $TUBEDIR/analysis/processing/Logs/skim_120_0.log
$GATDIR/Apps/skim_tube_data 130 0 >& $TUBEDIR/analysis/processing/Logs/skim_130_0.log


