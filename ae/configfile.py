#!/usr/bin/python
import re,os,sys
#This file contains the useful functions for interacting with the config files

class ConfigFile:
       
    def __init__(self, FileName="config.dat"):
        self.FileName=FileName
        self.Parameters={}
        self.infile=open(self.FileName,"r")
        self.LoadConfigFile()


    def GetParameters(self, name):
        if name in self.Parameters:
            return  self.Parameters[name]
        else:
            return False

    def GetParameter(self, name, i):
        if name in self.Parameters:
            return  self.Parameters[name][i]
        else:
            return False

    def GetnParameter(self):
        return len(self.Parameters)

    def GetnVals(self, name):
        if name in self.Parameters:
            return  len(self.Parameters[name])
        else:
            return False

    def LoadConfigFile(self):
        print "Loading "+self.FileName
        while 1:
            line = self.infile.readline()
            if not line:
                break
            if ("#" in line) or (line in ('\n', '\r\n')):
                continue
            else:
                line=line.rstrip()
                line=re.sub(r'\s+','',line)
                lineparts=re.split(':',line);
                parName= lineparts[0]         
                self.Parameters[parName]=re.split(',',lineparts[1]) 
                print "Loaded",parName+" -->",self.Parameters[parName]

    def Configure(self):
        print"\n\n Starting Configuration \n\n"
        self.infile.seek(0)
        while 1:
            line = self.infile.readline()
            if not line:
                print "Finally done!"
                break
            if ("#" in line) or (line in ('\n', '\r\n')):
                pass
            else:
                line=line.rstrip()
                line=re.sub(r'\s+','',line)
                lineparts=re.split(':',line);
                parName= lineparts[0]
                if(self.GetnVals(parName)<1):
                    print "Error, missing parameters from original Config"
                    sys.exit(0)  
                else:
                    parVals=raw_input("Enter values (comma separated) for "+parName+", default = "+str(self.Parameters[parName])+" : ")
                    if not parVals:
                        pass
                    else:
                        self.Parameters[parName]=[]
                        parVals.strip();
                        parVals=re.sub(r'\s+','',parVals)
                        self.Parameters[parName]=re.split(',',parVals) 
                        
                                      
                     

    def SaveConfigFile(self,FileName):
        if(os.path.isfile(FileName)):
            overwrite=raw_input("File "+FileName+" already exists, overwrite (y/n)? default=n :")
            if(not overwrite or overwrite =="n"):
                FileName=""
                while(not FileName or os.path.isfile(FileName)):
                    FileName=raw_input("Enter a new filename: ")

        self.FileName=FileName
        outfile = open(self.FileName,"w")
        #copy all the contents of the input file (comments) except for the paramers
        self.infile.seek(0)
        while 1:     
            line = self.infile.readline()
            if not line:
                break
            if ("#" in line): 
                outfile.write(line)
            elif (line in ('\n', '\r\n')):
                outfile.write("\n")               
            else:
                line=line.rstrip()
                line=re.sub(r'\s+','',line)
                lineparts=re.split(':',line);
                parName= lineparts[0]
                if(self.GetnVals(parName)<1):
                    print "Error, missing parameters from original Config"
                    sys.exit(0)
                else:
                    newline = parName+" : "
                    count=0
                    
                    for par in self.Parameters[parName]:
                        newline=newline+par;
                        count=count+1
                        if count<len(self.Parameters[parName]):
                            newline=newline+","
                    outfile.write(newline+"\n")
        self.infile.close()
                    
