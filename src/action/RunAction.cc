#include "action/RunAction.hh"

#include "Geant4/G4RunManager.hh"

namespace MATHUSLA { namespace MU {

RunAction::RunAction() : G4UserRunAction() {
  G4RunManager::GetRunManager()->SetPrintProgress(1000);
}

G4Run* RunAction::GenerateRun() { return G4UserRunAction::GenerateRun(); }

void RunAction::BeginOfRunAction(const G4Run*) {
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
}

void RunAction::EndOfRunAction(const G4Run*) {}

} } /* namespace MATHUSLA::MU */
