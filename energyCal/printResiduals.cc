{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  
 // TString dsArr[7] = {"10_0", "10_1", "10_2", "10_3", "10_4", "10_5", "10_6"};
   TString dsArr[46] = {"20_0", "30_0", "30_1", "40_0", "40_1", "40_2", "40_3","50_0", "50_1", "50_2", "50_3", "50_4", "50_5", "50_6", "60_0", "60_1", "70_0", "70_1", "80_0", "80_1", "90_0", "100_0", "110_0", "120_0", "130_0", "140_0", "140_1", "150_0", "150_1", "160_0", "160_1", "170_0", "180_0", "190_0", "190_1", "195_0", "195_1", "195_2", "200_0", "210_0", "210_1", "210_2", "210_3", "470_0", "470_1", "470_2"};

  TString fName, hTitle;
  TFile fData; 
  TGraphErrors* results;
  gStyle->SetOptStat(0);
  TCanvas* can = new TCanvas("can", "can", 600, 400);
  for(int i = 0; i<10; i++){
    cout<<" File "<<i<<": "<<dsArr[i]<<endl;

    fName =  "$TUBEDIR/analysis/energyCal/fitResults_DS";
    fName += dsArr[i];
    fName+=".root"; 
    TFile* f = TFile::Open(fName);

    hTitle.Form("DS%s", dsArr[i].Data());

    results = (TGraphErrors*) gDirectory->Get("mu_residuals");
    results->GetXaxis()->SetTitle("Energy (keV)"); 
    results->GetYaxis()->SetTitle("mu_free - mu_cal (keV)");
    results->SetMarkerColor(i+1); 
    if(i ==0){ 
     results->GetYaxis()->SetRangeUser(-1.5, 1.5);
     results->Draw("AP"); }
    else results->Draw("SAME P");

    i++;
  }
}
 
   
