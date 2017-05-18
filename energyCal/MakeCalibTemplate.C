{
  //Create a root file with several template fitters that can be used at various levels of statistics and with/without tail parameters
  
  TFile fout("CalibFitTemplates.root", "RECREATE");
  GATMultiPeakFitter fitter;

  //Setup the biggest template
  fitter.AddRegion(220, 320, {238.632, 277.371, 300.087}); //3 peaks
  fitter.AddRegion(470, 550, {510.999}); //1 peak
  fitter.AddRegion(570, 650, {583.191}); //1 peak
  fitter.AddRegion(650, 760, {727.33}); //1 peak
  fitter.AddRegion(800, 880, {860.557}); //1 peak
  fitter.AddRegion(1420, 1490, {1460.822}); //1 peak
  fitter.AddRegion(2050, 2200, {2103.533}); //1 peak
  fitter.AddRegion(2550, 2800, {2614.533}); //1 peak
    
  fitter.SetBGPars(0, 0.1, 0, 0);
  fitter.FixBGPar(0, 2, 0);
  fitter.SetBGPars(1, 0.03, 0, 0);
  fitter.FixBGPar(1, 2, 0);
  fitter.SetBGPars(2, 0.02, 0, 0);
  fitter.FixBGPar(2, 2, 0);
  fitter.SetBGPars(3, 0.013, -0.00012, 0);
  fitter.FixBGPar(3, 2, 0);
  fitter.SetBGPars(4, 0.008, 0 , 0);
  //fitter.FixBGPar(3, 1, 0);
  fitter.FixBGPar(4, 2, 0);
  fitter.SetBGPars(5, 0.004, 0 , 0);
  fitter.FixBGPar(5, 2, 0);
  fitter.SetBGPars(6, 0.005, 6E-6, 0);
  fitter.FixBGPar(6, 2, 0);
  fitter.SetBGPars(7, 0.0008, 0, 0);
  fitter.FixBGPar(7, 2, 0);

  fitter.SetParFunction(GATMultiPeakFitter::kAmp, GATMultiPeakFitter::kFree, {
/*region 0*/ 5.8, .60, .75,  
/*region 1*/ 1.1,
/*region 2*/ 2.3,
/*region 3*/ .52,
/*region 4*/ .33,
/*region 5*/ .17,
/*region 6*/ .19,
/*region 7*/ 1 });
  //fitter.SetParFunction(GATMultiPeakFitter::kMu, GATMultiPeakFitter::kLinear, {0., 1.000});
  fitter.SetParFunction(GATMultiPeakFitter::kMu, GATMultiPeakFitter::kQuad, {0., 1, 0});
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

  //Template for low stats
  fitter.RemovePeak(0, 1);
  fitter.RemovePeak(0, 2);
  fitter.RemoveRegion(5);
  fitter.RemoveRegion(6);
  fitter.GetRegion(4)->SetRange(2550, 2800);
  fitter.AddPeak(5, 2614.533);
  fitter.SetPar(GATMultiPeakFitter::kAmp, 5, 1);

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
  
  gSystem->Exit(0);
}
