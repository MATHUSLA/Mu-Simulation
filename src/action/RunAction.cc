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

#include <Geant4/G4Threading.hh>
#include <Geant4/G4AutoLock.hh>
#include <Geant4/G4MTRunManager.hh>

#include <ROOT/TFile.h>
#include <ROOT/TNamed.h>

#include "analysis.hh"
#include "geometry/Construction.hh"
#include "physics/Units.hh"

#include "util/io.hh"
#include "util/time.hh"

namespace MATHUSLA { namespace MU {

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Static Variables____________________________________________________________________________
G4ThreadLocal std::string _data_dir{};
G4ThreadLocal std::string _path{};
G4ThreadLocal uint_fast64_t _event_count{};
G4ThreadLocal uint_fast64_t _run_count{};
uint_fast16_t _cycle_count{};
G4Mutex _mutex = G4MUTEX_INITIALIZER;
//----------------------------------------------------------------------------------------------

//__Forward Arguments to String Stream__________________________________________________________
template<class T>
void _stream_forward(std::stringstream& s, T&& arg) {
  s << std::forward<T>(arg);
}
template<class T, class... Args>
void _stream_forward(std::stringstream& s, T&& arg, Args&&... args) {
  _stream_forward(s, arg);
  _stream_forward(s, args...);
}
//----------------------------------------------------------------------------------------------

//__Write Entry to ROOT File____________________________________________________________________
template<class... Args>
void _write_entry(const std::string& name,
                  Args&& ...args) {
  std::stringstream stream;
  _stream_forward(stream, args...);
  TNamed entry(name.c_str(), stream.str().c_str());
  entry.Write();
}
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

  G4AutoLock lock(&_mutex);
  #ifdef G4MULTITHREADED
    ++_cycle_count;
  #else
    _cycle_count = 2;
  #endif
  lock.unlock();

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

  G4AutoLock lock(&_mutex);
  if (--_cycle_count == 0) {
    auto root_file = TFile::Open((_path + ".root").c_str(), "UPDATE");
    if (root_file && !root_file->IsZombie()) {
      // TODO: move some of this code into generator itself
      const auto generator = GeneratorAction::GetGenerator();
      _write_entry("FILETYPE",   "MATHUSLA Mu-Simulation DATAFILE");
      _write_entry("DET",        Construction::Builder::GetDetectorName());
      _write_entry("GEN",        generator->GetName());
      _write_entry("GEN_PDG_ID", generator->id());
      _write_entry("GEN_PT",     generator->pT() / Units::Momentum, " ", Units::MomentumString);
      _write_entry("GEN_ETA",    generator->eta());
      _write_entry("GEN_PHI",    generator->phi() / Units::Angle, " ", Units::AngleString);
      _write_entry("GEN_KE",     generator->ke() / Units::Energy, " ", Units::EnergyString);
      _write_entry("GEN_P_UNIT", generator->p_unit());
      _write_entry("RUN",        _run_count++);
      _write_entry("EVENTS",     _event_count);
      _write_entry("TIMESTAMP",  util::time::GetString("%c %Z"));
      root_file->Close();
    }
    std::cout << "\nEnd of Run\nData File: " << _path << ".root\n\n";
  }
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
