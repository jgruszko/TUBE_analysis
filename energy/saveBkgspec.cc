{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  TString ds[3] = {"470_0", "470_1", "470_2"};
  double liveT[3] = {3.5433, 22.6875, 26.2939};
  double totalT = 0; 
  TString fName;
  TChain* skimTree;
  TFile* fOut = new TFile("$TUBEDIR/analysis/energy/bkgSpec.root", "UPDATE");
  
  TString selection = "channel == 112 && mClean == 1 && avse>0 && nX<2";
  TString energySel = "&& trapEMPZCal>100 && trapEMPZCal<6000";
  TString hName; 
  gStyle->SetOptStat(0);
  
  TH1D* hBkg = new TH1D("hBkg", "hBkg", 5900, 100, 6000); 
  for(int i = 0; i<3; i++){
    fName = "$TUBEDIR/data/skim/skimDS";
    fName += ds[i];
    fName+=".root"; 
    skimTree = new TChain("skimTree");
    skimTree->AddFile(fName);
  
    skimTree->Draw("trapEMPZCal>>hE(5900, 100, 6000)", selection+energySel);
    TH1D* hE = (TH1D*) gDirectory->Get("hE");
    hBkg->Add(hE); 
    totalT += liveT[i];
    hE->Scale(1./liveT[i]);
    hE->SetTitle("Bkg E Spectrum");
    hE->GetXaxis()->SetTitle("Energy (keV)");
    hE->GetYaxis()->SetTitle("Counts/hr/keV");
    //Write hists to file
    fOut->cd();
    hName.Form("h%s", ds[i].Data());
    hE->Write(hName);
  } 
  fOut->cd();
  hBkg->Scale(1./totalT);
  hBkg->Write("hBkg");
  fOut->Close();
 
} 
