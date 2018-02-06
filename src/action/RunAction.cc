#include "action/RunAction.hh"

#include "Geant4/G4RunManager.hh"

namespace MATHUSLA {

RunAction::RunAction() : G4UserRunAction() {
  // set printing event number per each 100 events
  G4RunManager::GetRunManager()->SetPrintProgress(1000);
}

RunAction::~RunAction() {}

G4Run* RunAction::GenerateRun() { return G4UserRunAction::GenerateRun(); }

void RunAction::BeginOfRunAction(const G4Run*) {
  // inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
}

void RunAction::EndOfRunAction(const G4Run*) {}

} /* namespace MATHUSLA */
