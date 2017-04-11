{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  TString ds[20] = {"50_0", "50_1", "50_2", "50_3", "50_4", "50_5", "50_6", "60_0", "60_1", "70_0", "70_1", "80_0", "80_1", "90_0", "100_0", "110_0"};
  double liveT[20] = {25.016, 24.516, 25.018, 24.975, 24.516, 24.519, 11.694, 24.016, 23.374, 25.016, 21.839, 25.022, 19.800, 24.727, 18.929, 25.107}; 
  //TString ds[20] = {"120_0", "130_0", "140_0", "140_1", "150_0", "150_1", "160_0", "160_1", "170_0", "180_0", "190_0", "190_1", "200_0", "210_0", "210_1", "210_2", "210_3", "470_0", "470_1", "470_2"};
  //double liveT[20] = {24.516, 26.021, 21.015, 22.333, 20.910, 25.778, 5.378, 24.365, 19.093, 21.820, 31.004, 15.201, 21.014, 25.016, 25.016, 24.019, 19.675, 3.543, 22.688, 26.293};
 
  TString fName;
  TH1D* hEcut90[20];
  TH1D* hEcut99[20];
  TH1D* hEcut90PZ[20];
  TH1D* hEcut99PZ[20];

  TString selection = "channel == 112 && mClean == 1 && avse>0 && nX<2";
  TString energySel = "&& trapEMPZCal>100 && trapEMPZCal<6000";
  TString dcrCut = "&& dcr90>0";
  for(int i = 0; i<16; i++){
    cout<<"Opening file "<<i<<endl; 
    TString fName = "$TUBEDIR/data/skim/skimDS";
    fName += ds[i];
    fName+=".root"; 
    TChain* skimTree = new TChain("skimTree");
    skimTree->AddFile(fName);
    
    fName =  "$TUBEDIR/analysis/hists/040617/DS";
    fName += ds[i];
    fName+=".root"; 
    
    TFile* fOut = new TFile(fName, "UPDATE");
  
    //plot 1: dcr90
    cout<<"Drawing plot 1"<<endl;
    dcrCut = "&& dcr90>0";
    skimTree->Draw("trapEMPZCal>>hist90(5900, 100, 6000)", selection+energySel+dcrCut);
    hEcut90[i] = (TH1D*) gDirectory->Get("hist90");
    hEcut90[i]->Scale(1./liveT[i]);
    hEcut90[i]->SetTitle("dcr90>0");
    hEcut90[i]->GetXaxis()->SetTitle("Energy (keV)");
    hEcut90[i]->GetYaxis()->SetTitle("Counts/hr/keV");

    //plot 2: dcr99
    cout<<"Drawing plot 2"<<endl;
    dcrCut = "&& dcr99>0";
    skimTree->Draw("trapEMPZCal>>hist99(5900, 100, 6000)", selection+energySel+dcrCut);
    hEcut99[i] = (TH1D*) gDirectory->Get("hist99");
    hEcut99[i]->Scale(1./liveT[i]);
    hEcut99[i]->SetTitle("dcr99>0");
    hEcut99[i]->GetXaxis()->SetTitle("Energy (keV)");
    hEcut99[i]->GetYaxis()->SetTitle("Counts/hr/keV");

    //plot 3: dcr90PZ
    cout<<"Drawing plot 3"<<endl;
    dcrCut = "&& dcrpzc90>0";
    skimTree->Draw("trapEMPZCal>>hist90PZ(5900, 100, 6000)", selection+energySel+dcrCut);
    hEcut90PZ[i] = (TH1D*) gDirectory->Get("hist90PZ");
    hEcut90PZ[i]->Scale(1./liveT[i]);
    hEcut90PZ[i]->SetTitle("dcrpzc90>0");
    hEcut90PZ[i]->GetXaxis()->SetTitle("Energy (keV)");
    hEcut90PZ[i]->GetYaxis()->SetTitle("Counts/hr/keV");

    //plot 4: dcr99PZ
    cout<<"Drawing plot 4"<<endl;
    dcrCut = "&& dcrpzc99>0";
    skimTree->Draw("trapEMPZCal>>hist99PZ(5900, 100, 6000)", selection+energySel+dcrCut);
    hEcut99PZ[i] = (TH1D*) gDirectory->Get("hist99PZ");
    hEcut99PZ[i]->Scale(1./liveT[i]);
    hEcut99PZ[i]->SetTitle("dcrpzc99>0");
    hEcut99PZ[i]->GetXaxis()->SetTitle("Energy (keV)");
    hEcut99PZ[i]->GetYaxis()->SetTitle("Counts/hr/keV");


    //Write hists to file
    fOut->cd();
    hEcut90[i]->Write("hEcut90");
    hEcut99[i]->Write("hEcut99");
    hEcut90PZ[i]->Write("hEcut90PZ");
    hEcut99PZ[i]->Write("hEcut99PZ");
    
    fOut->Close();
  } 
} 
