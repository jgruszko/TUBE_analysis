{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  
  TString dsArr[15] = {"10_0", "10_1", "10_2", "10_3", "10_4", "10_5", "10_6", "185_0", "185_1", "185_2", "185_3", "185_4", "185_5", "0_0"};
  //TString dsArr[46] = {"20_0", "30_0", "30_1", "40_0", "40_1", "40_2", "40_3","50_0", "50_1", "50_2", "50_3", "50_4", "50_5", "50_6", "60_0", "60_1", "70_0", "70_1", "80_0", "80_1", "90_0", "100_0", "110_0", "120_0", "130_0", "140_0", "140_1", "150_0", "150_1", "160_0", "160_1", "170_0", "180_0", "190_0", "190_1", "195_0", "195_1", "195_2", "200_0", "210_0", "210_1", "210_2", "210_3", "470_0", "470_1", "470_2"};

  TString fName;
  TFile fData; 
  for(int i = 3; i<15; i++){
    cout<<" File "<<i<<": "<<dsArr[i]<<endl;

    fName =  "$TUBEDIR/data/skim/skimDS";
    fName += dsArr[i];
    fName+=".root"; 
    TChain* skim = new TChain("skimTree");
    skim->Add(fName);

    skim->Draw("trapEMPZ>>h(20000, 0, 2100)", "trapEMPZ<2100 && channel == 112");
    TH1D* h = (TH1D*) gDirectory->Get("h");

    fName =  "$TUBEDIR/analysis/energyCal/hist_DS";
    fName += dsArr[i];
    fName+=".root"; 
    TFile *fOut = new TFile(fName, "RECREATE");
    h->Write("trapEMPZ");
    fOut->Close();
 
  }
}
 
   
