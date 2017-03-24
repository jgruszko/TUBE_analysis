{
  const char* histfile = "./hist_1630_1638.root";
  const char* histname = "trapEMPZ";
  const char* templatefile = "FitTemplates.root";
  //const char* templatename = "fitHighStats";
  const char* templatename = "fitNormal_noTails";
  const char* outputfile = "fitResults_noTails_1630_1638.root";
  double Elow_2614 = 1920;
  double Ehigh_2614 = 1945;
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
  double cal0, cal1;
  fitter->GetCalibrationPars(cal0, cal1);
  cout << "Calibration parameters: " << cal0 << ' ' << cal1 << endl;

	//Write calibration params to ascii file.
	GATCalibrationMapAscii calMap;
	calMap.AddEntryToVectors(112, histname, 1630, 1638);
	calMap.SetScale(112, histname, 1630, 1638, cal1);
	calMap.SetOffset(112, histname, 1630, 1638, cal0);
	calMap.WriteCalibrationMapToTextFile("/global/projecta/projectdirs/majorana/data-projecta/TUBE/analysis/energyCal/calibrationMap.txt", std::ios_base::app);


}
