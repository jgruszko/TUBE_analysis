{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  
  TString dsArr[7] = {"10_0", "10_1", "10_2", "10_3", "10_4", "10_5", "10_6"};
  // TString dsArr[46] = {"20_0", "30_0", "30_1", "40_0", "40_1", "40_2", "40_3","50_0", "50_1", "50_2", "50_3", "50_4", "50_5", "50_6", "60_0", "60_1", "70_0", "70_1", "80_0", "80_1", "90_0", "100_0", "110_0", "120_0", "130_0", "140_0", "140_1", "150_0", "150_1", "160_0", "160_1", "170_0", "180_0", "190_0", "190_1", "195_0", "195_1", "195_2", "200_0", "210_0", "210_1", "210_2", "210_3", "470_0", "470_1", "470_2"};

  TString fName, hTitle;
  TFile fData; 
  GATMultiPeakFitter* results;
  TH1F* h;
  gStyle->SetOptStat(0);
  for(int i = 0; i<7; i++){
    cout<<" File "<<i<<": "<<dsArr[i]<<endl;

    fName =  "$TUBEDIR/analysis/energyCal/fitResults_DS";
    fName += dsArr[i];
    fName+=".root"; 
    TFile* f = TFile::Open(fName);

    hTitle.Form("DS%s Fit Result", dsArr[i].Data());

    results = (GATMultiPeakFitter*) gDirectory->Get("results");
    h = (TH1F*) gDirectory->Get("h");
    h->SetTitle(hTitle); 
    h->GetXaxis()->SetTitle("Energy (ADC)"); 
    h->GetYaxis()->SetTitle("Events"); 
    
    TCanvas* can = new TCanvas("can", "can", 600, 400);
    can->SetLogy();
    results->DrawRegions();
    fName.Form("$TUBEDIR/analysis/energyCal/plots/fitResult_DS%s_all.gif", dsArr[i].Data());
    can->Print(fName);

    can->SetLogy(0);
    h->GetXaxis()->SetRangeUser(190, 280);
    results->DrawRegions();
    fName.Form("$TUBEDIR/analysis/energyCal/plots/fitResult_DS%s_1.gif", dsArr[i].Data());
    can->Print(fName);

    h->GetXaxis()->SetRangeUser(800, 1150);
    results->DrawRegions();
    fName.Form("$TUBEDIR/analysis/energyCal/plots/fitResult_DS%s_2.gif", dsArr[i].Data());
    can->Print(fName);
    
    h->GetXaxis()->SetRangeUser(1290, 1360);
    results->DrawRegions();
    fName.Form("$TUBEDIR/analysis/energyCal/plots/fitResult_DS%s_3.gif", dsArr[i].Data());
    can->Print(fName);
    
    h->GetXaxis()->SetRangeUser(1580, 1700);
    results->DrawRegions();
    fName.Form("$TUBEDIR/analysis/energyCal/plots/fitResult_DS%s_4.gif", dsArr[i].Data());
    can->Print(fName);
    
    h->GetXaxis()->SetRangeUser(1890, 1970);
    results->DrawRegions();
    fName.Form("$TUBEDIR/analysis/energyCal/plots/fitResult_DS%s_5.gif", dsArr[i].Data());
    can->Print(fName);


  }
  gSystem->Exit(0);
}
 
   
