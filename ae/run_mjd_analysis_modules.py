#!/usr/bin/python

#R.Martin Feb 2013

#TODO
#Add the part that makes coffee for Khang

#This runs the analysis modules defined in analysismodule.py
#Which modules to run is specified by the config file (passed as argument)
#If no config file is passed, the user can interactively create a new
#config file, based on defaultconfigfile (below)



import sys, re, os, subprocess, optparse, datetime, shutil
from configfile import *
from analysismodule import *

def GetDictOfRootFiles(dir):
    rootFileByRun={}
    ListOfRootFiles = os.listdir(dir)
    for InFile in (ListOfRootFiles):
        if not InFile.endswith(".root"):
            continue
        fileparts = re.split("run",InFile)
        infile = fileparts[0]+"run"
        fileparts2 = re.split("\.",fileparts[1]);
        run=fileparts2[0]; 
        rootFileByRun[run]=InFile  
    return rootFileByRun

def CreateDir(dirname):
    if(not os.path.exists(dirname)):
        print "Creating output directory "+dirname
        os.mkdir(dirname)

def DeleteFile(filename):
    if(filename.find("config.dat")>-1): # don't want to delete this one
        return
    if(os.path.isfile(filename)):
        print "deleting "+filename
        os.unlink(filename)

def CleanDir(dirname):
    if(os.path.exists(dirname)):
        for dfile in  os.listdir(dirname):
            file_path = os.path.join(dirname, dfile)
            DeleteFile(file_path)
         
#Directory with the mjd executables
#exedir = os.environ['GATDIR']+"/MJDApps/"
exedir = "/global/projecta/projectdirs/majorana/data-projecta/TUBE/analysis/ae/"
                    
                    
#Default options 
configfile ="config.dat"
defaultconfigfile=exedir+"default_config.dat" #change this to your own default starting config file, if you want...
mjddir=""
datatype=""
detector = "";
analysisdir=""
outputdir=""
mjdoutputdir="";

LogFileName="AnalysisLog.dat" #change if you want to rename the log file

parser = optparse.OptionParser(description='Run the mjd_* analysis executables on a data set')

parser.add_option("-c","--configfile",action="store",help="config file to use with full path (if not specified, you have the option to interactively create a new one",default=configfile);
parser.add_option("--clean",action="store_true",help="cleans ALL the output directories. Be careful using this, as some of the output is required to proceed!", dest='clean',default=False)
parser.add_option("--test-commands",action="store_true",help="Prints the commands to the screen instead of running them, for debugging", dest='test_commands',default=False)
parser.add_option("--configure",action="store_true",help="Interactively change the configuration file. Can create or edit a config file (will use the default one otherwise)",dest='configure',default=False)
(opts, args) =parser.parse_args()

configfile=opts.configfile
test_commands=opts.test_commands
clean=opts.clean
configure=opts.configure


#Check if the user specified a config file that exists
if(not os.path.isfile(configfile) and not configure):  
    answer = raw_input("Either you didn't specify an existing config file, or the one you specified does not exist. Proceed with guided configuration using the default configuration (y/n)? (default=n):").strip()
    if(not answer or answer=="n"):
        configure=False
    else:
        configure=True
        configfile=defaultconfigfile
    
    if (not configure):
        sys.exit(1)

#Check if the user wants to configure the config file, configure if yes!
#Likely, this means that the output directories do not exist yet...
if(configure):
    CFile=ConfigFile(configfile)
    CFile.Configure()


    #the default was used, so create a new one in the output directory
    mjddir=CFile.GetParameter("mjd_dir",0)
    if(not os.path.exists(mjddir)):
        print "The top level directory, "+mjddir+" does not seem to exist, exiting!"
        sys.exit(0)
    if(not mjddir.endswith("/")):
        mjddir=mjddir+"/"
    datatype=CFile.GetParameter("datatype",0)
    datatypedir=mjddir+datatype+"/"
    if(not os.path.exists(datatypedir)):
        print "The datatype directory, "+datatypedir+" does not seem to exist, exiting!"
        sys.exit(0)  
    detector=CFile.GetParameter("detector",0)

    mjdoutputdir=CFile.GetParameter("mjd_output_dir",0)
    if mjdoutputdir:
        if(not mjddir.endswith("/")):
            mjdoutputdir=mjdoutputdir+"/"
        CreateDir(mjdoutputdir)
        CreateDir(mjdoutputdir+datatype+"/")
        analysisdir=mjdoutputdir+datatype+"/analysis/"
    else:
        analysisdir=datatypedir+"analysis/"

    CreateDir(analysisdir)
    outputdir=analysisdir+detector+"/"
    CreateDir(outputdir)
    defaultconfigout=outputdir+"config.dat"
    
    outfilename=raw_input("Save new configuration file as? default="+defaultconfigout+":")
    if(not outfilename):
        outfilename=defaultconfigout

    CFile.SaveConfigFile(outfilename)
    configfile = CFile.FileName
 

#Load the configuration file

CFile=ConfigFile(configfile)

modules_to_run=CFile.GetParameters("modules")


#Setup the top level directories
mjddir=CFile.GetParameter("mjd_dir",0)
if(not os.path.exists(mjddir)):
    print "The top level directory, "+mjddir+" does not seem to exist, exiting!"
    sys.exit(0)
if(not mjddir.endswith("/")):
    mjddir=mjddir+"/"


datatype=CFile.GetParameter("datatype",0)
datatypedir=mjddir+datatype+"/"
if(not os.path.exists(datatypedir)):
    print "The datatype directory, "+datatypedir+" does not seem to exist, exiting!"
    sys.exit(0)  

detector=CFile.GetParameter("detector",0)

gatdir=CFile.GetParameter("gat_dir",0)
if not gatdir:
    gatdir=datatypedir+"data/gatified/"
if(not gatdir.endswith("/")):
    gatdir=gatdir+"/"

mcadir=CFile.GetParameter("mca_dir",0)
if not mcadir:
    mcadir=datatypedir+"data/mcaroot/"
if(not mcadir.endswith("/")):
    mcadir=mcadir+"/"

mjdoutputdir=CFile.GetParameter("mjd_output_dir",0)
if mjdoutputdir:
    if(not mjddir.endswith("/")):
        mjdoutputdir=mjdoutputdir+"/"
    CreateDir(mjdoutputdir)
    CreateDir(mjdoutputdir+datatype+"/")
    analysisdir=mjdoutputdir+datatype+"/analysis/"
else:
    analysisdir=datatypedir+"analysis/"

CreateDir(analysisdir)
outputdir=analysisdir+detector+"/"
CreateDir(outputdir)

logfile = open(outputdir+LogFileName,'a')
logfile.write("Analysis Started At: "+datetime.datetime.now().strftime("%Y-%m-%d-%H:%M:%S")+"\n")
logfile.flush()

################################################# End the config stuff#########################3




print "\n\nStarting the analysis"
print "Input dirs:"
print "   gat_dir:",gatdir
print "   mca_dir:",mcadir
print "Output dirs:"
print "   outputdir:",outputdir
print "***************************************\n\n"

#first the run by run modules:
startrun=int(CFile.GetParameter("startrun",0))
endrun=int(CFile.GetParameter("endrun",0))

for name in modules_to_run:
    module=Modules[name]
    if(not module.ByRun):
        continue
    if not module.InputsExist(outputdir):
        continue
    #Create output directories for the module
    CreateDir(outputdir+module.Name)
    if clean and not test_commands:
        CleanDir(outputdir+module.Name)
    for directory in module.OutputDirs:
        CreateDir(outputdir+directory)
        if clean and not test_commands:
            CleanDir(outputdir+directory)
    #Delete the files that it creates
    for outfile in module.OutputFiles:
        if(not test_commands):
            DeleteFile(outputdir+outfile)


    #Directory with the root files (loops through it to figure out all the available runs)
    inputdir=datatypedir+"data/"+str(module.InputDataDir)
    if(not inputdir.endswith("/")):
        inputdir=inputdir+"/"
    if(not os.path.exists(inputdir)):
        print "The directory, "+inputdir+" does not seem to exist, exiting!"
        sys.exit(0) 
    inputdir=inputdir+detector+"/"

    runFiles=GetDictOfRootFiles(inputdir)
    runs = runFiles.keys()
    runs.sort()   

    for run in runs:
        if(startrun!=-1 and int(run)<startrun):
            print "skipping run "+run+" (run number below range)"
            continue
        if(endrun!=-1 and int(run)>endrun):
            print "skipping run "+run+" (run number above range)"
            continue

        command = exedir+module.Name+" --startrun "+run+" --configfile "+configfile
        print "Running module (by run) :",module.Name,"on run",run
        if test_commands:
            print command
        else:
            proc = subprocess.Popen(command, shell=True, stdout=logfile, stderr=logfile)
            return_code = proc.wait()
            logfile.flush()
            


#Now run over all the other modules
for name in modules_to_run:
    module=Modules[name]
    if(module.ByRun):
        continue
    if not module.InputsExist(outputdir):
        continue   

    #Create output directories for the module
    CreateDir(outputdir+module.Name)
    if clean and not test_commands:
        CleanDir(outputdir+module.Name)
    for directory in module.OutputDirs:
        CreateDir(outputdir+directory)
        if clean and not test_commands:
            CleanDir(outputdir+directory)

    #Delete the files that it creates
    for outfile in module.OutputFiles:
        if not test_commands:
            DeleteFile(outputdir+outfile)

    print "Running module :",module.Name
    command = exedir+module.Name+" --configfile "+configfile
    
    if test_commands:
        print command
    else:
        proc = subprocess.Popen(command, shell=True, stdout=logfile, stderr=logfile)
        return_code = proc.wait()
        logfile.flush()

## The end    
logfile.write("Analysis Ended At: "+datetime.datetime.now().strftime("%Y-%m-%d-%H:%M:%S")+"\n")
logfile.flush()
logfile.close()
