#include "Pythia8/Pythia.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"

#include <vector> 
#include <cmath>

#include "TH1D.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TPad.h"

int main()
{
	const int    nEvents = 10000;
	      double R       = 0.5;

	std::vector<fastjet::PseudoJet> particles;	
	std::vector<fastjet::PseudoJet> jets;

	fastjet::JetDefinition jetDef( fastjet::antikt_algorithm, R );	

	Pythia8::Pythia pythia;
	pythia.readString("Beams:eCM = 7000.");
	pythia.readString("HardQCD:all = on");
       	pythia.readString("PhaseSpace:pTHatMin = 40.");

	pythia.init();

	TFile *outFile = new TFile("output.root", "RECREATE");
	
	TH1D *rapHist = new TH1D("rap_histo", "#it{y} Distribution",   20, -10, 10);
	TH1D *pTHist  = new TH1D("pt_histo", "#it{p_{T}} Distribution", 10, 0, 400);

	for (int iEvent = 0; iEvent < nEvents; iEvent++)
	{
		if (!pythia.next()) continue;

		for (int i = 0; i < pythia.event.size(); i++)
		{
			if (!pythia.event[i].isFinal())  continue;
			if (pythia.event[i].id() == 12)  continue;
			if (pythia.event[i].id() == 14)  continue;
			if (pythia.event[i].id() == 16)  continue;
			if (pythia.event[i].id() == -12) continue;
			if (pythia.event[i].id() == -14) continue;
			if (pythia.event[i].id() == -16) continue;
			particles.push_back( fastjet::PseudoJet( pythia.event[i].px(),
								 pythia.event[i].py(),
								 pythia.event[i].pz(),
								 pythia.event[i].e() ) );
		}
		
		fastjet::ClusterSequence cs( particles, jetDef );
		jets = sorted_by_pt( cs.inclusive_jets() );	

		for (int i = 0; i < jets.size(); i++)
		{
			if ( abs(jets[i].rap()) <= 0.3 ){ //continue;
				pTHist ->Fill(jets[i].pt());
			}
			rapHist->Fill(jets[i].rap());
		}

		jets.clear();
		particles.clear();
	}

	pythia.stat();
		


	TCanvas *c1 = new TCanvas();
	c1->Divide(2);

	c1->cd(1);
	gStyle->SetOptStat("");
	pTHist->GetXaxis()->SetTitle("p_{T} [GeV]");
	pTHist->GetYaxis()->SetTitle("Number of jets");
	gPad->SetLogy();
	pTHist->SetLineWidth(1);
	pTHist->SetLineColor(kBlue);
	pTHist->SetFillStyle(3003);
	pTHist->SetFillColor(kBlue);
	pTHist->Draw();	

	c1->cd(2);
	gStyle->SetOptStat("");
	rapHist->GetXaxis()->SetTitle("y");
	rapHist->GetYaxis()->SetTitle("Number of jets");
	rapHist->SetLineWidth(1);
	rapHist->SetLineColor(kBlue);
	rapHist->SetFillStyle(3003);
	rapHist->SetFillColor(kBlue);
	rapHist->Draw();	
	
	TCanvas *c2 = new TCanvas();
	gStyle->SetOptStat("");

	TH1D *crossHist = (TH1D*) pTHist->Clone("crossHist");
	crossHist->Scale(1e9*pythia.info.sigmaGen()/pythia.info.nAccepted()/0.6, "width");
	crossHist->SetTitle("Cross Section");
	crossHist->GetXaxis()->SetTitle("p_{T} [GeV]");
	crossHist->GetYaxis()->SetTitle("d^{2} #sigma / dp_{T}, dy [pb/GeV]");
	gPad->SetLogy();
	crossHist->SetLineWidth(3);
	crossHist->SetLineColor(kBlue);
	crossHist->SetFillStyle(3003);
	crossHist->SetFillColor(kWhite);
	crossHist->SetMarkerStyle(kFullTriangleUp);
	crossHist->SetMarkerColorAlpha(kBlue, 1);
	crossHist->SetMarkerSize(3);
	
	crossHist->Draw("HIST P L");	

	gStyle->SetOptStat("");
	gStyle->SetOptTitle(kTRUE);

	pTHist->Write();
	rapHist->Write();
	crossHist->Write();
	c1->Write();
	c2->Write();

	particles.clear();

	outFile->Close();

}
