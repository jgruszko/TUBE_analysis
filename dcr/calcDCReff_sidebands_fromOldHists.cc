{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  TString ds[20] = {"470_0", "470_1", "470_2", "120_0", "130_0", "140_0", "140_1", "150_0", "150_1", "160_0", "160_1", "170_0", "180_0", "190_0", "190_1", "200_0", "210_0", "210_1", "210_2", "210_3"};
  double liveT[20] = {3.543, 22.688, 26.293, 24.516, 26.021, 21.015, 22.333, 20.910, 25.778, 5.378, 24.365, 19.093, 21.820, 31.004, 15.201, 21.014, 25.016, 25.016, 24.019, 19.675};  
  TString fName =  "../efficiency/dcr90eff_byDS.txt";
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
  double eff = 0, err = 0, effBkgR = 0, errBkg = 0;

  int nDS = 12;
  double nTurns, dist;
  TCanvas* c1 = new TCanvas();
  TCanvas* c2 = new TCanvas();
  TGraphErrors *gEff = new TGraphErrors(nDS-3);
  TGraphErrors *gAlpha = new TGraphErrors(nDS-3);

  int minBinE[12] = {3001, 3001, 3001, 3001, 2751, 2351, 2601, 2121, 2451, 1701, 2201, 1801};
  int maxBinE[12] = {3400, 3350, 3350, 3350, 3050, 2750, 2950, 2500, 2820, 2400, 2600, 2350};
  int minBinDCR = 301;
  int maxBinDCR = 1000;
  int sideRmax = 5500;
  int sideRmin;
  bool kBkgFile = false;

  TH2D* hDCRvE;
  TFile fHist, fBkg;
  double liveTbkg = liveT[2];  
  fName = "$TUBEDIR/analysis/hists/040617/DS470_2.root";
  fBkg.Open(fName, "READ");
  TH2D* hDCRvE_bkg = (TH2D*)gDirectory->Get("hDCRvE");

  for(int i = 3; i<nDS; i++){
    kBkgFile = false;
    sideRmin = maxBinE[i]+1;
    sideRmax = 5500;
 
    minBinDCR = 301;
    cout<<"Opening file "<<i<<endl; 
    fName = "$TUBEDIR/analysis/hists/040617/DS";
    fName += ds[i];
    fName+=".root"; 
    fHist.Open(fName, "READ");
    hDCRvE = (TH2D*)gDirectory->Get("hDCRvE");
     
    //nTurns
    TString subStr(ds[i](0, ds[i].First('_')));
    nTurns = (double)subStr.Atoi()/10.;
    dist = (nTurns-21.5)*1.5; //in mm, assuming 21.5 is center

   //number of events in sideband without cut
    //Use background run if energy band overlaps with Th spectrum
    if(minBinE[i]<2131){ 
       kBkgFile = true;
       sideRmin = minBinE[i];
       sideRmax = maxBinE[i];
       nbR_tot = hDCRvE_bkg->Integral(sideRmin, sideRmax, minBinDCR, maxBinDCR);
       nbR_tot *= (liveT[i]/liveTbkg); 
      
       //number of background events expected in peak region
       npeak_expected = (double) nbR_tot;
    }
    //if not, use background run to estimate expected rate difference between sideband and peak region
    else{
      nbR_tot = hDCRvE->Integral(sideRmin, sideRmax, minBinDCR, maxBinDCR); 
   
      //background run event excess in peak region 
      int nBkg_sideband = hDCRvE_bkg->Integral(sideRmin, sideRmax, minBinDCR, maxBinDCR);
      int nBkg_peak = hDCRvE_bkg->Integral(minBinE[i], maxBinE[i], minBinDCR, maxBinDCR);
      double nBkg_peakDiff = ((double)nBkg_peak/(maxBinE[i]-minBinE[i]+1))/((double)nBkg_sideband/(sideRmax-sideRmin+1)); //for every event/kev in the sideband, how many events/keV you get in the peak, in the background runs
      cout<<nBkg_peakDiff<<" peak events/keV expected for every background event/keV"<<endl;
      npeak_expected = ((double)nbR_tot/(sideRmax-sideRmin+1));//number of background events per keV in the sideband
      npeak_expected *= nBkg_peakDiff;//number of background events per keV expected in the peak region, using background run spectral shape
      npeak_expected *= (maxBinE[i]-minBinE[i]+1);//total background events expected in peak region
    } 

    cout<<nbR_tot<<" events in background"<<endl; 
    cout<<npeak_expected<<" background events expected in peak region"<<endl; 
    
    //N_total 
    ns_tot = hDCRvE->Integral(minBinE[i], maxBinE[i], minBinDCR, maxBinDCR);
    cout<<ns_tot<<" events in peak region"<<endl; 

    npeak = ns_tot - npeak_expected;
    cout<<npeak<<" alpha events expected in peak region"<<endl; 
    rpeak = npeak/liveT[i]; 
    cout<<rpeak<<" alpha events per hour expected in peak region"<<endl; 
    //N_cut 
    minBinDCR = 335;
   
    if(minBinE[i]<2131){ 
      nbR_cut = hDCRvE_bkg->Integral(sideRmin, sideRmax, minBinDCR, maxBinDCR); 
      nbR_cut += (1./3)*hDCRvE_bkg->Integral(sideRmin, sideRmax, 334, 334);
      nbR_cut *= (liveT[i]/liveTbkg); 
    }
    
    else{
      nbR_cut = hDCRvE->Integral(sideRmin, sideRmax, minBinDCR, maxBinDCR); 
      nbR_cut += (1./3)*hDCRvE->Integral(sideRmin, sideRmax, 334, 334);
    }
    cout<<nbR_cut<<" events cut in background"<<endl; 
   
    effBkgR = (double)nbR_cut/nbR_tot;
    cout<<effBkgR<<" efficiency in right sideband"<<endl; 
//    effBkgL = (double)nbL_cut/nbL_tot;
 //   cout<<effBkgL<<" efficiency in left sideband"<<endl; 
     
    ns_cut = hDCRvE->Integral(minBinE[i], maxBinE[i], minBinDCR, maxBinDCR);
    ns_cut += (1./3)*hDCRvE->Integral(minBinE[i], maxBinE[i], 334, 334);
    cout<<ns_cut<<" events cut in peak region"<<endl; 

    eff = (ns_cut - effBkgR*(npeak_expected))/npeak;
    cout<<eff<<" efficiency in peak region"<<endl; 

    //error of efficiency-- binomial for now, switch to full bayesian using TGraphAsymmErrors?
    err = sqrt((eff)*(1-eff)/(npeak));
 
    //print output to file
   effFile<<kBkgFile<<std::setw(10)<<ds[i]<<std::setw(10)<< rpeak << std::setw(10)<< effBkgR <<std::setw(15)<<eff<<endl;
   
    //Fill graph of efficiencies
    if (i > 2){
     gAlpha->SetPoint(i-3, dist, rpeak); 
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
  gEff->SetTitle("DCR90 Efficiency");
  gEff->GetXaxis()->SetTitle("Source distance from P-contact (mm)");
  gEff->GetYaxis()->SetTitle("Rejection Efficiency");
  gEff->GetYaxis()->SetRangeUser(0, 1.2);

   gEff->Draw("AP");
   
 /*  TLine* lBkg = new TLine(gEff->GetXaxis()->GetBinLowEdge(gEff->GetXaxis()->GetFirst()), effBkg, gEff->GetXaxis()->GetBinUpEdge(gEff->GetXaxis()->GetLast()), effBkg); 
   TBox* bBkg = new TBox(gEff->GetXaxis()->GetBinLowEdge(gEff->GetXaxis()->GetFirst()), effBkg - errBkg, gEff->GetXaxis()->GetBinUpEdge(gEff->GetXaxis()->GetLast()), effBkg - errBkg); 
   lBkg->SetLineColor(kBlue);
   bBkg->SetFillColor(kBlue);
   bBkg->SetFillStyle(3004);
  
   lBkg->Draw("SAME");
   bBkg->Draw("SAME");
   */
   c1->Print("$TUBEDIR/analysis/efficiency/eff_byPos.gif");
   c1->Print("$TUBEDIR/analysis/efficiency/eff_byPos.pdf");
    
} 
