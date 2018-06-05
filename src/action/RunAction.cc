/* src/action/RunAction.cc
 *
 * Copyright 2018 Brandon Gomes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "action.hh"

#include <fstream>

#include <ROOT/TROOT.h>
#include <ROOT/TFile.h>
#include <ROOT/TMap.h>
#include <ROOT/TString.h>

#include "analysis.hh"
#include "geometry/Construction.hh"

#include "util/io.hh"
#include "util/time.hh"

namespace MATHUSLA { namespace MU {

namespace { ////////////////////////////////////////////////////////////////////////////////////
//__Output File Path____________________________________________________________________________
G4ThreadLocal std::string _data_dir;
G4ThreadLocal std::string _path;
G4ThreadLocal uint_fast64_t _event_count;
G4ThreadLocal uint_fast64_t _run_count;
//----------------------------------------------------------------------------------------------
} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__RunAction Constructor_______________________________________________________________________
RunAction::RunAction(const std::string& data_dir) : G4UserRunAction() {
  _data_dir = data_dir == "" ? "data" : data_dir;
}
//----------------------------------------------------------------------------------------------

//__Run Initialization__________________________________________________________________________
void RunAction::BeginOfRunAction(const G4Run* run) {
  _path = _data_dir;
  util::io::create_directory(_path);
  _path += '/' + util::time::GetDate();
  util::io::create_directory(_path);
  _path += '/' + util::time::GetTime();
  util::io::create_directory(_path);
  _path += "/run" + std::to_string(_run_count);

  _event_count = run->GetNumberOfEventToBeProcessed();

  Analysis::Setup();
  Analysis::Open(_path + ".root");
  Analysis::GenerateNTupleCollection(
    Construction::Builder::IsDetectorDataPerEvent() ? _event_count : 1,
    Construction::Builder::GetDetectorDataPrefix(),
    Construction::Builder::GetDetectorDataKeys());
}
//----------------------------------------------------------------------------------------------

//__Post-Run Processing_________________________________________________________________________
void RunAction::EndOfRunAction(const G4Run*) {
  if (!_event_count) return;

  Analysis::Save();

  std::ofstream _info(_path + ".info");

  // auto root_file = TFile::Open((_path + ".root").c_str(), "UPDATE");
  // root_file->WriteObject(new TString(Construction::Builder::GetDetectorName().c_str()), "detector");
  // root_file->Close();

  _info << "MATHUSLA -- Muon Simulation\n"
        << util::time::GetString("%c %Z") << "\n"
        << Construction::Builder::GetDetectorName() << " Detector\n"
        << "Run "      << _run_count++ << "\n"
        << "Events: "  << _event_count << "\n"
        << "Data: "    << _path << ".root\n"
        << *GeneratorAction::GetGenerator();

  _info.close();

  std::cout << "\nEnd of Run\n"
            << "Data File: " << _path << ".root\n"
            << "Info File: " << _path << ".info\n\n";
}
//----------------------------------------------------------------------------------------------

//__Get Current RunID___________________________________________________________________________
size_t RunAction::RunID() {
  return _run_count;
}
//----------------------------------------------------------------------------------------------

//__Get Current EventCount______________________________________________________________________
size_t RunAction::EventCount() {
  return _event_count;
}
//----------------------------------------------------------------------------------------------

} } /* namespace MATHUSLA::MU */
