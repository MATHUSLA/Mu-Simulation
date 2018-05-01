#include "action/RunAction.hh"

#include <fstream>

#include "Geant4/G4Run.hh"

#include "action/GeneratorAction.hh"
#include "analysis/AnalysisManager.hh"
#include "detector/Prototype.hh"
#include "util/FileIO.hh"
#include "util/Time.hh"

namespace MATHUSLA { namespace MU {

static G4ThreadLocal std::string _path = "";

void RunAction::BeginOfRunAction(const G4Run* run) {
  _path = "data";
  IO::create_directory(_path);
  _path += '/' + Time::GetDate();
  IO::create_directory(_path);
  _path += '/' + Time::GetTime();
  IO::create_directory(_path);
  _path += "/run" + std::to_string(run->GetRunID());

  AnalysisManager::Setup();
  Prototype::GenerateAnalysis(run->GetNumberOfEventToBeProcessed());
  AnalysisManager::Open(_path + ".root");
}

void RunAction::EndOfRunAction(const G4Run* run) {
  if (!run->GetNumberOfEvent()) return;

  AnalysisManager::Save();

  std::ofstream _info(_path + ".info");

  _info << "MATHUSLA -- Muon Simulation\n"
        << Time::GetString("%c %Z") << "\n\n"
        << "Run "     << run->GetRunID() << "\n"
        << "Events: " << run->GetNumberOfEventToBeProcessed() << "\n"
        << "Data: "   << _path << ".root\n\n"
        << GeneratorAction::GetGenerator()->InfoString();

  _info.close();

  std::cout << "\nEnd of Run\n"
            << "Data File: " << _path << ".root\n"
            << "Info File: " << _path << ".info\n\n";
}

} } /* namespace MATHUSLA::MU */
