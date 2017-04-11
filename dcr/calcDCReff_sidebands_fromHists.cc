{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  TString ds[20] = {"50_0", "50_1", "50_2", "50_3", "50_4", "50_5", "50_6", "60_0", "60_1", "70_0", "70_1", "80_0", "80_1", "90_0", "100_0", "110_0"};
  double liveT[20] = {25.016, 24.516, 25.018, 24.975, 24.516, 24.519, 11.694, 24.016, 23.374, 25.016, 21.839, 25.022, 19.800, 24.727, 18.929, 25.107}; 
  //TString ds[20] = {"470_0", "470_1", "470_2", "120_0", "130_0", "140_0", "140_1", "150_0", "150_1", "160_0", "160_1", "170_0", "180_0", "190_0", "190_1", "200_0", "210_0", "210_1", "210_2", "210_3"};
  //double liveT[20] = {3.543, 22.688, 26.293, 24.516, 26.021, 21.015, 22.333, 20.910, 25.778, 5.378, 24.365, 19.093, 21.820, 31.004, 15.201, 21.014, 25.016, 25.016, 24.019, 19.675};  
  TString fName =  "../efficiency/dcrpzc90eff_byDS_fromNewHists.txt";
  ofstream effFile(fName, ios::out | ios::app);
  if(!effFile.is_open()) cout<<"Failed to open output file."<<endl;

  TString selection = "channel == 112 && mClean == 1 && avse>-1 && nX<2";
  TString energySel = "&& trapEMPZCal>1000 && trapEMPZCal<5500";
  TString dcrSel = "&& dcr90>-.001";
  
  int ns_tot = 0, ns_cut= 0, nb_tot= 0, nbR_cut = 0, nbR_tot = 0;
  double  npeak_expected = 0, npeak = 0, ncut_expected = 0, ncut_peak = 0;
  double nEntries = 0;
  double rs_tot, rs_cut, rb_tot, rb_cut, rpeak;
  double tbkg = 0, tsig = 0;
  double eff = 0, err = 0, effBkgR = 0, errBkg = 0, effBkgPk = 0;

  int nDS = 16;
  double nTurns, dist;
  TCanvas* c1 = new TCanvas();
  TCanvas* c2 = new TCanvas();
  TCanvas* c3 = new TCanvas();
  TGraphErrors *gEff = new TGraphErrors(nDS-3);
  TGraphErrors *gAlpha = new TGraphErrors(nDS-3);
  TGraphErrors *gEffBkgR = new TGraphErrors(nDS-3);
  TGraphErrors *gEffBkgPk = new TGraphErrors(nDS-3);

  int minBinE[16] = {4201, 4201, 4201, 4201, 4201, 4201, 4201, 4101, 4101, 3951, 3951, 3751, 3851, 3651, 3651, 3551}; 
  int maxBinE[16] = {4450, 4450, 4450, 4450, 4450, 4450, 4450, 4350, 4350, 4200, 4200, 4050, 4101, 3950, 3950, 3850};
  //int minBinE[12] = {3001, 3001, 3001, 3401, 3151, 2751, 3001, 2521, 2951, 2101, 2601, 2201};
 // int maxBinE[12] = {3800, 3750, 3750, 3750, 3450, 3150, 3350, 2900, 3220, 2800, 3000, 2750};
  int minBinDCR = 0;
  int maxBinDCR = 3000;
  int sideRmax;
  int sideRmin;
  bool kBkgFile = false;

  TH2D* hDCRPZCvE;
  TFile fHist, fBkg;
  double liveTbkg = liveT[2];  
  fName = "$TUBEDIR/analysis/hists/DS470_2.root";
  fBkg.Open(fName, "READ");
  TH2D* hDCRPZCvE_bkg = (TH2D*)gDirectory->Get("hDCRPZCvE");

  for(int i = 3; i<nDS; i++){
    kBkgFile = false;
    sideRmin = maxBinE[i]+1;
    sideRmax = sideRmin+500;
 
    minBinDCR = 0;
    cout<<"Opening file "<<i<<endl; 
    fName = "$TUBEDIR/analysis/hists/DS";
    fName += ds[i];
    fName+=".root"; 
    fHist.Open(fName, "READ");
    hDCRPZCvE = (TH2D*)gDirectory->Get("hDCRPZCvE");
     
    //nTurns
    TString subStr(ds[i](0, ds[i].First('_')));
    nTurns = (double)subStr.Atoi()/10.;
    dist = (nTurns-21.5)*1.5; //in mm, assuming 21.5 is center
   //number of events in sideband without cut
    //Use background run if energy band overlaps with Th spectrum
    if(minBinE[i]<2531){ 
       kBkgFile = true;
       sideRmin = minBinE[i];
       sideRmax = maxBinE[i];
       nbR_tot = hDCRPZCvE_bkg->Integral(sideRmin, sideRmax, minBinDCR, maxBinDCR);
       nbR_tot *= (liveT[i]/liveTbkg); 
       //number of background events expected in peak region
       npeak_expected = (double) nbR_tot;
    }
    //if not, use background run to estimate expected rate difference between sideband and peak region
    else{
      nbR_tot = hDCRPZCvE->Integral(sideRmin, sideRmax, minBinDCR, maxBinDCR); 
   
      //background run event excess in peak region 
      int nBkg_sideband = hDCRPZCvE_bkg->Integral(sideRmin, sideRmax, minBinDCR, maxBinDCR);
      int nBkg_peak = hDCRPZCvE_bkg->Integral(minBinE[i], maxBinE[i], minBinDCR, maxBinDCR);
      double nBkg_peakDiff = ((double)nBkg_peak/(maxBinE[i]-minBinE[i]+1))/((double)nBkg_sideband/(sideRmax-sideRmin+1)); //for every event/kev in the sideband, how many events/keV you get in the peak, in the background runs
      cout<<nBkg_peakDiff<<" peak events/keV expected for every background event/keV"<<endl;
      npeak_expected = ((double)nbR_tot/(sideRmax-sideRmin+1));//number of background events per keV in the sideband
      npeak_expected *= nBkg_peakDiff;//number of background events per keV expected in the peak region, using background run spectral shape
      npeak_expected *= (maxBinE[i]-minBinE[i]+1);//total background events expected in peak region
    } 

    
    cout<<nbR_tot<<" events in background"<<endl; 
    cout<<npeak_expected<<" background events expected in peak region"<<endl; 
    //N_total 
    ns_tot = hDCRPZCvE->Integral(minBinE[i], maxBinE[i], minBinDCR, maxBinDCR);
    cout<<ns_tot<<" events in peak region"<<endl; 

    npeak = ns_tot - npeak_expected;
    cout<<npeak<<" alpha events expected in peak region"<<endl; 
    rpeak = npeak/liveT[i]; 
    cout<<rpeak<<" alpha events per hour expected in peak region"<<endl; 
    //N_cut 
    minBinDCR = 1001;
   
    if(minBinE[i]<2531){ 
      nbR_cut = hDCRPZCvE_bkg->Integral(sideRmin, sideRmax, minBinDCR, maxBinDCR); 
      nbR_cut *= (liveT[i]/liveTbkg); 
      ncut_expected = nbR_cut;
    }
    
    else{
      nbR_cut = hDCRPZCvE->Integral(sideRmin, sideRmax, minBinDCR, maxBinDCR); 
      
      //come up with a similar correction for the number of background events cut in the peak region
      int nCut_sideband = hDCRPZCvE_bkg->Integral(sideRmin, sideRmax, minBinDCR, maxBinDCR); 
      int nCut_peak = hDCRPZCvE_bkg->Integral(minBinE[i], maxBinE[i], minBinDCR, maxBinDCR);
      double nCut_peakDiff = ((double)nCut_peak/(maxBinE[i]-minBinE[i]+1))/((double)nCut_sideband/(sideRmax-sideRmin+1)); //for every event/kev in the sideband, how many events/keV you get in the peak, in the background runs
      cout<<nCut_peakDiff<<" peak events/keV expected for every background event/keV"<<endl;
      ncut_expected = ((double)nbR_cut/(sideRmax-sideRmin+1));//number of cut background events per keV in the sideband
      ncut_expected *= nCut_peakDiff;//number of background events per keV expected in the peak region, using background run spectral shape
      ncut_expected *= (maxBinE[i]-minBinE[i]+1);//total cut background events expected in peak region
    }
    cout<<nbR_cut<<" events cut in background"<<endl; 
    cout<<ncut_expected<<" background events expected to be cut in peak region"<<endl; 
   
    effBkgR = (double)nbR_cut/nbR_tot;
    cout<<effBkgR<<" rejection in right sideband"<<endl; 
    
    effBkgPk = (double)ncut_expected/npeak_expected;
    cout<<effBkgPk<<"expected rejection for background events in peak"<<endl; 
     
    ns_cut = hDCRPZCvE->Integral(minBinE[i], maxBinE[i], minBinDCR, maxBinDCR);
    cout<<ns_cut<<" events cut in peak region"<<endl; 

    eff = (ns_cut - effBkgPk*(npeak_expected))/npeak;
    cout<<eff<<" efficiency in peak region"<<endl; 

    //error of efficiency-- binomial for now, switch to full bayesian using TGraphAsymmErrors?
    err = sqrt((eff)*(1-eff)/(npeak));
 
    //print output to file
   effFile<<kBkgFile<<std::setw(10)<<ds[i]<<std::setw(10)<< rpeak << std::setw(10)<< effBkgR <<std::setw(10)<< effBkgPk <<std::setw(15)<<eff<<endl;
   
    //Fill graph of efficiencies
    if (i > 2){
     gAlpha->SetPoint(i-3, dist, rpeak); 
     gEffBkgR->SetPoint(i-3, dist, effBkgR); 
     gEffBkgPk->SetPoint(i-3, dist, effBkgR); 
     gEff->SetPoint(i-3, dist, eff); 
     gEff->SetPointError(i-3, 0, err); 
    }
   fHist.Close();
  }
  c2->cd();
  gAlpha->SetMarkerStyle(7);
  gAlpha->SetTitle("Estimated Alpha Rates");
  gAlpha->GetXaxis()->SetTitle("Source distance from P-contact (mm)");
  gAlpha->GetYaxis()->SetTitle("Events/Hour");

   gAlpha->Draw("AP");

  c1->cd();
  gEff->SetMarkerStyle(7);
  gEff->SetTitle("DCRPZC90 Alpha Efficiency");
  gEff->GetXaxis()->SetTitle("Source distance from P-contact (mm)");
  gEff->GetYaxis()->SetTitle("Rejection Efficiency");
  gEff->GetYaxis()->SetRangeUser(0, 1.2);

   gEff->Draw("AP");
 
  c3->cd();
  gEffBkgR->SetMarkerStyle(22);
  gEffBkgR->SetMarkerColor(kRed);
  gEffBkgPk->SetMarkerStyle(23);
  gEffBkgPk->SetMarkerColor(kBlue);
  gEffBkgR->SetTitle("DCRPZC90 Bulk Rejection");
  gEffBkgR->GetXaxis()->SetTitle("Source distance from P-contact (mm)");
  gEffBkgR->GetYaxis()->SetTitle("Bulk Sacrifice");
  gEffBkgR->GetYaxis()->SetRangeUser(0, 1.2);

  gEffBkgR->Draw("AP");
  gEffBkgPk->Draw("P SAME");
   
 /*  TLine* lBkg = new TLine(gEff->GetXaxis()->GetBinLowEdge(gEff->GetXaxis()->GetFirst()), effBkg, gEff->GetXaxis()->GetBinUpEdge(gEff->GetXaxis()->GetLast()), effBkg); 
   TBox* bBkg = new TBox(gEff->GetXaxis()->GetBinLowEdge(gEff->GetXaxis()->GetFirst()), effBkg - errBkg, gEff->GetXaxis()->GetBinUpEdge(gEff->GetXaxis()->GetLast()), effBkg - errBkg); 
   lBkg->SetLineColor(kBlue);
   bBkg->SetFillColor(kBlue);
   bBkg->SetFillStyle(3004);
  
   lBkg->Draw("SAME");
   bBkg->Draw("SAME");
   */
   c1->Print("$TUBEDIR/analysis/efficiency/effDCRPZ_byPos_fromNew.gif");
   c1->Print("$TUBEDIR/analysis/efficiency/effDCRPZ_byPos_fromNew.pdf");
    
} 
