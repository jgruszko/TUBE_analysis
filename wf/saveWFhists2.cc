{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  TString ds = "120_0";
  TString fName = "$TUBEDIR/data/skim/skimDS";
  fName += ds;
  fName+=".root"; 
  TChain* skimTree = new TChain("skimTree");
  skimTree->AddFile(fName);
  GATWaveformBrowser* b= new GATWaveformBrowser();
  
  
  double par0 = 0.000962482;
  double par1 = 7.07476e-07;
  int eLow = 2200;
  int eHi =2400;
  double dcrLow = -.002+eLow*par1+par0;
  double dcrHi = .0002+eHi*par1+par0;

  fName.Form("$TUBEDIR/analysis/wf/DS%s_%d_%d.root", ds.Data(), eLow, eHi); 
  TFile* fOut = new TFile(fName, "RECREATE");
  
  TString energySel;
  energySel.Form("&& trapEMPZCal>%d && trapEMPZCal<%d", eLow, eHi);
  TString dcrSel;
  dcrSel.Form("&& dcr90>%E && dcr90<%E", dcrLow, dcrHi);

  TString selection = "channel == 112 && mClean == 1 && avse>0 && nX<2";

  TCanvas* c1 = new TCanvas("c1", "c1", 1200, 800);
  c1->Divide(3, 2);
  TCanvas* c2 = new TCanvas();
  
  //Load waveforms in each wf browser (use mean +- sigma for E and dcr fits)
  b->LoadSkimWaveforms(skimTree, selection+energySel+dcrSel, "fWaveforms", 100);
/* 
 160:
  energy = (2640, 2810)
  dcr = (.00188, .00289)
 * 150:
  energySel = "&& trapEMPZCal>2742 && trapEMPZCal<2914";
  dcrSel = "&& dcr90>.00215 && dcr90<.00257";
  140_0: 
  energySel = "&& trapEMPZCal>2967 && trapEMPZCal<3107";
  dcrSel = "&& dcr90>.00250 && dcr90<.00277";
  //b140_0->LoadSkimWaveforms(skimTree140_0, selection+energySel+dcrSel, "fWaveforms", 100);
  140_1:
  energySel = "&& trapEMPZCal>3203 && trapEMPZCal<3335";
  dcrSel = "&& dcr90>.00274 && dcr90<.00297";
  b140_1->LoadSkimWaveforms(skimTree140_1, selection+energySel+dcrSel, "fWaveforms", 100);
  130:
  energySel = "&& trapEMPZCal>3384 && trapEMPZCal<3482";
  dcrSel = "&& dcr90>.00285 && dcr90<.00313";
  b130->LoadSkimWaveforms(skimTree130, selection+energySel+dcrSel, "fWaveforms", 100);
  470_1:
  energySel = "&& trapEMPZCal>2610 && trapEMPZCal<2630";
  dcrSel = "&& dcr90>-.0005 && dcr90<.0005";
  *///b470_1->LoadSkimWaveforms(skimTree470_1, selection+energySel+dcrSel, "fWaveforms", 100);
  //Draw hists for each of the first 10 waveforms

  cout<<b->GetNWaveforms()<<" waveforms loaded."<<endl;
  gStyle->SetOptStat(0);

  TString hName; 
  TH1D hTemp;
  TH1D hBlTemp;
  TH1D hTransTemp;
  TH1D h[10];
  TH1D hBl[10];
  TH1D hTrans[10];
  TH1D* hAvg;
  TH1D* hBlAvg;
  TH1D* hTransAvg;
  TH1D hNorm;

  MGWFBaselineRemover blr;
  blr.SetBaselineSamples(500);

  MGWFPoleZeroCorrection pzc;
  pzc.SetDecayConstant(44.224*CLHEP::us);
 
  MGTWaveform wfOut; 
  MGTWaveform wfOut2; 
  vector<MGTWaveform> waves; 
  vector<MGTWaveform> waves2; 
  vector<MGTWaveform> waves3;

  int nEntries = b->GetNWaveforms();
  if(nEntries>10) nEntries = 10;
 
  for(int i = 0; i<nEntries; i++){
    waves.push_back(*(b->GetWaveform(i)));
    waves.at(i).SetTOffset(0);
    waves.at(i).Draw();
    blr.Transform(waves.at(i), wfOut);
    waves3.push_back(wfOut);
    pzc.Transform(wfOut, wfOut2);
    waves2.push_back(wfOut2);
   
    c1->cd(1); 
    h[i] = *(waves.at(i).GimmeUniqueHist());
    hBl[i] = *(waves3.at(i).GimmeUniqueHist());
    h[i].SetLineColor(kRed);
    if(i == 0){
      h[0].SetTitle("Sample Alpha WFs");
      hBl[0].SetTitle("Sample Alpha WFs");
      h[0].Draw();
    }
    else h[i].Draw("SAME");
    
    c1->cd(2);
    hTrans[i] = *(waves2.at(i).GimmeUniqueHist());
    hTrans[i].SetLineColor(kRed);
    if(i == 0){
      hTrans[0].SetTitle("Sample PZ-corrected Alpha WFs");
      hTrans[0].Draw();
    }
    else hTrans[i].Draw("SAME");

  //Average waveforms
    if(i == 0){
      hTemp = h[0];
      hAvg = &hTemp;
      hBlTemp = hBl[0];
      hBlAvg = &hBlTemp;
      hTransTemp = hTrans[0];
      hTransAvg = &hTransTemp;
    }
    else{
      hAvg->Add(&h[i]);
      hBlAvg->Add(&hBl[i]);
      hTransAvg->Add(&hTrans[i]);
    }
//Write hists to file
  fOut->cd();
  hName.Form("h%d", i);
  h[i].Write(hName);
  hName.Form("hBl%d", i);
  hBl[i].Write(hName);
  hName.Form("hTrans%d", i);
  hTrans[i].Write(hName);
  

  }
  c1->cd(3);
  hAvg->SetTitle("Average of 10 Alpha WFs");
  hAvg->Scale(1.0/10.0);
  hAvg->SetLineColor(kRed);
  hAvg->Draw("HIST");

  hBlAvg->SetTitle("Average of 10 Alpha WFs");
  hBlAvg->Scale(1.0/10.0);
  hBlAvg->SetLineColor(kRed);
  hBlAvg->Draw("HIST");
  
  c1->cd(4);
  hTransAvg->SetTitle("Average of 10 PZ-Corrected Alpha WFs");
  hTransAvg->Scale(1.0/10.0);
  hTransAvg->SetLineColor(kRed);
  hTransAvg->Draw("HIST");

  c1->cd(5);
  hNorm = *hTransAvg;
  hNorm.SetTitle("Normalized Average of 10 PZ-Corrected Alpha WFs");
  hNorm.Scale(1.0/hNorm.At(hNorm.FindLastBinAbove(0)));
  hNorm.Draw("HIST");
/*
  c2->cd();
  TString hTitle = "Data Set ";
  hTitle += ds;
  hTitle += " Events";
  energySel = "&& trapEMPZCal>1000 && trapEMPZCal<4500";
  dcrSel = "&& dcr90>-.005 && dcr90<.01";
  skimTree->Draw("dcr90:trapEMPZCal>>hE(3500, 1000, 4500, 1000,-.01, .01)", selection+energySel+dcrSel);
  TH1D* hE = (TH1D*) gDirectory->Get("hE");
  hE->SetTitle(hTitle);
  hE->GetXaxis()->SetTitle("Energy (keV)");
  hE->GetYaxis()->SetTitle("DCR (ADC/ns)");
  
  c1->cd(6);
  hE->Draw();
  TBox* box1 = new TBox(eLow, dcrLow, eHi, dcrHi);
  box1->SetFillStyle(0);
  box1->SetLineColor(kRed);
  box1->Draw("SAME");
*/
 
  fOut->cd();
  hAvg->Write("hAvg");
  hBlAvg->Write("hBlAvg");
  hTransAvg->Write("hTransAvg");
  hNorm.Write("hNorm");
  fOut->Close();
 
} 
