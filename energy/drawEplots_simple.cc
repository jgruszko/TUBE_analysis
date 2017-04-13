{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");

//  TString dsArr[6] = {"160_0", "150_0", "140_0", "140_1", "130_0", "470_1"};
//  TString distArr[6] = {"8.25", "9.75", "11.25", "11.25", "12.75", "inf"};

  TString dsArr[30] = {"20_0", "30_0", "30_1", "40_0", "40_1", "40_2", "40_3","50_0", "50_1", "50_2", "50_3", "50_4", "50_5", "50_6", "60_0", "60_1", "70_0", "70_1", "80_0", "80_1", "90_0", "100_0", "110_0"};
//  double liveT[20] = {25.016, 24.516, 25.018, 24.975, 24.516, 24.519, 11.694, 24.016, 23.374, 25.016, 21.839, 25.022, 19.800, 24.727, 18.929, 25.107}; 
  //TString ds[20] = {"470_0", "470_1", "470_2", "120_0", "130_0", "140_0", "140_1", "150_0", "150_1", "160_0", "160_1", "170_0", "180_0", "190_0", "190_1", "200_0", "210_0", "210_1", "210_2", "210_3"};
  //double liveT[20] = {3.543, 22.688, 26.293, 24.516, 26.021, 21.015, 22.333, 20.910, 25.778, 5.378, 24.365, 19.093, 21.820, 31.004, 15.201, 21.014, 25.016, 25.016, 24.019, 19.675};  
 // TString dsArr[3] = {"50_0", "130_0", "470_1"};
 // TString distArr[3] = {"8.25", "12.75", "inf"};
  
  Color_t colArr[7] = {kRed, kOrange, kGreen, kBlue, kViolet, kBlack, kCyan};
 // Color_t colArr[3] = {kRed,kBlue, kGreen};
  TString fName;
  TFile fHist;
  TString legStr;
  
  TH1D* hE;

  TLegend* leg = new TLegend(.6, .5, .9, .9);
  TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
  double nTurns, dist;

  for(int i = 0; i<14; i++){
    fName =  "$TUBEDIR/analysis/hists/DS";
    fName += dsArr[i];
    fName+=".root"; 
    fHist.Open(fName, "READ");
     
    //nTurns
    TString subStr(dsArr[i](0, dsArr[i].First('_')));
    nTurns = (double)subStr.Atoi()/10.;
    dist = (nTurns-21.5)*1.5; //in mm, assuming 21.5 is center
    
    hE = (TH1D*) gDirectory->Get("hE");
     

    hE->SetLineColor(colArr[i%7]);
    hE->Rebin(5);
    hE->GetXaxis()->SetRangeUser(3000, 5000);

    legStr.Form("DS %s, source %4.2f mm from P-contact, dcrpzc90>0", dsArr[i].Data(), dist);
   hE->SetTitle(legStr);
   // if(i == 0){
      c1->SetLogy();
      hE->Draw("HIST");
   // }
  //  else{
   //   hE->Draw("SAME HIST");
   // } 

 
  //leg->AddEntry(hE, legStr, "L");
    fName =  "$TUBEDIR/analysis/hists/plots/DS";
    fName += dsArr[i];
    fName+="_E5keV.gif"; 
   c1->Print(fName);
    fHist.Close(); 
  }
  ///leg->Draw();
 // c1->Print("$TUBEDIR/analysis/hists/plots/5turn_runs.gif");
  
}
