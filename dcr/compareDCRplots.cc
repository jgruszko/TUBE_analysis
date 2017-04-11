{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");

//  TString dsArr[6] = {"160_0", "150_0", "140_0", "140_1", "130_0", "470_1"};
//  TString distArr[6] = {"8.25", "9.75", "11.25", "11.25", "12.75", "inf"};

  TString dsArr[20] = {"50_0", "50_1", "50_2", "50_3", "50_4", "50_5", "50_6", "60_0", "60_1", "70_0", "70_1", "80_0", "80_1", "90_0", "100_0", "110_0"};
  double liveT[20] = {25.016, 24.516, 25.018, 24.975, 24.516, 24.519, 11.694, 24.016, 23.374, 25.016, 21.839, 25.022, 19.800, 24.727, 18.929, 25.107}; 
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
  TH1D* hDCR;
  TH1D* hDCRPZC;
  TH2D* hDCRvE;
  TH2D* hDCRPZCvE;

  TLegend* leg = new TLegend(.1, .1, .9, .9);
  TCanvas *c1 = new TCanvas("c1", "c1", 1200, 800);
  c1->Divide(3, 2);
  double nTurns, dist;

  for(int i = 0; i<7; i++){
    fName =  "$TUBEDIR/analysis/hists/DS";
    fName += dsArr[i];
    fName+=".root"; 
    fHist.Open(fName, "READ");
     
    //nTurns
    TString subStr(dsArr[i](0, dsArr[i].First('_')));
    nTurns = (double)subStr.Atoi()/10.;
    dist = (nTurns-21.5)*1.5; //in mm, assuming 21.5 is center
    
    hE = (TH1D*) gDirectory->Get("hE");
    hDCR = (TH1D*) gDirectory->Get("hDCR");
    hDCRPZC = (TH1D*) gDirectory->Get("hDCRPZC");
    hDCRvE = (TH2D*) gDirectory->Get("hDCRvE");
    hDCRPZCvE = (TH2D*) gDirectory->Get("hDCRPZCvE");
     

    hE->SetLineColor(colArr[i]);
    hDCR->SetLineColor(colArr[i]);
    hDCRPZC->SetLineColor(colArr[i]);
    hDCRvE->SetMarkerColor(colArr[i]);
    hDCRPZCvE->SetMarkerColor(colArr[i]);
    if(i == 0){
      c1->cd(1);
      gPad->SetLogy();
      hE->Draw("HIST");
      c1->cd(2);
      gPad->SetLogy();
      hDCR->Draw("HIST");
      c1->cd(3);
      hDCRvE->Draw();
      c1->cd(5);
      gPad->SetLogy();
      hDCRPZC->Draw("HIST");
      c1->cd(6);
      hDCRPZCvE->Draw();
    }
    else{
      c1->cd(1);
      hE->Draw("SAME HIST");
      c1->cd(2);
      hDCR->Draw("SAME HIST");
      c1->cd(3);
      hDCRvE->Draw("SAME");
      c1->cd(5);
      hDCRPZC->Draw("SAME HIST");
      c1->cd(6);
      hDCRPZCvE->Draw("SAME");
    } 

    legStr.Form("DS %s, source %f mm from P-contact", dsArr[i].Data(), dist);
    leg->AddEntry(hE, legStr, "L");
    fHist.Close(); 
  }
  c1->cd(4);
  leg->Draw();
  c1->Print("$TUBEDIR/analysis/hists/5turn_comparison_zoom.gif");
  
}
