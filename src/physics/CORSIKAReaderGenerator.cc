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

#include "geometry/Construction.hh"
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

//__Pushback Particle Data______________________________________________________________________
void CORSIKAEvent::push_back(const Particle& particle) {
  push_back(particle.id, particle.t, particle.x, particle.y, particle.z, particle.px, particle.py, particle.pz);
}
//----------------------------------------------------------------------------------------------

//__Particle Data Index Accessor Operator_______________________________________________________
const Particle CORSIKAEvent::operator[](const std::size_t index) const {
  return Particle{id[index], t[index], x[index], y[index], z[index], px[index], py[index], pz[index]};
}
//----------------------------------------------------------------------------------------------

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Mutex for ROOT Interface____________________________________________________________________
G4Mutex _mutex = G4MUTEX_INITIALIZER;
//----------------------------------------------------------------------------------------------

//__CMS Plot Rotation Angle_____________________________________________________________________
const auto CMS_ROTATION_ANGLE = 80.0L * deg;
//----------------------------------------------------------------------------------------------

//__Calculate CMS Final Rotation________________________________________________________________
const std::pair<double, double> _cms_rotation(double x,
                                              double y) {
  return {x * std::cos(CMS_ROTATION_ANGLE / rad) + y * std::sin(CMS_ROTATION_ANGLE / rad),
          x * std::sin(CMS_ROTATION_ANGLE / rad) - y * std::cos(CMS_ROTATION_ANGLE / rad)};
}
//----------------------------------------------------------------------------------------------

//__Particle PDG Codes__________________________________________________________________________
#define PROTON_PDG 2212
#define NEUTRON_PDG 2112
//----------------------------------------------------------------------------------------------

//__Convert Primary ID from CORSIKA to PDG______________________________________________________
std::pair<int, int> _convert_primary_id(int corsika_id) {
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
  return std::make_pair(id_table[corsika_id - 1], corsika_id);
}
//----------------------------------------------------------------------------------------------

//__Prepare TTree Leaf for Scanning_____________________________________________________________
TLeaf* _load_leaf(TTree* tree,
                  const std::string& name) {
  tree->SetBranchStatus(name.c_str(), 1);
  return tree->GetLeaf(name.c_str());
}
//----------------------------------------------------------------------------------------------

//__Load Configuration from TTree to CORSIKAConfig______________________________________________
void _load_config(TTree* tree,
                  CORSIKAConfig& config) {
  auto primary_id = _load_leaf(tree, "run.ParticleID");
  auto energy_slope = _load_leaf(tree, "run.EnergySlope");
  auto energy_min = _load_leaf(tree, "run.EnergyMin");
  auto energy_max = _load_leaf(tree, "run.EnergyMax");
  auto azimuth_min = _load_leaf(tree, "run.AzimuthMin");
  auto azimuth_max = _load_leaf(tree, "run.AzimuthMax");
  auto zenith_min = _load_leaf(tree, "run.ZenithMin");
  auto zenith_max = _load_leaf(tree, "run.ZenithMax");
  tree->GetEntry(0);

  config.primary_id = _convert_primary_id(primary_id->GetValue(0)).first;
  config.energy_slope = energy_slope->GetValue(0);
  config.energy_min = energy_min->GetValue(0);
  config.energy_max = energy_max->GetValue(0);
  config.azimuth_min = azimuth_min->GetValue(0);
  config.azimuth_max = azimuth_max->GetValue(0);
  config.zenith_min = zenith_min->GetValue(0);
  config.zenith_max = zenith_max->GetValue(0);
}
//----------------------------------------------------------------------------------------------

//__Event Subtree Struct________________________________________________________________________
struct _event_subtree { TLeaf *id, *t, *x, *y, *z, *obs, *px, *py, *pz; };
//----------------------------------------------------------------------------------------------

//__Load Subtree From Tree______________________________________________________________________
_event_subtree _load_subtree(TTree* spec_tree,
                             TTree* data_tree) {
  _event_subtree out;
  data_tree->SetBranchStatus("*", 0);
  out.id = _load_leaf(data_tree, "particle..ParticleID");
  out.t = _load_leaf(data_tree, "particle..Time");
  out.x = _load_leaf(data_tree, "particle..x");
  out.y = _load_leaf(data_tree, "particle..y");
  out.z = _load_leaf(spec_tree, "run.ObservationLevel");
  out.px = _load_leaf(data_tree, "particle..Px");
  out.py = _load_leaf(data_tree, "particle..Py");
  out.pz = _load_leaf(data_tree, "particle..Pz");
  spec_tree->GetEntry(0);
  out.obs = _load_leaf(data_tree, "particle..ObservationLevel");
  return out;
}
//----------------------------------------------------------------------------------------------

//__Load Particle from Subtree__________________________________________________________________
void _load_particle(const std::size_t i,
                    _event_subtree& subtree,
                    const int particle_id,
                    const long double height,
                    CORSIKAEvent& out) {
  const auto position = _cms_rotation(subtree.x->GetValue(i), subtree.y->GetValue(i));
  const auto momentum = _cms_rotation(subtree.px->GetValue(i), subtree.py->GetValue(i));
  out.push_back(particle_id,
                subtree.t->GetValue(i) * ns,
                position.first * cm,
                position.second * cm,
                subtree.z->GetValue(subtree.obs->GetValue(i) - 1) * cm + height,
                momentum.first * GeVperC,
                momentum.second * GeVperC,
                subtree.pz->GetValue(i) * GeVperC);
}
//----------------------------------------------------------------------------------------------

//__Get Range of Thread in Data_________________________________________________________________
std::pair<std::size_t, std::size_t> _calculate_thread_range(const std::size_t total) {
  const auto bucket_size = std::ceil(total / static_cast<long double>(G4Threading::GetNumberOfRunningWorkerThreads()));
  const auto first = bucket_size * G4Threading::G4GetThreadId();
  return {first, first + bucket_size};
}
//----------------------------------------------------------------------------------------------

//__Sort Event in Time Order____________________________________________________________________
CORSIKAEvent _time_sort(const CORSIKAEvent& event) {
  const auto size = event.size();
  if (size == 0UL)
    return CORSIKAEvent{};
  CORSIKAEvent out;
  out.reserve(size);
  std::vector<std::size_t> indices;
  indices.reserve(size);
  for (std::size_t i{}; i < size; ++i)
    indices.push_back(i);
  std::sort(std::begin(indices), std::end(indices),
    [&](const auto left, const auto right) { return event.t[left] < event.t[right]; });
  for (std::size_t i{}; i < size; ++i)
    out.push_back(event[indices[i]]);
  return out;
}
//----------------------------------------------------------------------------------------------

//__Shift Event X and Y_________________________________________________________________________
CORSIKAEvent _shift_event_xy(CORSIKAEvent& event,
                             const std::size_t size,
                             const long double x_shift,
                             const long double y_shift) {
  CORSIKAEvent out;
  out.reserve(size);
  for (std::size_t i{}; i < size; ++i) {
    event.x[i] -= x_shift;
    event.y[i] -= y_shift;
    if (!( std::abs(event.x[i]) >= Construction::WorldLength / 2.0L
        || std::abs(event.y[i]) >= Construction::WorldLength / 2.0L))
      out.push_back(event[i]);
  }
  return out;
}
//----------------------------------------------------------------------------------------------

//__Split Event into Time Windows_______________________________________________________________
void _transform_and_split_event(const CORSIKAEvent& event,
                                const long double time_window,
                                const long double center_x,
                                const long double center_y,
                                CORSIKAEventVector& out) {
  const auto size = event.size();
  if (size == 0UL)
    return;

  const auto sorted = _time_sort(event);
  const auto min_time = sorted.t.front();

  CORSIKAEvent next;
  next.reserve(size);

  long double sum_x{}, sum_y{};
  std::size_t i{}, window_counter{};
  for (; i < size; ++i) {
    const auto next_size = next.size();
    if (sorted.t[i] >= min_time + time_window * window_counter) {
      if (next_size) {
        out.push_back(_shift_event_xy(next, next_size, sum_x / next_size - center_x, sum_y / next_size - center_y));
        next.clear();
      }
      ++window_counter;
      sum_x = 0.0L;
      sum_y = 0.0L;
    }
    next.push_back(sorted[i]);
    sum_x += next.x.back();
    sum_y += next.y.back();
  }

  if (i == size) {
    const auto next_size = next.size();
    if (next_size)
      out.push_back(_shift_event_xy(next, next_size, sum_x / next_size - center_x, sum_y / next_size - center_y));
  }
}
//----------------------------------------------------------------------------------------------

//__Fill Tree Data into EventVector_____________________________________________________________
template<class RangeCalculator>
void _fill_data(const std::string& path,
                const Particle& particle,
                const double time_block,
                CORSIKAConfig& config,
                CORSIKAEventVector& events,
                RangeCalculator range) {
  TFile file(path.c_str(), "READ");
  if (!file.IsZombie()) {
    file.cd();
    auto spec_tree = dynamic_cast<TTree*>(file.Get("run"));
    auto data_tree = dynamic_cast<TTree*>(file.Get("sim"));
    const auto data_entries = data_tree->GetEntries();
    if (spec_tree && data_tree && (spec_tree->GetEntries() == 1) && (data_entries > 0)) {
      spec_tree->SetBranchStatus("*", 0);
      _load_config(spec_tree, config);

      auto subtree = _load_subtree(spec_tree, data_tree);
      const auto range_pair = range(static_cast<std::size_t>(data_entries));

      for (std::size_t i{range_pair.first}; i < range_pair.second; ++i) {
        data_tree->GetEntry(i);
        const auto signed_event_size = subtree.id->GetLen();
        const auto event_size = signed_event_size > 0 ? static_cast<std::size_t>(signed_event_size) : 0UL;

        CORSIKAEvent next;
        next.reserve(event_size);

        for (std::size_t j{}; j < event_size; ++j) {
          const auto particle_id_pair = _convert_primary_id(subtree.id->GetValue(j));
          // fix for bad muons
          if (std::abs(particle_id_pair.first) == 13 && particle_id_pair.second > 10)
            continue;
          _load_particle(j, subtree, particle_id_pair.second, particle.z, next);
        }

        if (next.empty())
          continue;

        _transform_and_split_event(next, time_block, particle.x, particle.y, events);
      }
    }
  }

  file.Close();
}
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Initialize CORSIKA Data Vector______________________________________________________________
G4ThreadLocal CORSIKAEventVector* CORSIKAReaderGenerator::_data = nullptr;
G4ThreadLocal std::size_t CORSIKAReaderGenerator::_data_index = 0UL;
//----------------------------------------------------------------------------------------------

//__CORSIKA Reader Generator Constructor________________________________________________________
CORSIKAReaderGenerator::CORSIKAReaderGenerator()
    : Generator("corsika_reader", "CORSIKA Reader Generator."), _path(""), _time_block(1*ns) {
  _read_file = CreateCommand<Command::StringArg>("read_file", "Read CORSIKA ROOT File.");
  _read_file->AvailableForStates(G4State_PreInit, G4State_Idle);
  _set_time_block = CreateCommand<Command::DoubleUnitArg>("time_block", "Set Shower Time Block.");
  _set_time_block->AvailableForStates(G4State_PreInit, G4State_Idle);
  _set_time_block->SetParameterName("block", false, false);
  _set_time_block->SetRange("block > 0");
  _set_time_block->SetDefaultUnit("ns");
  _set_time_block->SetUnitCandidates("ns ms s");
  if (!_data)
    _data = new CORSIKAEventVector;
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
    if (G4Threading::IsWorkerThread()) {
      G4AutoLock lock(&_mutex);
      _fill_data(_path, _particle, _time_block, _config, *_data, _calculate_thread_range);
    }
  } else if (command == _set_time_block) {
    _time_block = _set_time_block->GetNewDoubleValue(value);
  } else {
    Generator::SetNewValue(command, value);
  }
}
//----------------------------------------------------------------------------------------------

//__Set Pythia Object from Settings_____________________________________________________________
void CORSIKAReaderGenerator::SetFile(const std::string& path) {
  _path = path;
  _data_index = 0UL;
}
//----------------------------------------------------------------------------------------------

//__CORSIKA Reader Generator Specifications_____________________________________________________
const Analysis::SimSettingList CORSIKAReaderGenerator::GetSpecification() const {
  return Analysis::Settings(SimSettingPrefix,
    "",              _name,
    "_INPUT_FILE",   _path,
    "_TIME_BLOCK",   std::to_string(_time_block),
    "_PRIMARY_ID",   std::to_string(_config.primary_id),
    "_ENERGY_SLOPE", std::to_string(_config.energy_slope),
    "_ENERGY_MIN",   std::to_string(_config.energy_min),
    "_ENERGY_MAX",   std::to_string(_config.energy_max),
    "_AZIMUTH_MIN",  std::to_string(_config.azimuth_min),
    "_AZIMUTH_MAX",  std::to_string(_config.azimuth_max),
    "_ZENITH_MIN",   std::to_string(_config.zenith_min),
    "_ZENITH_MAX",   std::to_string(_config.zenith_max)
  );
}
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
