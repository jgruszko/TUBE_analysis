{
  TString dsArr[7] = {"10_0", "10_1", "10_2", "10_3", "10_4", "10_5", "10_6"};
// TString dsArr[46] = {"20_0", "30_0", "30_1", "40_0", "40_1", "40_2", "40_3","50_0", "50_1", "50_2", "50_3", "50_4", "50_5", "50_6", "60_0", "60_1", "70_0", "70_1", "80_0", "80_1", "90_0", "100_0", "110_0", "120_0", "130_0", "140_0", "140_1", "150_0", "150_1", "160_0", "160_1", "170_0", "180_0", "190_0", "190_1", "195_0", "195_1", "195_2", "200_0", "210_0", "210_1", "210_2", "210_3", "470_0", "470_1", "470_2"};
  int startRun[7] = {2576, 2626, 2676, 2726, 2776, 2826, 2876};
// int startRun[46] = {2340, 2246, 2296, 2060, 2110, 2160, 2210, 1737, 1786, 1836, 1886, 1936, 1986, 2036, 1639, 1688, 1536, 1586, 1446, 1496, 1395, 1357, 1306, 1151, 334, 180, 387, 137, 1099, 62, 1050, 1011, 956, 833, 895, 2404, 2454, 2514, 790, 600, 650, 700, 750, 227, 235, 281};
  int endRun[7] = {2625, 2675, 2725, 2775, 2825, 2875, 2907};
// int endRun[46] = {2403, 2295, 2339, 2109, 2159, 2209, 2245, 1786, 1835, 1885, 1935, 1985, 2035, 2059, 1688, 1736, 1585, 1629, 1495, 1535, 1445, 1394, 1356, 1199, 386, 224, 431, 179, 1150, 72, 1098, 1049, 1003, 894, 937, 2453, 2513, 2573, 831, 649, 699, 749, 789, 234, 280, 333};
 
 TString hName, outName;
 for(int i = 0; i<7 ; i++){
  hName = "./hist_DS";
  hName += dsArr[i];
  hName += ".root";  
  
  const char* histfile = hName.Data();
  const char* histname = "trapEMPZ";
  const char* templatefile = "BkgFitTemplates.root";
  //const char* templatename = "fitHighStats";
  const char* templatename = "fitAllPeaks";

  outName = "fitResults_DS";
  outName += dsArr[i];
  outName +=".root";

  const char* outputfile = outName.Data();
  double Elow_2614 = 1920;
  double Ehigh_2614 = 1940;
  //double Elow_2614 = 12600;
  //double Ehigh_2614 = 12950;
  bool verbose = false;
  
  // open histogram
  TFile::Open(histfile);
  TH1F* hist = (TH1F*)gFile->Get(histname)->Clone();

  // Get fit template
  TFile::Open(templatefile);
  GATMultiPeakFitter* fitter = (GATMultiPeakFitter*)gFile->Get(templatename)->Clone();
  fitter->SetHists(hist);

  // Setup output file
  TFile* fout = TFile::Open(outputfile, "RECREATE");
  hist->Write();
  fitter->Write("initialFitter");

  // Scale energy/amp
  hist->GetXaxis()->SetRangeUser(Elow_2614, Ehigh_2614);
  Int_t maxbin = hist->GetMaximumBin();
  Double_t scaleenergy = hist->GetBinCenter(maxbin)/2614.5333;
  Double_t scaleamps = hist->Integral()*hist->GetBinWidth(maxbin);
  hist->GetXaxis()->SetRange();
  fitter->ScaleEnergy(scaleenergy);
  fitter->ScaleAmps(scaleamps);

  //perform HMC burn-in to refine parameter guess
  GATHybridMonteCarlo hmc;
  hmc.SetNLLFunc(dynamic_pointer_cast<ROOT::Math::IGradientFunctionMultiDim>(fitter->SetPoissonLLFCN()));
  // uncomment the following lines to generate a TTree with each MCMC step
  hmc.SetOutputFile(fout);
  hmc.SetRecordPaths();
  hmc.SetParameters(std::vector<double>(fitter->Parameters(), fitter->Parameters()+fitter->NPar()));
  // Set the HMC parameters. Step size is leapfrog step size. Step length is number of leapfrog steps for each MCMC step. NSteps is number of MCMC steps. Adapt step size and parameter scales will automatically adjust step size and individual parameter scales for each step
  hmc.SetStepSize(0.02);
  hmc.SetStepLength(50);
  hmc.SetNSteps(200);
  hmc.SetAdaptStepSize();
  hmc.SetAdaptParScales();
  hmc.SetLimits(fitter->GetParLimits());
  // output the random seed to be used. This might be useful for troubleshooting
  cout << "Random seed: " << hmc.GetCurrentSeed() << endl;
  // true -> verbose output that llists the negative loglikelihood at each step and the step size
  hmc.DoMCMC(verbose);
  fitter->SetParameters(hmc.GetLikeliestPars().data());
  fitter->Write("hmcResults");

  //perform a minuit fit using the most likely parameters found during the HMC as the initial parameters. Use minos error estimation
  fitter->FitToHists(0., verbose, false, true);
  fitter->Write("results");

//print out calibration parameters
  double cal0, cal1, cal2;
  const double* parList = fitter->GetParsForPar(1);
  cal0 = parList[0];
  cal1 = parList[1];
  cal2 = parList[2];
  cout << "Calibration parameters: " << cal0 << ' ' << cal1 << ' ' << cal2 <<endl;

	//Write calibration params to ascii file.
	GATCalibrationMapAscii calMap;
  int startrun = startRun[i];
  int endrun = endRun[i];

	calMap.AddEntryToVectors(112, histname, startrun, endrun);
   	 calMap.SetQuad(112, histname,  startrun, endrun, cal2);
	calMap.SetScale(112, histname,  startrun, endrun, cal1);
	calMap.SetOffset(112, histname, startrun, endrun, cal0);
	calMap.WriteCalibrationMapToTextFile("/global/projecta/projectdirs/majorana/data-projecta/TUBE/analysis/energyCal/calibrationMap.txt", std::ios_base::app);



  //Peak centroids with and without linear calibration fixed
  cout<<"Trying to get function"<<endl; 
  TF1 muFixed = fitter->GetParTF1(1);  
  fitter->SetParFunction(1, 0);
 
  cout<<"Re-fitting after freeing mu"<<endl; 
  fitter->FitToHists(0., verbose, false, true);
  fitter->Write("results_muFree");
  TGraphErrors muFits = fitter->GetParTGraph(1); 
  
  TCanvas* c1 = new TCanvas(); 
  muFits.Draw("PA");
  muFixed.Draw("SAME");

  //Make a TGraphErrors out of the TF1
  int nPoints = muFits.GetN();
  Double_t xVal, yVal1, yVal2;
  TGraphErrors gFixed(nPoints);
  TGraphErrors gRes(nPoints);
  for(int i = 0; i<nPoints; i++){
    muFits.GetPoint(i, xVal, yVal1);
    gFixed.SetPoint(i, xVal, muFixed.Eval(xVal, yVal2));
  }
  for(int i = 0; i<nPoints; i++){
    muFits.GetPoint(i, xVal, yVal2);
    gFixed.GetPoint(i, xVal, yVal1);
    gRes.SetPoint(i, xVal, yVal2-yVal1);
  }
  gRes.SetMarkerStyle(20);
  gRes.Draw("AP");
 
  outName = "muResiduals_DS";
  outName += dsArr[i];
  outName +=".gif";
  
  c1->Print(outName);

  muFits.Write("mu_float");
  gFixed.Write("mu_fixed");
  gRes.Write("mu_residuals");

 }
 gSystem->Exit(0);
  }
