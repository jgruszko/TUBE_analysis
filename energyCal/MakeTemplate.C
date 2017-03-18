{
  //Create a root file with several template fitters that can be used at various levels of statistics and with/without tail parameters
  
  TFile fout("FitTemplates.root", "RECREATE");
  GATMultiPeakFitter fitter;

  //Setup the biggest template
  fitter.AddRegion(70, 81, {74.815, 77.107}); // 2 peaks
  fitter.AddRegion(82, 100, {84.373, 86.83, 87.349, 89.784}); //4 peaks
  fitter.AddRegion(110, 120, {115.183});//1 peak
  fitter.AddRegion(200, 340, {215.983, 233.36, 238.632, 240.986, 252.61, 277.371, 288.2, 300.087, 328.03});//9 peaks
  fitter.AddRegion(420, 490, {452.98}); //1 peak
  fitter.AddRegion(530, 620, {549.73, 583.191}); //2 peaks
  fitter.AddRegion(710, 800, {722.04, 727.330, 763.130, 785.370});//4 peaks
  fitter.AddRegion(850, 1100, {860.557, 893.408, 927.6, 952.12, 982.7, 1078.62, 1093.9}); //7 peaks
  fitter.AddRegion(1450, 1570, {1512.7}); //1 peak
  fitter.AddRegion(1600, 1675, {1620.738}); //1 peak
  fitter.AddRegion(2550, 2800, {2614.533}); //1 peak
    
  fitter.SetBGPars(0, 0.088, 0.0013, 0);
  fitter.FixBGPar(0, 2, 0);
  fitter.SetBGPars(1, 0.098, 0.0011, 0);
  fitter.FixBGPar(1, 2, 0);
  fitter.SetBGPars(2, 0.13, 0.0004, 0);
  fitter.FixBGPar(2, 2, 0);
  fitter.SetBGPars(3, 0.070,-0.0003, 0);
  fitter.SetBGPars(4, 0.028, -0.0001, 0);
  fitter.SetBGPars(5, 0.012, 0, 0);
  fitter.SetBGPars(6, 0.0058, 0, 0);
  fitter.SetBGPars(7, 0.0037, 0, 0);
  fitter.SetBGPars(8, 0.0032, 0, 0);
  fitter.SetBGPars(9, 0.0037, 0, 0);
  fitter.SetBGPars(10, 8e-05, 0, 0);
  fitter.FixBGPar(10, 2, 0);

  fitter.SetParFunction(GATMultiPeakFitter::kAmp, GATMultiPeakFitter::kFree, {/*region 0*/ 0.0018, 0.0051,/*region 1*/ 0.0018, 0.0027, 0.0068, 0.0036,/*region 2*/ 0.011,/*region 3*/ 0.018, 0.0061, 3.14, 0.30, 0.021, 0.16, 0.023, 0.24, 0.0082,/*region 4*/ 0.023,/*region 5*/ 0.0066, 1.77,/*region 6*/ 0.0047, 0.36, 0.033, 0.056,/*region 7*/ 0.24, 0.018, 0.0024, 0.0085, 0.0036, 0.025, 0.0079,/*region 8*/ 0.012,/*region 9*/ 0.058,/*region 10*/ 1 });
  
  fitter.SetParFunction(GATMultiPeakFitter::kMu, GATMultiPeakFitter::kLinear, {0., 1.000});
  fitter.SetParFunction(GATMultiPeakFitter::kSig, GATMultiPeakFitter::kRootQuad, {0.2, 0.017, 0.0003});

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

  //Template for high stats (i.e. order 10 hrs of data)
  fitter.RemoveRegion(0);
  fitter.RemoveRegion(0);
  fitter.RemovePeak(5, 4);
  fitter.RemovePeak(5, 2);
  fitter.Write("fitHighStats");
  
  //Save versions with/without tails
  fitter.SetParFunction(GATMultiPeakFitter::kTauHT, GATMultiPeakFitter::kConst, {0.5});
  fitter.FixPar(GATMultiPeakFitter::kFht, 0, 0);
  fitter.FixPar(GATMultiPeakFitter::kTauHT, 0, 0.5);
  fitter.Write("fitHighStats_noHT");

  fitter.SetParFunction(GATMultiPeakFitter::kTau, GATMultiPeakFitter::kConst, {0.5});
  fitter.FixPar(GATMultiPeakFitter::kFt, 0, 0);
  fitter.FixPar(GATMultiPeakFitter::kTau, 0, 0.5);
  fitter.Write("fitHighStats_noTails");

  fitter.SetParFunction(GATMultiPeakFitter::kFht, GATMultiPeakFitter::kConst, {0.1});
  fitter.LimitPar(GATMultiPeakFitter::kFht, 0, 0., 1.);
  fitter.SetParFunction(GATMultiPeakFitter::kTauHT, GATMultiPeakFitter::kLinear, {0., 0.0005});
  fitter.FreePar(GATMultiPeakFitter::kTauHT, 0);
  fitter.Write("fitHighStats_noLT");
  
  fitter.SetParFunction(GATMultiPeakFitter::kFt, GATMultiPeakFitter::kConst, {0.1});
  fitter.LimitPar(GATMultiPeakFitter::kFt, 0, 0., 1.);
  fitter.SetParFunction(GATMultiPeakFitter::kTau, GATMultiPeakFitter::kLinear, {0., 0.0005});
  fitter.FreePar(GATMultiPeakFitter::kTau, 0);

  //Template for normal stats (i.e. order 1 hr of data)
  fitter.RemoveRegion(0);
  fitter.RemoveRegion(1);
  fitter.RemoveRegion(3);
  fitter.RemoveRegion(3);
  fitter.RemovePeak(0, 8);
  fitter.RemovePeak(0, 6);
  fitter.RemovePeak(0, 4);
  fitter.RemovePeak(0, 1);
  fitter.RemovePeak(0, 0);
  fitter.GetRegion(0)->SetRange(220, 320);
  
  fitter.RemovePeak(1, 0);
  fitter.GetRegion(1)->SetRange(560, 620);
  
  fitter.RemovePeak(2, 0);
  fitter.AddPeak(2, 860.557);
  fitter.SetPar(GATMultiPeakFitter::kAmp, 8, 0.24);
  fitter.GetRegion(2)->SetRange(710, 880);

  fitter.FixBGPar(1, 2, 0);
  fitter.FixBGPar(3, 1, 0);
  fitter.FixBGPar(3, 2, 0);
  fitter.FixBGPar(4, 1, 0);
  
  fitter.Write("fitNormal");
  
  //Save versions with/without tails
  fitter.SetParFunction(GATMultiPeakFitter::kTauHT, GATMultiPeakFitter::kConst, {0.5});
  fitter.FixPar(GATMultiPeakFitter::kFht, 0, 0);
  fitter.FixPar(GATMultiPeakFitter::kTauHT, 0, 0.5);
  fitter.Write("fitNormal_noHT");

  fitter.SetParFunction(GATMultiPeakFitter::kTau, GATMultiPeakFitter::kConst, {0.5});
  fitter.FixPar(GATMultiPeakFitter::kFt, 0, 0);
  fitter.FixPar(GATMultiPeakFitter::kTau, 0, 0.5);
  fitter.Write("fitNormal_noTails");

  fitter.SetParFunction(GATMultiPeakFitter::kFht, GATMultiPeakFitter::kConst, {0.1});
  fitter.LimitPar(GATMultiPeakFitter::kFht, 0, 0., 1.);
  fitter.SetParFunction(GATMultiPeakFitter::kTauHT, GATMultiPeakFitter::kLinear, {0., 0.0005});
  fitter.FreePar(GATMultiPeakFitter::kTauHT, 0);
  fitter.Write("fitNormal_noLT");
  
  fitter.SetParFunction(GATMultiPeakFitter::kFt, GATMultiPeakFitter::kConst, {0.1});
  fitter.LimitPar(GATMultiPeakFitter::kFt, 0, 0., 1.);
  fitter.SetParFunction(GATMultiPeakFitter::kTau, GATMultiPeakFitter::kLinear, {0., 0.0005});
  fitter.FreePar(GATMultiPeakFitter::kTau, 0);

  //template for low stats (only prominent peaks)
  fitter.RemovePeak(2, 3);
  fitter.RemovePeak(2, 2);
  fitter.RemovePeak(2, 1);
  fitter.GetRegion(2)->SetRange(710, 750);
  
  fitter.SetPeakEnergy(3, 0, 860.557);
  fitter.SetPar(GATMultiPeakFitter::kAmp, 6, 0.24);
  fitter.GetRegion(3)->SetRange(840, 880);
  fitter.SetBGPars(3, 0.0037, 0, 0);
  
  fitter.FixBGPar(1, 2, 0);
  fitter.FixBGPar(2, 1, 0);
  fitter.FixBGPar(2, 2, 0);
  fitter.FixBGPar(3, 1, 0);
  fitter.FixBGPar(3, 2, 0);
  fitter.FixBGPar(4, 1, 0);
  fitter.FixBGPar(4, 2, 0);

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
