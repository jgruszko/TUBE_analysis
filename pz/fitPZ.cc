{
	gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
	gROOT->ProcessLine("#include <iostream>");
	gROOT->ProcessLine("#include <fstream>");

	int startBin = 12700;	
	int endBin = 29000;
	TF1* f0 = new TF1("f0", "[0]*(exp(-x/[1]))", startBin, endBin);

	TChain* built = new TChain("MGTree");
	TChain* gat = new TChain("mjdTree");
	built->AddFile("$TUBEDIR/data/built/OR_run1002.root");
	gat->AddFile("$TUBEDIR/data/gatified/mjd_run1002.root");
        built->AddFriend(gat);

        GATWaveformBrowser b;
	b.LoadWaveforms(built,"trapEMPZCal>1000 && trapEMPZCal<2650  && channel ==112");

	MGWFBaselineRemover* rem = new MGWFBaselineRemover();
	rem->SetBaselineSamples(700);

	MGWFExtremumFinder *ex = new MGWFExtremumFinder();
	ex->SetFindMaximum();	
	MGTWaveform wf;

	int nEntries = b.GetNWaveforms();
//	if(nEntries > 100) nEntries = 100;
	TFitResultPtr fitRes;
	TH1D* wfHist;
	TH1D* amp = new TH1D("amp", "amp", 1000, 1E2, 1E4);
	TH1D* t1 = new TH1D("t1", "t1", 1000, 1E-5, 5E-5);
	TH1D* chi2 = new TH1D("chi2", "chi2", 100, 0, 50);
	
	int maxT = 0;
	TString funcStr;
        nEntries = 1000;
	for(int i = 0; i <nEntries; i++){
		wf = *(b.GetWaveform(i));
                wf.SetTOffset(0);
		rem->Transform(&wf);
		ex->Transform(&wf);
		maxT = (int)10*ex->GetTheExtremumPoint();
		startBin = maxT + 400;
		TF1* f1 = new TF1("f1", "[0]*exp(-[1]*x)", startBin, endBin);
		f1->SetParameter(0, 1E3);
                f1->SetRange(maxT, endBin); 
		wfHist = wf.GimmeHist();
		fitRes = wfHist->Fit(f1, "S R Q");
		if(fitRes->IsValid()){
			amp->Fill(f1->GetParameter(0));
			t1->Fill(f1->GetParameter(1));
		}
		delete f1;
	}
		
}
