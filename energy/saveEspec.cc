{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  TString ds = "140_1";
  double liveT = 22.3333;
  double minE = 2900;
  double maxE = 3600;
   /*
  liveT130 = 26.0211
  double liveT160 = 5.3775; //live time in hours 
  double liveT150 = 20.9097; //live time in hours 
  double liveT140_0 = 21.015; //live time in hours 
  double liveT140_1 = 22.3333; //live time in hours 
  double liveT470_1 = 22.6875; //live time in hours 
  */
  //get bkg spectra to do background subtraction
  TFile* fBkg = new TFile("$TUBEDIR/analysis/energy/bkgSpec.root", "READ");
  TH1D* hBkg = (TH1D*)gDirectory->Get("hBkg");

  TString fName = "$TUBEDIR/data/skim/skimDS";
  fName += ds;
  fName+=".root"; 
  TChain* skimTree = new TChain("skimTree");
  skimTree->AddFile(fName);
  
  fName =  "$TUBEDIR/analysis/energy/DS";
  fName += ds;
  fName+=".root"; 
  
  TFile* fOut = new TFile(fName, "UPDATE");
  
  TString selection = "channel == 112 && mClean == 1 && avse>0 && nX<2";
  TString energySel = "&& trapEMPZCal>100 && trapEMPZCal<6000";

  gStyle->SetOptStat(0);
  TCanvas* c1 = new TCanvas(); 
  skimTree->Draw("trapEMPZCal>>hE(5900, 100, 6000)", selection+energySel);
  TH1D* hE = (TH1D*) gDirectory->Get("hE");
  hE->Scale(1./liveT);
  hE->SetTitle("Alpha Scan E Spectra");
  hE->GetXaxis()->SetTitle("Energy (keV)");
  hE->GetYaxis()->SetTitle("Counts/hr/keV");
  
  TF1* fE = new TF1("fE", "gaus", minE, maxE);  
  TFitResultPtr res = hE->Fit(fE, "R S");
 
  TH1D* hEsub = new TH1D("hEsub", "hEsub", 5900, 100, 6000);
  hEsub->Add(hE, hBkg, 1, -1);
  TF1* fEsub = new TF1("fEsub", "gaus", minE, maxE);  
  TFitResultPtr res2 = hE->Fit(fEsub, "R S");
   
  TString hName; 
//Write hists to file
  fOut->cd();
  hE->Write("hE");
  if(res->IsValid()){
   res->Write("fitRes"); 
    fE->Write("fE");
  }
   
  hEsub->Write("hEsub");
  if(res2->IsValid()){
   fEsub->Write("fEsub");
   res2->Write("fitResSub"); 
  }
  fBkg->Close();  
  fOut->Close();
 
} 
