#include "action/RunAction.hh"
#include "analysis/HistoManager.hh"

#include "Geant4/G4Run.hh"
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4UnitsTable.hh"

#include "analysis/HistoManager.hh"

namespace MATHUSLA { namespace MU {

RunAction::RunAction() : G4UserRunAction(), fAvgEdep(0) {
  G4RunManager::GetRunManager()->SetPrintProgress(1000);
}

G4Run* RunAction::GenerateRun() { return G4UserRunAction::GenerateRun(); }

void RunAction::BeginOfRunAction(const G4Run*) {
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  fAvgEdep = 0;
  HistoManager::Book();
}

void RunAction::FillPerEvent(G4double Edep) {
  fAvgEdep += Edep;
}

void RunAction::EndOfRunAction(const G4Run* run) {
  auto event_number = run->GetNumberOfEvent();
  if (event_number == 0) return;

  fAvgEdep /= event_number;

  G4cout << "\nEnd of Run:\n\n mean Energy in Scinillator : "
         << G4BestUnit(fAvgEdep, "Energy")
         << '\n';

  HistoManager::PrintStatistic();
  HistoManager::Save();
}

} } /* namespace MATHUSLA::MU */
