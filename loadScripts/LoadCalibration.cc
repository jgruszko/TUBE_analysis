{
  gROOT->ProcessLine(".x $GATDIR/LoadGATClasses.C");
  TChain* gat = new TChain("mjdTree", "");
  int startRun = 118;
  int endRun = 136;
  TString gatFilePath, fileNum;

  for(int i = startRun; i<=endRun; i++)
  {
        fileNum.Form("%d", i);
        gatFilePath = "$TUBEDIR/data/gatified/mjd_run"+fileNum+".root";
        gat->AddFile(gatFilePath);
  }
}
