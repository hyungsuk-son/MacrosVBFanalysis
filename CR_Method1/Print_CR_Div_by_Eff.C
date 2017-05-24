void Print_CR_Div_by_Eff() {
  gROOT->ProcessLine(".L AtlasStyle.C+");
  gROOT->ProcessLine(".L AtlasUtils.C+");
  gROOT->ProcessLine(".L AtlasLabels.C+");
  gROOT->ProcessLine(".L Draw_CR_Div_by_Eff.C+");


  // How to print?
  // Draw_CR_Div_by_Eff(TString HISTO,TString XTITLE,float XMIN,float XMAX,bool REBIN,bool LOGY,bool PRINT,int XDIV,TString UNITS,bool ATLAS,bool DivBinWidth)

  Float_t pi = TMath::Pi();

  // Wmunu + Jets (CR)
  // For publication
  Draw_CR_Div_by_Eff("PseudoMET150MuonPt25DividedByEff","p_{T}^{miss}",200.,1400.,false,true,true,707,"GeV",true,false);
  Draw_CR_Div_by_Eff("MjjMuonPt25DividedByEff","m_{jj}",200.,4000.,false,true,true,707,"GeV",true,false);

  // Wenu + Jets (CR)
  // For publication
  Draw_CR_Div_by_Eff("PseudoMET150ElectronPt25DividedByEff","p_{T}^{miss}",200.,1400.,false,true,true,707,"GeV",true,false);
  Draw_CR_Div_by_Eff("MjjElectronPt25DividedByEff","m_{jj}",200.,4000.,false,true,true,707,"GeV",true,false);


}



