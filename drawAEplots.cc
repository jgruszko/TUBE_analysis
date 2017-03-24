{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  TString fName = "$TUBEDIR/data/skim/skimCalibDS1.root";
  TChain* skimTree = new TChain("skimTree", "");
  skimTree->AddFile(fName);
 
  double liveT = 17.8678; //live time in hours 
  TString selection = "channel == 112";
  TString energySel = "&& trapEMPZCal>100 && trapEMPZCal<3000";

  gStyle->SetOptStat(0);

  skimTree->Draw("trapEMPZCal>>h1(2900, 100, 3000)", selection+energySel);
  h1->SetTitle("228Th Calib., Pump On");
  h1->GetXaxis()->SetTitle("Energy (keV)");
  h1->GetYaxis()->SetTitle("Counts/hr/keV");
  h1->Scale(1./liveT);
 
  TCanvas* c1 = new TCanvas();
  c1->SetLogy();
  TLegend* leg = new TLegend(.1, .1, .5, .4);
  leg->AddEntry(h1, "No cuts");
    
  selection += " && mClean == 1";
  skimTree->Draw("trapEMPZCal>>h2(2900, 100, 3000)", selection+energySel);
  h2->SetLineColor(2);
  leg->AddEntry(h2, "mClean == 1");
  
  selection += " && avse>0";
  skimTree->Draw("trapEMPZCal>>h3(2900, 100, 3000)", selection+energySel);
  h3->SetLineColor(3);
  leg->AddEntry(h3, "mClean == 1 && avse>0");
  
  selection += " && nX<2";
  skimTree->Draw("trapEMPZCal>>h4(2900, 100, 3000)", selection+energySel);
  h4->SetLineColor(4);
  leg->AddEntry(h4, "mClean == 1 && avse>0 && nX<2");

  h1->Draw();
  h2->Draw("SAME");
  h3->Draw("SAME");
  h4->Draw("SAME");
  leg->Draw();

}
