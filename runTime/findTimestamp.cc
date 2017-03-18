{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  TString fName = "$TUBEDIR/data/skim/skimDS190_0.root";
  int runList[2] = {847, 848};
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
  TString drawStr, selStr;
  TH1D* hist;
  for(int i = 0; i<2; i++){
     nRun = runList[i];
     drawStr.Form("timestamp_s>>h%d(2000, 0, 2000)", i);
     selStr.Form("channel == 112 && run == %d", nRun);
     skimTree->Draw(drawStr, selStr);
     drawStr.Form("h%d", i);
     hist = (TH1D*) gDirectory->Get(drawStr);
     t1 = hist->GetXaxis()->GetBinLowEdge(hist->FindFirstBinAbove(0));
     t2 = hist->GetXaxis()->GetBinUpEdge(hist->FindLastBinAbove(0));
     totalT += t2-t1;
  }
  cout<<"Total run time (in s):"<<totalT<<endl;
  cout<<"Total run time (in hrs):"<<totalT/(60.*60.)<<endl;

}
