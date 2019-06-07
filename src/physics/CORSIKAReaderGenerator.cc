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

#include <TFile.h>
#include <TTree.h>
#include <TLeaf.h>

#include "geometry/Construction.hh"
#include "physics/Units.hh"
#include "util/random.hh"
#include "action.hh"
#include "tracking.hh"

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
  weight.clear();
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
  weight.reserve(capacity);
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
                             double new_pz,
                             double new_weight) {
  id.push_back(new_id);
  t.push_back(new_t);
  x.push_back(new_x);
  y.push_back(new_y);
  z.push_back(new_z);
  px.push_back(new_px);
  py.push_back(new_py);
  pz.push_back(new_pz);
  weight.push_back(new_weight);
}
//----------------------------------------------------------------------------------------------

//__Pushback Particle Data______________________________________________________________________
void CORSIKAEvent::push_back(const Particle& particle, double new_weight) {
  push_back(particle.id,
            particle.t,
            particle.x,
            particle.y,
            particle.z,
            particle.px,
            particle.py,
            particle.pz,
            new_weight);
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

//__Get Range of Thread in Data_________________________________________________________________
std::pair<std::size_t, std::size_t> _calculate_thread_range(const std::size_t total) {
  const auto bucket_size = std::ceil(total / static_cast<long double>(G4Threading::GetNumberOfRunningWorkerThreads()));
  const auto first = bucket_size * G4Threading::G4GetThreadId();
  return {first, first + bucket_size};
}
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

//__Event Subtree Struct________________________________________________________________________
struct _event_subtree {
  TTree *spec_tree, *data_tree;
  TLeaf *event_id, *energy, *theta, *phi, *z0, *electron_count, *muon_count, *hadron_count,
        *id, *t, *x, *y, *z, *obs, *px, *py, *pz, *weight,
        *primary_id,
        *energy_slope, *energy_min, *energy_max,
        *azimuth_min, *azimuth_max, *zenith_min, *zenith_max;

  _event_subtree(TTree* spec, TTree* data) : spec_tree(spec), data_tree(data) {
    data_tree->SetBranchStatus("*", 0);
    spec_tree->SetBranchStatus("*", 0);
    event_id       = _load_leaf(data_tree, "shower.EventID");
    energy         = _load_leaf(data_tree, "shower.Energy");
    theta          = _load_leaf(data_tree, "shower.Theta");
    phi            = _load_leaf(data_tree, "shower.Phi");
    z0             = _load_leaf(data_tree, "shower.FirstHeight");
    electron_count = _load_leaf(data_tree, "shower.nElectrons");
    muon_count     = _load_leaf(data_tree, "shower.nMuons");
    hadron_count   = _load_leaf(data_tree, "shower.nHadrons");
    id             = _load_leaf(data_tree, "particle..ParticleID");
    t              = _load_leaf(data_tree, "particle..Time");
    x              = _load_leaf(data_tree, "particle..x");
    y              = _load_leaf(data_tree, "particle..y");
    z              = _load_leaf(data_tree, "particle..ObservationLevel");
    px             = _load_leaf(data_tree, "particle..Px");
    py             = _load_leaf(data_tree, "particle..Py");
    pz             = _load_leaf(data_tree, "particle..Pz");
    weight         = _load_leaf(data_tree, "particle..Weight");
    obs            = _load_leaf(spec_tree, "run.ObservationLevel");
    primary_id     = _load_leaf(spec_tree, "run.ParticleID");
    energy_slope   = _load_leaf(spec_tree, "run.EnergySlope");
    energy_min     = _load_leaf(spec_tree, "run.EnergyMin");
    energy_max     = _load_leaf(spec_tree, "run.EnergyMax");
    azimuth_min    = _load_leaf(spec_tree, "run.AzimuthMin");
    azimuth_max    = _load_leaf(spec_tree, "run.AzimuthMax");
    zenith_min     = _load_leaf(spec_tree, "run.ZenithMin");
    zenith_max     = _load_leaf(spec_tree, "run.ZenithMax");
  }

  void load_config(CORSIKAConfig& config) {
    config.primary_id     = _convert_primary_id(primary_id->GetValue(0));
    config.energy         = energy->GetValue(0);
    config.theta          = theta->GetValue(0);
    config.phi            = phi->GetValue(0);
    config.z0             = z0->GetValue(0);
    config.electron_count = electron_count->GetValue(0);
    config.muon_count     = muon_count->GetValue(0);
    config.hadron_count   = hadron_count->GetValue(0);
    config.energy_slope   = energy_slope->GetValue(0);
    config.energy_min     = energy_min->GetValue(0);
    config.energy_max     = energy_max->GetValue(0);
    config.azimuth_min    = azimuth_min->GetValue(0);
    config.azimuth_max    = azimuth_max->GetValue(0);
    config.zenith_min     = zenith_min->GetValue(0);
    config.zenith_max     = zenith_max->GetValue(0);
  }
};
//----------------------------------------------------------------------------------------------

//__Load Particle from Subtree__________________________________________________________________
void _load_particle(const std::size_t i,
                    _event_subtree& subtree,
                    const int particle_id,
                    const Particle& origin,
                    CORSIKAEvent& out) {
  const auto position = _cms_rotation(subtree.x->GetValue(i) * cm,
                                      subtree.y->GetValue(i) * cm);
  const auto momentum = _cms_rotation(subtree.px->GetValue(i) * GeVperC,
                                      subtree.py->GetValue(i) * GeVperC);
  out.push_back(particle_id,
                subtree.t->GetValue(i) * ns,
                position.first + origin.x,
                position.second + origin.y,
                -(subtree.obs->GetValue(subtree.z->GetValue(i) - 1) * cm - origin.z),
                momentum.first,
                momentum.second,
                subtree.pz->GetValue(i) * GeVperC,
                subtree.weight->GetValue(i));
}
//----------------------------------------------------------------------------------------------

//__Perform Random Translation of Event Vector__________________________________________________
std::pair<double, double> _random_translation(double max_radius) {
  const auto r = max_radius * std::sqrt(util::random::uniform());
  const auto theta = 2.0L * 3.141592653589793238462643383279502884L * util::random::uniform();
  return std::make_pair(r * std::cos(theta), r * std::sin(theta));
}
//----------------------------------------------------------------------------------------------

//__Collect Data From Tree______________________________________________________________________
void _collect_source(const std::string& path,
                     const Particle& origin,
                     CORSIKAConfig& config,
                     CORSIKAEvent& event) {
  TFile file(path.c_str(), "READ");
  if (!file.IsZombie()) {
    std::cout << "\n\nLoading Data from " + path + " ...\n\n";
    file.cd();
    auto spec_tree = dynamic_cast<TTree*>(file.Get("run"));
    auto data_tree = dynamic_cast<TTree*>(file.Get("sim"));
    const auto data_entries = data_tree->GetEntries();
    if (spec_tree && data_tree && (spec_tree->GetEntries() == 1) && (data_entries > 0)) {
      _event_subtree subtree{spec_tree, data_tree};
      subtree.spec_tree->GetEntry(0);
      subtree.data_tree->GetEntry(config.event_id);
      subtree.load_config(config);

      const auto signed_event_size = subtree.id->GetLen();
      const auto event_size = signed_event_size > 0 ? static_cast<std::size_t>(signed_event_size) : 0UL;

      event.reserve(event_size);
      for (std::size_t i{}; i < event_size; ++i) {
        const auto particle_id_pair = _convert_primary_id(subtree.id->GetValue(i));
        if ((std::abs(particle_id_pair.first) == 13 && particle_id_pair.second > 10))
          continue;
        _load_particle(i, subtree, particle_id_pair.first, origin, event);
      }

      if (event.empty()) {
        std::cout << "No Event in CORSIKA File. Exiting.\n";
        file.Close();
        exit(0);
      }

    }
    std::cout << "Completed. Beginning Run ...\n\n";
  }
  file.Close();
}
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__CORSIKA Reader Generator Constructor________________________________________________________
CORSIKAReaderGenerator::CORSIKAReaderGenerator(const std::string& path)
    : Generator("corsika_reader", "CORSIKA Reader Generator."), _last_event({}), _translation({0, 0}), _path(path) {
  _read_file = CreateCommand<Command::StringArg>("read_file", "Read CORSIKA ROOT File.");
  _read_file->AvailableForStates(G4State_PreInit, G4State_Idle);

  _set_event_id = CreateCommand<Command::IntegerArg>("event_id", "Set Shower ID to Simulate.");
  _set_event_id->AvailableForStates(G4State_PreInit, G4State_Idle);
  _set_event_id->SetParameterName("event", false, false);
  _set_event_id->SetRange("event > 0");

  _set_max_radius = CreateCommand<Command::DoubleUnitArg>("max_radius", "Set Shower Shift Max Radius.");
  _set_max_radius->AvailableForStates(G4State_PreInit, G4State_Idle);
  _set_max_radius->SetParameterName("radius", false, false);
  _set_max_radius->SetRange("radius >= 0");
  _set_max_radius->SetDefaultUnit("m");
  _set_max_radius->SetUnitCandidates("m cm");
}
//----------------------------------------------------------------------------------------------

//__Generate Initial Particles__________________________________________________________________
void CORSIKAReaderGenerator::GeneratePrimaryVertex(G4Event* event) {
  _last_event.clear();
  _translation = _random_translation(_config.max_radius);
  for (std::size_t i{}; i < _event.size(); ++i) {
    auto particle = _event[i];
    particle.x -= _translation.first;
    particle.y -= _translation.second;
    if (std::abs(particle.x) >= Construction::WorldLength / 2.0L
        || std::abs(particle.y) >= Construction::WorldLength / 2.0L)
      continue;
    _last_event.push_back(particle);
    AddParticle(particle, *event);
  }
}
//----------------------------------------------------------------------------------------------

//__Get Previous Event__________________________________________________________________________
ParticleVector CORSIKAReaderGenerator::GetLastEvent() const {
  return _last_event;
}
//----------------------------------------------------------------------------------------------

//__Messenger Set Value_________________________________________________________________________
void CORSIKAReaderGenerator::SetNewValue(G4UIcommand* command,
                                         G4String value) {
  if (command == _read_file) {
    SetFile(value);
  } else if (command == _set_event_id) {
    _config.event_id = _set_event_id->GetNewIntValue(value);
  } else if (command == _set_max_radius) {
    _config.max_radius = _set_max_radius->GetNewDoubleValue(value);
  } else {
    Generator::SetNewValue(command, value);
  }
}
//----------------------------------------------------------------------------------------------

//__Set Pythia Object from Settings_____________________________________________________________
void CORSIKAReaderGenerator::SetFile(const std::string& path) {
  _path = path;
  if (G4Threading::IsWorkerThread()) {
    G4AutoLock lock(&_mutex);
    _event.clear();
    _collect_source(_path, _particle, _config, _event);
  }
}
//----------------------------------------------------------------------------------------------

//__CORSIKA Reader Generator Specifications_____________________________________________________
const Analysis::SimSettingList CORSIKAReaderGenerator::GetSpecification() const {
  return Analysis::Settings(SimSettingPrefix,
    "",                  _name,
    "_INPUT_FILE",       _path,
    "_EVENT_ID",         std::to_string(_config.event_id),
    "_PRIMARY_ENERGY",   std::to_string(_config.energy),
    "_THETA",            std::to_string(_config.theta),
    "_PHI",              std::to_string(_config.phi),
    "_FIRST_HEIGHT",     std::to_string(_config.z0),
    "_ELECTRON_COUNT",   std::to_string(_config.electron_count),
    "_MUON_COUNT",       std::to_string(_config.muon_count),
    "_HADRON_COUNT",     std::to_string(_config.hadron_count),
    "_PRIMARY_ID",       std::to_string(_config.primary_id.second),
    "_ENERGY_SLOPE",     std::to_string(_config.energy_slope),
    "_ENERGY_MIN",       std::to_string(_config.energy_min),
    "_ENERGY_MAX",       std::to_string(_config.energy_max),
    "_AZIMUTH_MIN",      std::to_string(_config.azimuth_min),
    "_AZIMUTH_MAX",      std::to_string(_config.azimuth_max),
    "_ZENITH_MIN",       std::to_string(_config.zenith_min),
    "_ZENITH_MAX",       std::to_string(_config.zenith_max),
    "_MAX_SHIFT_RADIUS", Units::to_string(_config.max_radius, Units::Length, Units::LengthString)
  );
}
//----------------------------------------------------------------------------------------------

//__CORSIKA Reader Generator Extra Details______________________________________________________
const std::vector<std::vector<double>> CORSIKAReaderGenerator::ExtraDetails() const {
  auto out = Tracking::EmptyExtra();
  out[0].push_back(_config.event_id);
  out[1].push_back(_translation.first);
  out[2].push_back(_translation.second);
  out[3].push_back(_config.energy);
  out[4].push_back(_config.theta);
  out[5].push_back(_config.phi);
  out[6].push_back(_config.z0);
  out[7].push_back(_config.electron_count);
  out[8].push_back(_config.muon_count);
  out[9].push_back(_config.hadron_count);
  out[10].push_back(_config.primary_id.second);
  return out;
}
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
