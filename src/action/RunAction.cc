#include "action/RunAction.hh"
#include "analysis/HistoManager.hh"

#include "Geant4/G4Run.hh"
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4UnitsTable.hh"

namespace MATHUSLA { namespace MU {

RunAction::RunAction(HistoManager* histo) 
: G4UserRunAction(),
  fHistoManager(histo),
  fAvgEdep(0.) 
  {G4RunManager::GetRunManager()->SetPrintProgress(1000);}

RunAction::~RunAction()
{}

G4Run* RunAction::GenerateRun() { return G4UserRunAction::GenerateRun(); }

void RunAction::BeginOfRunAction(const G4Run*) {
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  fAvgEdep = 0.;
  fHistoManager->Book();
}

void RunAction::FillPerEvent(G4double Edep)
{
	fAvgEdep += Edep;
}

void RunAction::EndOfRunAction(const G4Run* aRun) 
{
	G4int NbOfEvents = aRun->GetNumberOfEvent();
	if (NbOfEvents == 0) return;

	fAvgEdep /= NbOfEvents;

	G4cout
		<< "\nEnd of Run:\n"
     	<< "\n mean Energy in Scinillator : " << G4BestUnit(fAvgEdep,"Energy")
     	<< G4endl;

    fHistoManager->PrintStatistic();
  	fHistoManager->Save();   
}

} } /* namespace MATHUSLA::MU */
