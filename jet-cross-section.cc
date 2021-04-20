#include "Pythia8/Pythia.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"

#include <vector> 
#include <cmath>

#include "TH1D.h"
#include "TFile.h"

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
	
	TH1D *rapHist = new TH1D("rap_histo", "Rapidity Distribution",   50, -10, 10);
	TH1D *pTHist  = new TH1D("pt_histo", "Transverse Momentum Dist", 10, 0, 500);

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
			if ( abs(jets[i].rap()) >= 0.3 ) continue;
			rapHist->Fill(jets[i].rap());
			pTHist ->Fill(jets[i].pt());
		}
		
		std::cout <<   "        pt y phi" << std::endl;
		for (unsigned i = 0; i < jets.size(); i++) 
		{
			std::cout << "jet " << i << ": "<< jets[i].pt() << " " 
			<< jets[i].rap() << " " << jets[i].phi() << std::endl;
		}

		jets.clear();
		particles.clear();
	}

	pythia.stat();
		
	pTHist ->Scale(1e9*pythia.info.sigmaGen()/pythia.info.nAccepted(), "width");

	rapHist->Write();
	pTHist ->Write();

	particles.clear();

	outFile->Close();

}
