{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  TString ds[20] = {"120_0", "130_0", "140_0", "140_1", "150_0", "150_1", "160_0", "160_1", "170_0", "180_0", "190_0", "190_1", "200_0", "210_0", "210_1", "210_2", "210_3", "470_0", "470_1", "470_2"};
  double liveT[20] = {24.516, 26.021, 21.015, 22.333, 20.910, 25.778, 5.378, 24.365, 19.093, 21.820, 31.004, 15.201, 21.014, 25.016, 25.016, 24.019, 19.675, 3.543, 22.688, 26.293};
 
  TString fName;
  TH1D* hE[20];
  TH1D* hDCR[20];
  TH1D* hDCRPZC[20];
  TH2D* hDCRvE[20];
  TH2D* hDCRPZCvE[20];

  TString selection = "channel == 112 && mClean == 1 && avse>0 && nX<2";
  TString energySel = "&& trapEMPZCal>100 && trapEMPZCal<6000";
  for(int i = 0; i<20; i++){
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
    selection = "channel == 112 && mClean == 1 && avse>0 && nX<2";
    energySel = "&& trapEMPZCal>100 && trapEMPZCal<6000";
    skimTree->Draw("trapEMPZCal>>histE(5900, 100, 6000)", selection+energySel);
    hE[i] = (TH1D*) gDirectory->Get("histE");
    hE[i]->Scale(1./liveT[i]);
    hE[i]->SetTitle("Energy");
    hE[i]->GetXaxis()->SetTitle("Energy (keV)");
    hE[i]->GetYaxis()->SetTitle("Counts/hr/keV");

    //plot 2: DCR
    cout<<"Drawing plot 2"<<endl;
    selection += "&& dcr90>-2";
    energySel = "&& trapEMPZCal>1000 && trapEMPZCal<6000";
    skimTree->Draw("dcr90>>histDCR(1000, -.01, .02)", selection+energySel);
    hDCR[i] = (TH1D*) gDirectory->Get("histDCR");
    hDCR[i]->Scale(1./liveT[i]);
    hDCR[i]->SetTitle("DCR, 1MeV<E<6MeV");
    hDCR[i]->GetXaxis()->SetTitle("DCR (ADC/ns)");
    hDCR[i]->GetYaxis()->SetTitle("Counts/hr");


    //plot 3: DCRvE
    cout<<"Drawing plot 3"<<endl;
    energySel = "&& trapEMPZCal>500 && trapEMPZCal<6000";
    skimTree->Draw("dcr90:trapEMPZCal>>histDCRvE(5500, 500, 6000, 1000, -.01, .02)", selection+energySel);
    hDCRvE[i] = (TH2D*) gDirectory->Get("histDCRvE");
    hDCRvE[i]->SetTitle("DCR vs. E");
    hDCRvE[i]->GetXaxis()->SetTitle("Energy (keV)");
    hDCRvE[i]->GetYaxis()->SetTitle("DCR (ADC/ns)");

    //plot 4: DCRPZC
    cout<<"Drawing plot 4"<<endl;
    energySel = "&& trapEMPZCal>1000 && trapEMPZCal<6000";
    skimTree->Draw("dcrpzc90>>histDCRPZC(1000, -.01, .02)", selection+energySel);
    hDCRPZC[i] = (TH1D*) gDirectory->Get("histDCRPZC");
    hDCRPZC[i]->Scale(1./liveT[i]);
    hDCRPZC[i]->SetTitle("DCRPZC, 1MeV<E<6MeV");
    hDCRPZC[i]->GetXaxis()->SetTitle("DCRPZC (ADC/ns)");
    hDCRPZC[i]->GetYaxis()->SetTitle("Counts/hr");

    //plot 5: DCRPZCvE
    cout<<"Drawing plot 5"<<endl;
    energySel = "&& trapEMPZCal>500 && trapEMPZCal<6000";
    skimTree->Draw("dcrpzc90:trapEMPZCal>>histDCRPZCvE(5500, 500, 6000, 1000, -.01, .02)", selection+energySel);
    hDCRPZCvE[i] = (TH2D*) gDirectory->Get("histDCRPZCvE");
    hDCRPZCvE[i]->SetTitle("DCRPZC vs. E");
    hDCRPZCvE[i]->GetXaxis()->SetTitle("Energy (keV)");
    hDCRPZCvE[i]->GetYaxis()->SetTitle("DCRPZC (ADC/ns)");

    //Write hists to file
    fOut->cd();
    hE[i]->Write("hE");
    hDCR[i]->Write("hDCR");
    hDCRPZC[i]->Write("hDCRPZC");
    hDCRvE[i]->Write("hDCRvE");
    hDCRPZCvE[i]->Write("hDCRPZCvE");
    
    fOut->Close();
  } 
} 
