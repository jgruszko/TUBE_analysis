{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  TString ds[20] = {"470_0", "470_1", "470_2", "120_0", "130_0", "140_0", "140_1", "150_0", "150_1", "160_0", "160_1", "170_0", "180_0", "190_0", "190_1", "200_0", "210_0", "210_1", "210_2", "210_3"};
  double liveT[20] = {3.543, 22.688, 26.293, 24.516, 26.021, 21.015, 22.333, 20.910, 25.778, 5.378, 24.365, 19.093, 21.820, 31.004, 15.201, 21.014, 25.016, 25.016, 24.019, 19.675};
 
  TString fName =  "$TUBEDIR/analysis/efficiency/dcr90eff_byDS.txt";
  ofstream effFile(fName, ios::out | ios::app);
  if(!effFile.is_open()) cout<<"Failed to open output file."<<endl;

  TString selection = "channel == 112 && mClean == 1 && avse>-1 && nX<2";
  TString energySel = "&& trapEMPZCal>1000 && trapEMPZCal<5500";
  TString dcrSel = "&& dcr90>-.001";
  
  int ns_tot = 0, ns_cut= 0, nb_tot= 0, nb_cut= 0, nEntries = 0 ;
  double rs_tot, rs_cut, rb_tot, rb_cut;
  double tbkg = 0, tsig = 0;
  double eff = 0, err = 0, effBkg = 0, errBkg = 0;

  int nDS = 5;
  double nTurns, dist;
  TGraphErrors *gEff = new TGraphErrors(nDS-3);
  TCanvas* c1 = new TCanvas();

  for(int i = 0; i<nDS; i++){
    dcrSel = "&& dcr90>-.001";
    cout<<"Opening file "<<i<<endl; 
    TString fName = "$TUBEDIR/data/skim/skimDS";
    fName += ds[i];
    fName+=".root"; 
    TChain* skimTree = new TChain("skimTree");
    skimTree->AddFile(fName);
  
    //nTurns
    TString subStr(ds[i](0, ds[i].Find('_')));
    nTurns = (double)subStr.Atoi()/10.;
    dist = (nTurns-21.5)*1.5; //in mm, assuming 21.5 is center
  
  //time
    if(i<3) tbkg += liveT[i];
    else tsig = liveT[i];
 
    //N_total 
    cout<<"Getting N total"<<endl;
    nEntries = skimTree->GetEntries(selection+energySel+dcrSel);
    if(i<3) nb_tot += nEntries;
    else ns_tot = nEntries; 

    //R_total
    if(i == 2) rb_tot = nb_tot/tbkg;
    else if(i>2) rs_tot = ns_tot/tsig;   
 
    //N_cut 
    cout<<"Getting N cut"<<endl;
    dcrSel = "&& dcr90>0";
    nEntries = skimTree->GetEntries(selection+energySel+dcrSel);
    if(i<3) nb_cut += nEntries;
    else ns_cut = nEntries; 

    //R_cut
    if(i == 2) rb_cut = nb_cut/tbkg;
    else if(i>2) rs_cut = ns_cut/tsig;
   
    //efficiency
    if(i == 2){ eff = nb_cut/nb_tot; effBkg = eff; }
    else if (i>2) eff = (rs_cut - rb_cut)/(rs_tot - rb_tot);
    
    //error of efficiency-- binomial for now, switch to full bayesian using TGraphAsymmErrors?
    if(i == 2){ err = sqrt((eff)(1-eff)/(nb_tot)); errBkg = err; }
    else if(i>2) err = sqrt((eff)(1-eff)/(ns_tot-nb_tot));
 
    //print output to file
    if(i == 2) effFile<<std::setw(10)<<"470_all"<<std::setw(10)<< nb_tot << std::setw(10)<<rb_tot<<std::setw(10)<<nb_cut<<std::setw(10)<<rb_cut<<std::setw(10)<<eff<<std::setw(10)<<err<<endl;
    else if (i>2) effFile<<std::setw(10)<<ds[i]<<std::setw(10)<< ns_tot << std::setw(10)<<rs_tot<<std::setw(10)<<ns_cut<<std::setw(10)<<rs_cut<<std::setw(10)<<eff<<std::setw(10)<<err<<endl;
   
    //Fill graph of efficiencies
    if (i > 2){
     gEff->SetPoint(i-3, dist, eff); 
     gEff->SetPointError(i-3, 0, err); 
    }
  }
  gEff->SetMarkerStyle(7);
  gEff->SetTitle("DCR90 Efficiency");
  gEff->GetXaxis()->SetTitle("Source distance from P-contact (mm)");
  gEff->GetYaxis()->SetTitle("Rejection Efficiency");
  gEff->GetYaxis()->SetRangeUser(0, 1.2);

   gEff->Draw("AP");
   
   TLine* lBkg = new TLine(g1->GetXaxis()->GetBinLowEdge(g1->GetXaxis()->GetFirst()), effBkg, g1->GetXaxis()->GetBinUpEdge(g1->GetXaxis()->GetLast()), effBkg); 
   TBox* bBkg = new TBox(g1->GetXaxis()->GetBinLowEdge(g1->GetXaxis()->GetFirst()), effBkg - errBkg, g1->GetXaxis()->GetBinUpEdge(g1->GetXaxis()->GetLast()), effBkg - errBkg); 
   lBkg->SetLineColor(kBlue);
   bBkg->SetFillColor(kBlue);
   bBkg->SetFillStyle(3004);
  
   lBkg->Draw("SAME"):
   bBkg->Draw("SAME"):
   
   c1->Print("$TUBEDIR/analysis/efficiency/eff_byPos.gif");
   c1->Print("$TUBEDIR/analysis/efficiency/eff_byPos.pdf");
    
} 
