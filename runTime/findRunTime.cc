{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  TString fName = "$TUBEDIR/data/skim/skimDS170_0.root";
  TChain* skimTree = new TChain("skimTree", "");
  skimTree->AddFile(fName);
  
  vector<double>* start;
  vector<double>* stop;
  skimTree->SetBranchAddress("startTime", &start);
  skimTree->SetBranchAddress("stopTime", &stop);

  int currentRun = 0;
  Int_t nRun;
  skimTree->SetBranchAddress("run", &nRun);

  int nEntries = skimTree->GetEntries();
  cout<<nEntries<<" entries in tree."<<endl;
  double totalT = 0;
  double t1, t2;
  for(int i = 0; i<nEntries; i++)
  {
     skimTree->GetEntry(i);
     if(nRun > currentRun){ 
       t2 = stop->at(0);
       t1 = start->at(0);
       if(t2-t1<0) cout<<"Run has length "<<t2-t1<<". Skipping run "<<nRun<<endl;
       else totalT += t2-t1;
       currentRun = nRun;
     }
  }
  cout<<"Total run time (in s):"<<totalT<<endl;
  cout<<"Total run time (in hrs):"<<totalT/(60.*60.)<<endl;

}
