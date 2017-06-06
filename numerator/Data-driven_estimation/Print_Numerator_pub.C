void Print_Numerator_pub() {
  gROOT->ProcessLine(".L AtlasStyle.C+");
  gROOT->ProcessLine(".L AtlasUtils.C+");
  gROOT->ProcessLine(".L AtlasLabels.C+");
  gROOT->ProcessLine(".L Draw_Numerator_pub.C+");


  // How to print?
  // Draw_Numerator_pub(TString HISTO,TString XTITLE,float XMIN,float XMAX,bool REBIN,bool LOGY,bool PRINT,int XDIV,TString UNITS,bool TheorySYS, bool ATLAS,bool DivBinWidth)

  Float_t pi = TMath::Pi();

  // Znunu + Jets
  // For publication
  Draw_Numerator_pub("MET_mono","p_{T}^{miss}",200.,1400.,false,true,true,707,"GeV",false,true,true);
  Draw_Numerator_pub("MET_search","p_{T}^{miss}",200.,1400.,false,true,true,707,"GeV",false,true,true);
  Draw_Numerator_pub("Mjj_search","m_{jj}",200.,4000.,false,true,true,707,"GeV",false,true,true);
  Draw_Numerator_pub("dPhijj","#Delta#phi_{jj}",0.,3.141592,false,false,true,707,"rad",false,true,true);

}



