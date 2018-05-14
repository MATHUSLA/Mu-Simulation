#include "action.hh"

#include <fstream>

#include "analysis.hh"
#include "detector/Prototype.hh"

#include "util/io.hh"
#include "util/time.hh"

namespace MATHUSLA { namespace MU {

namespace {
G4ThreadLocal std::string _path;
//----------------------------------------------------------------------------------------------
} /* anonymous namespace */

void RunAction::BeginOfRunAction(const G4Run* run) {
  _path = "data";
  util::io::create_directory(_path);
  _path += '/' + util::time::GetDate();
  util::io::create_directory(_path);
  _path += '/' + util::time::GetTime();
  util::io::create_directory(_path);
  _path += "/run" + std::to_string(run->GetRunID());

  Analysis::Setup();
  Prototype::Detector::GenerateAnalysis(run->GetNumberOfEventToBeProcessed());
  Analysis::Open(_path + ".root");
}
//----------------------------------------------------------------------------------------------

void RunAction::EndOfRunAction(const G4Run* run) {
  if (!run->GetNumberOfEvent()) return;

  Analysis::Save();

  std::ofstream _info(_path + ".info");

  _info << "MATHUSLA -- Muon Simulation\n"
        << util::time::GetString("%c %Z") << "\n\n"
        << "Run "     << run->GetRunID() << "\n"
        << "Events: " << run->GetNumberOfEventToBeProcessed() << "\n"
        << "Data: "   << _path << ".root\n\n"
        << GeneratorAction::GetGenerator()->InfoString();

  _info.close();

  std::cout << "\nEnd of Run\n"
            << "Data File: " << _path << ".root\n"
            << "Info File: " << _path << ".info\n\n";
}
//----------------------------------------------------------------------------------------------

} } /* namespace MATHUSLA::MU */
