{
  //Create a root file with several template fitters that can be used at various levels of statistics and with/without tail parameters
  
  TFile fout("BkgFitTemplates.root", "RECREATE");
  GATMultiPeakFitter fitter;

  //Setup the biggest template
  fitter.AddRegion(270, 370, {295.223, 338.320, 351.932}); //3 peaks
  fitter.AddRegion(470, 550, {510.999}); //1 peak
  fitter.AddRegion(570, 650, {583.191, 609.32}); //2 peaks
  fitter.AddRegion(1100, 1350, {1120.294, 1155.210, 1238.122, 1280.976}); //4 peaks
  fitter.AddRegion(1420, 1490, {1460.822}); //1 peak
  fitter.AddRegion(1740, 1830, {1764.491}); //1 peak
  fitter.AddRegion(2140, 2270, {2204.059}); //1 peak
  fitter.AddRegion(2550, 2650, {2614.533}); //1 peak
    
  fitter.SetBGPars(0, 1, -0.008, 0);
  fitter.SetBGPars(1, 0.35, -0.0006, 0);
  fitter.FixBGPar(1, 2, 0);
  fitter.SetBGPars(2, 0.3, -0.0006, 0);
  fitter.FixBGPar(2, 2, 0);
  fitter.SetBGPars(3, 0.05, -0.00012, 0);
  fitter.SetBGPars(4, 0.03, 0 , 0);
  fitter.FixBGPar(4, 2, 0);
  fitter.SetBGPars(5, 0.01, 0, 0);
  fitter.FixBGPar(5, 2, 0);
  fitter.SetBGPars(6, 0.006, 0, 0);
  fitter.FixBGPar(6, 2, 0);
  fitter.SetBGPars(7, 0.0008, 0, 0);
  fitter.FixBGPar(7, 2, 0);

  fitter.SetParFunction(GATMultiPeakFitter::kAmp, GATMultiPeakFitter::kFree, {
 /*region 0*/ 1.8, .7, 4., 
 /*region 1*/ 1.6,
 /*region 2*/ 1.5, 4.5,
 /*region 3*/ 1.7, .5, 1.08, .5,
 /*region 4*/ 6.2,
 /*region 5*/ 1.8,
 /*region 6*/ .4,
 /*region 7*/ 1 });
  
  //fitter.SetParFunction(GATMultiPeakFitter::kMu, GATMultiPeakFitter::kLinear, {0., 1.000});
  fitter.SetParFunction(GATMultiPeakFitter::kMu, GATMultiPeakFitter::kQuad, {0., 1.000, 0.});
  fitter.SetParFunction(GATMultiPeakFitter::kSig, GATMultiPeakFitter::kRootQuad, {0.9, 0.03, 0.0003});

  fitter.SetParFunction(GATMultiPeakFitter::kFt, GATMultiPeakFitter::kConst, {0.3});
  fitter.LimitPar(GATMultiPeakFitter::kFt, 0, 0., 1.);
  fitter.SetParFunction(GATMultiPeakFitter::kTau, GATMultiPeakFitter::kLinear, {0., 0.0005});

  fitter.SetParFunction(GATMultiPeakFitter::kFht, GATMultiPeakFitter::kConst, {0.1});
  fitter.LimitPar(GATMultiPeakFitter::kFht, 0, 0., 1.);
  fitter.SetParFunction(GATMultiPeakFitter::kTauHT, GATMultiPeakFitter::kLinear, {0., 0.0005});

  fitter.SetParFunction(GATMultiPeakFitter::kHs, GATMultiPeakFitter::kStepHeightFun, {750, 0.03, -0.5});

  //This template has LE peaks. Need a DS worth of combined runs
  fitter.Write("fitAllPeaks");

  //Save versions with/without tails
  fitter.SetParFunction(GATMultiPeakFitter::kTauHT, GATMultiPeakFitter::kConst, {0.5});
  fitter.FixPar(GATMultiPeakFitter::kFht, 0, 0);
  fitter.FixPar(GATMultiPeakFitter::kTauHT, 0, 0.5);
  fitter.Write("fitAllPeaks_noHT");

  fitter.SetParFunction(GATMultiPeakFitter::kTau, GATMultiPeakFitter::kConst, {0.5});
  fitter.FixPar(GATMultiPeakFitter::kFt, 0, 0);
  fitter.FixPar(GATMultiPeakFitter::kTau, 0, 0.5);
  fitter.Write("fitAllPeaks_noTails");

  fitter.SetParFunction(GATMultiPeakFitter::kFht, GATMultiPeakFitter::kConst, {0.1});
  fitter.LimitPar(GATMultiPeakFitter::kFht, 0, 0., 1.);
  fitter.SetParFunction(GATMultiPeakFitter::kTauHT, GATMultiPeakFitter::kLinear, {0., 0.0005});
  fitter.FreePar(GATMultiPeakFitter::kTauHT, 0);
  fitter.Write("fitAllPeaks_noLT");
  
  fitter.SetParFunction(GATMultiPeakFitter::kFt, GATMultiPeakFitter::kConst, {0.1});
  fitter.LimitPar(GATMultiPeakFitter::kFt, 0, 0., 1.);
  fitter.SetParFunction(GATMultiPeakFitter::kTau, GATMultiPeakFitter::kLinear, {0., 0.0005});
  fitter.FreePar(GATMultiPeakFitter::kTau, 0);
/*
  //Template for low stats
  fitter.RemovePeak(0, 1);
  fitter.RemovePeak(2, 0);
  fitter.RemovePeak(3, 1);
  fitter.RemovePeak(3, 2);
  fitter.RemovePeak(3, 3);
  fitter.RemoveRegion(6);
  fitter.GetRegion(6)->SetRange(2550, 2800);
  fitter.AddPeak(7, 2614.533);
  fitter.SetPar(GATMultiPeakFitter::kAmp, 7, 1);

  fitter.FixBGPar(2, 2, 0);
  fitter.Write("fitLowStats");
  
  //Save versions with/without tails
  fitter.SetParFunction(GATMultiPeakFitter::kTauHT, GATMultiPeakFitter::kConst, {0.5});
  fitter.FixPar(GATMultiPeakFitter::kFht, 0, 0);
  fitter.FixPar(GATMultiPeakFitter::kTauHT, 0, 0.5);
  fitter.Write("fitLowStats_noHT");

  fitter.SetParFunction(GATMultiPeakFitter::kTau, GATMultiPeakFitter::kConst, {0.5});
  fitter.FixPar(GATMultiPeakFitter::kFt, 0, 0);
  fitter.FixPar(GATMultiPeakFitter::kTau, 0, 0.5);
  fitter.Write("fitLowStats_noTails");

  fitter.SetParFunction(GATMultiPeakFitter::kFht, GATMultiPeakFitter::kConst, {0.1});
  fitter.LimitPar(GATMultiPeakFitter::kFht, 0, 0., 1.);
  fitter.SetParFunction(GATMultiPeakFitter::kTauHT, GATMultiPeakFitter::kLinear, {0., 0.0005});
  fitter.FreePar(GATMultiPeakFitter::kTauHT, 0);
  fitter.Write("fitLowStats_noLT");
  
  fitter.SetParFunction(GATMultiPeakFitter::kFt, GATMultiPeakFitter::kConst, {0.1});
  fitter.LimitPar(GATMultiPeakFitter::kFt, 0, 0., 1.);
  fitter.SetParFunction(GATMultiPeakFitter::kTau, GATMultiPeakFitter::kLinear, {0., 0.0005});
  fitter.FreePar(GATMultiPeakFitter::kTau, 0);
 */ 
  gSystem->Exit(0);
}
