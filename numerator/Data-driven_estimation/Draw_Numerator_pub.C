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


void Draw_Numerator_pub(TString HISTO,TString XTITLE,float XMIN,float XMAX,bool REBIN,bool LOGY,bool PRINT,int XDIV,TString UNITS,bool TheorySYS, bool ATLAS, bool DivBinWidth)
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
  if ( HISTO.Contains("mono") ) {
    channel = "Z#rightarrow#nu#nu #geq1 jet"; //Zmunu monojet
  } else {
    channel = "Z#rightarrow#nu#nu VBF"; //Znunu VBF
  }

  // PATH to the location of input histograms
  TString PATH("/home/hson/Work/Run2_Study/VBF_study/Macro/numerator_hist/");


  // Retrieve files
  TString filename_Data("ZnunuUnblinded.root"); // Data
  TString filename_Bkg_Znunu("ZnunuMC.root"); // MC BKG Znunu
  TString filename_Bkg_Zll("ZllBackground.root"); // Data-Driven BKG Zll
  TString filename_Bkg_Wenu("WenuBackground.root"); // Data-Driven BKG Wenu
  TString filename_Bkg_Wmunu("WmunuBackground.root"); // Data-Driven BKG Wmunu
  TString filename_Bkg_Wtaunu("WtaunuBackground.root"); // Data-Driven BKG Wtaunu
  TString filename_Bkg_Multijet("mj_final_161123.root"); // Multijet BKG
  TString filename_Tot_Sys("TotalSystematic.root"); // Total Systematics uncertainties
  TFile *file_Data = TFile::Open(PATH+filename_Data);
  TFile *file_Bkg_Znunu = TFile::Open(PATH+filename_Bkg_Znunu);
  TFile *file_Bkg_Zll = TFile::Open(PATH+filename_Bkg_Zll);
  TFile *file_Bkg_Wenu = TFile::Open(PATH+filename_Bkg_Wenu);
  TFile *file_Bkg_Wmunu = TFile::Open(PATH+filename_Bkg_Wmunu);
  TFile *file_Bkg_Wtaunu = TFile::Open(PATH+filename_Bkg_Wtaunu);
  TFile *file_Bkg_Multijet = TFile::Open(PATH+filename_Bkg_Multijet);
  TFile *file_Tot_Sys = TFile::Open(PATH+filename_Tot_Sys);

  // Define Rebinning
  Double_t binsMET[] = {200.,250.,300.,350.,500.,700.,1400.};
  Int_t nbinMET = sizeof(binsMET)/sizeof(Double_t) - 1;
  Double_t binsMjj[] = {200.,400.,600.,1000.,2000.,4000.};
  Int_t nbinMjj = sizeof(binsMjj)/sizeof(Double_t) - 1;

  char name[1000];

  ////////////////////////////////////////////////////////////
  // Retrieve Histograms (Data, background and Systematics) //
  ////////////////////////////////////////////////////////////

  // HISTO names : 'MET_mono' for the >=1 jet phase space
  //               'MET_search' , 'Mjj_search' and 'dPhijj' for the VBF phase space
  //               The dPhijj plot is called 'DeltaPhiAll' in the multijet file.

  TH1::SetDefaultSumw2(kTRUE);

  // Retrieve histograms
  TH1F *h_Data = (TH1F*)file_Data->Get(HISTO);
  TH1F *h_Bkg_Znunu = (TH1F*)file_Bkg_Znunu->Get(HISTO);
  TH1F *h_Bkg_Zll = (TH1F*)file_Bkg_Zll->Get(HISTO);
  TH1F *h_Bkg_Wenu = (TH1F*)file_Bkg_Wenu->Get(HISTO);
  TH1F *h_Bkg_Wmunu = (TH1F*)file_Bkg_Wmunu->Get(HISTO);
  TH1F *h_Bkg_Wtaunu = (TH1F*)file_Bkg_Wtaunu->Get(HISTO);
  TH1F *h_Bkg_Multijet;
  if (HISTO.Contains("dPhijj")){
    h_Bkg_Multijet = (TH1F*)file_Bkg_Multijet->Get("DeltaPhiAll");
  } else {
    h_Bkg_Multijet = (TH1F*)file_Bkg_Multijet->Get(HISTO);
  }
  TH1F *h_Tot_Sys_nominal = (TH1F*)file_Tot_Sys->Get(HISTO+"TotalSys"); // This contains SM prediction histograms (bin content) and total systematic uncertainty (bin error)
  TH1F *h_Tot_Sys_ratio = (TH1F*)file_Tot_Sys->Get(HISTO+"hRatioTotalSys"); // This contains SM prediction ratio (bin content) and total systematic uncertainty ratio (bin error)
  TH1F *h_Tot_Sys_up = (TH1F*)file_Tot_Sys->Get(HISTO+"TotalSysUp");
  TH1F *h_Tot_Sys_down = (TH1F*)file_Tot_Sys->Get(HISTO+"TotalSysDown");
  // Rebin histograms (only for VBF MET and Mjj histograms)
  if (REBIN && HISTO.Contains("MET_search")) h_Data = dynamic_cast<TH1F*>(h_Data->Rebin(nbinMET,"h_Data",binsMET));
  if (REBIN && HISTO.Contains("Mjj_search")) h_Data = dynamic_cast<TH1F*>(h_Data->Rebin(nbinMET,"h_Data",binsMjj));
  if (REBIN && HISTO.Contains("MET_search")) h_Bkg_Znunu = dynamic_cast<TH1F*>(h_Bkg_Znunu->Rebin(nbinMET,"h_Bkg_Znunu",binsMET));
  if (REBIN && HISTO.Contains("Mjj_search")) h_Bkg_Znunu = dynamic_cast<TH1F*>(h_Bkg_Znunu->Rebin(nbinMET,"h_Bkg_Znunu",binsMjj));
  if (REBIN && HISTO.Contains("MET_search")) h_Bkg_Zll = dynamic_cast<TH1F*>(h_Bkg_Zll->Rebin(nbinMET,"h_Bkg_Zll",binsMET));
  if (REBIN && HISTO.Contains("Mjj_search")) h_Bkg_Zll = dynamic_cast<TH1F*>(h_Bkg_Zll->Rebin(nbinMET,"h_Bkg_Zll",binsMjj));
  if (REBIN && HISTO.Contains("MET_search")) h_Bkg_Wenu = dynamic_cast<TH1F*>(h_Bkg_Wenu->Rebin(nbinMET,"h_Bkg_Wenu",binsMET));
  if (REBIN && HISTO.Contains("Mjj_search")) h_Bkg_Wenu = dynamic_cast<TH1F*>(h_Bkg_Wenu->Rebin(nbinMET,"h_Bkg_Wenu",binsMjj));
  if (REBIN && HISTO.Contains("MET_search")) h_Bkg_Wmunu = dynamic_cast<TH1F*>(h_Bkg_Wmunu->Rebin(nbinMET,"h_Bkg_Wmunu",binsMET));
  if (REBIN && HISTO.Contains("Mjj_search")) h_Bkg_Wmunu = dynamic_cast<TH1F*>(h_Bkg_Wmunu->Rebin(nbinMET,"h_Bkg_Wmunu",binsMjj));
  if (REBIN && HISTO.Contains("MET_search")) h_Bkg_Wtaunu = dynamic_cast<TH1F*>(h_Bkg_Wtaunu->Rebin(nbinMET,"h_Bkg_Wtaunu",binsMET));
  if (REBIN && HISTO.Contains("Mjj_search")) h_Bkg_Wtaunu = dynamic_cast<TH1F*>(h_Bkg_Wtaunu->Rebin(nbinMET,"h_Bkg_Wtaunu",binsMjj));
  if (REBIN && HISTO.Contains("MET_search")) h_Bkg_Multijet = dynamic_cast<TH1F*>(h_Bkg_Multijet->Rebin(nbinMET,"h_Bkg_Multijet",binsMET));
  if (REBIN && HISTO.Contains("Mjj_search")) h_Bkg_Multijet = dynamic_cast<TH1F*>(h_Bkg_Multijet->Rebin(nbinMET,"h_Bkg_Multijet",binsMjj));
  if (REBIN && HISTO.Contains("MET_search")) h_Tot_Sys_nominal = dynamic_cast<TH1F*>(h_Tot_Sys_nominal->Rebin(nbinMET,"h_Tot_Sys_nominal",binsMET));
  if (REBIN && HISTO.Contains("Mjj_search")) h_Tot_Sys_nominal = dynamic_cast<TH1F*>(h_Tot_Sys_nominal->Rebin(nbinMET,"h_Tot_Sys_nominal",binsMjj));
  if (REBIN && HISTO.Contains("MET_search")) h_Tot_Sys_up = dynamic_cast<TH1F*>(h_Tot_Sys_up->Rebin(nbinMET,"h_Tot_Sys_up",binsMET));
  if (REBIN && HISTO.Contains("Mjj_search")) h_Tot_Sys_up = dynamic_cast<TH1F*>(h_Tot_Sys_up->Rebin(nbinMET,"h_Tot_Sys_up",binsMjj));
  if (REBIN && HISTO.Contains("MET_search")) h_Tot_Sys_down = dynamic_cast<TH1F*>(h_Tot_Sys_down->Rebin(nbinMET,"h_Tot_Sys_down",binsMET));
  if (REBIN && HISTO.Contains("Mjj_search")) h_Tot_Sys_down = dynamic_cast<TH1F*>(h_Tot_Sys_down->Rebin(nbinMET,"h_Tot_Sys_down",binsMjj));

  // Retrieve the MC Prediction (SM) histogram
  TH1F *h_SM = (TH1F*)h_Bkg_Znunu->Clone("h_SM");
  h_SM->Add(h_Bkg_Zll);
  h_SM->Add(h_Bkg_Wenu);
  h_SM->Add(h_Bkg_Wmunu);
  h_SM->Add(h_Bkg_Wtaunu);
  h_SM->Add(h_Bkg_Multijet);


  // Set the errors in Data (The histograms in data don't have the error set to sqrt(bin content) by default)
  for (int binno = 0; binno < h_Data->GetNbinsX() + 2; ++binno) {
    float content = h_Data->GetBinContent(binno);
    h_Data->SetBinError(binno, std::sqrt(content));
  }



  // Dividing by bin width as we are more used to in “measurement” papers
  if (DivBinWidth) {

    // Set the statistics error in SM MC for ratio plot (because this histogram have the error as not statistics but the total systematic uncertainty)
    for (int binno = 0; binno < h_SM->GetNbinsX() + 2; ++binno) {
      //cout << "binno = " << binno << " binwidth = " << h_SM->GetBinWidth(binno) << endl;

      // Scale Contents
      h_Data->SetBinContent(binno, h_Data->GetBinContent(binno)/h_Data->GetBinWidth(binno));
      h_SM->SetBinContent(binno, h_SM->GetBinContent(binno)/h_SM->GetBinWidth(binno));
      h_Bkg_Znunu->SetBinContent(binno, h_Bkg_Znunu->GetBinContent(binno)/h_Bkg_Znunu->GetBinWidth(binno));
      h_Bkg_Zll->SetBinContent(binno, h_Bkg_Zll->GetBinContent(binno)/h_Bkg_Zll->GetBinWidth(binno));
      h_Bkg_Wenu->SetBinContent(binno, h_Bkg_Wenu->GetBinContent(binno)/h_Bkg_Wenu->GetBinWidth(binno));
      h_Bkg_Wmunu->SetBinContent(binno, h_Bkg_Wmunu->GetBinContent(binno)/h_Bkg_Wmunu->GetBinWidth(binno));
      h_Bkg_Wtaunu->SetBinContent(binno, h_Bkg_Wtaunu->GetBinContent(binno)/h_Bkg_Wtaunu->GetBinWidth(binno));
      h_Bkg_Multijet->SetBinContent(binno, h_Bkg_Multijet->GetBinContent(binno)/h_Bkg_Multijet->GetBinWidth(binno));
      h_Tot_Sys_nominal->SetBinContent(binno, h_Tot_Sys_nominal->GetBinContent(binno)/h_Tot_Sys_nominal->GetBinWidth(binno));

      // Scale Errors
      h_Data->SetBinError(binno, h_Data->GetBinError(binno)/h_Data->GetBinWidth(binno));
      h_SM->SetBinError(binno, h_SM->GetBinError(binno)/h_SM->GetBinWidth(binno));
      h_Tot_Sys_nominal->SetBinError(binno, h_Tot_Sys_nominal->GetBinError(binno)/h_Tot_Sys_nominal->GetBinWidth(binno));
    }

  }





  ////////////////////////////////////////////////////
  // The theory uncertainties by Christian Gutschow //
  ////////////////////////////////////////////////////
  // https://indico.cern.ch/event/580269/contributions/2352327/attachments/1362194/2062389/cg_truthDefinition.pdf

  // binning method {underflow,value1,value2, ... , valueN,overflow}
  float Znunu_Etmiss_PS_Monojet_total_up[9]={0.,0.130118933608,0.153684603578,0.180755759225,0.175962431584,0.196143299948,0.179129422053,0.179124655347,0.};
  float Znunu_Etmiss_PS_Monojet_total_down[9]={0.,0.148261355671,0.166736901891,0.187275873477,0.180791635245,0.197744733244,0.190344587805,0.192612717029,0.};
  float Znunu_Etmiss_PS_VBFDM_total_up[8]={0.,0.0940737925869,0.111726350061,0.125721253291,0.132177798409,0.156377195211,0.144968691929,0.};
  float Znunu_Etmiss_PS_VBFDM_total_down[8]={0.,0.109425473404,0.12947580024,0.14306102392,0.143506196767,0.162783752848,0.156173263749,0.};
  float Znunu_Mjj_PS_VBFDM_total_up[7]={0.,0.109149381028,0.107635755273,0.103251076678,0.0973786331944,0.101886446385,0.};
  float Znunu_Mjj_PS_VBFDM_total_down[7]={0.,0.127964960757,0.121377237512,0.114128956928,0.102473919039,0.101501176668,0.};
  float Znunu_DeltaPhi_PS_VBFDM_total_up[8]={0.,0.0962331961673,0.0967751674251,0.10728838848,0.116113215142,0.113804241232,0.0988471650871,0.};
  float Znunu_DeltaPhi_PS_VBFDM_total_down[8]={0.,0.11147611395,0.113725341604,0.12304175276,0.133720225494,0.128861953082,0.109774828582,0.};


  float frac_unc_up = 0;
  float frac_unc_down = 0;

  TH1F *h_frac_uncertain_up;
  TH1F *h_frac_uncertain_down;

  if (TheorySYS) {

    h_frac_uncertain_up = (TH1F*)h_Data->Clone("h_manual_frac_uncertain_up");
    h_frac_uncertain_up->Reset();
    h_frac_uncertain_down = (TH1F*)h_Data->Clone("h_manual_frac_uncertain_down");
    h_frac_uncertain_down->Reset();

    for (int binno = 0; binno < h_frac_uncertain_up->GetNbinsX() + 2; ++binno) {
      if (HISTO.Contains("MET_mono")){
        frac_unc_up = Znunu_Etmiss_PS_Monojet_total_up[binno];
        frac_unc_down = Znunu_Etmiss_PS_Monojet_total_down[binno];
        h_frac_uncertain_up->SetBinContent(binno, frac_unc_up); 
        h_frac_uncertain_down->SetBinContent(binno, frac_unc_down); 
      }
      if (HISTO.Contains("MET_search")){
        frac_unc_up = Znunu_Etmiss_PS_VBFDM_total_up[binno];
        frac_unc_down = Znunu_Etmiss_PS_VBFDM_total_down[binno];
        h_frac_uncertain_up->SetBinContent(binno, frac_unc_up); 
        h_frac_uncertain_down->SetBinContent(binno, frac_unc_down); 
      }
      if (HISTO.Contains("Mjj_search")){
        frac_unc_up = Znunu_Mjj_PS_VBFDM_total_up[binno];
        frac_unc_down = Znunu_Mjj_PS_VBFDM_total_down[binno];
        h_frac_uncertain_up->SetBinContent(binno, frac_unc_up); 
        h_frac_uncertain_down->SetBinContent(binno, frac_unc_down); 
      }
      if (HISTO.Contains("dPhijj")){
        frac_unc_up = Znunu_DeltaPhi_PS_VBFDM_total_up[binno];
        frac_unc_down = Znunu_DeltaPhi_PS_VBFDM_total_down[binno];
        h_frac_uncertain_up->SetBinContent(binno, frac_unc_up); 
        h_frac_uncertain_down->SetBinContent(binno, frac_unc_down); 
      }
      cout << "bin# = " << binno << " frac_unc_up = " << h_frac_uncertain_up->GetBinContent(binno) << endl;
      cout << "bin# = " << binno << " frac_unc_down = " << h_frac_uncertain_down->GetBinContent(binno) << endl;
    }

  }


  // Define Stack
  THStack *h_stackSM = new THStack("sm","sm");


  // Set Style for Data
  h_Data->SetMarkerStyle(kFullCircle);
  h_Data->SetMarkerSize(0.8);
  h_Data->SetLineColor(kBlack);

  // Ratio of Data/MC
  TH1F *h_Ratio = (TH1F*)h_Data->Clone("Ratio");
  h_Ratio->Divide( (TH1F*)h_SM ); // Divided by Total Bkg (Data/MC)
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
  h_Data->SetLineColor(kBlack);
  h_Bkg_Znunu->SetLineColor(kBlack);
  h_Bkg_Zll->SetLineColor(kBlack);
  h_Bkg_Wenu->SetLineColor(kBlack);
  h_Bkg_Wmunu->SetLineColor(kBlack);
  h_Bkg_Wtaunu->SetLineColor(kBlack);
  h_Bkg_Multijet->SetLineColor(kBlack);
  h_Tot_Sys_nominal->SetLineColor(kBlack);
  h_Tot_Sys_up->SetLineColor(kBlack);
  h_Tot_Sys_down->SetLineColor(kBlack);

  h_Bkg_Znunu->SetFillColor(kRed-7);
  h_Bkg_Zll->SetFillColor(kOrange+1);
  h_Bkg_Wenu->SetFillColor(kCyan+1);
  h_Bkg_Wmunu->SetFillColor(kRed-3);
  h_Bkg_Wtaunu->SetFillColor(kYellow-7);
  h_Bkg_Multijet->SetFillColor(kGray);
  //h_Bkg_Ztautau->SetFillColor(kBlue-3);
  //h_ttbar->SetFillColor(kGreen+2);
  //h_Diboson->SetFillColor(kMagenta+1);



  // Fill Stack
  h_stackSM->Add(h_Bkg_Multijet);
  h_stackSM->Add(h_Bkg_Zll);
  h_stackSM->Add(h_Bkg_Wenu);
  h_stackSM->Add(h_Bkg_Wtaunu);
  h_stackSM->Add(h_Bkg_Wmunu);
  h_stackSM->Add(h_Bkg_Znunu);




  //////////////////////////////////
  // Total systematic uncertainty //
  //////////////////////////////////

  // Calculate the total systematic uncertainty from all systematics sources
  // Tot_sys_error = Sqrt( max( abs(sys1_up - nominal) ,abs(sys1_down - nominal))^2 + max( abs(sys2_up - nominal) ,abs(sys2_down - nominal))^2 + ... )  where sys1_up is the ratio of sys1_up to nominal
  TH1F *h_total_sys = (TH1F*)h_Tot_Sys_nominal->Clone("h_total_sys");
  h_total_sys->SetLineColor(kBlack);

  // Calculate the total fraction uncertainty from all systematics sources (add it to the ratio)
  // Tot_sys_frac = Sqrt( max( abs(sys1_up - nominal) ,sys1_down - nominal)^2 + max( abs(sys2_up - nominal) ,sys2_down - nominal)^2 + ... ) / nominal  , where sys1_up is the ratio of sys1_up to nominal
  TH1F *h_total_sys_frac = (TH1F*)h_Tot_Sys_nominal->Clone("h_total_sys_frac");
  h_total_sys_frac->Reset();
  h_total_sys_frac->SetLineColor(kBlack);

  for (int binno = 0; binno < h_total_sys->GetNbinsX() + 2; ++binno) {
    if ( h_Tot_Sys_nominal->GetBinContent(binno) == 0. ) { // When using blinded histograms, bin contents has 0 values which turns out that total uncertainty become very high in blinded regions.
      h_total_sys->SetBinError(binno, 0.); // Make errors 0% in blinded region.
      h_total_sys_frac->SetBinContent(binno, 0.); // Make contents 0% in blinded region.
      h_total_sys_frac->SetBinError(binno, 0.); // Make errors 0% in blinded region.
    }
    else {
      h_total_sys_frac->SetBinContent(binno, 1.); // Make contents 1. for each bin for ratio plot
      float nominal = h_total_sys->GetBinContent(binno);
      float theory_unc_up = 0.; // Calculate the theory uncertainty (Christian Gutschow)
      float theory_unc_down = 0.; // Calculate the theory uncertainty (Christian Gutschow)
      if ( TheorySYS ) { // Calculate the theory uncertainty by Christian Gutschow
        theory_unc_up = h_frac_uncertain_up->GetBinContent(binno) * nominal;
        theory_unc_down = h_frac_uncertain_down->GetBinContent(binno) * nominal;
      }
      // Total systematics uncertainty = quardratic sum of the total uncertainty (Valentinos) and the theory uncertainty (Chris)
      // , where the tatal uncertainty (Valentinos) is contained in BinError of h_Tot_Sys_nominal and the theory uncertainty is contained in BinContent of theory_unc_up or theory_unc_down.
      // I sysmmetrize the theory uncertainty by taking maximum value between theory_unc_up and theory_unc_down
      float syst_tot = std::sqrt( std::pow(std::abs(h_Tot_Sys_nominal->GetBinError(binno)),2) + // For norminal systematic uncertainties
                       std::pow(std::max(std::abs(theory_unc_up),std::abs(theory_unc_down)),2) + // For the theory uncertainty by Christian Gutschow
                       std::pow(std::abs(h_SM->GetBinError(binno)),2) // For MC statistical uncertainty
          );

      cout << "bin # = " << binno << " N_Data = " << h_Data->GetBinContent(binno) << ", Data statistical uncertainty  = " << h_Data->GetBinError(binno) << endl;
      cout << "bin # = " << binno << " N_MC_est = " << h_SM->GetBinContent(binno)  << ", MC statistical uncertainty  = " << h_SM->GetBinError(binno) << endl;
      cout << "bin # = " << binno << " Ratio (Data/MC) statistical uncertainty  = " << h_Ratio->GetBinError(binno) << endl;
      cout << "bin # = " << binno << " total error = " << syst_tot << endl;

      float syst_tot_frac = syst_tot / nominal;
      h_total_sys->SetBinError(binno, syst_tot);
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
    else ymax = 3.5*h_Data->GetBinContent(h_Data->GetMaximumBin());
  } else ymax = 1.6*h_Data->GetBinContent(h_Data->GetMaximumBin()); // For linear histogram
  h_aux->SetMaximum(ymax);
  h_aux->Draw();
  h_stackSM->Draw("same hist");
  h_Data->Draw("same E");
  h_total_sys->Draw("same E2");

  if (ATLAS) {
    if (LOGY){
      myText( 0.30, 0.79, 1, "#scale[0.8]{#sqrt{s} = 13 TeV,}");
      myText( 0.45, 0.79, 1, "#scale[0.8]{"+intLumi+"}");
      myText( 0.31, 0.74, 1, "#scale[0.8]{"+channel+"}");
      //ATLASLabel(0.30,0.84,"Internal");
      //ATLASLabel(0.30,0.84,"Preliminary");
      ATLASLabel(0.30,0.84,"");
    } else {
      myText( 0.40, 0.79, 1, "#scale[0.8]{#sqrt{s} = 13 TeV,}");
      myText( 0.55, 0.79, 1, "#scale[0.8]{"+intLumi+"}");
      myText( 0.40, 0.74, 1, "#scale[0.8]{"+channel+"}");
      //ATLASLabel(0.40,0.84,"Internal");
      //ATLASLabel(0.40,0.84,"Preliminary");
      ATLASLabel(0.40,0.84,"");
    }
  }

  TLegend *leg;
  if (LOGY){
    leg = new TLegend(0.60,0.60,0.85,0.86);
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
  leg->AddEntry(h_total_sys,"Total Syst. Unc.","F");
  leg->AddEntry(h_Bkg_Znunu,"MC Z(#rightarrow #nu#nu)+jets","F");
  leg->AddEntry(h_Bkg_Wmunu,"Data driven #mu#nu Bkg","F");
  leg->AddEntry(h_Bkg_Wtaunu,"Data driven #tau#nu Bkg","F");
  leg->AddEntry(h_Bkg_Wenu,"Data driven e#nu Bkg","F");
  leg->AddEntry(h_Bkg_Zll,"MC Z(#rightarrow ll)+jets","F");
  leg->AddEntry(h_Bkg_Multijet,"Data driven Multijet Bkg","F");
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



  if (PRINT) {
    //can->Print(TString(can->GetTitle())+".png");
    can->Print("Znunu_"+HISTO+".pdf");

    // Delete objects
    delete can;
    //delete can2;
    //delete can3;
    //delete can4;
    delete file_Data;
    delete file_Bkg_Znunu;
    delete file_Bkg_Zll;
    delete file_Bkg_Wenu;
    delete file_Bkg_Wmunu;
    delete file_Bkg_Wtaunu;
    delete file_Bkg_Multijet;
    delete file_Tot_Sys;

  }

}


/*
#ifndef __CINT__
int main(){
DrawHistogram(HISTO,XTITLE,XMIN,XMAX,REBIN,LOGY,PRINT,XDIV,UNITS);
}
#endif
 */
