{
  const char* histfile = "./hist_DS190_1.root";
  const char* histname = "trapEMPZ";
  const char* templatefile = "CalibFitTemplates.root";
  //const char* templatename = "fitHighStats";
  const char* templatename = "fitAllPeaks";
  const char* outputfile = "fitResults_DS190_1.root";
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
	calMap.AddEntryToVectors(112, histname, 833, 894);
	         calMap.SetQuad(112, histname,  833, 894, cal2);
	        calMap.SetScale(112, histname,  833, 894, cal1);
	        calMap.SetOffset(112, histname, 833, 894, cal0);
	calMap.WriteCalibrationMapToTextFile("/global/projecta/projectdirs/majorana/data-projecta/TUBE/analysis/energyCal/calibrationMap.txt", std::ios_base::app);



  //Peak centroids with and without linear calibration fixed
  TF1 muFixed = fitter->GetParTF1(1);  
  fitter->SetParFunction(1, 0);
 
  cout<<"Re-fitting after freeing mu"<<endl; 
  fitter->FitToHists(0., verbose, false, true);
  fitter->Write("results_muFree");
  TGraphErrors muFits = fitter->GetParTGraph(1); 
   
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
  muFits.Write("mu_float");
  gFixed.Write("mu_fixed");
  gRes.Write("mu_residuals");


  }
