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

#include "analysis.hh"
#include "geometry/Construction.hh"

#include "util/io.hh"
#include "util/time.hh"

namespace MATHUSLA { namespace MU {

namespace { ////////////////////////////////////////////////////////////////////////////////////
//__Output File Path____________________________________________________________________________
G4ThreadLocal std::string _path;
//----------------------------------------------------------------------------------------------
} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

/*
TODO: Save Event Count for User to Access
 */

//__Run Initialization__________________________________________________________________________
void RunAction::BeginOfRunAction(const G4Run* run) {
  _path = "data";
  util::io::create_directory(_path);
  _path += '/' + util::time::GetDate();
  util::io::create_directory(_path);
  _path += '/' + util::time::GetTime();
  util::io::create_directory(_path);
  _path += "/run" + std::to_string(run->GetRunID());

  Analysis::Setup();
  Analysis::Open(_path + ".root");
  Analysis::GenerateNTupleCollection(run->GetNumberOfEventToBeProcessed(), "event", {
    "Deposit", "Time", "Detector",
    "PDG", "Track", "X", "Y", "Z", "E", "PX", "PY", "PZ", "D_PMT",
    "TimestampDate", "TimestampTime" });
}
//----------------------------------------------------------------------------------------------

//__Post-Run Processing_________________________________________________________________________
void RunAction::EndOfRunAction(const G4Run* run) {
  const auto event_count = run->GetNumberOfEventToBeProcessed();
  if (!event_count) return;

  Analysis::Save();

  std::ofstream _info(_path + ".info");

  _info << "MATHUSLA -- Muon Simulation\n"
        << util::time::GetString("%c %Z") << "\n\n"
        << Construction::Builder::GetDetector() << " Detector\n\n"
        << "Run "      << run->GetRunID() << "\n"
        << "Events: "  << event_count << "\n"
        << "Data: "    << _path << ".root\n\n"
        << *GeneratorAction::GetGenerator();

  _info.close();

  std::cout << "\nEnd of Run\n"
            << "Data File: " << _path << ".root\n"
            << "Info File: " << _path << ".info\n\n";
}
//----------------------------------------------------------------------------------------------

} } /* namespace MATHUSLA::MU */
