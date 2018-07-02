/*
 * src/action/RunAction.cc
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
#include <ostream>

#include <Geant4/G4Threading.hh>
#include <Geant4/G4AutoLock.hh>
#include <Geant4/G4MTRunManager.hh>

#include <ROOT/TFile.h>
#include <ROOT/TNamed.h>
#include <ROOT/TTree.h>
#include <ROOT/TChain.h>

#include "analysis.hh"
#include "geometry/Construction.hh"
#include "physics/Units.hh"

#include "util/io.hh"
#include "util/time.hh"
#include "util/stream.hh"

namespace MATHUSLA { namespace MU {

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Data Directory Variables____________________________________________________________________
G4ThreadLocal std::string _data_dir{};
G4ThreadLocal std::string _prefix{};
G4ThreadLocal std::string _path{};
const std::string _temp_path = ".temp.root";
std::vector<std::string> _worker_tags;
//----------------------------------------------------------------------------------------------

//__Environment Counters________________________________________________________________________
G4ThreadLocal std::size_t _worker_count{};
G4ThreadLocal std::size_t _event_count{};
G4ThreadLocal std::size_t _run_count{};
//----------------------------------------------------------------------------------------------

//__Mutex for ROOT Interface____________________________________________________________________
G4Mutex _mutex = G4MUTEX_INITIALIZER;
//----------------------------------------------------------------------------------------------

//__Write Entry to ROOT File____________________________________________________________________
template<class... Args>
void _write_entry(TFile* file,
                  const std::string& name,
                  Args&& ...args) {
  std::stringstream stream;
  util::stream::forward(stream, args...);
  TNamed entry(name.c_str(), stream.str().c_str());
  file->cd();
  entry.Write();
}
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__RunAction Constructor_______________________________________________________________________
RunAction::RunAction(const std::string& data_dir) : G4UserRunAction() {
  _data_dir = data_dir == "" ? "data" : data_dir;
  _worker_count = static_cast<std::size_t>(G4Threading::GetNumberOfRunningWorkerThreads());
  _worker_tags.clear();
  _worker_tags.reserve(_worker_count);
  for (std::size_t i = 0; i < _worker_count; ++i)
    _worker_tags.push_back(".temp_t" + std::to_string(i) + ".root");
}
//----------------------------------------------------------------------------------------------

//__Run Initialization__________________________________________________________________________
void RunAction::BeginOfRunAction(const G4Run* run) {
  _prefix = _data_dir;
  util::io::create_directory(_prefix);
  _prefix += '/' + util::time::GetDate();
  util::io::create_directory(_prefix);
  _prefix += '/' + util::time::GetTime();
  util::io::create_directory(_prefix);
  _prefix += "/run";
  _path = _prefix + std::to_string(_run_count) + ".root";

  _event_count = run->GetNumberOfEventToBeProcessed();

  Analysis::ROOT::Setup();
  Analysis::ROOT::Open(_prefix + _temp_path);
  Analysis::ROOT::CreateNTuple(
    Construction::Builder::GetDetectorDataName(),
    Construction::Builder::GetDetectorDataKeys(),
    Construction::Builder::GetDetectorDataKeyTypes());
}
//----------------------------------------------------------------------------------------------

//__Post-Run Processing_________________________________________________________________________
void RunAction::EndOfRunAction(const G4Run*) {
  if (!_event_count)
    return;

  Analysis::ROOT::Save();

  G4AutoLock lock(&_mutex);
  if (!G4Threading::IsWorkerThread()) {
    auto file = TFile::Open(_path.c_str(), "UPDATE");
    if (file && !file->IsZombie()) {
      file->cd();
      auto chain = new TChain(Construction::Builder::GetDetectorDataName().c_str());
      for (const auto& tag : _worker_tags)
        chain->Add((_prefix + tag).c_str());

      TTree* tree = chain;
      file->cd();
      tree->CloneTree()->Write();
      delete chain;

      util::io::remove_file(_prefix + _temp_path);
      for (const auto& tag : _worker_tags)
        util::io::remove_file(_prefix + tag);

      file->cd();

      _write_entry(file, "FILETYPE", "MATHULSA MU-SIM DATAFILE");
      _write_entry(file, "DET", Construction::Builder::GetDetectorName());

      for (const auto& entry : GeneratorAction::GetGenerator()->GetSpecification())
        _write_entry(file, entry.name, entry.text);

      _write_entry(file, "RUN", _run_count);
      _write_entry(file, "EVENTS", _event_count);
      _write_entry(file, "TIMESTAMP", util::time::GetString("%c %Z"));

      file->Close();

      ++_run_count;
      std::cout << "\nEnd of Run\nData File: " << _path << "\n\n";
    }
  }
  lock.unlock();
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
