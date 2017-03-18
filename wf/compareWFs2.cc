{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");

//  TString dsArr[6] = {"160_0", "150_0", "140_0", "140_1", "130_0", "470_1"};
//  TString distArr[6] = {"8.25", "9.75", "11.25", "11.25", "12.75", "inf"};

  TString dsArr[4] = {"120_0_3600_3700", "120_0_3100_3200", "120_0_2800_2900", "120_0_2200_2400"};
  TString eArr[4] = {"3.6-3.7", "3.2-3.2", "2.8-2.9", "2.2-2.4"};
  
  Color_t colArr[6] = {kRed, kOrange, kGreen, kBlue, kViolet, kBlack};
  TString fName;
  TFile fWF;
  TString legStr;
  
  TH1D* hAvg;
  TH1D* hTransAvg;
  TH1D* hNorm;
  TH1D* hArr[10];

  TLegend* leg = new TLegend(.1, .1, .9, .9);
  TCanvas *c1 = new TCanvas("c1", "c1", 800, 800);
  c1->Divide(2, 2);
  TCanvas* c2 = new TCanvas("c2", "c2", 1200, 400);
  c2->Divide(3, 1);
  TCanvas* canArr[4];
  TString histName;
  for(int i = 0; i<4; i++){
    fName =  "$TUBEDIR/analysis/wf/DS";
    fName += dsArr[i];
    fName+=".root"; 
    fWF.Open(fName, "READ");
   
    canArr[i] = new TCanvas();
    canArr[i]->cd();
    for(int j = 0; j<10; j++){
      histName.Form("hTrans%d", j);
      hArr[j] = (TH1D*)gDirectory->Get(histName);
      hArr[j]->SetLineColor(j+1);
      if( j == 0) hArr[j]->Draw("HIST");
      else hArr[j]->Draw("HIST SAME");
    }     
    hAvg = (TH1D*) gDirectory->Get("hBlAvg");
    hTransAvg = (TH1D*)  gDirectory->Get("hTransAvg");
    hNorm = (TH1D*)  gDirectory->Get("hNorm");
     
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
    hNorm->SetLineColor(colArr[i]);
    if(i == 0){
      hNorm->GetYaxis()->SetRangeUser(.95, 1.01);
      hNorm->GetXaxis()->SetRangeUser(9000, 30000);
      hNorm->SetTitle("Average of 10 Normalized PZ-Corrected WFs");
      hNorm->GetYaxis()->SetTitle("Normalized ADC (arb)");
      hNorm->DrawCopy("HIST");
      c2->cd(1);
      hNorm->DrawCopy("HIST");
    }
    else{
      hNorm->DrawCopy("SAME HIST");
      c2->cd(1);
      hNorm->DrawCopy("SAME HIST");
    }
    
    c2->cd(2);
    if(i == 0){
      hNorm->GetYaxis()->SetRangeUser(-.005, .02);
      hNorm->GetXaxis()->SetRangeUser(8000, 10000);
      hNorm->DrawCopy("HIST");
    }
    else hNorm->DrawCopy("HIST SAME");
    

    legStr.Form("DS %s, %s MeV", dsArr[i].Data(), eArr[i].Data());
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
 // c1->Print("~/MondayMeeting/wfComparison_allDS.gif");
//  c1->Print("~/MondayMeeting/normalizedWFComparison_allDS.gif");
  
  
  
}
