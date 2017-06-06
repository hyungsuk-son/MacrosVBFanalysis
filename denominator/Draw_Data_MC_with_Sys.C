#include <iostream>
#include <cmath>

#include "Rtypes.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TRandom.h"
#include "TGraphErrors.h"
#include "TString.h"
#include "TAxis.h"
#include "TF1.h"
#include "THStack.h"
#include "map"
#include "TGraphAsymmErrors.h"
#include "TLegend.h"
#include "TStyle.h"
#include "vector"
#include "string"

#include "AtlasUtils.h"
#include "AtlasStyle.h"
#include "AtlasLabels.h"

//#ifdef __CINT__
//#include "AtlasUtils.C"
//#include "AtlasLabels.C"
//#endif


void Draw_Data_MC_with_Sys(TString HISTO,TString XTITLE,float XMIN,float XMAX,bool REBIN,bool LOGY,bool PRINT,int XDIV,TString UNITS,bool GeneratorSYS,bool TheorySYS,bool TopReweight,bool ATLAS,bool DivBinWidth)
{
  gROOT->ForceStyle();

  // Load ATLAS Style
  //Change to your location
/* 
#ifdef __CINT__
  gROOT->LoadMacro("AtlasStyle.C");
  gROOT->LoadMacro("AtlasUtils.C");
  gROOT->LoadMacro("AtlasLabels.C");
#endif
*/

  if (ATLAS) {
    SetAtlasStyle();
  }

  TString intLumi = "3.2 fb^{-1}";

  TString channel;
  if ( (HISTO.Contains("Zee") || HISTO.Contains("zee")) && HISTO.Contains("monojet") ) {
    channel = "Z#rightarrowee #geq1 jet"; //Zee monojet
  }
  if ( (HISTO.Contains("Zmumu") || HISTO.Contains("zmumu")) && HISTO.Contains("monojet") ) {
    channel = "Z#rightarrow#mu#mu #geq1 jet"; //Zmunu monojet
  }
  if ( (HISTO.Contains("Zll") || HISTO.Contains("zll")) && HISTO.Contains("monojet") ) {
    channel = "Z#rightarrowll #geq1 jet"; //Zll monojet
  }
  if ( (HISTO.Contains("Zee") || HISTO.Contains("zee")) && HISTO.Contains("vbf") ) {
    channel = "Z#rightarrowee VBF"; //Zee VBF
  }
  if ( (HISTO.Contains("Zmumu") || HISTO.Contains("zmumu")) && HISTO.Contains("vbf") ) {
    channel = "Z#rightarrow#mu#mu VBF"; //Zmunu VBF
  }
  if ( (HISTO.Contains("Zll") || HISTO.Contains("zll")) && HISTO.Contains("vbf") ) {
    channel = "Z#rightarrowll VBF"; //Zee VBF
  }


  TString PATH("/home/hson/Work/Run2_Study/VBF_study/Macro/");


  // Retrieve files
  TString filename_MCData("hist_all_MC_Data.root");
  //TString filename_Emily("EmilysHistograms.root"); // use the Emily histograms
  TString filename_Sys("hist_sys_uncertain.root");
  TFile *file_MCData = TFile::Open(PATH+filename_MCData);
  //TFile *file_Emily = TFile::Open(PATH+filename_Emily); // use the Emily histograms
  TFile *file_Sys = TFile::Open(PATH+filename_Sys);


  // Define Rebinning
  Double_t binsMET[] = {200.,250.,300.,350.,500.,700.,1400.};
  Int_t nbinMET = sizeof(binsMET)/sizeof(Double_t) - 1;
  Double_t binsMjj[] = {200.,400.,600.,1000.,2000.,4000.};
  Int_t nbinMjj = sizeof(binsMjj)/sizeof(Double_t) - 1;

  char name[1000];

  ///////////////////////////////////////
  // Retrieve Histograms (MC and Data) //
  ///////////////////////////////////////

  // Define new histograms
  TH1F *h_Data;
  TH1F *h_SM;
  TH1F *h_Znunu;
  TH1F *h_Wenu;
  TH1F *h_Wmunu;
  TH1F *h_Wtaunu;
  TH1F *h_Zee;
  TH1F *h_Zmumu;
  TH1F *h_Ztautau;
  TH1F *h_ttbar;
  TH1F *h_Multijet;
  TH1F *h_Diboson;
  TH1F *h_Zll;
  TH1F *h_Wlnu;

  // For Zll histograms when combining Zmumu and Zee for each backgronds (Ex: h_zll_monojet_met_emulmet -> monojet_met_emulmet)
  TString OBSERV(HISTO); // initialize OBSERV with the name of HISTO
  if ( HISTO.Contains("zll") ){
    OBSERV.Remove(0,6);
  }
  //cout << "HISTO = " << HISTO << " , OBSERV = " << OBSERV << endl;

  // Retrieve Data
  if ( HISTO.Contains("zll") ){ // Zmumu + Zee channel histograms for Data
    h_Data = (TH1F*)file_MCData->Get("Data_h_zmumu_"+OBSERV);
    h_Data->Add((TH1F*)file_MCData->Get("Data_h_zee_"+OBSERV));
    h_Data->SetName("h_Data");
  } else {
    h_Data = (TH1F*)file_MCData->Get("Data_"+HISTO);
  }
  if (REBIN && HISTO.Contains("met")) h_Data = dynamic_cast<TH1F*>(h_Data->Rebin(nbinMET,"h_Data",binsMET));
  if (REBIN && HISTO.Contains("mjj")) h_Data = dynamic_cast<TH1F*>(h_Data->Rebin(nbinMjj,"h_Data",binsMjj));
  // Retrieve SM MC prediction
  if ( HISTO.Contains("zll") ){ // Zmumu + Zee channel histograms for SM MC
    h_SM = (TH1F*)file_MCData->Get("SM_MC_h_zmumu_"+OBSERV);
    h_SM->Add((TH1F*)file_MCData->Get("SM_MC_h_zee_"+OBSERV));
    h_SM->SetName("h_SM");
  } else {
    h_SM = (TH1F*)file_MCData->Get("SM_MC_"+HISTO);
  }
  if (REBIN && HISTO.Contains("met")) h_SM = dynamic_cast<TH1F*>(h_SM->Rebin(nbinMET,"h_SM",binsMET));
  if (REBIN && HISTO.Contains("mjj")) h_SM = dynamic_cast<TH1F*>(h_SM->Rebin(nbinMjj,"h_SM",binsMjj));
  // Retrieve Znunu MC background
  if ( HISTO.Contains("zll") ){ // Zmumu + Zee channel histograms for Znunu MC background
    h_Znunu = (TH1F*)file_MCData->Get("Znunu_MC_h_zmumu_"+OBSERV);
    h_Znunu->Add((TH1F*)file_MCData->Get("Znunu_MC_h_zee_"+OBSERV));
    h_Znunu->SetName("h_Znunu");
  } else {
    h_Znunu = (TH1F*)file_MCData->Get("Znunu_MC_"+HISTO);
  }
  if (REBIN && HISTO.Contains("met")) h_Znunu = dynamic_cast<TH1F*>(h_Znunu->Rebin(nbinMET,"h_Znunu",binsMET));
  if (REBIN && HISTO.Contains("mjj")) h_Znunu = dynamic_cast<TH1F*>(h_Znunu->Rebin(nbinMjj,"h_Znunu",binsMjj));
  // Retrieve Wenu MC background
  if ( HISTO.Contains("zll") ){ // Zmumu + Zee channel histograms for Wenu MC background
    h_Wenu = (TH1F*)file_MCData->Get("Wenu_MC_h_zmumu_"+OBSERV);
    h_Wenu->Add((TH1F*)file_MCData->Get("Wenu_MC_h_zee_"+OBSERV));
    h_Wenu->SetName("h_Wenu");
  } else {
    h_Wenu = (TH1F*)file_MCData->Get("Wenu_MC_"+HISTO);
  }
  if (REBIN && HISTO.Contains("met")) h_Wenu = dynamic_cast<TH1F*>(h_Wenu->Rebin(nbinMET,"h_Wenu",binsMET));
  if (REBIN && HISTO.Contains("mjj")) h_Wenu = dynamic_cast<TH1F*>(h_Wenu->Rebin(nbinMjj,"h_Wenu",binsMjj));
  // Retrieve Wmunu MC background
  if ( HISTO.Contains("zll") ){ // Zmumu + Zee channel histograms for Wmunu MC background
    h_Wmunu = (TH1F*)file_MCData->Get("Wmunu_MC_h_zmumu_"+OBSERV);
    h_Wmunu->Add((TH1F*)file_MCData->Get("Wmunu_MC_h_zee_"+OBSERV));
    h_Wmunu->SetName("h_Wmunu");
  } else {
    h_Wmunu = (TH1F*)file_MCData->Get("Wmunu_MC_"+HISTO);
  }
  if (REBIN && HISTO.Contains("met")) h_Wmunu = dynamic_cast<TH1F*>(h_Wmunu->Rebin(nbinMET,"h_Wmunu",binsMET));
  if (REBIN && HISTO.Contains("mjj")) h_Wmunu = dynamic_cast<TH1F*>(h_Wmunu->Rebin(nbinMjj,"h_Wmunu",binsMjj));
  // Retrieve Wtaunu MC background
  if ( HISTO.Contains("zll") ){ // Zmumu + Zee channel histograms for Wtaunu MC background
    h_Wtaunu = (TH1F*)file_MCData->Get("Wtaunu_MC_h_zmumu_"+OBSERV);
    h_Wtaunu->Add((TH1F*)file_MCData->Get("Wtaunu_MC_h_zee_"+OBSERV));
    h_Wtaunu->SetName("h_Wtaunu");
  } else {
    h_Wtaunu = (TH1F*)file_MCData->Get("Wtaunu_MC_"+HISTO);
  }
  if (REBIN && HISTO.Contains("met")) h_Wtaunu = dynamic_cast<TH1F*>(h_Wtaunu->Rebin(nbinMET,"h_Wtaunu",binsMET));
  if (REBIN && HISTO.Contains("mjj")) h_Wtaunu = dynamic_cast<TH1F*>(h_Wtaunu->Rebin(nbinMjj,"h_Wtaunu",binsMjj));
  // Retrieve Zee MC background
  if ( HISTO.Contains("zll") ){ // Zmumu + Zee channel histograms for Zee MC background
    h_Zee = (TH1F*)file_MCData->Get("Zee_MC_h_zmumu_"+OBSERV);
    h_Zee->Add((TH1F*)file_MCData->Get("Zee_MC_h_zee_"+OBSERV));
    h_Zee->SetName("h_Zee");
  } else {
    h_Zee = (TH1F*)file_MCData->Get("Zee_MC_"+HISTO);
  }
  if (REBIN && HISTO.Contains("met")) h_Zee = dynamic_cast<TH1F*>(h_Zee->Rebin(nbinMET,"h_Zee",binsMET));
  if (REBIN && HISTO.Contains("mjj")) h_Zee = dynamic_cast<TH1F*>(h_Zee->Rebin(nbinMjj,"h_Zee",binsMjj));
  // Retrieve Zmumu MC background
  if ( HISTO.Contains("zll") ){ // Zmumu + Zee channel histograms for Zmumu MC background
    h_Zmumu = (TH1F*)file_MCData->Get("Zmumu_MC_h_zmumu_"+OBSERV);
    h_Zmumu->Add((TH1F*)file_MCData->Get("Zmumu_MC_h_zee_"+OBSERV));
    h_Zmumu->SetName("h_Zmumu");
  } else {
    h_Zmumu = (TH1F*)file_MCData->Get("Zmumu_MC_"+HISTO);
  }
  if (REBIN && HISTO.Contains("met")) h_Zmumu = dynamic_cast<TH1F*>(h_Zmumu->Rebin(nbinMET,"h_Zmumu",binsMET));
  if (REBIN && HISTO.Contains("mjj")) h_Zmumu = dynamic_cast<TH1F*>(h_Zmumu->Rebin(nbinMjj,"h_Zmumu",binsMjj));
  // Retrieve Ztautau MC background
  if ( HISTO.Contains("zll") ){ // Zmumu + Zee channel histograms for Ztautau MC background
    h_Ztautau = (TH1F*)file_MCData->Get("Ztautau_MC_h_zmumu_"+OBSERV);
    h_Ztautau->Add((TH1F*)file_MCData->Get("Ztautau_MC_h_zee_"+OBSERV));
    h_Ztautau->SetName("h_Ztautau");
  } else {
    h_Ztautau = (TH1F*)file_MCData->Get("Ztautau_MC_"+HISTO);
  }
  if (REBIN && HISTO.Contains("met")) h_Ztautau = dynamic_cast<TH1F*>(h_Ztautau->Rebin(nbinMET,"h_Ztautau",binsMET));
  if (REBIN && HISTO.Contains("mjj")) h_Ztautau = dynamic_cast<TH1F*>(h_Ztautau->Rebin(nbinMjj,"h_Ztautau",binsMjj));
  // Retrieve ttbar MC background
  if ( HISTO.Contains("zll") ){ // Zmumu + Zee channel histograms for ttbar MC background
    h_ttbar = (TH1F*)file_MCData->Get("TTbar_MC_h_zmumu_"+OBSERV);
    h_ttbar->Add((TH1F*)file_MCData->Get("TTbar_MC_h_zee_"+OBSERV));
    h_ttbar->SetName("h_ttbar");
  } else {
    h_ttbar = (TH1F*)file_MCData->Get("TTbar_MC_"+HISTO);
  }
  if (REBIN && HISTO.Contains("met")) h_ttbar = dynamic_cast<TH1F*>(h_ttbar->Rebin(nbinMET,"h_ttbar",binsMET));
  if (REBIN && HISTO.Contains("mjj")) h_ttbar = dynamic_cast<TH1F*>(h_ttbar->Rebin(nbinMjj,"h_ttbar",binsMjj));
  // Retrieve Multijet MC background
  if ( HISTO.Contains("zll") ){ // Zmumu + Zee channel histograms for Multijet MC background
    h_Multijet = (TH1F*)file_MCData->Get("Multijet_MC_h_zmumu_"+OBSERV);
    h_Multijet->Add((TH1F*)file_MCData->Get("Multijet_MC_h_zee_"+OBSERV));
    h_Multijet->SetName("h_Multijet");
  } else {
    h_Multijet = (TH1F*)file_MCData->Get("Multijet_MC_"+HISTO);
  }
  if (REBIN && HISTO.Contains("met")) h_Multijet = dynamic_cast<TH1F*>(h_Multijet->Rebin(nbinMET,"h_Multijet",binsMET));
  if (REBIN && HISTO.Contains("mjj")) h_Multijet = dynamic_cast<TH1F*>(h_Multijet->Rebin(nbinMjj,"h_Multijet",binsMjj));
  // Retrieve Diboson MC background
  if ( HISTO.Contains("zll") ){ // Zmumu + Zee channel histograms for Diboson MC background
    h_Diboson = (TH1F*)file_MCData->Get("Diboson_MC_h_zmumu_"+OBSERV);
    h_Diboson->Add((TH1F*)file_MCData->Get("Diboson_MC_h_zee_"+OBSERV));
    h_Diboson->SetName("h_Diboson");
  } else {
    h_Diboson = (TH1F*)file_MCData->Get("Diboson_MC_"+HISTO);
  }
  if (REBIN && HISTO.Contains("met")) h_Diboson = dynamic_cast<TH1F*>(h_Diboson->Rebin(nbinMET,"h_Diboson",binsMET));
  if (REBIN && HISTO.Contains("mjj")) h_Diboson = dynamic_cast<TH1F*>(h_Diboson->Rebin(nbinMjj,"h_Diboson",binsMjj));

  // Integrate W production events into h_Wlnu histogram (Wlnu = Wenu + Wmunu + Wtaunu)
  if ( HISTO.Contains("zll") ){
    h_Wlnu = (TH1F*)h_Wenu->Clone("h_Wlnu");
    h_Wlnu->Add(h_Wmunu);
    h_Wlnu->Add(h_Wtaunu);
  }
  // Integrate Z production events into h_Zll histogram (Zll = Zee + Zmumu)
  if ( HISTO.Contains("zll") ){
    h_Zll = (TH1F*)h_Zmumu->Clone("h_Zll");
    h_Zll->Add(h_Zee);
  }



  ///////////////////////////////////////
  // Retrieve systematic uncertainties //
  ///////////////////////////////////////

  std::vector<std::string> systematics;
  systematics.push_back("");
  systematics.push_back("EG_RESOLUTION_ALL__1down");
  systematics.push_back("EG_RESOLUTION_ALL__1up");
  systematics.push_back("EG_SCALE_ALL__1down");
  systematics.push_back("EG_SCALE_ALL__1up");
  systematics.push_back("EL_EFF_ID_TotalCorrUncertainty__1down");
  systematics.push_back("EL_EFF_ID_TotalCorrUncertainty__1up");
  systematics.push_back("EL_EFF_Iso_TotalCorrUncertainty__1down");
  systematics.push_back("EL_EFF_Iso_TotalCorrUncertainty__1up");
  systematics.push_back("EL_EFF_Reco_TotalCorrUncertainty__1down");
  systematics.push_back("EL_EFF_Reco_TotalCorrUncertainty__1up");
  systematics.push_back("EL_EFF_TriggerEff_TotalCorrUncertainty__1down");
  systematics.push_back("EL_EFF_TriggerEff_TotalCorrUncertainty__1up");
  systematics.push_back("EL_EFF_Trigger_TotalCorrUncertainty__1down");
  systematics.push_back("EL_EFF_Trigger_TotalCorrUncertainty__1up");
  systematics.push_back("JET_EtaIntercalibration_NonClosure__1up");
  systematics.push_back("JET_EtaIntercalibration_NonClosure__1down");
  systematics.push_back("JET_GroupedNP_1__1up");
  systematics.push_back("JET_GroupedNP_1__1down");
  systematics.push_back("JET_GroupedNP_2__1up");
  systematics.push_back("JET_GroupedNP_2__1down");
  systematics.push_back("JET_GroupedNP_3__1up");
  systematics.push_back("JET_GroupedNP_3__1down");
  systematics.push_back("JET_JER_SINGLE_NP__1up");
  systematics.push_back("JvtEfficiencyDown");
  systematics.push_back("JvtEfficiencyUp");
  systematics.push_back("MET_SoftTrk_ResoPara");
  systematics.push_back("MET_SoftTrk_ResoPerp");
  systematics.push_back("MET_SoftTrk_ScaleDown");
  systematics.push_back("MET_SoftTrk_ScaleUp");
  systematics.push_back("MUONS_ID__1down");
  systematics.push_back("MUONS_ID__1up");
  systematics.push_back("MUONS_MS__1down");
  systematics.push_back("MUONS_MS__1up");
  systematics.push_back("MUONS_SCALE__1down");
  systematics.push_back("MUONS_SCALE__1up");
  systematics.push_back("MUON_EFF_STAT__1down");
  systematics.push_back("MUON_EFF_STAT__1up");
  systematics.push_back("MUON_EFF_STAT_LOWPT__1down");
  systematics.push_back("MUON_EFF_STAT_LOWPT__1up");
  systematics.push_back("MUON_EFF_SYS__1down");
  systematics.push_back("MUON_EFF_SYS__1up");
  systematics.push_back("MUON_EFF_SYS_LOWPT__1down");
  systematics.push_back("MUON_EFF_SYS_LOWPT__1up");
  systematics.push_back("MUON_EFF_TrigStatUncertainty__1down");
  systematics.push_back("MUON_EFF_TrigStatUncertainty__1up");
  systematics.push_back("MUON_EFF_TrigSystUncertainty__1down");
  systematics.push_back("MUON_EFF_TrigSystUncertainty__1up");
  systematics.push_back("MUON_ISO_STAT__1down");
  systematics.push_back("MUON_ISO_STAT__1up");
  systematics.push_back("MUON_ISO_SYS__1down");
  systematics.push_back("MUON_ISO_SYS__1up");
  systematics.push_back("MUON_TTVA_STAT__1down");
  systematics.push_back("MUON_TTVA_STAT__1up");
  systematics.push_back("MUON_TTVA_SYS__1down");
  systematics.push_back("MUON_TTVA_SYS__1up");
  systematics.push_back("PRW_DATASF__1down");
  systematics.push_back("PRW_DATASF__1up");


  // Systematics histogram
  std::map<TString, TH1*> h_background_sys;

  // Retrieve All systematic histograms (for SM backgrounds)
  std::vector<std::string>::const_iterator sys;
  for (sys = systematics.begin(); sys != systematics.end(); sys++){
    TString sys_name = *sys;
    if ( HISTO.Contains("zll") ) { // Combine Zmumu + Zee systematic histograms
      h_background_sys[sys_name] = (TH1F*)file_Sys->Get("SM_h_zmumu_"+OBSERV+sys_name);
      h_background_sys[sys_name]->Add((TH1F*)file_Sys->Get("SM_h_zee_"+OBSERV+sys_name));
    } else {
      h_background_sys[sys_name] = (TH1F*)file_Sys->Get("SM_"+HISTO+sys_name);
    }
    if (REBIN && HISTO.Contains("met")) h_background_sys[sys_name] = dynamic_cast<TH1F*>(h_background_sys[sys_name]->Rebin(nbinMET,"SM_"+HISTO+sys_name,binsMET));
    if (REBIN && HISTO.Contains("mjj")) h_background_sys[sys_name] = dynamic_cast<TH1F*>(h_background_sys[sys_name]->Rebin(nbinMjj,"SM_"+HISTO+sys_name,binsMjj));
  }




  /////////////////////////////////////////////////////////////////////
  // Retrieve Truth for Sherpa and MGPy8EG for generator uncertainty //
  /////////////////////////////////////////////////////////////////////
  TString truth_HISTO(HISTO); // initialize the name of truth histogram
  TH1F *h_truth_Sherpa;
  TH1F *h_truth_MGPy8EG;
  if (GeneratorSYS) {
    if ( HISTO.Contains("zee") ) {
      truth_HISTO.Insert(6,"truth_"); // define the truth histogram for Zee (i.e. h_zee_monojet_met_emulmet -> h_zee_truth_monojet_met_emulmet)
      h_truth_Sherpa = (TH1F*)file_MCData->Get("Zee_Sherpa_"+truth_HISTO);
      h_truth_MGPy8EG = (TH1F*)file_MCData->Get("Zee_MGPy8EG_"+truth_HISTO);
    }
    if ( HISTO.Contains("zmumu") ) {
      truth_HISTO.Insert(8,"truth_"); // define the truth histogram for Zmumu
      h_truth_Sherpa = (TH1F*)file_MCData->Get("Zmumu_Sherpa_"+truth_HISTO);
      h_truth_MGPy8EG = (TH1F*)file_MCData->Get("Zmumu_MGPy8EG_"+truth_HISTO);
    }
    // combine truth for zmumu and zee into zll
    if ( HISTO.Contains("zll") ) {
      truth_HISTO.Remove(0,6); // define the truth histogram for Zll (i.e. h_zll_monojet_met_emulmet -> monojet_met_emulmet)
      h_truth_Sherpa = (TH1F*)file_MCData->Get("Zmumu_Sherpa_h_zmumu_truth_"+truth_HISTO);
      h_truth_Sherpa->Add((TH1F*)file_MCData->Get("Zee_Sherpa_h_zee_truth_"+truth_HISTO)); 
      h_truth_MGPy8EG = (TH1F*)file_MCData->Get("Zmumu_MGPy8EG_h_zmumu_truth_"+truth_HISTO);
      h_truth_MGPy8EG->Add((TH1F*)file_MCData->Get("Zee_MGPy8EG_h_zee_truth_"+truth_HISTO)); 
    }
    if ( !HISTO.Contains("zee") && !HISTO.Contains("zmumu") && !HISTO.Contains("zll") ) { // Truth histograms exist only in Zee and Zmumu at this time
      h_truth_Sherpa = (TH1F*)file_MCData->Get("Znunu_MC_"+HISTO); // Get dummy values
      h_truth_MGPy8EG = (TH1F*)file_MCData->Get("Znunu_MC_"+HISTO); // Get dummy values
      // With these same values, the generator error will be calculated as 0.
    }

    // Rebin truth histograms
    if (REBIN && HISTO.Contains("met")) h_truth_Sherpa = dynamic_cast<TH1F*>(h_truth_Sherpa->Rebin(nbinMET,"h_truth_Sherpa",binsMET));
    if (REBIN && HISTO.Contains("mjj")) h_truth_Sherpa = dynamic_cast<TH1F*>(h_truth_Sherpa->Rebin(nbinMjj,"h_truth_Sherpa",binsMjj));
    if (REBIN && HISTO.Contains("met")) h_truth_MGPy8EG = dynamic_cast<TH1F*>(h_truth_MGPy8EG->Rebin(nbinMET,"h_truth_MGPy8EG",binsMET));
    if (REBIN && HISTO.Contains("mjj")) h_truth_MGPy8EG = dynamic_cast<TH1F*>(h_truth_MGPy8EG->Rebin(nbinMjj,"h_truth_MGPy8EG",binsMjj));

  }


  // Check the statistics error in MC
  for (int binno = 0; binno < h_SM->GetNbinsX() + 2; ++binno) {
    float ttbar_val  = h_ttbar->GetBinContent(binno);
    float ttbar_err  = h_ttbar->GetBinError(binno);
    cout << "ttbar_val = " << ttbar_val << endl;
    cout << "ttbar_err = " << ttbar_err << endl;
    float sm_val  = h_SM->GetBinContent(binno);
    float sm_err  = h_SM->GetBinError(binno);
    cout << "sm_val = " << sm_val << endl;
    cout << "sm_err = " << sm_err << endl;
    //h_SM->SetBinError(binno, std::sqrt(content));
  }


  // Set the errors in Data (The histograms in data don't have the error set to sqrt(bin content) by default)
  for (int binno = 0; binno < h_Data->GetNbinsX() + 2; ++binno) {
    float content = h_Data->GetBinContent(binno);
    h_Data->SetBinError(binno, std::sqrt(content));
  }



  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Top background re-weighting factor and systematics obtained by fitting the ratio Data/MC in the e-mu control region //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Retrieve top re-weighting histograms (from e-mu control region)
  TH1F *h_emuData; // Data in the e-mu control region
  TH1F *h_TopReweight; // re-weighting factor histogram
  TH1F *h_TopSys; // re-weighting factor histogram
  if (TopReweight) {
    TString finename_TopReweight("hist_zemu_ttbar_unc.root");
    TFile *file_TopReweight = TFile::Open(PATH+finename_TopReweight);
    // Data
    if ( HISTO.Contains("monojet_met") ) h_emuData = (TH1F*)file_TopReweight->Get("Data_MET_mono");
    if ( HISTO.Contains("vbf_met") ) h_emuData = (TH1F*)file_TopReweight->Get("Data_MET_search");
    if ( HISTO.Contains("vbf_mjj") ) h_emuData = (TH1F*)file_TopReweight->Get("Data_Mjj_search");
    if ( HISTO.Contains("vbf_dPhijj") ) h_emuData = (TH1F*)file_TopReweight->Get("Data_DeltaPhiAll");
    // Re-weighting histogram
    if ( HISTO.Contains("monojet_met") ) h_TopReweight = (TH1F*)file_TopReweight->Get("Fit_Nominal_MET_mono");
    if ( HISTO.Contains("vbf_met") ) h_TopReweight = (TH1F*)file_TopReweight->Get("Fit_Nominal_MET_search");
    if ( HISTO.Contains("vbf_mjj") ) h_TopReweight = (TH1F*)file_TopReweight->Get("Fit_Nominal_Mjj_search");
    if ( HISTO.Contains("vbf_dPhijj") ) h_TopReweight = (TH1F*)file_TopReweight->Get("Fit_Nominal_DeltaPhiAll");
    // Rebin
    if (REBIN && HISTO.Contains("met")) h_emuData = dynamic_cast<TH1F*>(h_emuData->Rebin(nbinMET,"h_emuData",binsMET));
    if (REBIN && HISTO.Contains("mjj")) h_emuData = dynamic_cast<TH1F*>(h_emuData->Rebin(nbinMjj,"h_emuData",binsMjj));
    if (REBIN && HISTO.Contains("met")) h_TopReweight = dynamic_cast<TH1F*>(h_TopReweight->Rebin(nbinMET,"h_TopReweight",binsMET));
    if (REBIN && HISTO.Contains("mjj")) h_TopReweight = dynamic_cast<TH1F*>(h_TopReweight->Rebin(nbinMjj,"h_TopReweight",binsMjj));

    // Re-weight top background by fitting histogram and obtain the systematic from the difference between unreweighted and reweighted top bkg
    float reweight_factor = 0.;
    h_TopSys = (TH1F*)h_TopReweight->Clone("h_TopSys");
    h_TopSys->Reset();
    for (int binno = 0; binno < h_TopReweight->GetNbinsX() + 2; ++binno) {
      // In bins where there is not sufficient data, the value of the reweighting function from the last bin with enough is used
      if (h_emuData->GetBinContent(binno) > 3) reweight_factor = h_TopReweight->GetBinContent(binno);
      //cout << "bin # = " << binno << " reweighting factor = " << reweight_factor << endl;
      float content_ttbar = h_ttbar->GetBinContent(binno);
      float reweighted_ttbar = content_ttbar * reweight_factor;
      h_ttbar->SetBinContent(binno, reweighted_ttbar);
      h_ttbar->SetBinError(binno, h_ttbar->GetBinError(binno) * reweight_factor);
      // The difference between unreweighted and reweighted top bkg
      float diff = std::abs(content_ttbar - reweighted_ttbar);
      //cout << "bin # = " << binno << " The difference between unreweighted and reweighted top bkg = " << diff << endl;
      h_TopSys->SetBinContent(binno, diff);
    }


    // Recalculate SM for Zmumu or Zee
    if ( HISTO.Contains("mumu") || HISTO.Contains("ee") ) {
      h_SM->Reset();
      h_SM->Add(h_Zmumu);
      h_SM->Add(h_Zee);
      h_SM->Add(h_Multijet);
      h_SM->Add(h_Znunu);
      h_SM->Add(h_Ztautau);
      h_SM->Add(h_Wtaunu);
      h_SM->Add(h_Wmunu);
      h_SM->Add(h_Wenu);
      h_SM->Add(h_Diboson);
      h_SM->Add(h_ttbar);
    }

    // Recalcualte SM for Zll
    if ( HISTO.Contains("zll") ){
      h_SM->Reset();
      h_SM->Add(h_Zll);
      h_SM->Add(h_Multijet);
      h_SM->Add(h_Znunu);
      h_SM->Add(h_Ztautau);
      h_SM->Add(h_Wlnu);
      h_SM->Add(h_Diboson);
      h_SM->Add(h_ttbar);
    }


  } // TopReweight End








  // Dividing by bin width as we are more used to in “measurement” papers
  if (DivBinWidth) {

    // Set the statistics error in SM MC for ratio plot (because this histogram have the error as not statistics but the total systematic uncertainty)
    for (int binno = 0; binno < h_SM->GetNbinsX() + 2; ++binno) {
      //cout << "binno = " << binno << " binwidth = " << h_SM->GetBinWidth(binno) << endl;

      // Scale Contents
      h_Data->SetBinContent(binno, h_Data->GetBinContent(binno)/h_Data->GetBinWidth(binno));
      h_SM->SetBinContent(binno, h_SM->GetBinContent(binno)/h_SM->GetBinWidth(binno));
      h_Znunu->SetBinContent(binno, h_Znunu->GetBinContent(binno)/h_Znunu->GetBinWidth(binno));
      h_Wenu->SetBinContent(binno, h_Wenu->GetBinContent(binno)/h_Wenu->GetBinWidth(binno));
      h_Wmunu->SetBinContent(binno, h_Wmunu->GetBinContent(binno)/h_Wmunu->GetBinWidth(binno));
      h_Wtaunu->SetBinContent(binno, h_Wtaunu->GetBinContent(binno)/h_Wtaunu->GetBinWidth(binno));
      h_Zee->SetBinContent(binno, h_Zee->GetBinContent(binno)/h_Zee->GetBinWidth(binno));
      h_Zmumu->SetBinContent(binno, h_Zmumu->GetBinContent(binno)/h_Zmumu->GetBinWidth(binno));
      h_Ztautau->SetBinContent(binno, h_Ztautau->GetBinContent(binno)/h_Ztautau->GetBinWidth(binno));
      h_ttbar->SetBinContent(binno, h_ttbar->GetBinContent(binno)/h_ttbar->GetBinWidth(binno));
      h_Multijet->SetBinContent(binno, h_Multijet->GetBinContent(binno)/h_Multijet->GetBinWidth(binno));
      h_Diboson->SetBinContent(binno, h_Diboson->GetBinContent(binno)/h_Diboson->GetBinWidth(binno));
      if ( HISTO.Contains("zll") ){
        h_Zll->SetBinContent(binno, h_Zll->GetBinContent(binno)/h_Zll->GetBinWidth(binno));
        h_Wlnu->SetBinContent(binno, h_Wlnu->GetBinContent(binno)/h_Wlnu->GetBinWidth(binno));
      }
      for (sys = systematics.begin(); sys != systematics.end(); sys++){
        TString sys_name = *sys;
        h_background_sys[sys_name]->SetBinContent(binno, h_background_sys[sys_name]->GetBinContent(binno)/h_background_sys[sys_name]->GetBinWidth(binno));
      }
      if (TopReweight) {
        h_TopSys->SetBinContent(binno, h_TopSys->GetBinContent(binno)/h_TopSys->GetBinWidth(binno));
      }

      // Scale Errors
      h_Data->SetBinError(binno, h_Data->GetBinError(binno)/h_Data->GetBinWidth(binno));
      h_SM->SetBinError(binno, h_SM->GetBinError(binno)/h_SM->GetBinWidth(binno));
      for (sys = systematics.begin(); sys != systematics.end(); sys++){
        TString sys_name = *sys;
        h_background_sys[sys_name]->SetBinError(binno, h_background_sys[sys_name]->GetBinError(binno)/h_background_sys[sys_name]->GetBinWidth(binno));
      }


    }

  }




  ///////////////////////////////////////////////////////////////////
  // Calculate the generator (theoretical) systematics uncertainty //
  ///////////////////////////////////////////////////////////////////
  float sum_truth_Sherpa = 0;
  float sum_truth_MGPy8EG = 0;

  // Define related histograms
  TH1F *h_normalized_truth_MGPy8EG;
  TH1F *h_Ratio_MGPy8EG_Sherpa;
  TH1F *h_Deviation_MGPy8EG_Sherpa;


  if (GeneratorSYS) {
    // Integrate truth histograms over x
    sum_truth_Sherpa = h_truth_Sherpa->Integral();
    sum_truth_MGPy8EG = h_truth_MGPy8EG->Integral();

    // Normalize MGPy8EG histogram
    h_normalized_truth_MGPy8EG = (TH1F*)h_truth_MGPy8EG->Clone("h_normalized_truth_MGPy8EG");
    h_normalized_truth_MGPy8EG->Reset();

    for (int binno = 0; binno < h_normalized_truth_MGPy8EG->GetNbinsX() + 2; ++binno) {
      float content = h_truth_MGPy8EG->GetBinContent(binno)*sum_truth_Sherpa/sum_truth_MGPy8EG;
      h_normalized_truth_MGPy8EG->SetBinContent(binno, content);
    }

    // Ratio of MGPy8EG to Sherpa
    h_Ratio_MGPy8EG_Sherpa = (TH1F*)h_normalized_truth_MGPy8EG->Clone("h_Ratio_MGPy8EG_Sherpa");
    h_Ratio_MGPy8EG_Sherpa->Divide( (TH1F*)h_truth_Sherpa ); // MGPy8EG/Sherpa
    h_Ratio_MGPy8EG_Sherpa->SetMarkerStyle(kFullCircle);
    h_Ratio_MGPy8EG_Sherpa->SetMarkerSize(0.8);

    // Deviation between MGPy8EG and Sherpa: |1-(MGPy8EG/Sherpa)|
    h_Deviation_MGPy8EG_Sherpa = (TH1F*)h_Ratio_MGPy8EG_Sherpa->Clone("h_Deviation_MGPy8EG_Sherpa");
    h_Deviation_MGPy8EG_Sherpa->Reset();

    for (int binno = 0; binno < h_Deviation_MGPy8EG_Sherpa->GetNbinsX() + 2; ++binno) {
      float content = std::abs(1.-h_Ratio_MGPy8EG_Sherpa->GetBinContent(binno));
      h_Deviation_MGPy8EG_Sherpa->SetBinContent(binno, content);
      // Set proper statistical error bar
      float N = h_truth_MGPy8EG->GetBinContent(binno);
      float error = std::sqrt(N)/N;
      h_Deviation_MGPy8EG_Sherpa->SetBinError(binno, error);
    }

    // Set color to truth histograms
    h_truth_Sherpa->SetLineColor(kBlue);
    h_truth_MGPy8EG->SetLineColor(kRed);
    h_normalized_truth_MGPy8EG->SetLineColor(kGreen);

  }


  /*
  // Draw truth Sherpa, MGPy8EG and normalized MGPy8EG
  TCanvas *can2 = new TCanvas("can2_"+HISTO,"can2_"+HISTO,700,600);
  can2->cd(1);
  if (LOGY) gPad->SetLogy();
  h_truth_Sherpa->Draw();
  h_normalized_truth_MGPy8EG->Draw("same hist");
  h_truth_MGPy8EG->Draw("same hist");
  TLegend *leg2 = new TLegend(0.6,0.63,0.85,0.88);
  leg2->SetShadowColor(0);
  leg2->SetLineColor(0);
  leg2->SetFillStyle(0);
  leg2->SetFillColor(0);
  leg2->SetTextFont(42);
  leg2->SetTextSize(0.025);
  leg2->SetBorderSize(0);
  leg2->AddEntry(h_truth_Sherpa,"Truth Sherpa","l");
  leg2->AddEntry(h_truth_MGPy8EG,"Truth MGPy8EG","l");
  leg2->AddEntry(h_normalized_truth_MGPy8EG,"Truth normalized MGPy8EG","l");
  leg2->Draw();
 


  // Draw Ratio of MGPy8EG to Sherpa
  TCanvas *can3 = new TCanvas("can3_"+HISTO,"can3_"+HISTO,700,600);
  can3->cd(1);
  h_Ratio_MGPy8EG_Sherpa->Draw();


  // Draw Deviation between normalized MGPy8EG and Sherpa
  TCanvas *can4 = new TCanvas("can4_"+HISTO,"can4_"+HISTO,700,600);
  can4->cd(1);
  h_Deviation_MGPy8EG_Sherpa->Draw();

  */


  // Fit deviation plot
  TF1* fit;
  TH1F *h_Fit_MGPy8EG_Sherpa;
  if (GeneratorSYS) {
    fit = new TF1("fit", "pol1");
    h_Deviation_MGPy8EG_Sherpa->Fit("fit");

    h_Fit_MGPy8EG_Sherpa = (TH1F*)h_Deviation_MGPy8EG_Sherpa->Clone("h_Fit_MGPy8EG_Sherpa");
    h_Fit_MGPy8EG_Sherpa->Reset();

    for (int binno = 0; binno < h_Fit_MGPy8EG_Sherpa->GetNbinsX() + 2; ++binno) {
      float x = h_Fit_MGPy8EG_Sherpa->GetXaxis()->GetBinCenter(binno);
      float content = fit->Eval(x);
      if (content < 0) {
        h_Fit_MGPy8EG_Sherpa->SetBinContent(binno, 0.);
      } else {
        h_Fit_MGPy8EG_Sherpa->SetBinContent(binno, content);
      }
      cout << "bin# = " << binno << " content = " << h_Fit_MGPy8EG_Sherpa->GetBinContent(binno) << endl;
    }

  }


  ////////////////////////////////////////////////////
  // The theory uncertainties by Christian Gutschow //
  ////////////////////////////////////////////////////
  // https://indico.cern.ch/event/580269/contributions/2352327/attachments/1362194/2062389/cg_truthDefinition.pdf

  // binning method {underflow,value1,value2, ... , valueN,overflow}
  float Zee_Etmiss_PS_Monojet_total_up[9]={0.,0.128213921244,0.151689787837,0.176782035894,0.17055598344,0.188916179828,0.178233716093,0.169468548735,0.};
  float Zee_Etmiss_PS_Monojet_total_down[9]={0.,0.143957595568,0.163623820659,0.185525906704,0.173995128729,0.189700044818,0.184732315052,0.179526800963,0.};
  float Zee_Etmiss_PS_VBFDM_total_up[8]={0.,0.0945153376428,0.110437652562,0.1214425971,0.127979558951,0.146467472903,0.140158103081,0.};
  float Zee_Etmiss_PS_VBFDM_total_down[8]={0.,0.109667037385,0.127112284632,0.139360846035,0.138789958902,0.155541904387,0.149986600853,0.};
  float Zee_Mjj_PS_VBFDM_total_up[7]={0.,0.110054128119,0.106461826653,0.100876474903,0.0952445260859,0.0861262745987,0.};
  float Zee_Mjj_PS_VBFDM_total_down[7]={0.,0.127703924906,0.121397857345,0.111704522993,0.100927242988,0.0801153718991,0.};
  float Zee_DeltaPhi_PS_VBFDM_total_up[8]={0.,0.0932461754951,0.0982137626174,0.107368161081,0.116278644142,0.11396391007,0.0966099408819,0.};
  float Zee_DeltaPhi_PS_VBFDM_total_down[8]={0.,0.106673587707,0.11586687958,0.12384402717,0.133048232979,0.127975787634,0.106999884666,0.};

  float Zmumu_Etmiss_PS_Monojet_total_up[9]={0.,0.12913163862,0.15189204901,0.177056089376,0.168434742583,0.182461402868,0.171633954064,0.18192446784,0.};
  float Zmumu_Etmiss_PS_Monojet_total_down[9]={0.,0.146685228333,0.162131570174,0.182941790608,0.177015784301,0.186766602265,0.176747026218,0.174375164573,0.};
  float Zmumu_Etmiss_PS_VBFDM_total_up[8]={0.,0.0935974143635,0.112469941037,0.120332277251,0.125397340432,0.14180631603,0.136620927015,0.};
  float Zmumu_Etmiss_PS_VBFDM_total_down[8]={0.,0.105981815906,0.121190155581,0.135821096751,0.133797397569,0.14878370497,0.14070731886,0.};
  float Zmumu_Mjj_PS_VBFDM_total_up[7]={0.,0.11030968499,0.103782450654,0.100156613323,0.0949798598482,0.0901494058983,0.};
  float Zmumu_Mjj_PS_VBFDM_total_down[7]={0.,0.122555279417,0.115736458886,0.110011216987,0.0988980853456,0.0922805166684,0.};
  float Zmumu_DeltaPhi_PS_VBFDM_total_up[8]={0.,0.0930104041836,0.0988976102395,0.105283927382,0.118683884769,0.111876546835,0.0938216477327,0.};
  float Zmumu_DeltaPhi_PS_VBFDM_total_down[8]={0.,0.105036625162,0.0982639650397,0.118332307329,0.134088762318,0.125821758133,0.101737849571,0.};


  float frac_unc = 0;

  TH1F *h_manual_frac_uncertain;

  if (TheorySYS) {

    h_manual_frac_uncertain = (TH1F*)h_Data->Clone("h_manual_frac_uncertain");
    h_manual_frac_uncertain->Reset();

    for (int binno = 0; binno < h_manual_frac_uncertain->GetNbinsX() + 2; ++binno) {
      if (HISTO.Contains("znunu")){
        h_manual_frac_uncertain->SetBinContent(binno, 0.);
      }
      if (HISTO.Contains("zee")){
        if (HISTO.Contains("monojet_met")){
          frac_unc = std::max(Zee_Etmiss_PS_Monojet_total_up[binno],Zee_Etmiss_PS_Monojet_total_down[binno]);
          h_manual_frac_uncertain->SetBinContent(binno, frac_unc); 
        }
        if (HISTO.Contains("vbf_met")){
          frac_unc = std::max(Zee_Etmiss_PS_VBFDM_total_up[binno],Zee_Etmiss_PS_VBFDM_total_down[binno]);
          h_manual_frac_uncertain->SetBinContent(binno, frac_unc); 
        }
        if (HISTO.Contains("vbf_mjj")){
          frac_unc = std::max(Zee_Mjj_PS_VBFDM_total_up[binno],Zee_Mjj_PS_VBFDM_total_down[binno]);
          h_manual_frac_uncertain->SetBinContent(binno, frac_unc); 
        }
        if (HISTO.Contains("vbf_dPhijj")){
          frac_unc = std::max(Zee_DeltaPhi_PS_VBFDM_total_up[binno],Zee_DeltaPhi_PS_VBFDM_total_down[binno]);
          h_manual_frac_uncertain->SetBinContent(binno, frac_unc); 
        }
      }
      if (HISTO.Contains("zmumu")){
        if (HISTO.Contains("monojet_met")){
          frac_unc = std::max(Zmumu_Etmiss_PS_Monojet_total_up[binno],Zmumu_Etmiss_PS_Monojet_total_down[binno]);
          h_manual_frac_uncertain->SetBinContent(binno, frac_unc); 
        }
        if (HISTO.Contains("vbf_met")){
          frac_unc = std::max(Zmumu_Etmiss_PS_VBFDM_total_up[binno],Zmumu_Etmiss_PS_VBFDM_total_down[binno]);
          h_manual_frac_uncertain->SetBinContent(binno, frac_unc); 
        }
        if (HISTO.Contains("vbf_mjj")){
          frac_unc = std::max(Zmumu_Mjj_PS_VBFDM_total_up[binno],Zmumu_Mjj_PS_VBFDM_total_down[binno]);
          h_manual_frac_uncertain->SetBinContent(binno, frac_unc); 
        }
        if (HISTO.Contains("vbf_dPhijj")){
          frac_unc = std::max(Zmumu_DeltaPhi_PS_VBFDM_total_up[binno],Zmumu_DeltaPhi_PS_VBFDM_total_down[binno]);
          h_manual_frac_uncertain->SetBinContent(binno, frac_unc); 
        }
      }
      if (HISTO.Contains("zll")){
        if (HISTO.Contains("monojet_met")){
          frac_unc = (std::max(Zee_Etmiss_PS_Monojet_total_up[binno],Zee_Etmiss_PS_Monojet_total_down[binno])+std::max(Zmumu_Etmiss_PS_Monojet_total_up[binno],Zmumu_Etmiss_PS_Monojet_total_down[binno]))/2.;
          h_manual_frac_uncertain->SetBinContent(binno, frac_unc); 
        }
        if (HISTO.Contains("vbf_met")){
          frac_unc = (std::max(Zee_Etmiss_PS_VBFDM_total_up[binno],Zee_Etmiss_PS_VBFDM_total_down[binno])+std::max(Zmumu_Etmiss_PS_VBFDM_total_up[binno],Zmumu_Etmiss_PS_VBFDM_total_down[binno]))/2.;
          h_manual_frac_uncertain->SetBinContent(binno, frac_unc); 
        }
        if (HISTO.Contains("vbf_mjj")){
          frac_unc = (std::max(Zee_Mjj_PS_VBFDM_total_up[binno],Zee_Mjj_PS_VBFDM_total_down[binno])+std::max(Zmumu_Mjj_PS_VBFDM_total_up[binno],Zmumu_Mjj_PS_VBFDM_total_down[binno]))/2.;
          h_manual_frac_uncertain->SetBinContent(binno, frac_unc); 
        }
        if (HISTO.Contains("vbf_dPhijj")){
          frac_unc = (std::max(Zee_DeltaPhi_PS_VBFDM_total_up[binno],Zee_DeltaPhi_PS_VBFDM_total_down[binno])+std::max(Zmumu_DeltaPhi_PS_VBFDM_total_up[binno],Zmumu_DeltaPhi_PS_VBFDM_total_down[binno]))/2.;
          h_manual_frac_uncertain->SetBinContent(binno, frac_unc); 
        }
      }
      cout << "bin# = " << binno << " frac_unc = " << h_manual_frac_uncertain->GetBinContent(binno) << endl;
    }


  } //TheorySys End











  // Define Stack
  THStack *h_stackSM = new THStack("sm","sm");


  // Set Style for Data
  h_Data->SetMarkerStyle(kFullCircle);
  h_Data->SetMarkerSize(0.8);
  h_Data->SetLineColor(kBlack);

  // Ratio of Data/MC
  TH1F *h_Ratio = (TH1F*)h_Data->Clone("Ratio");

  h_Ratio->Divide( (TH1F*)h_SM ); // Divided by MC (Data/MC)
  h_Ratio->SetMarkerStyle(kFullCircle);
  h_Ratio->SetMarkerSize(0.8);

  // Set errors for ratio (Data/MC)
  // Assuming uncorrelated errors, the formula for the error on a ratio (x/y) is
  // d(x/y) = (x/y) * sqrt( sqr( dx/x ) + sqr ( dy/y ) )
  for (int binno = 0; binno < h_SM->GetNbinsX() + 2; ++binno) {
    float data_val = h_Data->GetBinContent(binno);
    float mc_val = h_SM->GetBinContent(binno);
    float data_err =  h_Data->GetBinError(binno);
    float mc_err =  h_SM->GetBinError(binno);
    float ratio_err = 0.;
    if (data_val == 0 || mc_val == 0) {
      ratio_err = 0.;
    } else {
      ratio_err = (data_val/mc_val)*std::sqrt( std::pow(data_err/data_val,2) + std::pow(mc_err/mc_val,2) );
    }

    cout << " ratio_err (auto) = " << h_Ratio->GetBinError(binno) << endl;

    h_Ratio->SetBinError(binno, ratio_err);

    cout << " ratio_err (manual) = " << h_Ratio->GetBinError(binno) << endl;
  }



  // Set style for MC backgrounds
  h_Multijet->SetLineColor(kBlack);
  h_Znunu->SetLineColor(kBlack);
  h_Wenu->SetLineColor(kBlack);
  h_Wmunu->SetLineColor(kBlack);
  h_Wtaunu->SetLineColor(kBlack);
  h_Zee->SetLineColor(kBlack);
  h_Zmumu->SetLineColor(kBlack);
  h_Ztautau->SetLineColor(kBlack);
  h_ttbar->SetLineColor(kBlack);
  h_Diboson->SetLineColor(kBlack);

  h_Multijet->SetFillColor(kGray);
  h_Znunu->SetFillColor(kRed-7);
  h_Wenu->SetFillColor(kCyan+1);
  h_Wmunu->SetFillColor(kRed-3);
  h_Wtaunu->SetFillColor(kYellow-7);
  h_Zee->SetFillColor(kOrange+1);
  h_Zmumu->SetFillColor(kOrange+8);
  h_Ztautau->SetFillColor(kBlue-3);
  h_ttbar->SetFillColor(kGreen+2);
  h_Diboson->SetFillColor(kMagenta+1);

  // For Zll
  if ( HISTO.Contains("zll") ){
    h_Zll->SetLineColor(kBlack);
    h_Zll->SetFillColor(kOrange+1);
  }

  // For Wlnu
  if ( HISTO.Contains("zll") ){
    h_Wlnu->SetLineColor(kBlack);
    h_Wlnu->SetFillColor(kCyan+1);
  }



  // Fill Stack
  // Zmumu
  if ( HISTO.Contains("mumu") ) {
    h_stackSM->Add(h_Zee);
    h_stackSM->Add(h_Multijet);
    h_stackSM->Add(h_Znunu);
    h_stackSM->Add(h_Ztautau);
    h_stackSM->Add(h_Wtaunu);
    h_stackSM->Add(h_Wmunu);
    h_stackSM->Add(h_Wenu);
    h_stackSM->Add(h_Diboson);
    h_stackSM->Add(h_ttbar);
    h_stackSM->Add(h_Zmumu);
  }

  // Zee
  if ( HISTO.Contains("ee") ) {
    h_stackSM->Add(h_Zmumu);
    h_stackSM->Add(h_Multijet);
    h_stackSM->Add(h_Znunu);
    h_stackSM->Add(h_Ztautau);
    h_stackSM->Add(h_Wtaunu);
    h_stackSM->Add(h_Wmunu);
    h_stackSM->Add(h_Wenu);
    h_stackSM->Add(h_Diboson);
    h_stackSM->Add(h_ttbar);
    h_stackSM->Add(h_Zee);
  }

  // Znunu
  if ( HISTO.Contains("nunu") ){
    h_stackSM->Add(h_Multijet);
    h_stackSM->Add(h_Zee);
    h_stackSM->Add(h_Ztautau);
    h_stackSM->Add(h_Zmumu);
    h_stackSM->Add(h_Diboson);
    h_stackSM->Add(h_ttbar);
    h_stackSM->Add(h_Wenu);
    h_stackSM->Add(h_Wmunu);
    h_stackSM->Add(h_Wtaunu);
    h_stackSM->Add(h_Znunu);
  }

  // Zll
  if ( HISTO.Contains("zll") ){
    h_stackSM->Add(h_Multijet);
    h_stackSM->Add(h_Znunu);
    h_stackSM->Add(h_Ztautau);
    h_stackSM->Add(h_Wlnu);
    //h_stackSM->Add(h_Wtaunu);
    //h_stackSM->Add(h_Wmunu);
    //h_stackSM->Add(h_Wenu);
    h_stackSM->Add(h_Diboson);
    h_stackSM->Add(h_ttbar);
    h_stackSM->Add(h_Zll);
  }





  //////////////////////////////////
  // Total systematic uncertainty //
  //////////////////////////////////

  // Calculate the total systematic uncertainty from all systematics sources
  // Tot_sys_error = Sqrt( max( abs(sys1_up - nominal) ,abs(sys1_down - nominal))^2 + max( abs(sys2_up - nominal) ,abs(sys2_down - nominal))^2 + ... )  where sys1_up is the ratio of sys1_up to nominal
  TH1F *h_total_sys = (TH1F*)h_background_sys[""]->Clone("h_total_sys");
  h_total_sys->SetLineColor(kBlack);

  // Calculate the total fraction uncertainty from all systematics sources (add it to the ratio)
  // Tot_sys_frac = Sqrt( max( abs(sys1_up - nominal) ,sys1_down - nominal)^2 + max( abs(sys2_up - nominal) ,sys2_down - nominal)^2 + ... ) / nominal  , where sys1_up is the ratio of sys1_up to nominal
  TH1F *h_total_sys_frac = (TH1F*)h_background_sys[""]->Clone("h_total_sys_frac");
  h_total_sys_frac->Reset();
  h_total_sys_frac->SetLineColor(kBlack);


  for (int binno = 0; binno < h_background_sys[""]->GetNbinsX() + 2; ++binno) {
    if ( h_background_sys[""]->GetBinContent(binno) == 0. ) { // When using blinded histograms, bin contents has 0 values which turns out that total uncertainty become very high in blinded regions.
      h_total_sys->SetBinError(binno, 0.); // Make errors 0% in blinded region.
      h_total_sys_frac->SetBinContent(binno, 0.); // Make contents 0% in blinded region.
      h_total_sys_frac->SetBinError(binno, 0.); // Make errors 0% in blinded region.
    }
    else {
      h_total_sys_frac->SetBinContent(binno, 1.); // Make contents 1. for each bin for ratio plot
      float nominal = h_background_sys[""]->GetBinContent(binno);
      float generator_unc = 0.; // Calculate the generator uncertainty (my method)
      float theory_unc = 0.; // Calculate the theory uncertainty (Christian Gutschow)
      float top_unc = 0.; // Calculate the top background uncertainty from the e-mu control region
      if (HISTO.Contains("zee") || HISTO.Contains("zmumu") || HISTO.Contains("zll") ) { // Truth histograms exist only in Zee, Zmumu and Zll at this time
        if ( GeneratorSYS ) { // Calculate the generator uncertainty from my method (fitting)
          generator_unc = h_Fit_MGPy8EG_Sherpa->GetBinContent(binno) * nominal;
        }
        if ( TheorySYS ) { // Calculate the generator uncertainty by Christian Gutschow
          theory_unc = h_manual_frac_uncertain->GetBinContent(binno) * nominal;
        }
        if ( TopReweight ) {
          top_unc = h_TopSys->GetBinContent(binno);
        }
      }
      float syst_tot = std::sqrt( std::pow(std::max(std::abs(h_background_sys["EG_RESOLUTION_ALL__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["EG_RESOLUTION_ALL__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["EG_SCALE_ALL__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["EG_SCALE_ALL__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["EL_EFF_ID_TotalCorrUncertainty__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["EL_EFF_ID_TotalCorrUncertainty__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["EL_EFF_Iso_TotalCorrUncertainty__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["EL_EFF_Iso_TotalCorrUncertainty__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["EL_EFF_Reco_TotalCorrUncertainty__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["EL_EFF_Reco_TotalCorrUncertainty__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["EL_EFF_TriggerEff_TotalCorrUncertainty__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["EL_EFF_TriggerEff_TotalCorrUncertainty__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["EL_EFF_Trigger_TotalCorrUncertainty__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["EL_EFF_Trigger_TotalCorrUncertainty__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["JET_EtaIntercalibration_NonClosure__1up"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["JET_EtaIntercalibration_NonClosure__1down"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["JET_GroupedNP_1__1up"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["JET_GroupedNP_1__1down"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["JET_GroupedNP_2__1up"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["JET_GroupedNP_2__1down"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["JET_GroupedNP_3__1up"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["JET_GroupedNP_3__1down"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::abs(h_background_sys["JET_JER_SINGLE_NP__1up"]->GetBinContent(binno)-nominal),2) +
          std::pow(std::max(std::abs(h_background_sys["JvtEfficiencyDown"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["JvtEfficiencyUp"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::abs(h_background_sys["MET_SoftTrk_ResoPara"]->GetBinContent(binno)-nominal),2) +
          std::pow(std::abs(h_background_sys["MET_SoftTrk_ResoPerp"]->GetBinContent(binno)-nominal),2) +
          std::pow(std::max(std::abs(h_background_sys["MET_SoftTrk_ScaleDown"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["MET_SoftTrk_ScaleUp"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["MUONS_ID__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["MUONS_ID__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["MUONS_MS__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["MUONS_MS__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["MUONS_SCALE__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["MUONS_SCALE__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["MUON_EFF_STAT__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["MUON_EFF_STAT__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["MUON_EFF_STAT_LOWPT__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["MUON_EFF_STAT_LOWPT__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["MUON_EFF_SYS__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["MUON_EFF_SYS__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["MUON_EFF_SYS_LOWPT__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["MUON_EFF_SYS_LOWPT__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["MUON_EFF_TrigStatUncertainty__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["MUON_EFF_TrigStatUncertainty__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["MUON_EFF_TrigSystUncertainty__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["MUON_EFF_TrigSystUncertainty__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["MUON_ISO_STAT__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["MUON_ISO_STAT__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["MUON_ISO_SYS__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["MUON_ISO_SYS__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["MUON_TTVA_STAT__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["MUON_TTVA_STAT__1up"]->GetBinContent(binno)-nominal)),2) +
          std::pow(std::max(std::abs(h_background_sys["MUON_TTVA_SYS__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["MUON_TTVA_SYS__1up"]->GetBinContent(binno)-nominal)),2) +
          //std::pow(std::max(std::abs(h_background_sys["PRW_DATASF__1down"]->GetBinContent(binno)-nominal),std::abs(h_background_sys["PRW_DATASF__1up"]->GetBinContent(binno)-nominal)),2) + // commented out asked by Prof. Beauchemin
          std::pow(std::abs(generator_unc),2) + // For generator uncertainty by my method
          std::pow(std::abs(theory_unc),2) + // For theory uncertainty by Christian Gutschow
          std::pow(top_unc,2) + // Difference between unreweighted and reweighted top background
          std::pow(std::abs(h_SM->GetBinError(binno)),2)  // For MC statistical uncertainty
          );
        cout << "bin # = " << binno << " N_Data = " << h_Data->GetBinContent(binno) << ", Data statistical uncertainty  = " << h_Data->GetBinError(binno) << endl;
        cout << "bin # = " << binno << " N_MC_est = " << h_SM->GetBinContent(binno)  << ", MC statistical uncertainty  = " << h_SM->GetBinError(binno) << endl;
        cout << "bin # = " << binno << " Ratio (Data/MC) statistical uncertainty  = " << h_Ratio->GetBinError(binno) << endl;
        cout << "bin # = " << binno << " total error = " << syst_tot << endl;
      if (GeneratorSYS) {
        //cout << "bin # = " << binno << " content = " << nominal << endl;
        //cout << "bin # = " << binno << " generator error (original) = " << h_Deviation_MGPy8EG_Sherpa->GetBinContent(binno) * nominal << endl;
        //cout << "bin # = " << binno << " generator error (fit) = " << generator_unc << endl;
        //cout << "bin # = " << binno << " total error = " << syst_tot << endl;
      }
      h_total_sys->SetBinError(binno, syst_tot);
      // For the fraction
      float syst_tot_frac = syst_tot / nominal;
      h_total_sys_frac->SetBinError(binno, syst_tot_frac);

    }
  }

  // Set style for h_total_sys
  h_total_sys->SetFillColor(kBlack);
  h_total_sys->SetFillStyle(3004);
  h_total_sys->SetMarkerSize(0);

  // Set style for h_total_sys_frac
  h_total_sys_frac->SetFillColor(kBlack);
  h_total_sys_frac->SetFillStyle(3004);
  h_total_sys_frac->SetMarkerSize(0);






  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);


  // Draw Data/MC Comparison plots
  TCanvas *can = new TCanvas("can_"+HISTO,"can_"+HISTO,700,600);
  can->cd(1);
  can->SetBottomMargin(0.3);
  //can->SetRightMargin(0.2);
  if (LOGY) gPad->SetLogy();
  // Ticks on both sides of the histogram
  gPad->SetTickx();
  gPad->SetTicky();
  TH1F *h_aux = (TH1F*)h_SM->Clone("aux");
  h_aux->Reset();
  if (h_aux->GetBinWidth(1) != h_aux->GetBinWidth(4)) {
    if (DivBinWidth) {
      sprintf(name,"Events / "+UNITS);
    } else {
      sprintf(name,"Number of events");
    }
  }
  else {
    if (h_aux->GetBinWidth(1) >= 1) {
      //sprintf(name,"Number of events / %1.0f %s",h_aux->GetBinWidth(1),UNITS.Data());
      if (DivBinWidth) {
        sprintf(name,"Events / "+UNITS);
      } else {
        sprintf(name,"Number of events");
      }
    }
    else {
      //sprintf(name,"Number of events / %1.2f %s",h_aux->GetBinWidth(1),UNITS.Data());
      if (DivBinWidth) {
        sprintf(name,"Events / "+UNITS);
      } else {
        sprintf(name,"Number of events");
      }
    }
  }
  h_aux->GetYaxis()->SetTitle(name);

  //h_aux->GetXaxis()->SetTitle(XTITLE);
  h_aux->GetXaxis()->SetTitleSize(0.0);
  h_aux->GetXaxis()->SetLabelSize(0.0);
  h_aux->GetYaxis()->SetTitleSize(0.035);
  h_aux->GetYaxis()->SetLabelSize(0.030);
  h_aux->GetYaxis()->SetTitleOffset(1.4);
  h_aux->GetYaxis()->SetNdivisions(505);
  h_aux->GetXaxis()->SetNdivisions(XDIV);
  h_aux->GetXaxis()->SetRangeUser(XMIN,XMAX);
  if (LOGY){ // For log histograms
    if (DivBinWidth) {
      h_aux->SetMinimum(0.001); // log plot (when divided by BinWidth)
    } else {
      h_aux->SetMinimum(0.5); // log plot
    }
  } else {
    h_aux->SetMinimum(0.); // linear plot
  }
  //h_aux->SetMaximum(1.2*h_Data->GetBinContent(h_Data->GetMaximumBin()));
  float ymax; // Set maximum y xaxis depending on Logy or not
  if (LOGY){ // For log histograms
    if (HISTO.Contains("Phi")) ymax = 1000.*h_Data->GetBinContent(h_Data->GetMaximumBin()); // For deltaPhijj histogram
    else ymax = 2.*h_Data->GetBinContent(h_Data->GetMaximumBin());
  } else ymax = 1.3*h_Data->GetBinContent(h_Data->GetMaximumBin()); // For linear histogram
  h_aux->SetMaximum(ymax);
  h_aux->Draw();
  h_stackSM->Draw("same hist");
  h_Data->Draw("same E");
  h_total_sys->Draw("same E2");

  if (ATLAS) {
    if (LOGY){
      myText( 0.33, 0.79, 1, "#scale[0.8]{#sqrt{s} = 13 TeV,}");
      myText( 0.48, 0.79, 1, "#scale[0.8]{"+intLumi+"}");
      myText( 0.33, 0.74, 1, "#scale[0.8]{"+channel+"}");
      //ATLASLabel(0.33,0.84,"Internal");
      //ATLASLabel(0.33,0.84,"Preliminary");
      ATLASLabel(0.33,0.84,"");
    } else {
      myText( 0.38, 0.79, 1, "#scale[0.8]{#sqrt{s} = 13 TeV,}");
      myText( 0.53, 0.79, 1, "#scale[0.8]{"+intLumi+"}");
      myText( 0.38, 0.74, 1, "#scale[0.8]{"+channel+"}");
      //ATLASLabel(0.38,0.84,"Internal");
      //ATLASLabel(0.38,0.84,"Preliminary");
      ATLASLabel(0.38,0.84,"");
    }
  }

  TLegend *leg;
  if (LOGY){
    leg = new TLegend(0.65,0.60,0.88,0.86);
  } else {
    leg = new TLegend(0.13,0.60,0.38,0.86);
  }
  //leg->SetHeader("Presel. & Trigger");

  //leg->SetMargin(0.1);
  leg->SetShadowColor(0);
  leg->SetLineColor(0);
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.028);
  leg->SetBorderSize(0);
  leg->AddEntry(h_Data,"Data 2015","P");
  leg->AddEntry(h_total_sys,"MC Syst. Unc.","F");
  if (HISTO.Contains("mumu") || HISTO.Contains("ee") || HISTO.Contains("ll") ) {
    if (HISTO.Contains("mumu")) leg->AddEntry(h_Zmumu,"Z(#rightarrow #mu#mu)+jets","F"); // Z->mumu
    if (HISTO.Contains("ee")) leg->AddEntry(h_Zee,"Z(#rightarrow ee)+jets","F"); // Z->ee
    if (HISTO.Contains("ll")) leg->AddEntry(h_Zll,"Z(#rightarrow ll)+jets","F"); // Z->ll
    leg->AddEntry(h_ttbar,"t#bar{t} (+X) + single top","F");
    leg->AddEntry(h_Diboson,"Diboson","F");
    if (HISTO.Contains("mumu") || HISTO.Contains("ee") ) {
      leg->AddEntry(h_Wenu,"W(#rightarrow e#nu)+jets","F");
      leg->AddEntry(h_Wmunu,"W(#rightarrow #mu#nu)+jets","F");
      leg->AddEntry(h_Wtaunu,"W(#rightarrow #tau#nu)+jets","F");
    }
    if (HISTO.Contains("ll")) {
      leg->AddEntry(h_Wlnu,"W(#rightarrow l#nu)+jets","F");
    }
    leg->AddEntry(h_Ztautau,"Z(#rightarrow #tau#tau)+jets","F");
    //leg->AddEntry(h_Multijet,"Multijet","F");
  }
  if (HISTO.Contains("nunu")) {
    leg->AddEntry(h_Znunu,"Z(#rightarrow #nu#nu)+jets","F"); // Z->nunu
    leg->AddEntry(h_Wtaunu,"W(#rightarrow #tau#nu)+jets","F");
    leg->AddEntry(h_Wmunu,"W(#rightarrow #mu#nu)+jets","F");
    leg->AddEntry(h_Wenu,"W(#rightarrow e#nu)+jets","F");
    leg->AddEntry(h_ttbar,"t#bar{t} (+X) + single top","F");
    leg->AddEntry(h_Diboson,"Diboson","F");
    leg->AddEntry(h_Zmumu,"Z(#rightarrow #mu#mu)+jets","F"); // Z->mumu
    leg->AddEntry(h_Ztautau,"Z(#rightarrow #tau#tau)+jets","F");
    leg->AddEntry(h_Zee,"Z(#rightarrow ee)+jets","F"); // Z->ee
    //leg->AddEntry(h_Multijet,"Multijet","F");
  }
  leg->Draw();
  gPad->RedrawAxis();
  TPad* pad = new TPad("pad", "pad", 0., 0., 1., 1.);
  pad->SetTopMargin(0.72);
  //pad->SetRightMargin(0.2);
  pad->SetFillColor(0);
  pad->SetFillStyle(0);
  pad->Draw();
  pad->cd(0);
  gPad->SetGridy();
  // Ticks on both sides of the histogram
  gPad->SetTickx();
  gPad->SetTicky();
  h_Ratio->SetMinimum(0.5);
  h_Ratio->SetMaximum(1.5);
  h_Ratio->GetYaxis()->SetTitle("Data / SM");
  if (UNITS == "") {
    h_Ratio->GetXaxis()->SetTitle(XTITLE);
  }
  else {
    h_Ratio->GetXaxis()->SetTitle(XTITLE+" ["+UNITS+"]");
  }
  h_Ratio->GetYaxis()->SetNdivisions(505);
  h_Ratio->GetYaxis()->SetTickLength(0.06);
  h_Ratio->GetXaxis()->SetTitleSize(0.040);
  h_Ratio->GetXaxis()->SetLabelSize(0.030);
  h_Ratio->GetYaxis()->SetTitleSize(0.035);
  h_Ratio->GetYaxis()->SetLabelSize(0.030);
  h_Ratio->GetYaxis()->SetTitleOffset(1.4);
  h_Ratio->GetXaxis()->SetNdivisions(XDIV);
  h_Ratio->GetYaxis()->CenterTitle(kTRUE);
  h_Ratio->GetXaxis()->SetRangeUser(XMIN,XMAX);
  h_Ratio->Draw();
  h_total_sys_frac->Draw("same E2");


/*

  //////////////////////////
  /////// Test!!!! /////////
  //////////////////////////
  TFile *f_test = new TFile("hist_test.root","UPDATE");

  TString hist1_name;
  TString hist2_name;
  TString ratio1_name;
  
  if ( HISTO.Contains("monojet_met") ) {
    hist1_name = "top_monojet_met";
    hist2_name = "W_monojet_met";
    ratio1_name = "ratio_monojet_met";
  }
  if ( HISTO.Contains("vbf_met") ) {
    hist1_name = "top_vbf_met";
    hist2_name = "W_vbf_met";
    ratio1_name = "ratio_vbf_met";
  }
  if ( HISTO.Contains("vbf_mjj") ) {
    hist1_name = "top_vbf_mjj";
    hist2_name = "W_vbf_mjj";
    ratio1_name = "ratio_vbf_mjj";
  }
  if ( HISTO.Contains("vbf_dPhijj") ) {
    hist1_name = "top_vbf_dPhijj";
    hist2_name = "W_vbf_dPhijj";
    ratio1_name = "ratio_vbf_dPhijj";
  }


  TH1F *h_hist1 = (TH1F*)h_ttbar->Clone(hist1_name);
  TH1F *h_hist2 = (TH1F*)h_Wenu->Clone(hist2_name);
  h_hist2->Add(h_Wmunu);
  h_hist2->Add(h_Wtaunu);

  // Ratio of hist1/hist2
  TH1F *h_ratio1 = (TH1F*)h_hist1->Clone(ratio1_name);

  h_ratio1->Divide( (TH1F*)h_hist2 ); // Divided by hist2 (hist1/hist2)
  h_ratio1->SetMarkerStyle(kFullCircle);
  h_ratio1->SetMarkerSize(0.8);


  // Write a file
  // Data
  h_ratio1->SetDirectory(f_test);

  f_test->Write();
  f_test->Close();
  //////////////////////////
  /////// Test End /////////
  //////////////////////////

*/






  if (PRINT) {
    //can->Print(TString(can->GetTitle())+".png");
    can->Print(HISTO+".pdf");

    // Delete objects
    delete can;
    //delete can2;
    //delete can3;
    //delete can4;
    delete file_MCData;
    delete file_Sys;
  }

}


/*
#ifndef __CINT__
int main(){
DrawHistogram(HISTO,XTITLE,XMIN,XMAX,REBIN,LOGY,PRINT,XDIV,UNITS);
}
#endif
 */
