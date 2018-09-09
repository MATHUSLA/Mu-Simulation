/*
 * src/physics/CORSIKAReaderGenerator.cc
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

#include "physics/CORSIKAReaderGenerator.hh"

#include <Geant4/G4Threading.hh>
#include <Geant4/G4AutoLock.hh>
#include <Geant4/G4MTRunManager.hh>
#include <Geant4/tls.hh>

#include <ROOT/TFile.h>
#include <ROOT/TTree.h>
#include <ROOT/TLeaf.h>

#include "physics/Units.hh"

namespace MATHUSLA { namespace MU {

namespace Physics { ////////////////////////////////////////////////////////////////////////////

//__Whether or Not the Particle Data is Empty___________________________________________________
bool CORSIKAEvent::empty() const {
  return id.empty();
}
//----------------------------------------------------------------------------------------------

//__Size of Particle Data_______________________________________________________________________
std::size_t CORSIKAEvent::size() const {
  return id.size();
}
//----------------------------------------------------------------------------------------------

//__Clear Particle Data_________________________________________________________________________
void CORSIKAEvent::clear() {
  id.clear();
  t.clear();
  x.clear();
  y.clear();
  z.clear();
  px.clear();
  py.clear();
  pz.clear();
}
//----------------------------------------------------------------------------------------------

//__Reserve Memory for Particle Data____________________________________________________________
void CORSIKAEvent::reserve(std::size_t capacity) {
  id.reserve(capacity);
  t.reserve(capacity);
  x.reserve(capacity);
  y.reserve(capacity);
  z.reserve(capacity);
  px.reserve(capacity);
  py.reserve(capacity);
  pz.reserve(capacity);
}
//----------------------------------------------------------------------------------------------

//__Pushback Particle Data______________________________________________________________________
void CORSIKAEvent::push_back(int new_id,
                             double new_t,
                             double new_x,
                             double new_y,
                             double new_z,
                             double new_px,
                             double new_py,
                             double new_pz) {
  id.push_back(new_id);
  t.push_back(new_t);
  x.push_back(new_x);
  y.push_back(new_y);
  z.push_back(new_z);
  px.push_back(new_px);
  py.push_back(new_py);
  pz.push_back(new_pz);
}
//----------------------------------------------------------------------------------------------

//__Particle Data Index Accessor Operator_______________________________________________________
const Particle CORSIKAEvent::operator[](const std::size_t index) const {
  return Particle{id[index], t[index], x[index], y[index], z[index], px[index], py[index], pz[index]};
}
//----------------------------------------------------------------------------------------------

//__Initialize CORSIKA Data Vector______________________________________________________________
G4ThreadLocal CORSIKAEventVector* CORSIKAReaderGenerator::_data = nullptr;
G4ThreadLocal std::size_t CORSIKAReaderGenerator::_data_index = 0UL;
//----------------------------------------------------------------------------------------------

//__CORSIKA Reader Generator Constructor________________________________________________________
CORSIKAReaderGenerator::CORSIKAReaderGenerator()
    : Generator("corsica_reader", "CORSIKA Reader Generator."), _path("") {
  _read_file = CreateCommand<Command::StringArg>("read_file", "Read CORSIKA ROOT File.");
  if (!_data)
    _data = new CORSIKAEventVector;
  // TODO: _read_directory = CreateCommand<Command::StringArg>("read_directory", "Read Directory with CORSIKA ROOT Files.");
}
//----------------------------------------------------------------------------------------------

//__Generate Initial Particles__________________________________________________________________
void CORSIKAReaderGenerator::GeneratePrimaryVertex(G4Event* event) {
  if (_data_index < _data->size()) {
    const auto entry = (*_data)[_data_index];
    for (std::size_t i{}; i < entry.size(); ++i)
      AddParticle(entry[i], *event);
    ++_data_index;
  }
}
//----------------------------------------------------------------------------------------------

//__Messenger Set Value_________________________________________________________________________
void CORSIKAReaderGenerator::SetNewValue(G4UIcommand* command,
                                         G4String value) {
  if (command == _read_file) {
    SetFile(value);
  }
}
//----------------------------------------------------------------------------------------------

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Mutex for ROOT Interface____________________________________________________________________
G4Mutex _mutex = G4MUTEX_INITIALIZER;
//----------------------------------------------------------------------------------------------

//__Prepare TTree Leaf for Scanning_____________________________________________________________
TLeaf* _prepare_leaf(TTree* tree,
                     const std::string& name) {
  tree->SetBranchStatus(name.c_str(), 1);
  return tree->GetLeaf(name.c_str());
}
//----------------------------------------------------------------------------------------------

//__Size Limit of ROOT Stack Object_____________________________________________________________
#define STACK_WIDTH 65536
//----------------------------------------------------------------------------------------------

//__CMS Plot Rotation Angle_____________________________________________________________________
const auto CMS_ROTATION_ANGLE = 80.0L * deg;
//----------------------------------------------------------------------------------------------

//__Calculate CMS Final X Rotation Value________________________________________________________
double _cms_x_rotation(double x,
                       double y) {
  return x * std::cos(CMS_ROTATION_ANGLE / rad) + y * std::sin(CMS_ROTATION_ANGLE / rad);
}
//----------------------------------------------------------------------------------------------

//__Calculate CMS Final Y Rotation Value________________________________________________________
double _cms_y_rotation(double x,
                       double y) {
  return x * std::sin(CMS_ROTATION_ANGLE / rad) - y * std::cos(CMS_ROTATION_ANGLE / rad);
}
//----------------------------------------------------------------------------------------------

//__Particle PDG Codes__________________________________________________________________________
#define PROTON_PDG 2212
#define NEUTRON_PDG 2112
//----------------------------------------------------------------------------------------------

//__Convert Primary ID from CORSIKA to PDG______________________________________________________
double _convert_primary_id(int corsika_id) {
  static const int id_table[] = {
       22,   -11,    11,     0,   -13,
       13,   111,   211,  -211,   130,
      321,  -321,  2112,  2212, -2212,
      310,   221,  3122,  3222,  3212,
     3112,  3322,  3312,  3334, -2112,
    -3122, -3112, -3212, -3222, -3322,
    -3312, -3334,     0,     0,     0,
        0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,
        0,     0,     0,     0,   223,
      113,   213,  -213,  2224,  2214,
     2114,  1114, -2224, -2214, -2114,
    -2214,   313,   323,  -323,  -313,
       12,   -12,    14,   -14,     0,
        0,     0,     0,     0,   -13,
       13,     0,     0,     0,     0,
        0,     0,     0,     0,   -13,
       13,     0,     0,     0,     0,
        0,     0,     0,     0,   -13,
       13,     0,     0,     0,     0,
        0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,
      421,   411,  -411,  -421,   431,
     -431,   441,   423,   413,  -413,
     -423,   433,  -433,     0,   443,
      -15,    15,    16,   -16,     0,
        0,  4122,  4232,  4132,  4222,
     4212,  4112,  4322,  4312,  4332,
        0,     0,     0, -4122, -4323,
    -4132, -4222, -4212, -4112, -4322,
    -4312, -4332,     0,     0,     0,
     4224,  4214,  4114,     0,     0,
        0,     0,     0,     0,     0,
    -4224, -4214, -4114,     0,     0,
        0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,
        0,     0,     0,     0,     0
  };
  return id_table[corsika_id - 1];
}
//----------------------------------------------------------------------------------------------

//__Load Configuration from TTree to CORSIKAConfig______________________________________________
void _load_config(TTree* tree,
                  CORSIKAConfig& config) {
  auto primary_id = _prepare_leaf(tree, "run.ParticleID");
  auto energy_slope = _prepare_leaf(tree, "run.EnergySlope");
  auto energy_min = _prepare_leaf(tree, "run.EnergyMin");
  auto energy_max = _prepare_leaf(tree, "run.EnergyMax");
  auto azimuth_min = _prepare_leaf(tree, "run.AzimuthMin");
  auto azimuth_max = _prepare_leaf(tree, "run.AzimuthMax");
  auto zenith_min = _prepare_leaf(tree, "run.ZenithMin");
  auto zenith_max = _prepare_leaf(tree, "run.ZenithMax");
  tree->GetEntry(0);

  config.primary_id = _convert_primary_id(primary_id->GetValue(0));
  config.energy_slope = energy_slope->GetValue(0);
  config.energy_min = energy_min->GetValue(0);
  config.energy_max = energy_max->GetValue(0);
  config.azimuth_min = azimuth_min->GetValue(0);
  config.azimuth_max = azimuth_max->GetValue(0);
  config.zenith_min = zenith_min->GetValue(0);
  config.zenith_max = zenith_max->GetValue(0);
}
//----------------------------------------------------------------------------------------------

//__Get Count When Using Threads________________________________________________________________
std::size_t _threaded_count(const std::size_t total) {
  const auto ratio = total / G4Threading::GetNumberOfRunningWorkerThreads();
  return !G4Threading::G4GetThreadId() ? ratio + total % G4Threading::GetNumberOfRunningWorkerThreads() : ratio;
}
//----------------------------------------------------------------------------------------------

//__Get Thread Index in List of Threads_________________________________________________________
std::size_t _thread_index(const std::size_t cycle) {
  return cycle * G4Threading::GetNumberOfRunningWorkerThreads() + G4Threading::G4GetThreadId();
}
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Set Pythia Object from Settings_____________________________________________________________
void CORSIKAReaderGenerator::SetFile(const std::string& path) {
  _path = path;

  G4AutoLock lock(&_mutex);
  TFile file(path.c_str(), "READ");
  if (!file.IsZombie()) {
    file.cd();
    auto spec_tree = dynamic_cast<TTree*>(file.Get("run"));
    auto data_tree = dynamic_cast<TTree*>(file.Get("sim"));
    const auto data_entries = data_tree->GetEntries();
    if (spec_tree && data_tree && (spec_tree->GetEntries() == 1) && (data_entries > 0)) {
      spec_tree->SetBranchStatus("*", 0);
      if (_config.primary_id == 0)
        _load_config(spec_tree, _config);

      data_tree->SetBranchStatus("*", 0);
      auto id = _prepare_leaf(data_tree, "particle..ParticleID");
      auto t = _prepare_leaf(data_tree, "particle..Time");
      auto x = _prepare_leaf(data_tree, "particle..x");
      auto y = _prepare_leaf(data_tree, "particle..y");
      auto px = _prepare_leaf(data_tree, "particle..Px");
      auto py = _prepare_leaf(data_tree, "particle..Py");
      auto pz = _prepare_leaf(data_tree, "particle..Pz");
      auto observation_level = _prepare_leaf(data_tree, "particle..ObservationLevel");

      auto z_level = _prepare_leaf(spec_tree, "run.ObservationLevel");
      spec_tree->GetEntry(0);

      // FIXME: sizes
      const auto size = _threaded_count(static_cast<std::size_t>(data_entries));
      _data->clear();
      _data->reserve(size);

      for (std::size_t i{}; i < size; ++i) {
        data_tree->GetEntry(_thread_index(i));
        const auto signed_event_size = id->GetLen();
        const auto event_size = signed_event_size > 0 ? static_cast<std::size_t>(signed_event_size) : 0UL;

        _data->emplace_back();
        auto& next = _data->back();
        next.reserve(event_size);

        std::cout << i << ":\n";
        std::size_t counter{};
        long double average_t{}, average_x{}, average_y{};
        for (std::size_t j{}; j < event_size; ++j) {
          const auto particle_id = _convert_primary_id(id->GetValue(j));
          if (particle_id == PROTON_PDG) {
            ++counter;
            continue;
          }
          const auto old_x = x->GetValue(j) * cm;
          const auto old_y = y->GetValue(j) * cm;
          const auto old_px = px->GetValue(j) * GeVperC;
          const auto old_py = py->GetValue(j) * GeVperC;
          next.push_back(particle_id,
                         t->GetValue(j) * ns,
                         _cms_x_rotation(old_x, old_y),
                         _cms_y_rotation(old_x, old_y),
                         z_level->GetValue(observation_level->GetValue(j) - 1) * cm - 40*m, // figure out constant
                         _cms_x_rotation(old_px, old_py),
                         _cms_y_rotation(old_px, old_py),
                         pz->GetValue(j) * GeVperC);
          average_t += next.t.back();
          average_x += next.x.back();
          average_y += next.y.back();

          /*
          std::cout << j << ": "
                    << next.id.back() << " "
                    << next.t.back() / ns << " "
                    << next.x.back() / cm << " "
                    << next.y.back() / cm << " "
                    << next.z.back() / cm << " "
                    << next.px.back() / GeVperC << " "
                    << next.py.back() / GeVperC << " "
                    << next.pz.back() / GeVperC << "\n";
          */
        }
        average_t /= (event_size - counter);
        average_x /= (event_size - counter);
        average_y /= (event_size - counter);

        counter = 0UL;
        for (std::size_t j{}; j < event_size; ++j) {
          if (_convert_primary_id(id->GetValue(j)) == PROTON_PDG) {
            ++counter;
            continue;
          }
          next.t[j - counter] -= average_t;
          next.x[j - counter] -= average_x;
          next.y[j - counter] -= average_y;

          std::cout //<< j << ": "
                    //<< next.id[j] << " "
                    //<< next.t[j] / ns << " "
                    << next.x[j - counter] / cm << "\n";
                    //<< next.y[j] / cm << " "
                    //<< next.z[j] / cm << " "
                    //<< next.px[j] / GeVperC << " "
                    //<< next.py[j] / GeVperC << " "
                    //<< next.pz[j] / GeVperC << "\n";
        }

        std::cout << "\n";
      }
    }
  }
  file.Close();
  //exit(1);
}
//----------------------------------------------------------------------------------------------

//__CORSIKA Reader Generator Specifications_____________________________________________________
const Analysis::SimSettingList CORSIKAReaderGenerator::GetSpecification() const {
  return {};
}
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
