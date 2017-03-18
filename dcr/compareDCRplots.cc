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
  TCanvas *c1 = new TCanvas("c1", "c1", 800, 800);
  c1->Divide(2, 2);
  TCanvas* c2 = new TCanvas("c2", "c2", 1200, 400);
  c2->Divide(3, 1);

  for(int i = 0; i<3; i++){
    fName =  "$TUBEDIR/analysis/hists/DS";
    fName += dsArr[i];
    fName+=".root"; 
    fHist.Open(fName, "READ");
     
    hAvg = (TH1D*) gDirectory->Get("hBlAvg");
    hTransAvg = (TH1D*)  gDirectory->Get("hTransAvg");
     
    c1->cd(1);

    hAvg->SetLineColor(colArr[i]);
    if(i == 0){
      hAvg->GetYaxis()->SetRangeUser(-100, 2700);
      hAvg->SetTitle("Average of 10 WFs");
      hAvg->Draw("HIST");
    }
    else hAvg->Draw("SAME HIST");
    
    c1->cd(2);
    hTransAvg->SetLineColor(colArr[i]);
    if(i == 0){
      hTransAvg->GetYaxis()->SetRangeUser(-100, 2700);
      hTransAvg->SetTitle("Average of 10 PZ-Corrected WFs");
      hTransAvg->Draw("HIST");
    }
    else hTransAvg->Draw("SAME HIST");
    
    c1->cd(3);
    if(i == 0){
      hNorm = (TH1D*)  gDirectory->Get("hNorm");
    hNorm->SetLineColor(colArr[i]);
      hNorm->GetYaxis()->SetRangeUser(0, 1.1);
      hNorm->GetXaxis()->SetRangeUser(9000, 12000);
      hNorm->SetTitle("Average of 10 Normalized PZ-Corrected WFs");
      hNorm->GetYaxis()->SetTitle("Normalized ADC (arb)");
      hNorm->DrawCopy("HIST");
      c2->cd(1);
      hNorm->DrawCopy("HIST");
    }
    else{
      if(i==2)  hNorm = (TH1D*)  gDirectory->Get("hTrans4");
      else hNorm = (TH1D*)  gDirectory->Get("hNorm");
      hNorm->Scale(1./hNorm->At(hNorm->FindLastBinAbove(0)));
      hNorm->SetLineColor(colArr[i]);
      hNorm->DrawCopy("SAME HIST");
      c2->cd(1);
      hNorm->DrawCopy("SAME HIST");
    }
    
    c2->cd(2);
    if(i == 0){
      hNorm->GetYaxis()->SetRangeUser(0, 1.1);
      hNorm->GetXaxis()->SetRangeUser(9000, 12000);
      hNorm->DrawCopy("HIST");
    }
    else hNorm->DrawCopy("HIST SAME");
    

    legStr.Form("DS %s, source %s mm from P-contact", dsArr[i].Data(), distArr[i].Data());
    if(i == 5) leg->AddEntry(hAvg, "DS470_1, no alpha source", "L");
    else leg->AddEntry(hAvg, legStr, "L");
    fWF.Close(); 
  }
  c1->cd(4);
  leg->Draw();
  c2->cd(3);
  leg->Draw();
  //c1->Print("$TUBEDIR/analysis/wf/wfComparison_allDS.gif");
  //c1->Print("$TUBEDIR/analysis/wf/normalizedWFComparison_allDS.gif");
//  c1->Print("~/MondayMeeting/wfComparison_allDS.gif");
//  c1->Print("~/MondayMeeting/normalizedWFComparison_allDS.gif");
  
  
  
}
