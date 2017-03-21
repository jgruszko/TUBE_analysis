{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  TString ds[1] = {"130_0"};
  
  double minE[1] = {3200};
  double maxE[1] = {3700};
  
  //get bkg spectra to do background subtraction
  TFile* fBkg = new TFile("$TUBEDIR/analysis/hists/DS470_1.root", "READ");
  TH1D* hBkg = (TH1D*)gDirectory->Get("hE");
 
  TFile *fHist; 
  TString fName;
  TString hName;
  TString selection = "channel == 112 && mClean == 1 && avse>0 && nX<2";
  TString energySel = "&& trapEMPZCal>100 && trapEMPZCal<6000";
  
  TH1D* hEsub[6];
  TF1* fE[6];
  for(int i = 0; i<1; i++){ 
    fName =  "$TUBEDIR/analysis/hists/DS";
    fName += ds[i];
    fName+=".root"; 
    //make background-subtracted energy plot, save to hist file 
    fHist = new TFile(fName, "UPDATE");
    TH1D* hE = (TH1D*) gDirectory->Get("hE"); 
    hName.Form("hEsub%d", i);
    hEsub[i] = new TH1D(hName, hName, 5900, 100, 6000);
    hEsub[i]->Add(hE, hBkg, 1, -1);
    hEsub[i]->Write("hEsub");

    //fit gaussian to alpha peak
    fE[i] = new TF1("f1", "gaus", minE[i], maxE[i]);
    TFitResultPtr res = hEsub[i]->Fit(fE[i], "R S");
    if(res->IsValid()){
     res->Write("fitRes"); 
      fE[i]->Write("fE");
    }
    fHist->Close();
  } 
  fBkg->Close();  
} 
