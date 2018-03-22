#include "action/RunAction.hh"

#include "Geant4/G4Run.hh"
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4UnitsTable.hh"

#include "analysis/Analysis.hh"
#include "util/FileIO.hh"
#include "util/Time.hh"

namespace MATHUSLA { namespace MU {

static G4String _dir = "";
static G4String _file = "";

RunAction::RunAction() : G4UserRunAction() {
  G4RunManager::GetRunManager()->SetPrintProgress(1000);
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  G4AnalysisManager::Instance()->SetNtupleMerging(true);
}

G4Run* RunAction::GenerateRun() {
  return G4UserRunAction::GenerateRun();
}

void RunAction::BeginOfRunAction(const G4Run*) {
  auto dir = "data/" + Time::GetDate();
  IO::create_directory(dir);
  dir += "/" + Time::GetTime();
  _dir = dir;
  IO::create_directory(dir);
  _file = "Prototype.root";
  Analysis::OpenFile(dir, _file);
}

void RunAction::EndOfRunAction(const G4Run* run) {
  if (!run->GetNumberOfEvent()) return;

  G4cout << "\nEnd of Run\n"
         << "Data Files: " << _dir << '/' << _file << "\n\n";

  Analysis::Write();
  Analysis::CloseFile();
}

} } /* namespace MATHUSLA::MU */
