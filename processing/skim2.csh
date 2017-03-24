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

$GATDIR/Apps/skim_tube_data 90 0 >& $TUBEDIR/analysis/processing/Logs/skim_90_0.log
$GATDIR/Apps/skim_tube_data 80 0 >& $TUBEDIR/analysis/processing/Logs/skim_80_0.log
$GATDIR/Apps/skim_tube_data 80 1 >& $TUBEDIR/analysis/processing/Logs/skim_80_1.log
$GATDIR/Apps/skim_tube_data 70 0 >& $TUBEDIR/analysis/processing/Logs/skim_70_0.log
$GATDIR/Apps/skim_tube_data 70 1 >& $TUBEDIR/analysis/processing/Logs/skim_70_1.log
