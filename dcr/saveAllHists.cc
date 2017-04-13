{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  //TString ds[24] = {"160_0", "40_0", "40_1", "40_2", "40_3", "30_0", "30_1", "20_0", "50_0", "50_1", "50_2", "50_3", "50_4", "50_5", "50_6", "60_0", "60_1", "70_0", "70_1", "80_0", "80_1", "90_0", "100_0", "110_0"};
  //double liveT[24] = {5.378, 25.017, 25.017, 25.018, 17.766, 25.015, 21.779, 31.836, 25.016, 24.516, 25.018, 24.975, 24.516, 24.519, 11.694, 24.016, 23.374, 25.016, 21.839, 25.022, 19.800, 24.727, 18.929, 25.107}; 
  TString ds[23] = {"195_0", "195_1", "195_2", "120_0", "130_0", "140_0", "140_1", "150_0", "150_1", "160_0", "160_1", "170_0", "180_0", "190_0", "190_1", "200_0", "210_0", "210_1", "210_2", "210_3", "470_0", "470_1", "470_2"};
  double liveT[23] = {23.014, 30.019, 29.340, 24.516, 26.021, 21.015, 22.333, 20.910, 25.778, 5.378, 24.365, 19.093, 21.820, 31.004, 15.201, 21.014, 25.016, 25.016, 24.019, 19.675, 3.543, 22.688, 26.293};
 
  TString fName, histName;
    TString selection = "channel == 112 && mClean == 1 && avse>-1 && nX<2";
  TString energySel = "&& trapEMPZCal>100 && trapEMPZCal<6000";
    TH1D* hE;
    TH1D* hDCR;
    TH1D* hDCRPZC;
    TH2D* hDCRvE;
    TH2D* hDCRPZCvE;

    TH1D* hEcut90;
    TH1D* hEcut99;
    TH1D* hEcut90PZ;
    TH1D* hEcut99PZ;
  


  for(int i = 0; i<23; i++){
      cout<<"Opening file "<<i<<endl; 
    TString fName = "$TUBEDIR/data/skim/skimDS";
    fName += ds[i];
    fName+=".root"; 
    TChain* skimTree = new TChain("skimTree");
    skimTree->AddFile(fName);
    
    fName =  "$TUBEDIR/analysis/hists/DS";
    fName += ds[i];
    fName+=".root"; 
    
    TFile* fOut = new TFile(fName, "UPDATE");
  
    //plot 1: Energy
    cout<<"Drawing plot 1"<<endl;
    selection = "channel == 112 && mClean == 1 && avse>-1 && nX<2";
    energySel = "&& trapEMPZCal>100 && trapEMPZCal<10000";
    skimTree->Draw("trapEMPZCal>>histE(9900, 100, 10000)", selection+energySel);
    hE = (TH1D*) gDirectory->Get("histE");
    hE->Sumw2();
    hE->Scale(1./liveT[i]);
    hE->SetTitle("Energy");
    hE->GetXaxis()->SetTitle("Energy (keV)");
    hE->GetYaxis()->SetTitle("Counts/hr/keV");

    //plot 2: DCR
    cout<<"Drawing plot 2"<<endl;
    selection += "&& dcr90>-2";
    energySel = "&& trapEMPZCal>1000 && trapEMPZCal<6000";
    skimTree->Draw("dcr90>>histDCR(3000, -.01, .02)", selection+energySel);
    hDCR = (TH1D*) gDirectory->Get("histDCR");
    hDCR->Sumw2();
    hDCR->Scale(1./liveT[i]);
    hDCR->SetTitle("DCR, 1MeV<E<6MeV");
    hDCR->GetXaxis()->SetTitle("DCR (ADC/ns)");
    hDCR->GetYaxis()->SetTitle("Counts/hr");


    //plot 3: DCRvE
    cout<<"Drawing plot 3"<<endl;
    energySel = "&& trapEMPZCal>100 && trapEMPZCal<10000";
    skimTree->Draw("dcr90:trapEMPZCal>>histDCRvE(9900, 100, 10000, 3000, -.01, .02)", selection+energySel);
    hDCRvE = (TH2D*) gDirectory->Get("histDCRvE");
    hDCRvE->SetTitle("DCR vs. E");
    hDCRvE->GetXaxis()->SetTitle("Energy (keV)");
    hDCRvE->GetYaxis()->SetTitle("DCR (ADC/ns)");

    //plot 3b: energy spectrum with dcr90 cut
    int firstBin = hDCRvE->GetYaxis()->FindBin(0.);
    histName.Form("hEcut90_%d", i);
    hEcut90 = hDCRvE->ProjectionX(histName, firstBin, -1, "e");
    hEcut90->Scale(1./liveT[i]);
    hEcut90->SetTitle("Energy after dcr90 cut");
    hEcut90->GetXaxis()->SetTitle("Energy (keV)");
    hEcut90->GetYaxis()->SetTitle("Counts/hr/keV");
 
    //plot 4: DCRPZC
    cout<<"Drawing plot 4"<<endl;
    energySel = "&& trapEMPZCal>1000 && trapEMPZCal<6000";
    skimTree->Draw("dcrpzc90>>histDCRPZC(3000, -.01, .02)", selection+energySel);
    hDCRPZC = (TH1D*) gDirectory->Get("histDCRPZC");
    hDCRPZC->Sumw2();
    hDCRPZC->Scale(1./liveT[i]);
    hDCRPZC->SetTitle("DCRPZC, 1MeV<E<6MeV");
    hDCRPZC->GetXaxis()->SetTitle("DCRPZC (ADC/ns)");
    hDCRPZC->GetYaxis()->SetTitle("Counts/hr");

    //plot 5: DCRPZCvE
    cout<<"Drawing plot 5"<<endl;
    energySel = "&& trapEMPZCal>100 && trapEMPZCal<10000";
    skimTree->Draw("dcrpzc90:trapEMPZCal>>histDCRPZCvE(9900, 100, 10000, 3000, -.01, .02)", selection+energySel);
    hDCRPZCvE = (TH2D*) gDirectory->Get("histDCRPZCvE");
    hDCRPZCvE->SetTitle("DCRPZC vs. E");
    hDCRPZCvE->GetXaxis()->SetTitle("Energy (keV)");
    hDCRPZCvE->GetYaxis()->SetTitle("DCRPZC (ADC/ns)");

    //plot 5b: energy spectrum with dcrpzc90 cut
    firstBin = hDCRPZCvE->GetYaxis()->FindBin(0.);
    histName.Form("hEcut90PZ_%d", i);
    hEcut90PZ = hDCRPZCvE->ProjectionX(histName, firstBin, -1, "e");
    hEcut90PZ->Scale(1./liveT[i]);
    hEcut90PZ->SetTitle("Energy after dcrpzc90 cut");
    hEcut90PZ->GetXaxis()->SetTitle("Energy (keV)");
    hEcut90PZ->GetYaxis()->SetTitle("Counts/hr/keV");
 

    //Write hists to file
    fOut->cd();
    hE->Write("hE");
    hDCR->Write("hDCR");
    hDCRPZC->Write("hDCRPZC");
    hDCRvE->Write("hDCRvE");
    hEcut90->Write("hEcut90");
    hDCRPZCvE->Write("hDCRPZCvE");
    hEcut90PZ->Write("hEcut90PZ");
    
    fOut->Close();
  } 
} 
