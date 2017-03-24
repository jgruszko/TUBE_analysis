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

$GATDIR/Apps/skim_tube_data 140 0 >& $TUBEDIR/analysis/processing/Logs/skim_140_0.log
$GATDIR/Apps/skim_tube_data 140 1 >& $TUBEDIR/analysis/processing/Logs/skim_140_1.log
$GATDIR/Apps/skim_tube_data 150 0 >& $TUBEDIR/analysis/processing/Logs/skim_150_0.log
$GATDIR/Apps/skim_tube_data 150 1 >& $TUBEDIR/analysis/processing/Logs/skim_150_1.log
$GATDIR/Apps/skim_tube_data 160 0 >& $TUBEDIR/analysis/processing/Logs/skim_160_0.log
$GATDIR/Apps/skim_tube_data 160 1 >& $TUBEDIR/analysis/processing/Logs/skim_160_1.log


