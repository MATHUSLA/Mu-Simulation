#include "action/RunAction.hh"

#include "Geant4/G4Run.hh"
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4UnitsTable.hh"

#include "analysis/Analysis.hh"

namespace MATHUSLA { namespace MU {

RunAction::RunAction() : G4UserRunAction(), _deposit(0) {
  G4RunManager::GetRunManager()->SetPrintProgress(1000);
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  auto analysis = G4AnalysisManager::Instance();
  analysis->SetVerboseLevel(1);
  analysis->SetFirstHistoId(1);

  analysis->CreateH1("test1", "test1", 10, 0, 1*MeV);
}

RunAction::~RunAction() {
  delete G4AnalysisManager::Instance();
}

G4Run* RunAction::GenerateRun() {
  return G4UserRunAction::GenerateRun();
}

void RunAction::BeginOfRunAction(const G4Run* run) {
  _deposit = 0;
  auto analysis = G4AnalysisManager::Instance();
  analysis->OpenFile("data/Prototype.root");
}

void RunAction::FillPerEvent(G4double deposit) {
  _deposit += deposit;
}

void RunAction::EndOfRunAction(const G4Run* run) {
  auto event_count = run->GetNumberOfEvent();
  if (event_count == 0) return;

  auto avg_deposit = _deposit / event_count;

  G4cout << "\nEnd of Run:\n\n mean Energy in Scinillator : "
         << G4BestUnit(_deposit / event_count, "Energy")
         << "\n\n";

  auto manager = G4AnalysisManager::Instance();
  manager->Write();
  manager->CloseFile();
}

} } /* namespace MATHUSLA::MU */
