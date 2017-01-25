
void massPeakCompare(){

    TFile * f = new TFile("ertool_hist.root"); 
    //TFile * f2 = new TFile("ertool_hist50.root");
    //TFile * f = new TFile("ertool_hist.root"); 
    //TFile * f2 = new TFile("ertool_w_nearestclus.root");
    //TFile * f = new TFile("ertool_w_nearestclus.root");
    //TFile * f2 = new TFile("ertool_v05_08.root");

    TTree * before = (TTree*)f->Get("_pi0_selected_tree") ;
    //TTree * after = (TTree*)f2->Get("_pi0_selected_tree") ;

    TCanvas * c0 = new TCanvas("c0","c0");
    TH1D * h0 = new TH1D("h0","Pi0 Mass; Mass [MeV]",17,0,400);
    TH1D * h1 = new TH1D("h1","Pi0 Mass; Mass [MeV]",17,0,400);
    before->Draw("_Rmass_S>>h0");
    h0->SetFillColorAlpha(kBlue,0.35);
    h0->SetLineColorAlpha(kBlue,0.9);
    gStyle->SetOptStat(1);
    //after->Draw("_Rmass_S>>h1");
    //h1->SetFillColorAlpha(kRed,0.35);
    //h1->SetLineColorAlpha(kRed,0.9);
    c0->SetGrid();
    //h1->DrawCopy();
    //gStyle->SetOptStat(1);
    //h0->Draw("same");
    h0->DrawCopy();

 //   TCanvas * c1 = new TCanvas("c1","c1");
 //   TH1D * h2 = new TH1D("h2","(Max Width*Contour Perimeter/2) / Bounding Box Area ",50,0,2.5);
 //   TH1D * h3 = new TH1D("h3","(Max Width*Contour Perimeter/2) / Bounding Box Area",50,0,2.5);
}
