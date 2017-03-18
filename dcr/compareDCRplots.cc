{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");

//  TString dsArr[6] = {"160_0", "150_0", "140_0", "140_1", "130_0", "470_1"};
//  TString distArr[6] = {"8.25", "9.75", "11.25", "11.25", "12.75", "inf"};

  TString dsArr[3] = {"160_0", "130_0", "470_1"};
  TString distArr[3] = {"8.25", "12.75", "inf"};
  
 // Color_t colArr[6] = {kRed, kOrange, kGreen, kBlue, kViolet, kBlack};
  Color_t colArr[3] = {kRed,kBlue, kGreen};
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

  for(int i = 0; i<3; i++){
    fName =  "$TUBEDIR/analysis/hists/DS";
    fName += dsArr[i];
    fName+=".root"; 
    fHist.Open(fName, "READ");
     
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

    legStr.Form("DS %s, source %s mm from P-contact", dsArr[i].Data(), distArr[i].Data());
    else leg->AddEntry(hAvg, legStr, "L");
    fHist.Close(); 
  }
  c1->cd(4);
  leg->Draw();
  c1->Print("$TUBEDIR/analysis/hists/compare_4DS.gif");
  
}
