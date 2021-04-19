#include "Pythia8/Pythia.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"

#include <vector> 

int main()
{
	const int    nEvents = 100;
	const double R       = 0.5;

	std::vector<fastjet::PseudoJet> particles;	

	Pythia8::Pythia pythia;
	pythia.readString("Beams:eCM = 7000.");
	pythia.readString("HardQCD:all = on"); pythia.readString("PhaseSpace:pTHatMin = 20.");

	pythia.init();

	for (int iEvent = 0; iEvent < nEvents; iEvent++)
	{
		if (!pythia.next()) continue;

		for (int i = 0; i < pythia.event.size(); i++)
			if (pythia.event[i].isFinal() && !pythia.event[i].isCharged())
			{
				particles.push_back( fastjet::PseudoJet( pythia.event[i].px(),
								      pythia.event[i].py(),
								      pythia.event[i].pz(),
								      pythia.event[i].e()) );
			}
	}
		
	fastjet::JetDefinition jetDef( fastjet::antikt_algorithm, R );	
	fastjet::ClusterSequence cs( particles, jetDef );

	std::vector<fastjet::PseudoJet> jets = sorted_by_pt( cs.inclusive_jets() );	


	std::cout <<   "        pt y phi" << std::endl;
	for (unsigned i = 0; i < jets.size(); i++) 
	{
		std::cout << "jet " << i << ": "<< jets[i].pt() << " " 
		<< jets[i].rap() << " " << jets[i].phi() << std::endl;
	}
}

