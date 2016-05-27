#include <iostream>
#include "TH1.h"
#include "TF1.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TStyle.h"

TF1 *generateGaus(double mean, double sigma);
TH1D *generateHist(TF1 *func, int ndice);
double getUL(TH1D *hist, double alpha);
double getInterval(TH1D *hist, double mean, double alpha, int mode);
void setGraphRange(TGraph *g, double x1, double x2, double y1, double y2);

int fig2and3()
{
    gStyle->SetGridStyle(1);
    gStyle->SetGridColor(kGray);

    int ndice = 10000; // set number of rolling the dice.
    int nstep = 999;    // set how many point in x axis.
    double alpha = 0.9; // set confidence level.
    double xlimit1 = -2.0, xlimit2 = 4.0;
    
    double mean = 1.0;
    double sigma = 1.0;

    // make graph
    TGraph *gUL = new TGraph();
    gUL->SetTitle(";Measured Mean x;Mean  #mu");

    TGraph *gInterval0 = new TGraph();
    gInterval0->SetTitle(";Measured Mean x;Mean  #mu");


    TGraph *gInterval1 = new TGraph();
    gInterval1->SetTitle(";Measured Mean x;Mean  #mu");

    
    for (int istep = 0; istep < nstep; istep++){
        mean = (xlimit2 - xlimit1) * double(istep)/double(nstep) + xlimit1;
        TH1D *hist = generateHist(generateGaus(mean, sigma), ndice);
        //        hist->Draw();
        gUL->SetPoint(gUL->GetN(), mean, getUL(hist, alpha));
        gInterval0->SetPoint(gInterval0->GetN(), mean, getInterval(hist, mean, alpha, 0));
        gInterval1->SetPoint(gInterval1->GetN(), mean, getInterval(hist, mean, alpha, 1));
        delete hist;
    }
    
    TCanvas *cUL = new TCanvas("cUL", "cUL", 800, 800);
    cUL->SetGridx();
    cUL->SetGridy();
    gUL->Draw("al");
    setGraphRange(gUL, -2, 4, 0, 6);
    gUL->GetXaxis()->CenterTitle();
    gUL->GetYaxis()->CenterTitle();
    cUL->SaveAs("../fig/fig2.png");

    
    TCanvas *cInterval = new TCanvas("cInterval", "cInterval", 800, 800);
    cInterval->SetGridx();
    cInterval->SetGridy();
    gInterval0->Draw("al");
    gInterval1->Draw("l");
    setGraphRange(gInterval0, -2, 4, 0, 6);
    gInterval0->GetXaxis()->CenterTitle();
    gInterval0->GetYaxis()->CenterTitle();
    cInterval->SaveAs("../fig/fig3.png");
    
    return 0;
}


TH1D *generateHist(TF1 *func, int ndice){
    TH1D *hist = new TH1D("hist", "hist", 200, -10., 10.);
    for (int idice = 0; idice < ndice; idice++){
        hist->Fill(func->GetRandom());
    }
    delete func;
    return hist;
}

TF1 *generateGaus(double mean, double sigma){
    TF1 *func = new TF1("func", "gaus", -10, 10);
    func->SetParameters(1.0, mean, sigma);
    return func;
}

double getUL(TH1D *hist, double alpha){
    double upperLimit = -99999.9;
    double entries = double(hist->GetEntries());
    double sum = 0.0;
    int nbin = hist->GetNbinsX();
    for (int ibin = 0; ibin < nbin; ibin++){
        sum += double(hist->GetBinContent(ibin));
        double ratio = sum/entries;
        if (ratio > alpha){
            upperLimit = hist->GetBinCenter(ibin);
            break;
        }
    }
    return upperLimit;
}

double getInterval(TH1D *hist, double mean, double alpha, int mode){

    double limit = -99999.9;
    
    double entries = double(hist->GetEntries());
    int centerbin = hist->GetXaxis()->FindBin(mean);
    int nbin = hist->GetNbinsX();

    double sum = 0.0;
    if (mode == 0){ // Find lowerlimit
    for (int ibin = 0; ibin < nbin; ibin++){
        sum += double(hist->GetBinContent(centerbin - ibin));
        // This calculation includes center bin for both upper and lower limit.
        // So limit will be underestimated.
        
        double ratio = sum/entries;
        if (ratio > alpha/2.0){
            limit = hist->GetBinCenter(centerbin-ibin);
            break;
        }
    }
    }
    else if (mode == 1)    // Find upper limit
    for (int ibin = 0; ibin < nbin; ibin++){
        sum += double(hist->GetBinContent(centerbin + ibin));
        // This calculation includes center bin for both upper and lower limit.
        // So limit will be underestimated.
        
        double ratio = sum/entries;
        if (ratio > alpha/2.0){
            limit = hist->GetBinCenter(centerbin+ibin);
            break;
        }
    }
    
    return limit;
}


void setGraphRange(TGraph *g, double x1, double x2, double y1, double y2){
//    g->GetXaxis()->SetLimits(x1, x2);
//    g->GetHistogram()->SetMaximum(y1);
//    g->GetHistogram()->SetMinimum(y2);
    g->GetXaxis()->SetRangeUser(x1, x2);
    g->GetYaxis()->SetRangeUser(y1, y2);
}

