#!/usr/bin/python
#R. Martin Feb 2013
import os

#Define the properties of the analysis modules in here. Just add modules at the end.
#All paths and filenames are relative to the output directory, for example:
#mjd/ortec/analysis/P42537A/

#When coding a new analysis module, use the properties below to guide your code

class AnalysisModule:
    def __init__(self, Name):
        self.Name=Name #name of the executable in MJDApps
        self.OutputFiles=[] #files this module outputs (not figures), specify the relative path if these should be in a directory below the detector directory. The modules should create figures in their own specific directories.
        self.OutputDirs=[]#directories to be created for the output (in addition to the one which has the module name, which should be the default
        self.InputFiles=[] #files this module depends on
        self.ByRun=False #True if used on each run
        self.DependsOn=[] #Other modules this depends on (not used yet!)
        self.InputDataDir="" #Can be "gatified", "mcaroot". Required if Run By Run = True. (This directory is used to list all the ROOT files available)

    def InputsExist(self,directory):
        foundall=True
        for file in self.InputFiles:
            if not os.path.isfile(directory+file):
                foundall=False
                print "Module",self.Name," requires ",directory+file," IT CANNOT BE RUN"
        return foundall

#####################################################################################
#####################################################################################


Modules={}

##List modules here, follow the syntax to add them to the Modules dictionnary:
#mjd_calibrate_and_fit_run
Modules["mjd_calibrate_and_fit_run"]=AnalysisModule("mjd_calibrate_and_fit_run")
Modules["mjd_calibrate_and_fit_run"].ByRun=True
Modules["mjd_calibrate_and_fit_run"].InputDataDir='gatified/'
Modules["mjd_calibrate_and_fit_run"].OutputDirs=['mjd_calibrate_and_fit_run/calibrationfits/']
Modules["mjd_calibrate_and_fit_run"].OutputFiles=['calibration_byrun.dat','meanfwhmkeV_byrun.dat']

#mjd_get_event_rate_by_run
Modules["mjd_get_event_rate_by_run"]=AnalysisModule("mjd_get_event_rate_by_run")
Modules["mjd_get_event_rate_by_run"].ByRun=True
Modules["mjd_get_event_rate_by_run"].InputDataDir='gatified/'
Modules["mjd_get_event_rate_by_run"].InputFiles=['calibration_byrun.dat']
Modules["mjd_get_event_rate_by_run"].OutputFiles=['rates_byrun.dat']

#mjd_plot_run_by_run_energy
Modules["mjd_plot_run_by_run_energy"]=AnalysisModule("mjd_plot_run_by_run_energy")
Modules["mjd_plot_run_by_run_energy"].InputFiles=['calibration_byrun.dat','meanfwhmkeV_byrun.dat']
Modules["mjd_plot_run_by_run_energy"].OutputFiles=['calibrationsummary.dat']


#mjd_energy_analysis
Modules["mjd_energy_analysis"]=AnalysisModule("mjd_energy_analysis")
Modules["mjd_energy_analysis"].InputFiles=['calibration_byrun.dat','calibrationsummary.dat']
Modules["mjd_energy_analysis"].OutputDirs=['mjd_energy_analysis/calibrationfits/','skim/']
Modules["mjd_energy_analysis"].OutputFiles=['summaryfwhm.dat','skim/energyskim.root']

#mjd_xyscan_analysis
Modules["mjd_xyscan_analysis"]=AnalysisModule("mjd_xyscan_analysis")
#Modules["mjd_xyscan_analysis"].InputFiles=['calibration_byrun.dat','calibrationsummary.dat']
Modules["mjd_xyscan_analysis"].OutputDirs=['mjd_xyscan_analysis/calibrationfits_for_peak_seaking_only/','mjd_xyscan_analysis/Summary/','mjd_xyscan_analysis/Recalibrated_E/','mjd_xyscan_analysis/risetime/','mjd_xyscan_analysis/AoverE/','skim/']
Modules["mjd_xyscan_analysis"].OutputFiles=['summaryfwhm.dat','xyscanskim.root']

#mjd_string_analysis_EZ
Modules["mjd_string_analysis_EZ"]=AnalysisModule("mjd_string_analysis_EZ")
Modules["mjd_string_analysis_EZ"].InputFiles=['calibration_byrun.dat','calibrationsummary.dat']
Modules["mjd_string_analysis_EZ"].OutputDirs=['mjd_string_analysis_EZ/calibrationfits_for_peak_seaking_only/','skim/']
Modules["mjd_string_analysis_EZ"].OutputFiles=['summaryfwhm.dat','stringskim.root']


#mjd_psa_analysis
Modules["mjd_psa_analysis"]=AnalysisModule("mjd_psa_analysis")
Modules["mjd_psa_analysis"].InputFiles=['calibration_byrun.dat','skim/energyskim.root']
Modules["mjd_psa_analysis"].OutputDirs=['skim/']
Modules["mjd_psa_analysis"].OutputFiles=['psaresults.dat','skim/psaskim.root']


#mjd_psa_analysis_calibrated
Modules["mjd_psa_analysis_calibrated"]=AnalysisModule("mjd_psa_analysis_calibrated")
#Modules["mjd_psa_analysis_calibrated"].InputFiles=['calibration_byrun.dat','skim/energyskim.root']
Modules["mjd_psa_analysis_calibrated"].OutputDirs=['skim/']

Modules["mjd_psa_analysis_calibrated"].OutputFiles=['psaresults.dat','skim/psaskim.root']

#mjd_psa_analysis_EZ
Modules["mjd_psa_analysis_EZ"]=AnalysisModule("mjd_psa_analysis_EZ")
Modules["mjd_psa_analysis_EZ"].InputFiles=['calibration_byrun.dat','skim/energyskim.root']
Modules["mjd_psa_analysis_EZ"].OutputDirs=['skim/']
Modules["mjd_psa_analysis_EZ"].OutputFiles=['psaresults.dat','skim/psaskim.root']



#mjd_charge_trapping_rise_time_analysis
Modules["mjd_charge_trapping_rise_time_analysis"]=AnalysisModule("mjd_charge_trapping_rise_time_analysis")
Modules["mjd_charge_trapping_rise_time_analysis"].InputFiles=['calibration_byrun.dat','skim/psaskim.root']
Modules["mjd_charge_trapping_rise_time_analysis"].OutputDirs=['skim/']
Modules["mjd_charge_trapping_rise_time_analysis"].OutputFiles=['chargetrappingsummary.dat','skim/ctskim.root']

#mjd_mca_60Co_analyze_run
Modules["mjd_mca_60Co_analyze_run"]=AnalysisModule("mjd_mca_60Co_analyze_run")
Modules["mjd_mca_60Co_analyze_run"].ByRun=True
Modules["mjd_mca_60Co_analyze_run"].InputDataDir='mcaroot/'
Modules["mjd_mca_60Co_analyze_run"].OutputDirs=['mjd_mca_60Co_analyze_run/calibrationfits/']
Modules["mjd_mca_60Co_analyze_run"].OutputFiles=['mca_60Co_fits_by_run.dat','mca_60Co_calibration_by_run.dat']

#mjd_mca_plot_60Co_analysis
Modules["mjd_mca_plot_60Co_analysis"]=AnalysisModule("mjd_mca_plot_60Co_analysis");
Modules["mjd_mca_plot_60Co_analysis"].InputFiles=['mca_60Co_fits_by_run.dat','mca_60Co_calibration_by_run.dat']
Modules["mjd_mca_plot_60Co_analysis"].OutputFiles=['mca_60Co_efficiency','mca_60Co_fit_the_sum.dat']

#mjd_mca_133Ba_analyze_run(NOT IMPLEMENTED YET!!!)
Modules["mjd_mca_133Ba_analyze_run"]=AnalysisModule("mjd_mca_133Ba_analyze_run")
Modules["mjd_mca_133Ba_analyze_run"].ByRun=True
Modules["mjd_mca_133Ba_analyze_run"].InputDataDir='mcaroot/'
Modules["mjd_mca_133Ba_analyze_run"].OutputDirs=['mjd_mca_133Ba_analyze_run/calibrationfits/']
Modules["mjd_mca_133Ba_analyze_run"].OutputFiles=['mca_133Ba_peakratios_by_run.dat','mca_133Ba_calibration_by_run.dat']


#mjd_mca_133Ba_flood_analyze_run 
Modules["mjd_mca_133Ba_flood_analyze_run"]=AnalysisModule("mjd_mca_133Ba_flood_analyze_run")
Modules["mjd_mca_133Ba_flood_analyze_run"].ByRun=True
Modules["mjd_mca_133Ba_flood_analyze_run"].InputDataDir='mcaroot/'
Modules["mjd_mca_133Ba_flood_analyze_run"].OutputDirs=['mjd_mca_133Ba_flood_analyze_run/','mjd_mca_133Ba_flood_analyze_run/calibrationfits/']
Modules["mjd_mca_133Ba_flood_analyze_run"].OutputFiles=['mca_133Ba_fits_by_run.dat','mca_133Ba_calibration_by_run.dat']

#mjd_mca_plot_133Ba_analysis
Modules["mjd_mca_plot_133Ba_analysis"]=AnalysisModule("mjd_mca_plot_133Ba_analysis");
Modules["mjd_mca_plot_133Ba_analysis"].InputFiles=['mca_133Ba_fits_by_run.dat','mca_133Ba_calibration_by_run.dat']
Modules["mjd_mca_plot_133Ba_analysis"].OutputFiles=['mca_133Ba_ratios_by_run.dat','mca_133Ba_fit_the_sum.dat']

#mjd_mca_241Am_flood_analyze_run
Modules["mjd_mca_241Am_flood_analyze_run"]=AnalysisModule("mjd_mca_241Am_flood_analyze_run")
Modules["mjd_mca_241Am_flood_analyze_run"].ByRun=True
Modules["mjd_mca_241Am_flood_analyze_run"].InputDataDir='mcaroot/'
Modules["mjd_mca_241Am_flood_analyze_run"].OutputDirs=['mjd_mca_241Am_flood_analyze_run/']
Modules["mjd_mca_241Am_flood_analyze_run"].OutputFiles=['mca_241Am_fits_by_run.dat']


#mjd_mca_plot_241Am_analysis
Modules["mjd_mca_plot_241Am_analysis"]=AnalysisModule("mjd_mca_plot_241Am_analysis");
Modules["mjd_mca_plot_241Am_analysis"].InputFiles=['mca_241Am_fits_by_run.dat']
Modules["mjd_mca_plot_241Am_analysis"].OutputFiles=['mca_241Am_fit_the_sum.dat']














