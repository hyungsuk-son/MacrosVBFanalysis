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


void Draw_CR_Div_by_Eff(TString HISTO,TString XTITLE,float XMIN,float XMAX,bool REBIN,bool LOGY,bool PRINT,int XDIV,TString UNITS,bool ATLAS,bool DivBinWidth)
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
  if ( HISTO.Contains("Muon") ) {
    if ( HISTO.Contains("PseudoMET") ) {
      channel = "W#rightarrow#mu#nu #geq1 jet"; //Wmunu monojet
    } else {
      channel = "W#rightarrow#mu#nu VBF"; //Wmunu VBF
    }
  }
  if ( HISTO.Contains("Electron") ) {
    if ( HISTO.Contains("PseudoMET") ) {
      channel = "W#rightarrowe#nu #geq1 jet"; //Wenu monojet
    } else {
      channel = "W#rightarrowe#nu VBF"; //Wenu VBF
    }
  }

  // PATH to the location of input histograms
  TString PATH("/home/hson/Work/Run2_Study/VBF_study/Macro/numerator_hist/");


  // Retrieve files
  TString filename;
  if ( HISTO.Contains("Muon") ) {
    filename = "MuonWmunuAll.root"; // All histograms for CR in Wmunu
  }
  if ( HISTO.Contains("Electron") ) {
    filename = "ElectronWenuAll.root"; // All histograms for CR in Wenu
  }
  TFile *file = TFile::Open(PATH+filename);

  // Define Rebinning
  Double_t binsMET[] = {200.,250.,300.,350.,500.,700.,1400.};
  Int_t nbinMET = sizeof(binsMET)/sizeof(Double_t) - 1;
  Double_t binsMjj[] = {200.,400.,600.,1000.,2000.,4000.};
  Int_t nbinMjj = sizeof(binsMjj)/sizeof(Double_t) - 1;

  char name[1000];

  ///////////////////////////////////////////////
  // Retrieve Histograms (Data and background) //
  ///////////////////////////////////////////////

  // HISTO names : 'PseudoMET' for the >=1 jet phase space
  //               'Mjj' for the VBF phase space

  // Retrieve histograms
  TH1F *h_Data = (TH1F*)file->Get(HISTO+"Data");
  TH1F *h_SM = (TH1F*)file->Get(HISTO);
  TH1F *h_Wlepnu;
  TH1F *h_WlepnuEWK;
  if ( HISTO.Contains("Muon") ) {
    h_Wlepnu = (TH1F*)file->Get(HISTO+"Wmunu");
    h_WlepnuEWK = (TH1F*)file->Get(HISTO+"WmunuEWK");
  }
  if ( HISTO.Contains("Electron") ) {
    h_Wlepnu = (TH1F*)file->Get(HISTO+"Wenu");
    h_WlepnuEWK = (TH1F*)file->Get(HISTO+"WenuEWK");
  }
  TH1F *h_Wtaunu = (TH1F*)file->Get(HISTO+"Wtaunu");
  TH1F *h_WtaunuEWK = (TH1F*)file->Get(HISTO+"WtaunuEWK");
  TH1F *h_Top = (TH1F*)file->Get(HISTO+"Top");
  TH1F *h_Diboson = (TH1F*)file->Get(HISTO+"Diboson");
  // Rebin histograms (only for VBF MET and Mjj histograms)
  if (REBIN && HISTO.Contains("MET")) h_Data = dynamic_cast<TH1F*>(h_Data->Rebin(nbinMET,"h_Data",binsMET));
  if (REBIN && HISTO.Contains("Mjj")) h_Data = dynamic_cast<TH1F*>(h_Data->Rebin(nbinMET,"h_Data",binsMjj));
  if (REBIN && HISTO.Contains("MET")) h_SM = dynamic_cast<TH1F*>(h_SM->Rebin(nbinMET,"h_SM",binsMET));
  if (REBIN && HISTO.Contains("Mjj")) h_SM = dynamic_cast<TH1F*>(h_SM->Rebin(nbinMET,"h_SM",binsMjj));
  if (REBIN && HISTO.Contains("MET")) h_Wlepnu = dynamic_cast<TH1F*>(h_Wlepnu->Rebin(nbinMET,"h_Wlepnu",binsMET));
  if (REBIN && HISTO.Contains("Mjj")) h_Wlepnu = dynamic_cast<TH1F*>(h_Wlepnu->Rebin(nbinMET,"h_Wlepnu",binsMjj));
  if (REBIN && HISTO.Contains("MET")) h_WlepnuEWK = dynamic_cast<TH1F*>(h_WlepnuEWK->Rebin(nbinMET,"h_WlepnuEWK",binsMET));
  if (REBIN && HISTO.Contains("Mjj")) h_WlepnuEWK = dynamic_cast<TH1F*>(h_WlepnuEWK->Rebin(nbinMET,"h_WlepnuEWK",binsMjj));
  if (REBIN && HISTO.Contains("MET")) h_Wtaunu = dynamic_cast<TH1F*>(h_Wtaunu->Rebin(nbinMET,"h_Wtaunu",binsMET));
  if (REBIN && HISTO.Contains("Mjj")) h_Wtaunu = dynamic_cast<TH1F*>(h_Wtaunu->Rebin(nbinMET,"h_Wtaunu",binsMjj));
  if (REBIN && HISTO.Contains("MET")) h_WtaunuEWK = dynamic_cast<TH1F*>(h_WtaunuEWK->Rebin(nbinMET,"h_WtaunuEWK",binsMET));
  if (REBIN && HISTO.Contains("Mjj")) h_WtaunuEWK = dynamic_cast<TH1F*>(h_WtaunuEWK->Rebin(nbinMET,"h_WtaunuEWK",binsMjj));
  if (REBIN && HISTO.Contains("MET")) h_Top = dynamic_cast<TH1F*>(h_Top->Rebin(nbinMET,"h_Top",binsMET));
  if (REBIN && HISTO.Contains("Mjj")) h_Top = dynamic_cast<TH1F*>(h_Top->Rebin(nbinMET,"h_Top",binsMjj));
  if (REBIN && HISTO.Contains("MET")) h_Diboson = dynamic_cast<TH1F*>(h_Diboson->Rebin(nbinMET,"h_Diboson",binsMET));
  if (REBIN && HISTO.Contains("Mjj")) h_Diboson = dynamic_cast<TH1F*>(h_Diboson->Rebin(nbinMET,"h_Diboson",binsMjj));


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
      h_Wlepnu->SetBinContent(binno, h_Wlepnu->GetBinContent(binno)/h_Wlepnu->GetBinWidth(binno));
      h_WlepnuEWK->SetBinContent(binno, h_WlepnuEWK->GetBinContent(binno)/h_WlepnuEWK->GetBinWidth(binno));
      h_Wtaunu->SetBinContent(binno, h_Wtaunu->GetBinContent(binno)/h_Wtaunu->GetBinWidth(binno));
      h_WtaunuEWK->SetBinContent(binno, h_WtaunuEWK->GetBinContent(binno)/h_WtaunuEWK->GetBinWidth(binno));
      h_Top->SetBinContent(binno, h_Top->GetBinContent(binno)/h_Top->GetBinWidth(binno));
      h_Diboson->SetBinContent(binno, h_Diboson->GetBinContent(binno)/h_Diboson->GetBinWidth(binno));

      // Scale Errors
      h_Data->SetBinError(binno, h_Data->GetBinError(binno)/h_Data->GetBinWidth(binno));
      h_SM->SetBinError(binno, h_SM->GetBinError(binno)/h_SM->GetBinWidth(binno));
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
  h_SM->SetLineColor(kBlack);
  h_Wlepnu->SetLineColor(kBlack);
  h_WlepnuEWK->SetLineColor(kBlack);
  h_Wtaunu->SetLineColor(kBlack);
  h_WtaunuEWK->SetLineColor(kBlack);
  h_Top->SetLineColor(kBlack);
  h_Diboson->SetLineColor(kBlack);

  //h_Bkg_Wlepnu->SetFillColor(kRed-3);
  h_Wlepnu->SetFillColor(kBlue-6);
  h_WlepnuEWK->SetFillColor(kGreen-3);
  h_Wtaunu->SetFillColor(kRed-7);
  h_WtaunuEWK->SetFillColor(kYellow-9);
  h_Diboson->SetFillColor(kMagenta-7);
  h_Top->SetFillColor(kGray);



  // Fill Stack
  h_stackSM->Add(h_WtaunuEWK);
  h_stackSM->Add(h_Diboson);
  h_stackSM->Add(h_WlepnuEWK);
  h_stackSM->Add(h_Wtaunu);
  h_stackSM->Add(h_Top);
  h_stackSM->Add(h_Wlepnu);





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
      sprintf(name,"Efficiency-corrected number of events");
    }
  }
  else {
    if (h_aux->GetBinWidth(1) >= 1) {
      //sprintf(name,"Number of events / %1.0f %s",h_aux->GetBinWidth(1),UNITS.Data());
      if (DivBinWidth) {
        sprintf(name,"Events / "+UNITS);
      } else {
        sprintf(name,"Efficiency-corrected number of events");
      }
    }
    else {
      //sprintf(name,"Number of events / %1.2f %s",h_aux->GetBinWidth(1),UNITS.Data());
      if (DivBinWidth) {
        sprintf(name,"Events / "+UNITS);
      } else {
        sprintf(name,"Efficiency-corrected number of events");
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

  if (ATLAS) {
    if (LOGY){
      myText( 0.30, 0.79, 1, "#scale[0.8]{#sqrt{s} = 13 TeV,}");
      myText( 0.45, 0.79, 1, "#scale[0.8]{"+intLumi+"}");
      myText( 0.31, 0.74, 1, "#scale[0.8]{"+channel+"}");
      //ATLASLabel(0.2,0.2,"Preliminary");
      ATLASLabel(0.30,0.85,"Internal");
    } else {
      myText( 0.40, 0.79, 1, "#scale[0.8]{#sqrt{s} = 13 TeV,}");
      myText( 0.55, 0.79, 1, "#scale[0.8]{"+intLumi+"}");
      myText( 0.40, 0.74, 1, "#scale[0.8]{"+channel+"}");
      //ATLASLabel(0.2,0.2,"Preliminary");
      ATLASLabel(0.40,0.85,"Internal");
    }
  }

  TLegend *leg;
  if (LOGY){
    leg = new TLegend(0.65,0.60,0.90,0.86);
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
  if ( HISTO.Contains("Muon") ) {
    leg->AddEntry(h_Wlepnu,"W #rightarrow #mu#nu","F");
    leg->AddEntry(h_WlepnuEWK,"EWK W #rightarrow #mu#nu","F");
  }
  if ( HISTO.Contains("Electron") ) {
    leg->AddEntry(h_Wlepnu,"W #rightarrow e#nu","F");
    leg->AddEntry(h_WlepnuEWK,"EWK W #rightarrow e#nu","F");
  }
  leg->AddEntry(h_Wtaunu,"W #rightarrow #tau#nu","F");
  leg->AddEntry(h_WtaunuEWK,"EWK W #rightarrow #tau#nu","F");
  leg->AddEntry(h_Top,"Top","F");
  leg->AddEntry(h_Diboson,"Diboson","F");
  
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
  if (UNITS == "" || UNITS.Contains("rad")) {
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



  if (PRINT) {
    if ( HISTO.Contains("Muon") ){
      if (HISTO.Contains("PseudoMET150MuonPt25DividedByEff"))
        can->Print("PseudoMET150MuonPt25DividedByEff.pdf");
      if (HISTO.Contains("MjjMuonPt25DividedByEff"))
        can->Print("MjjMuonPt25DividedByEff.pdf");
    }
    if ( HISTO.Contains("Electron") ){
      if (HISTO.Contains("PseudoMET150ElectronPt25DividedByEff"))
        can->Print("PseudoMET150ElectronPt25DividedByEff.pdf");
      if (HISTO.Contains("MjjElectronPt25DividedByEff"))
        can->Print("MjjElectronPt25DividedByEff.pdf");
    }

    // Delete objects
    delete can;
    //delete can2;
    //delete can3;
    //delete can4;
    delete file;

  }

}


/*
#ifndef __CINT__
int main(){
DrawHistogram(HISTO,XTITLE,XMIN,XMAX,REBIN,LOGY,PRINT,XDIV,UNITS);
}
#endif
 */
