#include "GATMultiPeakFitter.hh"
#include "GATCalibrationMapAscii.hh"
#include "GATCalibrateSpectrum.hh"
#include "GATConfigFile.hh"

#include <TFile.h>
#include <TLegend.h>

#include <TTree.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TStyle.h>

#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <getopt.h>

using namespace std;

//R. Martin Feb 2013
//C. Cuesta Nov 2016 (on-going)

int main(int argc, char** argv)
{
  /*Chains all the data together to look at PSA performance.
    It looks at the different current calculations (specified in the code below)
  */
  
  //******************************************** Configure ********************************************//
   
  string configFileName="config.dat";

  int opt = 0;
  int option_index = 0;
  static struct option long_options[] =
    {
      {"configfile",    required_argument, 0, 'c'},  
      {0, 0, 0, 0}
    };

  opt = getopt_long (argc, argv, "c:", long_options, &option_index);
  while( opt != -1 ) 
    {
      switch( opt ) 
        {
	case 'c':
	  configFileName = optarg;
	  break;
	case '?':
	  cout<<" usage: mjd_psa_analysis -c configfile \n\n"<<endl;
	  return 1; 
	default :
	  cerr <<"Error: unrecognized option"<<endl;
	  return 1;
	}  
      opt = getopt_long (argc, argv, "c:", long_options, &option_index);
    }                  

  GATConfigFile config(configFileName);
  if(!config.FileExists(configFileName)){
    cout<<"Error: config file :"<<configFileName.c_str()<<" does not exist"<<endl;
  }
  cout<<"config file: "<<configFileName.c_str()<<endl;

  string detector = config.GetParameter("detector",0);
  string infilename= config.GetParameter("gatfilename",0);
  string treename= config.GetParameter("gattreename",0);
  string mjddir=config.GetParameter("mjd_dir",0);
  if(!(mjddir[mjddir.size()-1]=='/'))mjddir+="/";	
  string datatype=config.GetParameter("datatype",0);

  string gatdir = mjddir+datatype+"/data/gatified/";
  string outputdir ="";
  if(config.ParExists("mjd_output_dir")){
    string mjdoutputdir = config.GetParameter("mjd_output_dir",0);
    if(!(mjdoutputdir[mjdoutputdir.size()-1]=='/'))mjdoutputdir+="/";	
    outputdir=mjdoutputdir+datatype+"/analysis/"+detector+"/";
  }
  else{
    outputdir=mjddir+datatype+"/analysis/"+detector+"/";
  }
  

  vector<string> currentName;
  if(!config.LoadParameterIntoStringVector("mjd_psa_analysis_CurrentNames",currentName)){cout<<"Error Parameter mjd_psa_analysis_CurrentNames not in config file"<<endl; return(1);};
  size_t nCurrent = currentName.size();
    
    size_t startrun;
    size_t endrun;
    if(!config.LoadParameterIntoSizeT("startrun",startrun)){cout<<"Error Parameter startrun not in config file"<<endl; return(1);};
    if(!config.LoadParameterIntoSizeT("endrun",endrun)){cout<<"Error Parameter endrun not in config file"<<endl; return(1);};
  
    printf("Start run is %lu\n", startrun);
    printf("End run is %lu\n", endrun);

  //Load any bad runs
  vector<size_t> badRuns;
  config.LoadParameterIntoSizeTVector("badRuns",badRuns);
    
    //This can also handle more than 1 run
    TChain *t = new TChain(treename.c_str());
    vector<size_t> run;
    for(size_t i=startrun;i<=endrun;i++){
        char number[1000];
        sprintf(number,"%zu",i);
        //string fullfilename = gatdir+detector+"/"+infilename+number+".root";
        string fullfilename = "/projecta/projectdirs/majorana/data-projecta/TUBE/data/gatified/"+infilename+number+".root";
        if(!config.FileExists(fullfilename))continue;
        if(find(badRuns.begin(),badRuns.end(),i)!=badRuns.end()){
            cout<<"Not Loading "<<fullfilename <<" bad run "<<endl;
            continue;
        }
        t->AddFile(fullfilename.c_str());
        cout<<"Now Loading "<<fullfilename.c_str()<<endl;
        run.push_back(i);
    }
    size_t nRuns = run.size();
    printf("Number of runs is %lu\n", nRuns);
    sort(run.begin(),run.begin()+nRuns);
   
    //set all the branches inactive unless we're going to be using them
    TObjArray* branchList = t->GetListOfBranches();
    size_t nBranch = t->GetNbranches();
    TBranch* branch;
    
    bool setBranchInactive;
    for(size_t iBranch=0;iBranch<nBranch;iBranch++){
        setBranchInactive = true;
        branch = static_cast<TBranch *>( branchList->At(iBranch));
        if (!strcmp(branch->GetName(), "trapEMPZCal")){setBranchInactive = false;}
        if (!strcmp(branch->GetName(), "trapEMPZ")){setBranchInactive = false;}
        if (!strcmp(branch->GetName(), "run")){setBranchInactive = false;}
        if (!strcmp(branch->GetName(), "channel")){continue;}
        //also, the current estimators
        for (size_t jCurrent=0; jCurrent<nCurrent;jCurrent++){
            if (!strcmp(branch->GetName(),currentName[jCurrent].c_str())){
                setBranchInactive = false;
                continue;
            }
        }
        if (setBranchInactive){
            branch->SetStatus(0);
        }
    }

    //Find what channels are in the data
    vector<size_t> channel;
    string chanName = "channel";//name of the channel branch in the tree
    if(!config.LoadParameterIntoSizeTVector("Channels",channel)){
        size_t maxChans = (size_t)1e6;//the highest channel number in the data
        char draw[500];
        sprintf(draw,"%s >> tempChanH(%zu,0,%zu)",chanName.c_str(),maxChans,maxChans);
        t->Draw(draw);
        TH1* h = (TH1*)gDirectory->Get("tempChanH");
        for(int i=1;i<h->GetNbinsX();i++){
            if(h->GetBinContent(i)!=0)channel.push_back((size_t)h->GetBinLowEdge(i));
        }
    }
    size_t nChannel = channel.size();
    printf("Number of channels is %lu\n", nChannel);
    cout<<"Available channels:"<<endl;
    for(size_t jChan=0;jChan<nChannel;jChan++){
        char cut[500];
        char draw[500];
        size_t maxChans = (size_t)1e6;//the highest channel number in the data
        sprintf(draw,"channel >> tempChanH(%zu,0,%zu)",maxChans,maxChans);
        sprintf(cut, "channel == %lu",channel[jChan]);
        t->Draw(draw, cut);
        TH1* h = (TH1*)gDirectory->Get("tempChanH");
        printf("%lu has %f entries\n",channel[jChan], h->GetEntries());
    }
 
  string calibrationfile= "/global/projecta/projectdirs/majorana/data-projecta/TUBE/analysis/energyCal/calibrationMap.txt";
  GATCalibrationMapAscii calibrationMapByRun;
  cout<<"Loading calibration map from "<<calibrationfile.c_str()<<endl;
  vector<string> energyName = calibrationMapByRun.GetEnergyNameVector();
  calibrationMapByRun.ReadCalibrationMapFromTextFile(calibrationfile); 

  size_t nEbinskeV=3000;//15000;// number of bins for the histos in keV
  double EminkeV=0;
  double EmaxkeV=3000;
  
  size_t nAbins=1000;//200 number of bins for the histos in keV
  double Amin=0;
  double Amax=20;//220;//0.03;
  double CAmin=-15;
  double CAmax=5;//15;
  double avsemin=-20;
  double avsemax=5;
  
  TH2F*** AvsE = new TH2F ** [nChannel];
  TH2F*** CorrectedAvsE = new TH2F ** [nChannel];
  TH2F*** AvsE2 = new TH2F ** [nChannel];
  TH2F*** avsevsE = new TH2F ** [nChannel];
  TH1F*** avse2 = new TH1F ** [nChannel];
  TH2F*** CorrectedAvsE2 = new TH2F ** [nChannel];
  TH1F*** CorrectedA = new TH1F ** [nChannel];
  TH1F*** CorrectedAdep = new TH1F ** [nChannel];
  TH1F*** CorrectedAnbb = new TH1F ** [nChannel];
  vector< vector<string> > correctedAName(nChannel);
  
  //Stuff for doing the energy dependence
  vector<double> energyMin;
  energyMin.push_back(200);
  energyMin.push_back(310);
  energyMin.push_back(400);
  energyMin.push_back(530);
  energyMin.push_back(600);
  energyMin.push_back(700);
  energyMin.push_back(800);
  energyMin.push_back(900);
  energyMin.push_back(1050);
  energyMin.push_back(1180);
  energyMin.push_back(1250);
  energyMin.push_back(1300);
  energyMin.push_back(1465);
  energyMin.push_back(1550);
  energyMin.push_back(1680);
  energyMin.push_back(1750);
  energyMin.push_back(1800);
  energyMin.push_back(1900);
  energyMin.push_back(2000);
  energyMin.push_back(2150);
  energyMin.push_back(2220);
  energyMin.push_back(2300);
  size_t nEnergy=energyMin.size();

  double step=25;//use 25keV windows

  TGraphErrors*** gAvsE = new TGraphErrors**[nChannel];
  TF1* fitmean = new TF1("fitmean","gaus",0,10);
  TF1* fitAvsE = new TF1("fitAvsE","pol2",0,30);
  double xmax;
  //double CorrectedAdepmax;
  //double CorrectedAnbbmax;
  //double CorrectedAdiff;

  //Stuff for  optimizing the cut
  vector< vector <double> > DEPEff(nChannel);//DEP efficiency and error
  vector< vector <double> > DEPEffError(nChannel);
  vector< vector <double> > DEPDiffPos(nChannel);
  vector< vector <double> > DEPDiffNeg(nChannel);
  vector< vector <double> > SEPEff(nChannel);//SEP
  vector< vector <double> > SEPEffError(nChannel);
  vector< vector <double> > NBBCEff(nChannel);
  vector< vector <double> > NBBCEffError(nChannel);  
  vector< vector <double> > CutValue(nChannel);
  vector< vector <double> > meanAvsEcor(nChannel);

  vector<size_t> bestcurrent(nChannel,0);
  vector<double> bestDEP(nChannel,1);
  vector<double> bestSEP(nChannel,1);
  vector<double> bestNBBC(nChannel,1);
  vector<double> bestDEPError(nChannel,1);
  vector<double> bestDEPDiffPos(nChannel,1);
  vector<double> bestDEPDiffNeg(nChannel,1);
  vector<double> bestSEPError(nChannel,1);
  vector<double> bestNBBCError(nChannel,1);
  vector<double> bestCutValue(nChannel,1);

  TGraphErrors*** gDEP = new TGraphErrors**[nChannel];
  TGraphErrors*** gSEP = new TGraphErrors**[nChannel];
  TGraphErrors*** gNBBC = new TGraphErrors**[nChannel];

  TH1F*** EnergyAll = new TH1F** [nChannel];
  TH1F*** EnergyCut = new TH1F** [nChannel];
  TH1F*** EnergyCut2 = new TH1F** [nChannel];
  TH1F*** EnergyCut3 = new TH1F** [nChannel];


  //******************** Start the loop over channels
  //char histname[1000],drawcommand[1000],chancut[1000],title[2000],cut[2000],psacut[1000],energycut[1000];
  char histname[1000],drawcommand[1000],chancut[1000],title[2000],cut[2000],psacut[1000],energycut[1000],depcut[1000],nbbcut[1000],psacut_nbbpos[1000],psacut_nbbneg[1000];

  for(size_t ichan=0;ichan<nChannel;ichan++){
    printf("Now running on Channel %lu\n", channel[ichan]);
    AvsE[ichan] = new TH2F* [nCurrent];
    CorrectedAvsE[ichan] = new TH2F* [nCurrent];
    AvsE2[ichan] = new TH2F* [nCurrent];
    avsevsE[ichan] = new TH2F* [nCurrent];
    avse2[ichan] = new TH1F* [nCurrent];
    CorrectedAvsE2[ichan] = new TH2F* [nCurrent];
    CorrectedA[ichan] = new TH1F* [nCurrent];
    CorrectedAdep[ichan] = new TH1F* [nCurrent];
    CorrectedAnbb[ichan] = new TH1F* [nCurrent];
    gAvsE[ichan] = new TGraphErrors* [nCurrent];
    sprintf(chancut,"channel == %zu",channel[ichan]);
    
    DEPEff[ichan].resize(nCurrent);
    DEPEffError[ichan].resize(nCurrent);
    DEPDiffPos[ichan].resize(nCurrent);
    DEPDiffNeg[ichan].resize(nCurrent);
    SEPEff[ichan].resize(nCurrent);
    SEPEffError[ichan].resize(nCurrent);
    NBBCEff[ichan].resize(nCurrent);
    NBBCEffError[ichan].resize(nCurrent);	
    CutValue[ichan].resize(nCurrent);
    meanAvsEcor[ichan].resize(nCurrent);	
   
    gDEP[ichan]= new TGraphErrors*[nCurrent];
    gSEP[ichan]= new TGraphErrors*[nCurrent];
    gNBBC[ichan]= new TGraphErrors*[nCurrent];
    
    EnergyAll[ichan] = new TH1F*[nCurrent];
    EnergyCut[ichan] = new TH1F* [nCurrent];
    EnergyCut2[ichan] = new TH1F* [nCurrent];
    EnergyCut3[ichan] = new TH1F* [nCurrent];
    
    correctedAName[ichan].resize(nCurrent);
    for (size_t iC=0;iC<nCurrent;iC++){
     
      sprintf(histname,"Chan%zu_%s_AvsE",channel[ichan],currentName[iC].c_str());
      AvsE[ichan][iC] = new TH2F();
      sprintf(drawcommand,"(%s*(trapEMPZCal)/(trapEMPZ)):trapEMPZCal>>%s(%zu,%lf,%lf,%zu,%lf,%lf)",currentName[iC].c_str(),histname,nEbinskeV,EminkeV,EmaxkeV,nAbins,Amin,Amax);
      t->Draw(drawcommand,chancut);
      AvsE[ichan][iC] = (TH2F*)gDirectory->Get(histname);
      AvsE[ichan][iC]->SetName(histname);

   //******************************************** End of the energy dependance determination ********************************************//

    //******************************************** Get the energy dependence of A ********************************************//

      cout<<endl<<endl<<"******* Now doing the energy dependance of AvsE for chan "<<channel[ichan]<<" and "<< currentName[iC].c_str()<<endl;
      gAvsE[ichan][iC]= new TGraphErrors();
      CorrectedA[ichan][iC] = new TH1F();
      for(size_t iEmin=0;iEmin<nEnergy;iEmin++){
	sprintf(histname,"Chan%zu_%s_A",channel[ichan],currentName[iC].c_str());
	sprintf(drawcommand,"(%s*(trapEMPZCal)/(trapEMPZ))>>%s(2000)",currentName[iC].c_str(),histname);
	sprintf(cut,"trapEMPZCal>%lf && trapEMPZCal<=%lf && %s",energyMin[iEmin],energyMin[iEmin]+step,chancut);
	t->Draw(drawcommand,cut);
	CorrectedA[ichan][iC] = (TH1F*) gDirectory->Get(histname);
	CorrectedA[ichan][iC]->SetName(histname);
	xmax=CorrectedA[ichan][iC]->GetBinCenter(CorrectedA[ichan][iC]->GetMaximumBin());
	gAvsE[ichan][iC]->SetMarkerStyle(8);
	gAvsE[ichan][iC]->SetPoint(iEmin,energyMin[iEmin]+step/2.,xmax);
      }
      
      fitAvsE->SetRange(energyMin[0]+step/2.,energyMin[nEnergy-1]+step/2.);
      fitAvsE->SetParameters(0,0.01,0);
      gAvsE[ichan][iC]->Fit("fitAvsE","rm");

      sprintf(title,"(%s*(trapEMPZCal)/(trapEMPZ))-(%.14lf)-(%.14lf*(trapEMPZCal))-(%.14lf*(trapEMPZCal)*(trapEMPZCal))",currentName[iC].c_str(),fitAvsE->GetParameter(0),fitAvsE->GetParameter(1),fitAvsE->GetParameter(2));
      
      correctedAName[ichan][iC]=title; 
      
      CorrectedAvsE[ichan][iC] = new TH2F();
      sprintf(histname,"Chan%zu_%s_AvsECorrected",channel[ichan],currentName[iC].c_str());
      sprintf(drawcommand, "%s:trapEMPZCal>>%s(%zu,%lf,%lf,%zu,%lf,%lf)",correctedAName[ichan][iC].c_str(),histname,nEbinskeV,EminkeV,EmaxkeV,nAbins,CAmin,CAmax);
      t->Draw(drawcommand,chancut);
      CorrectedAvsE[ichan][iC] = (TH2F*)gDirectory->Get(histname);
      CorrectedAvsE[ichan][iC]->SetName(histname);
      
      sprintf(drawcommand,"%s>>%s(1000)",correctedAName[ichan][iC].c_str(),histname);
      sprintf(energycut,"trapEMPZCal>100 && trapEMPZCal<2800");//From here on the 1D AvsE has an energy cut
      sprintf(cut,"%s && %s",energycut,chancut);
      t->Draw(drawcommand,cut);
      CorrectedA[ichan][iC] = (TH1F*) gDirectory->Get(histname);
      CorrectedA[ichan][iC]->SetName(histname);     
      cout<<"The corrected AvsE value was found to be "<<correctedAName[ichan][iC].c_str()<<endl;


   //******************************************** Now figure out the best cut ********************************************//

      gDEP[ichan][iC]= new TGraphErrors();
      gDEP[ichan][iC]->SetMarkerColor(1);
      gDEP[ichan][iC]->SetMarkerStyle(20);
      gSEP[ichan][iC]= new TGraphErrors();
      gSEP[ichan][iC]->SetMarkerColor(2);
      gSEP[ichan][iC]->SetMarkerStyle(21);
      gNBBC[ichan][iC]= new TGraphErrors();
      gNBBC[ichan][iC]->SetMarkerColor(4);
      gNBBC[ichan][iC]->SetMarkerStyle(22);
      
      EnergyAll[ichan][iC] = new TH1F();
      EnergyCut[ichan][iC] = new TH1F();
      EnergyCut2[ichan][iC] = new TH1F();
      EnergyCut3[ichan][iC] = new TH1F();
      
      //The code to make the efficiency vs cut value
      double depeff=1;
      double depeff_nbbpos=1;
      double depeff_nbbneg=1;
      double difference=1;
      double sepeff=1;
      double nbbeff=1;
      double depefferr=0;
      double sepefferr=0;
      double nbbefferr=0;
      double tau=20./5.;
      double DEPUncutS=0;
      double DEPUncutB=0;
      double DEPUncut=0;
      double DEPCutS=0;
      double DEPCutB=0;
      double DEPCut=0;
      double SEPUncutS=0;
      double SEPUncutB=0;
      double SEPUncut=0;
      double SEPCutS=0;
      double SEPCutB=0;
      double SEPCut=0;
      double NBBUncut=0;
      double NBBCut=0;
      double DEPCutS_nbbpos=0;
      double DEPCutB_nbbpos=0;
      double DEPCut_nbbpos=0;
      double DEPCutS_nbbneg=0;
      double DEPCutB_nbbneg=0;
      double DEPCut_nbbneg=0;
      
          
      sprintf(histname,"Chan%zu_%s_EnergyAll",channel[ichan],currentName[iC].c_str());
      sprintf(drawcommand,"trapEMPZCal>>%s(%zu,%lf,%lf)",histname,nEbinskeV,EminkeV,EmaxkeV);
      t->Draw(drawcommand,chancut);
      EnergyAll[ichan][iC]=(TH1F*)gDirectory->Get(histname);
      EnergyAll[ichan][iC]->SetName(histname);
     
      DEPUncutS=EnergyAll[ichan][iC]->Integral(EnergyAll[ichan][iC]->FindBin(1590),EnergyAll[ichan][iC]->FindBin(1594));
      DEPUncutB=EnergyAll[ichan][iC]->Integral(EnergyAll[ichan][iC]->FindBin(1570),EnergyAll[ichan][iC]->FindBin(1579))+EnergyAll[ichan][iC]->Integral(EnergyAll[ichan][iC]->FindBin(1600),EnergyAll[ichan][iC]->FindBin(1609));
      DEPUncut=DEPUncutS-DEPUncutB/tau;//(EnergyAll[ichan][iC]->Integral(EnergyAll[ichan][iC]->FindBin(1590),EnergyAll[ichan][iC]->FindBin(1594)))-(EnergyAll[ichan][iC]->Integral(EnergyAll[ichan][iC]->FindBin(1570),EnergyAll[ichan][iC]->FindBin(1579))+EnergyAll[ichan][iC]->Integral(EnergyAll[ichan][iC]->FindBin(1600),EnergyAll[ichan][iC]->FindBin(1609)))*5/20;
      SEPUncutS=EnergyAll[ichan][iC]->Integral(EnergyAll[ichan][iC]->FindBin(2101),EnergyAll[ichan][iC]->FindBin(2105));
      SEPUncutB=EnergyAll[ichan][iC]->Integral(EnergyAll[ichan][iC]->FindBin(2080),EnergyAll[ichan][iC]->FindBin(2089))+EnergyAll[ichan][iC]->Integral(EnergyAll[ichan][iC]->FindBin(2115),EnergyAll[ichan][iC]->FindBin(2124));
      SEPUncut=SEPUncutS-SEPUncutB/tau;//(EnergyAll[ichan][iC]->Integral(EnergyAll[ichan][iC]->FindBin(2101),EnergyAll[ichan][iC]->FindBin(2105)))-(EnergyAll[ichan][iC]->Integral(EnergyAll[ichan][iC]->FindBin(2080),EnergyAll[ichan][iC]->FindBin(2089))+EnergyAll[ichan][iC]->Integral(EnergyAll[ichan][iC]->FindBin(2115),EnergyAll[ichan][iC]->FindBin(2124)))*5/20;
      NBBUncut=EnergyAll[ichan][iC]->Integral(EnergyAll[ichan][iC]->FindBin(1989),EnergyAll[ichan][iC]->FindBin(2088));
      
      //Fit CorrectedA to get an idea of where to set the cut (cmin and cmax)
      xmax=CorrectedA[ichan][iC]->GetBinCenter(CorrectedA[ichan][iC]->GetMaximumBin());
      fitmean->SetRange(xmax-3.*CorrectedA[ichan][iC]->GetBinWidth(1),xmax+3.*CorrectedA[ichan][iC]->GetBinWidth(1));
      fitmean->SetParameters(CorrectedA[ichan][iC]->GetMaximum(),xmax,xmax/10.);  
      CorrectedA[ichan][iC]->Fit("fitmean","qrem");

      double minAvsE=CorrectedA[ichan][iC]->GetBinLowEdge(1);
      double maxAvsE=CorrectedA[ichan][iC]->GetBinLowEdge(CorrectedA[ichan][iC]->GetNbinsX());	
      double cmin=TMath::Max(fitmean->GetParameter(1)-fitmean->GetParameter(2),minAvsE);
      double cmax=TMath::Min(fitmean->GetParameter(1)-3*fitmean->GetParameter(2),maxAvsE);
      meanAvsEcor[ichan][iC]= fitmean->GetParameter(1);
 
      //optimize cmin:
      depeff=0.8;
      cout<<"Optimizing the range for the PSA cut"<<endl;
      size_t infiniteloop=0;
      while(depeff<0.95 && infiniteloop<1000 && cmin>minAvsE){
	sprintf(psacut,"%s>%.18lf",correctedAName[ichan][iC].c_str(),cmin);
	sprintf(histname,"Chan%zu_%s_EnergyCut",channel[ichan],currentName[iC].c_str());
	sprintf(drawcommand,"trapEMPZCal>>%s(%zu,%lf,%lf)",histname,nEbinskeV,EminkeV,EmaxkeV);
	sprintf(cut,"%s && %s",chancut,psacut);
	
	t->Draw(drawcommand,cut);
	EnergyCut[ichan][iC]=(TH1F*)gDirectory->Get(histname);
	EnergyCut[ichan][iC]->SetName(histname);
      	DEPCutS=EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1590),EnergyCut[ichan][iC]->FindBin(1594));
        DEPCutB=EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1570),EnergyCut[ichan][iC]->FindBin(1579))+EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1600),EnergyCut[ichan][iC]->FindBin(1609));
      	DEPCut=DEPCutS-DEPCutB/tau;//(EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1590),EnergyCut[ichan][iC]->FindBin(1594)))-(EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1570),EnergyCut[ichan][iC]->FindBin(1579))+EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1600),EnergyCut[ichan][iC]->FindBin(1609)))*5/20;
        depeff=DEPCut/DEPUncut;

	cout<<"With "<<psacut<<" depeff = "<<depeff<<endl;
	cmin-=fitmean->GetParameter(2);
	infiniteloop++;
     }      
     cout<<"found mininum of range at "<<cmin<<endl;
      if(infiniteloop==1000){
	cout<<"Did not converge on minimum value of the cut"<<endl;
	cmin=minAvsE;
      }
      
      //optimize cmax:
      infiniteloop=0;
      while(depeff>0.85 && infiniteloop<1000 && cmax<maxAvsE){
	sprintf(psacut,"%s>%.18lf",correctedAName[ichan][iC].c_str(),cmax);
	sprintf(histname,"Chan%zu_%s_EnergyCut",channel[ichan],currentName[iC].c_str());
	sprintf(drawcommand,"trapEMPZCal>>%s(%zu,%lf,%lf)",histname,nEbinskeV,EminkeV,EmaxkeV);
	sprintf(cut,"%s && %s",chancut,psacut);
	t->Draw(drawcommand,cut);
	EnergyCut[ichan][iC]=(TH1F*)gDirectory->Get(histname);
	EnergyCut[ichan][iC]->SetName(histname);
      	DEPCutS=EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1590),EnergyCut[ichan][iC]->FindBin(1594));
        DEPCutB=EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1570),EnergyCut[ichan][iC]->FindBin(1579))+EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1600),EnergyCut[ichan][iC]->FindBin(1609));
      	DEPCut=DEPCutS-DEPCutB/tau;//(EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1590),EnergyCut[ichan][iC]->FindBin(1594)))-(EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1570),EnergyCut[ichan][iC]->FindBin(1579))+EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1600),EnergyCut[ichan][iC]->FindBin(1609)))*5/20;
        depeff=DEPCut/DEPUncut;
	cout<<"With "<<psacut<<" depeff = "<<depeff<<endl;
	cmax+=fitmean->GetParameter(2);
	infiniteloop++;
      }
      cout<<"found maximum of range at "<<cmax<<endl;
      if(infiniteloop==1000){
	cout<<"Did not converge on maximum value of Dhe cut"<<endl;
	cmax=maxAvsE;
      }

      size_t cn=25;
      double cstep=(cmax-cmin)/cn;
      double cutvalue; 
      double cutvalue_nbbpos; 
      double cutvalue_nbbneg; 
      
      //vary the cut
      int count=0;
      for(size_t icut=0;icut<cn;icut++){
	cutvalue=cmin+cstep*icut;
	sprintf(psacut,"%s>%.18lf",correctedAName[ichan][iC].c_str(),cutvalue);
	
	sprintf(histname,"Chan%zu_%s_EnergyCut",channel[ichan],currentName[iC].c_str());
	sprintf(drawcommand,"trapEMPZCal>>%s(%zu,%lf,%lf)",histname,nEbinskeV,EminkeV,EmaxkeV);
	sprintf(cut,"%s && %s",chancut,psacut);
	t->Draw(drawcommand,cut);
	EnergyCut[ichan][iC]=(TH1F*)gDirectory->Get(histname);
	EnergyCut[ichan][iC]->SetName(histname);
     	
      	DEPCutS=EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1590),EnergyCut[ichan][iC]->FindBin(1594));
        DEPCutB=EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1570),EnergyCut[ichan][iC]->FindBin(1579))+EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1600),EnergyCut[ichan][iC]->FindBin(1609));
      	DEPCut=DEPCutS-DEPCutB/tau;//(EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1590),EnergyCut[ichan][iC]->FindBin(1594)))-(EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1570),EnergyCut[ichan][iC]->FindBin(1579))+EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1600),EnergyCut[ichan][iC]->FindBin(1609)))*5/20;
	depeff=DEPCut/DEPUncut;
      	SEPCutS=EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(2101),EnergyCut[ichan][iC]->FindBin(2105));
        SEPCutB=EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(2080),EnergyCut[ichan][iC]->FindBin(2089))+EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(2115),EnergyCut[ichan][iC]->FindBin(2124));
      	SEPCut=SEPCutS-SEPCutB/tau;//(EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(2101),EnergyCut[ichan][iC]->FindBin(2105)))-(EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(2080),EnergyCut[ichan][iC]->FindBin(2089))+EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(2115),EnergyCut[ichan][iC]->FindBin(2124)))*5./20.;
        NBBCut=EnergyCut[ichan][iC]->Integral(EnergyCut[ichan][iC]->FindBin(1989),EnergyCut[ichan][iC]->FindBin(2088));
        sepeff=SEPCut/SEPUncut;
        nbbeff=NBBCut/NBBUncut;
        depefferr=sqrt((1./DEPUncut)*(((DEPCutS+DEPCutB/tau/tau)/DEPUncut)*(1.-2.*depeff)+(((DEPUncutS+DEPUncutB/tau/tau)/DEPUncut)*depeff*depeff)));//sqrt(SEPCut)/SEPUncut;
        sepefferr=sqrt((1./SEPUncut)*(((SEPCutS+SEPCutB/tau/tau)/SEPUncut)*(1.-2.*sepeff)+(((SEPUncutS+SEPUncutB/tau/tau)/SEPUncut)*sepeff*sepeff)));//sqrt(SEPCut)/SEPUncut;
        nbbefferr=sqrt(NBBCut)/NBBUncut;
	cout<<"Cut = "<<cutvalue<<" DEP = "<<depeff<<" SEP = "<<sepeff<<" NBB = "<<nbbeff<<endl;
	cout<<"DEPUncutS = "<<DEPUncutS<<" DEPUncutB = "<<DEPUncutB<<" DEPUncut = "<<DEPUncut<<" tau = "<<tau<<endl;
	cout<<"DEPCutS = "<<DEPCutS<<" DEPCutB = "<<DEPCutB<<" DEPCut = "<<DEPCut<<" tau = "<<tau<<endl;
	cout<<"depeff = "<<depeff<<" depefferr = "<<depefferr <<endl;
	cout<<"SEPUncutS = "<<SEPUncutS<<" SEPUncutB = "<<SEPUncutB<<" SEPUncut = "<<SEPUncut<<" tau = "<<tau<<endl;
	cout<<"SEPCutS = "<<SEPCutS<<" SEPCutB = "<<SEPCutB<<" SEPCut = "<<SEPCut<<" tau = "<<tau<<endl;
	cout<<"sepeff = "<<sepeff<<" sepefferr = "<<sepefferr <<endl;
	//cout<<"Cut = "<<cutvalue<<" DEP = "<<depeff<<" FEP = "<<PeaksDEPCut->GetParameter(10)<<" Cont = "<<PeaksDEPCut->GetParameter(8)<<" SEP "<<PeakSEPCut->GetParameter(7)<<" NBB = "<<(NBBUncut==0 ? 1: NBBCut/NBBUncut)<<endl;

        //set DEP efficiency to 90%
	if(fabs(depeff-0.9)<difference){
	  difference=fabs(depeff-0.9);   
       
	  DEPEff[ichan][iC]= depeff;
	  SEPEff[ichan][iC]= sepeff;
	  NBBCEff[ichan][iC]= nbbeff;
	  DEPEffError[ichan][iC]=depefferr;
	  SEPEffError[ichan][iC]= sepefferr;
	  NBBCEffError[ichan][iC]=nbbefferr; //(NBBUncut==0 ? 1:sqrt(NBBCut)/NBBUncut);
	  CutValue[ichan][iC]= cutvalue;	  
	
        }
	
	gDEP[ichan][iC]->SetPoint(count,cutvalue,depeff);
	gDEP[ichan][iC]->SetPointError(count,0,depefferr);
    	gSEP[ichan][iC]->SetPoint(count,cutvalue,sepeff);
    	gSEP[ichan][iC]->SetPointError(count,0,sepefferr);
	gNBBC[ichan][iC]->SetPoint(count,cutvalue,nbbeff);
	gNBBC[ichan][iC]->SetPointError(count,0,nbbefferr);
	
	count++;
      }//end of loop over the cut value

      //Now make the plots with the determined value of the cut:
      
      sprintf(psacut,"%s>%.18lf",correctedAName[ichan][iC].c_str(),CutValue[ichan][iC]);
      sprintf(histname,"Chan%zu_%s_EnergyCut",channel[ichan],currentName[iC].c_str());
      sprintf(drawcommand,"trapEMPZCal>>%s(%zu,%lf,%lf)",histname,nEbinskeV,EminkeV,EmaxkeV);
      sprintf(cut,"%s && %s",chancut,psacut);
      t->Draw(drawcommand,cut);
      EnergyCut[ichan][iC]=(TH1F*)gDirectory->Get(histname);
      EnergyCut[ichan][iC]->SetName(histname);
      
      cout<<"The best cut was found to be at "<<correctedAName[ichan][iC].c_str()<<" = "<<CutValue[ichan][iC]<<endl;
      cout<<"DEP = "<<DEPEff[ichan][iC]<<" +/- "<<DEPEffError[ichan][iC]<<endl;
      cout<<"SEP = "<<SEPEff[ichan][iC]<<" +/- "<<SEPEffError[ichan][iC]<<endl;
      cout<<"0nbb Continuum = "<<NBBCEff[ichan][iC]<<" +/- "<<NBBCEffError[ichan][iC]<<endl;
  
      //Calculate energy dependance error
      //avse for DEP 
      sprintf(histname,"Chan%zu_%s_CorrectedAdep",channel[ichan],currentName[iC].c_str());
      sprintf(drawcommand,"%s>>%s(%zu,%lf,%lf)",correctedAName[ichan][iC].c_str(),histname,nAbins,CAmin,CAmax);
      sprintf(depcut,"trapEMPZCal>1590 && trapEMPZCal<1594");
      sprintf(cut,"%s && %s",depcut,chancut);
      t->Draw(drawcommand,cut);
      CorrectedAdep[ichan][iC]=(TH1F*)gDirectory->Get(histname);
      CorrectedAdep[ichan][iC]->SetName(histname);
      double CorrectedAdepmax=CorrectedAdep[ichan][iC]->GetBinCenter(CorrectedAdep[ichan][iC]->GetMaximumBin());
  
      //avse for NBBC
      sprintf(histname,"Chan%zu_%s_CorrectedAnbb",channel[ichan],currentName[iC].c_str());
      sprintf(drawcommand,"%s>>%s(%zu,%lf,%lf)",correctedAName[ichan][iC].c_str(),histname,nAbins,CAmin,CAmax);
      //sprintf(drawcommand,"((-1)*(%s)/(%.18lf))>>%s(%zu,%lf,%lf)",correctedAName[ichan][iC].c_str(),CutValue[ichan][iC],histname,nAbins,avsemin,avsemax);
      sprintf(nbbcut,"trapEMPZCal>1989 && trapEMPZCal<2088");
      sprintf(cut,"%s && %s",nbbcut,chancut);
      t->Draw(drawcommand,cut);
      CorrectedAnbb[ichan][iC]=(TH1F*)gDirectory->Get(histname);
      CorrectedAnbb[ichan][iC]->SetName(histname);
      double CorrectedAnbbmax=CorrectedAnbb[ichan][iC]->GetBinCenter(CorrectedAnbb[ichan][iC]->GetMaximumBin());
      double CorrectedAdiff=fabs(CorrectedAnbbmax-CorrectedAdepmax);
      cout << "CorrectedAdepmax: " << CorrectedAdepmax <<endl;      
      cout << "CorrectedAnbbmax: " << CorrectedAnbbmax <<endl;      
      cout << "CorrectedAdiff: " << CorrectedAdiff <<endl;      

      double cutvalue2= CutValue[ichan][iC];
      cout << "cutvalue2: " << cutvalue2 <<endl;
      cutvalue_nbbpos=cutvalue2+CorrectedAdiff; 
      cutvalue_nbbneg=cutvalue2-CorrectedAdiff; 
      sprintf(psacut_nbbpos,"%s>%.18lf",correctedAName[ichan][iC].c_str(),cutvalue_nbbpos);
      sprintf(histname,"Chan%zu_%s_EnergyCut2",channel[ichan],currentName[iC].c_str());
      sprintf(drawcommand,"trapEMPZCal>>%s(%zu,%lf,%lf)",histname,nEbinskeV,EminkeV,EmaxkeV);
      sprintf(cut,"%s && %s",chancut,psacut_nbbpos);
      t->Draw(drawcommand,cut);
      EnergyCut2[ichan][iC]=(TH1F*)gDirectory->Get(histname);
      EnergyCut2[ichan][iC]->SetName(histname);

      DEPCutS_nbbpos=EnergyCut2[ichan][iC]->Integral(EnergyCut2[ichan][iC]->FindBin(1590),EnergyCut2[ichan][iC]->FindBin(1594));
      DEPCutB_nbbpos=EnergyCut2[ichan][iC]->Integral(EnergyCut2[ichan][iC]->FindBin(1570),EnergyCut2[ichan][iC]->FindBin(1579))+EnergyCut2[ichan][iC]->Integral(EnergyCut2[ichan][iC]->FindBin(1600),EnergyCut2[ichan][iC]->FindBin(1609));
      DEPCut_nbbpos=DEPCutS_nbbpos-DEPCutB_nbbpos/tau;//(EnergyCut2[ichan][iC]->Integral(EnergyCut2[ichan][iC]->FindBin(1590),EnergyCut2[ichan][iC]->FindBin(1594)))-(EnergyCut2[ichan][iC]->Integral(EnergyCut2[ichan][iC]->FindBin(1570),EnergyCut2[ichan][iC]->FindBin(1579))+EnergyCut2[ichan][iC]->Integral(EnergyCut2[ichan][iC]->FindBin(1600),EnergyCut2[ichan][iC]->FindBin(1609)))*5./20.;
      depeff_nbbpos=DEPCut_nbbpos/DEPUncut;
      double depeff2 = DEPEff[ichan][iC];
      double depdiff_pos = fabs(depeff2-depeff_nbbpos); 
      DEPDiffPos[ichan][iC]=depdiff_pos;
      cout << "depdiff_pos: " << depdiff_pos <<endl;      
      cout << "depeff: " << depeff2 <<endl;      
      cout << "depeff_nbbpos: " << depeff_nbbpos <<endl;      

      sprintf(psacut_nbbneg,"%s>%.18lf",correctedAName[ichan][iC].c_str(),cutvalue_nbbneg);
      sprintf(histname,"Chan%zu_%s_EnergyCut3",channel[ichan],currentName[iC].c_str());
      sprintf(drawcommand,"trapEMPZCal>>%s(%zu,%lf,%lf)",histname,nEbinskeV,EminkeV,EmaxkeV);
      sprintf(cut,"%s && %s",chancut,psacut_nbbneg);
      t->Draw(drawcommand,cut);
      EnergyCut3[ichan][iC]=(TH1F*)gDirectory->Get(histname);
      EnergyCut3[ichan][iC]->SetName(histname);

      DEPCutS_nbbneg=EnergyCut3[ichan][iC]->Integral(EnergyCut3[ichan][iC]->FindBin(1590),EnergyCut3[ichan][iC]->FindBin(1594));
      DEPCutB_nbbneg=EnergyCut3[ichan][iC]->Integral(EnergyCut3[ichan][iC]->FindBin(1570),EnergyCut3[ichan][iC]->FindBin(1579))+EnergyCut3[ichan][iC]->Integral(EnergyCut3[ichan][iC]->FindBin(1600),EnergyCut3[ichan][iC]->FindBin(1609));
      DEPCut_nbbneg=DEPCutS_nbbneg-DEPCutB_nbbneg/tau;
      depeff_nbbneg=DEPCut_nbbneg/DEPUncut;
      double depdiff_neg = fabs(depeff2-depeff_nbbneg); 
      DEPDiffNeg[ichan][iC]=depdiff_neg;
      cout << "depdiff_neg: " << depdiff_neg <<endl;      
      cout << "depeff: " << depeff2 <<endl;      
      cout << "depeff_nbbneg: " << depeff_nbbneg <<endl;      
  
    }//end of loop over different currents

  }//end of loop over channels
  //****************************************************************************************************
  
  //******************************************** Make some plots  ********************************************//

  TCanvas c2;
  //Title, Stats, Date off by default
    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0);
    gStyle->SetOptDate(0);
                         
  for(size_t ichan=0;ichan<nChannel;ichan++){
    for (size_t iC=0;iC<nCurrent;iC++){
  
   //Canvas 1 A vs E fit
      c2.cd();
      c2.SetLogy(0);
      gAvsE[ichan][iC]->GetXaxis()->SetRangeUser(0,3000);
      gAvsE[ichan][iC]->GetXaxis()->SetTitle("Energy (keV)");
      gAvsE[ichan][iC]->GetYaxis()->SetTitle("Mean AvsE");  
      sprintf(title,"Chan %zu, %s, Energy dependence of AvsE",channel[ichan],currentName[iC].c_str());
      gAvsE[ichan][iC]->SetTitle(title);
      gAvsE[ichan][iC]->GetXaxis()->SetRangeUser(0,3000);
      gAvsE[ichan][iC]->Draw("ap");
      sprintf(title,"%smjd_psa_analysis_calibrated/Chan%zu_%s_MeanAvsE.pdf",outputdir.c_str(),channel[ichan],currentName[iC].c_str());
      c2.Print(title);

  //Canvas 2 Corrected A
      c2.cd();
      c2.SetLogy();
      sprintf(title,"%s",correctedAName[ichan][iC].c_str());
      CorrectedA[ichan][iC]->GetXaxis()->SetTitle("Corrected A");
      CorrectedA[ichan][iC]->GetYaxis()->SetTitle("Counts");  
      sprintf(title,"Chan %zu, %s, Corrected A",channel[ichan],currentName[iC].c_str());
      CorrectedA[ichan][iC]->SetTitle(title);
      CorrectedA[ichan][iC]->Draw();
      sprintf(title,"%smjd_psa_analysis_calibrated/Chan%zu_%s_CorrectedA.pdf",outputdir.c_str(),channel[ichan],currentName[iC].c_str());
      c2.Print(title);

    //Canvas 3 Corrected A vs E 0-3000 keV
      CorrectedAvsE2[ichan][iC] = new TH2F();
      sprintf(histname,"Chan%zu_%s_AvsECorrected",channel[ichan],currentName[iC].c_str());
      sprintf(drawcommand, "%s:trapEMPZCal>>%s(%zu,%lf,%lf,%zu,%lf,%lf)",correctedAName[ichan][iC].c_str(),histname,nEbinskeV,EminkeV,EmaxkeV,nAbins,CAmin,CAmax);
      t->Draw(drawcommand,chancut);
      CorrectedAvsE2[ichan][iC] = (TH2F*)gDirectory->Get(histname);
      CorrectedAvsE2[ichan][iC]->SetName(histname);
      c2.SetLogy(0);
      c2.SetLogz();
      CorrectedAvsE2[ichan][iC]->GetXaxis()->SetRangeUser(0,3000);
      CorrectedAvsE2[ichan][iC]->GetXaxis()->SetTitle("Energy (keV)");
      CorrectedAvsE2[ichan][iC]->GetYaxis()->SetTitle("Corrected A");
      c2.cd();
      sprintf(title,"%s",CorrectedAvsE2[ichan][iC]->GetName());
      CorrectedAvsE2[ichan][iC]->SetTitle(title);
      CorrectedAvsE2[ichan][iC]->Draw("colz");
      sprintf(title,"%smjd_psa_analysis_calibrated/Chan%zu_%s_CorrectedAvsE2.pdf",outputdir.c_str(),channel[ichan],currentName[iC].c_str());
      c2.Print(title);

    //Canvas 4 Corrected A vs E 1550-1650 keV     
      CorrectedAvsE2[ichan][iC]->GetXaxis()->SetRangeUser(1550,1650);
      CorrectedAvsE2[ichan][iC]->GetXaxis()->SetTitle("Energy (keV)");
      CorrectedAvsE2[ichan][iC]->GetYaxis()->SetTitle("Corrected A");
      c2.cd();
      sprintf(title,"%s",CorrectedAvsE2[ichan][iC]->GetName());
      CorrectedAvsE2[ichan][iC]->SetTitle(title);
      CorrectedAvsE2[ichan][iC]->Draw("colz");
      sprintf(title,"%smjd_psa_analysis_calibrated/Chan%zu_%s_CorrectedAvsE2_DEP.pdf",outputdir.c_str(),channel[ichan],currentName[iC].c_str());
      c2.Print(title);

   //Canvas 3 A vs E 0-3000 keV
      AvsE2[ichan][iC] = new TH2F();
      sprintf(histname,"Chan%zu_%s_AvsE2",channel[ichan],currentName[iC].c_str());
      sprintf(drawcommand, "(%s*trapEMPZCal/trapEMPZ):trapEMPZCal>>%s(%zu,%lf,%lf,%zu,%lf,%lf)",currentName[iC].c_str(),histname,nEbinskeV,EminkeV,EmaxkeV,nAbins,Amin,Amax);
      t->Draw(drawcommand,chancut);
      AvsE2[ichan][iC] = (TH2F*)gDirectory->Get(histname);
      AvsE2[ichan][iC]->SetName(histname);
      AvsE2[ichan][iC]->GetXaxis()->SetRangeUser(0,3000);
      AvsE2[ichan][iC]->GetXaxis()->SetTitle("Energy (keV)");
      AvsE2[ichan][iC]->GetYaxis()->SetTitle("A");
      c2.cd();
      sprintf(title,"%s",AvsE2[ichan][iC]->GetName());
      AvsE2[ichan][iC]->SetTitle(title);
      AvsE2[ichan][iC]->Draw("colz");
      sprintf(title,"%smjd_psa_analysis_calibrated/Chan%zu_%s_AvsE.pdf",outputdir.c_str(),channel[ichan],currentName[iC].c_str());
      c2.Print(title);

    //Canvas 5 Energy spectrum before and after cut 0-3000 keV
      c2.SetLogy();      
      sprintf(title,"Chan %zu, %s, Energy After cut;Energy (keV)",channel[ichan],currentName[iC].c_str());
      EnergyAll[ichan][iC]->SetTitle(title);
      EnergyAll[ichan][iC]->GetXaxis()->SetRangeUser(0,3000);
      EnergyAll[ichan][iC]->GetXaxis()->SetTitle("Energy (keV)");
      EnergyAll[ichan][iC]->GetYaxis()->SetTitle("Counts");
      EnergyAll[ichan][iC]->GetYaxis()->SetRangeUser(0.1,1.2*EnergyAll[ichan][iC]->GetMaximum());
      EnergyCut[ichan][iC]->SetLineColor(1);
      c2.cd();
      EnergyAll[ichan][iC]->Draw();
      EnergyCut[ichan][iC]->SetLineColor(2);
      EnergyCut[ichan][iC]->Draw("same");

      TLegend leg0(0.3,0.73,0.5,0.85);
      leg0.SetTextSize(22);
      leg0.SetTextFont(133);
      leg0.SetBorderSize(0);
      leg0.SetFillColor(0);
      leg0.AddEntry(EnergyAll[ichan][iC],"No cuts","l");
      leg0.AddEntry(EnergyCut[ichan][iC],"avse cut","l");
      leg0.Draw("same");
      sprintf(title,"%smjd_psa_analysis_calibrated/Chan%zu_%s_EnergyPSA.pdf",outputdir.c_str(),channel[ichan],currentName[iC].c_str());
      c2.Print(title);
       
    //Canvas 7 Energy spectrum before and after cut 2050-2150
      c2.SetLogy(0);      
      c2.cd();
      sprintf(title,"Chan %zu, %s, Energy After cut;Energy (keV)",channel[ichan],currentName[iC].c_str());
      EnergyAll[ichan][iC]->SetTitle(title);
      EnergyAll[ichan][iC]->GetXaxis()->SetRangeUser(2000,2150);
      EnergyAll[ichan][iC]->GetYaxis()->SetRangeUser(0,0.02*EnergyAll[ichan][iC]->GetMaximum());
      EnergyAll[ichan][iC]->Draw();
      EnergyCut[ichan][iC]->Draw("same");

      TLegend leg1(0.2,0.667,0.4,0.85);
      leg1.SetTextSize(20);
      leg1.SetTextFont(133);
      leg1.SetBorderSize(0);
      leg1.SetFillColor(0);
      leg1.AddEntry(EnergyAll[ichan][iC],"No cuts");
      leg1.AddEntry(EnergyCut[ichan][iC],"avse cut");
      leg1.Draw("same");
      sprintf(title,"%smjd_psa_analysis_calibrated/Chan%zu_%s_EnergyPSA_SEP.pdf",outputdir.c_str(),channel[ichan],currentName[iC].c_str());
      c2.Print(title);

    //Canvas 7 Energy spectrum before and after cut 2050-2150
      c2.SetLogy(0);      
      c2.cd();
      sprintf(title,"Chan %zu, %s, Energy After cut;Energy (keV)",channel[ichan],currentName[iC].c_str());
      EnergyAll[ichan][iC]->SetTitle(title);
      EnergyAll[ichan][iC]->GetXaxis()->SetRangeUser(1550,1650);
      EnergyAll[ichan][iC]->GetYaxis()->SetRangeUser(0,EnergyAll[ichan][iC]->GetMaximum());
      EnergyAll[ichan][iC]->Draw();
      EnergyCut[ichan][iC]->Draw("same");

      TLegend leg2(0.2,0.667,0.4,0.85);
      leg2.SetTextSize(22);
      leg2.SetTextFont(133);
      leg2.SetBorderSize(0);
      leg2.SetFillColor(0);
      leg2.AddEntry(EnergyAll[ichan][iC],"No cuts");
      leg2.AddEntry(EnergyCut[ichan][iC],"avse cut");
      leg2.Draw("same");
      sprintf(title,"%smjd_psa_analysis_calibrated/Chan%zu_%s_EnergyPSA_DEP.pdf",outputdir.c_str(),channel[ichan],currentName[iC].c_str());
      c2.Print(title);

    //Canvas 8  Efficiency vs cut
      c2.SetLogy(0);      
      c2.cd();
      sprintf(title,"Chan %zu, %s, Efficiency vs cut;value of cut on %s; Efficiency",channel[ichan],currentName[iC].c_str(),correctedAName[ichan][iC].c_str()); 
   
      TH1F h("h",title,1000,gDEP[ichan][iC]->GetXaxis()->GetXmin(),gDEP[ichan][iC]->GetXaxis()->GetXmax());
      h.GetXaxis()->SetTitle("Corrected A");
      h.GetYaxis()->SetRangeUser(0,1);
      h.Draw();
      gDEP[ichan][iC]->Draw("p");
      gSEP[ichan][iC]->Draw("p");
      gNBBC[ichan][iC]->Draw("p");

      TLegend leg3(0.75,0.667,0.87,0.85);
      leg3.SetTextSize(22);
      leg3.SetTextFont(133);
      leg3.SetFillColor(0);
      leg3.SetBorderSize(0);
      leg3.AddEntry(gDEP[ichan][iC],"DEP");
      leg3.AddEntry(gSEP[ichan][iC],"SEP");
      leg3.AddEntry(gNBBC[ichan][iC],"NBBC");
      leg3.Draw("same");
      sprintf(title,"%smjd_psa_analysis_calibrated/Chan%zu_%s_PSAEffVsCut.pdf",outputdir.c_str(),channel[ichan],currentName[iC].c_str());
      c2.Print(title);

   //Canvas 9 avse vs E 
      avsevsE[ichan][iC] = new TH2F();
      sprintf(histname,"Chan%zu_%s_avsevsE",channel[ichan],currentName[iC].c_str());
      sprintf(drawcommand, "((-1)*(%s)/(%.18lf)):trapEMPZCal>>%s(%zu,%lf,%lf,%zu,%lf,%lf)",correctedAName[ichan][iC].c_str(),CutValue[ichan][iC],histname,nEbinskeV,EminkeV,EmaxkeV,nAbins,avsemin,avsemax);
      t->Draw(drawcommand,chancut);
      avsevsE[ichan][iC] = (TH2F*)gDirectory->Get(histname);
      avsevsE[ichan][iC]->SetName(histname);
      avsevsE[ichan][iC]->GetXaxis()->SetRangeUser(0,3000);
      avsevsE[ichan][iC]->GetXaxis()->SetTitle("Energy (keV)");
      avsevsE[ichan][iC]->GetYaxis()->SetTitle("avse (a.u.)");
      c2.cd();
      sprintf(title,"%s",avsevsE[ichan][iC]->GetName());
      avsevsE[ichan][iC]->SetTitle(title);
      avsevsE[ichan][iC]->Draw("colz");
      sprintf(title,"%smjd_psa_analysis_calibrated/Chan%zu_%s_avsevsE.pdf",outputdir.c_str(),channel[ichan],currentName[iC].c_str());
      c2.Print(title);
   
//Canvas 10 avse 
      c2.SetLogy();      
      avse2[ichan][iC] = new TH1F();
      sprintf(histname,"Chan%zu_%s_avse",channel[ichan],currentName[iC].c_str());
      sprintf(drawcommand, "((-1)*(%s)/(%.18lf))>>%s(%zu,%lf,%lf)",correctedAName[ichan][iC].c_str(),CutValue[ichan][iC],histname,nAbins,avsemin,avsemax);
      t->Draw(drawcommand,chancut);
      avse2[ichan][iC] = (TH1F*)gDirectory->Get(histname);
      avse2[ichan][iC]->SetName(histname);
      //avse2[ichan][iC]->GetXaxis()->SetRangeUser(-15,5);
      avse2[ichan][iC]->GetXaxis()->SetTitle("avse (a.u.)");
      avse2[ichan][iC]->GetYaxis()->SetTitle("Counts");
      c2.cd();
      sprintf(title,"%s",avse2[ichan][iC]->GetName());
      avse2[ichan][iC]->SetTitle(title);
      avse2[ichan][iC]->Draw();
      sprintf(title,"%smjd_psa_analysis_calibrated/Chan%zu_%s_avse.pdf",outputdir.c_str(),channel[ichan],currentName[iC].c_str());
      c2.Print(title);

   //Canvas 11 avse vs E DEP 
      avsevsE[ichan][iC]->GetXaxis()->SetRangeUser(1550,1650);
      avsevsE[ichan][iC]->GetXaxis()->SetTitle("Energy (keV)");
      avsevsE[ichan][iC]->GetYaxis()->SetTitle("avse");
      c2.cd();
      sprintf(title,"%s",avsevsE[ichan][iC]->GetName());
      avsevsE[ichan][iC]->SetTitle(title);
      avsevsE[ichan][iC]->Draw("colz");
      sprintf(title,"%smjd_psa_analysis_calibrated/Chan%zu_%s_avsevsE_DEP.pdf",outputdir.c_str(),channel[ichan],currentName[iC].c_str());
      c2.Print(title);
  
    //Canvas avse for DEP
      c2.SetLogy();      
      sprintf(title,"Chan %zu, %s, avse for DEP",channel[ichan],currentName[iC].c_str());
      CorrectedAdep[ichan][iC]->SetTitle(title);
      CorrectedAdep[ichan][iC]->GetXaxis()->SetTitle("avse (1590<E<1594");
      CorrectedAdep[ichan][iC]->GetYaxis()->SetTitle("Counts");
      c2.cd();
      CorrectedAdep[ichan][iC]->Draw();
      CorrectedAnbb[ichan][iC]->SetLineColor(1);
      CorrectedAnbb[ichan][iC]->Draw("same");
      sprintf(title,"%smjd_psa_analysis_calibrated/Chan%zu_%s_CorrectedAdep.pdf",outputdir.c_str(),channel[ichan],currentName[iC].c_str());
      c2.Print(title);

}
}
  //******************************************** Save results in files  ********************************************//
	//file 1 psaresults - save the Efficiencies in 8 column: ch, current, DEP, err, SEP, err, NBBC, err
	  string outfilename = outputdir+"psaresults.dat";	
	  ofstream outfile(outfilename.c_str());

	  for(size_t ichan=0;ichan<nChannel;ichan++){
	    for (size_t iC=0;iC<nCurrent;iC++){
    
	    outfile<<channel[ichan]<<" "<<currentName[iC].c_str()<<" "<<DEPEff[ichan][iC]<<" "<< DEPEffError[ichan][iC]<<" "<<SEPEff[ichan][iC]<<" "<< SEPEffError[ichan][iC]<<" "<<NBBCEff[ichan][iC]<<" "<< NBBCEffError[ichan][iC]<<endl;
 		} 
	  }	  
	  outfile.close();

	//file 2 psacut - save in the format required for the skimfiles
	  string outfilename2 = outputdir+"psacut.dat";
	  ofstream outfile2(outfilename2.c_str());

	  for(size_t ichan=0;ichan<nChannel;ichan++){
	    for (size_t iC=0;iC<nCurrent;iC++){

	    outfile2<<" "<<"if    (channel=="<<channel[ichan]<<") return "<<"(-1*("<<correctedAName[ichan][iC].c_str()<<")/"<<CutValue[ichan][iC]<<")"<<endl;
	    }
	  }
	  outfile2.close();

	//file 3 systematics
	  string outfilename3 = outputdir+"systematics.dat";
	  ofstream outfile3(outfilename3.c_str());

	  for(size_t ichan=0;ichan<nChannel;ichan++){
	    for (size_t iC=0;iC<nCurrent;iC++){
                      
            outfile3 << channel[ichan] << " " << DEPEffError[ichan][iC] <<" "<< DEPDiffPos[ichan][iC] <<" "<< DEPDiffNeg[ichan][iC] << endl;

	    }
	  }


	//Find the best PSA by looking at the best reduction in the continuum at 2039 (NBB)
	  for(size_t ichan=0;ichan<nChannel;ichan++){
	    for (size_t iC=0;iC<nCurrent;iC++){
	        if(NBBCEff[ichan][iC]<bestNBBC[ichan]){
	        bestcurrent[ichan]=iC;
		bestCutValue[ichan]=CutValue[ichan][iC];
	        bestDEP[ichan]=DEPEff[ichan][iC];
	       	bestSEP[ichan]=SEPEff[ichan][iC];
	       	bestNBBC[ichan]=NBBCEff[ichan][iC];
	       	bestDEPError[ichan]=DEPEffError[ichan][iC];
	       	bestDEPDiffPos[ichan]=DEPDiffPos[ichan][iC];
	       	bestDEPDiffNeg[ichan]=DEPDiffNeg[ichan][iC];
	       	bestSEPError[ichan]=SEPEffError[ichan][iC];
	       	bestNBBCError[ichan]=NBBCEffError[ichan][iC];
	      	}
	    } 
	    cout << "For channel"<< channel[ichan] << "the best reduction in the continuum at 2039 (NBB) is" << bestNBBC[ichan] << "+/-" << bestNBBCError[ichan] << "achieved with the current estimator" << currentName[bestcurrent[ichan]].c_str() <<endl;

	      //save best current results in files

	      //file 4 best current psa results 
	      string outfilename4 = outputdir+"bestpsaresults.dat";
	      ofstream outfile4(outfilename4.c_str());
	      outfile4 <<" " <<channel[ichan] <<" "<<bestDEP[ichan]<<" "<<bestDEPError[ichan]<<" "<<bestSEP[ichan]<<" "<<bestSEPError[ichan]<<" "<<bestNBBC[ichan]<<" "<<bestNBBCError[ichan]<<endl;
	      outfile3.close();
    
	      //file 5 best current psa cut 
	      string outfilename5 = outputdir+"bestpsacut.dat";
	      ofstream outfile5(outfilename5.c_str());
	      outfile5 <<" "<<"if    (channel=="<<channel[ichan]<<") return "<<"(-1*("<<correctedAName[ichan][bestcurrent[ichan]].c_str()<<")/"<<bestCutValue[ichan]<<");"<<endl;
	      outfile5.close();
      
	      //file 6 systematics
	      string outfilename6 = outputdir+"bestsystematics.dat";
	      ofstream outfile6(outfilename6.c_str());
              outfile6 << channel[ichan] << " "<< bestDEPError[ichan] <<" "<< bestDEPDiffPos[ichan] <<" "<< bestDEPDiffNeg[ichan] << endl;

 	}

}//end of main
