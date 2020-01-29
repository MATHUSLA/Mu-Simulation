/* src/geometry/prototype/Prototype.cc
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

#include "geometry/Prototype.hh"

#include <unordered_map>
#include <cstddef>
#include <algorithm>
#include <cmath>
#include <string>

#include <G4HCofThisEvent.hh>
#include <G4Step.hh>
#include <tls.hh>

#include "action.hh"
#include "analysis.hh"
#include "geometry/Cavern.hh"
#include "physics/Units.hh"
#include "tracking.hh"
#include "geometry/Earth.hh"

namespace MATHUSLA { namespace MU {

namespace Prototype { //////////////////////////////////////////////////////////////////////////

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Scintillators and RPCs for Prototype________________________________________________________
std::vector<Scintillator*> _scintillators;
std::vector<RPC*> _rpcs;
std::vector<UChannel *> _uchannels;
//----------------------------------------------------------------------------------------------

//__Prototype Hit Collection____________________________________________________________________
G4ThreadLocal Tracking::HitCollection* _hit_collection;
//----------------------------------------------------------------------------------------------

//__Encoding/Decoding Maps______________________________________________________________________
G4ThreadLocal std::unordered_map<std::string, Scintillator*> _scintillator_map;
G4ThreadLocal std::unordered_map<std::string, double>        _encoding;
G4ThreadLocal std::unordered_map<int, std::string>           _decoding;
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Prototype Data Variables____________________________________________________________________
const std::string& Detector::DataName = "prototype_run";
const Analysis::ROOT::DataKeyList Detector::DataKeys = Analysis::ROOT::DefaultDataKeyList;
const Analysis::ROOT::DataKeyTypeList Detector::DataKeyTypes = Analysis::ROOT::DefaultDataKeyTypeList;
bool Detector::SaveAll = false;
//----------------------------------------------------------------------------------------------

//__Prototype Constructor_______________________________________________________________________
Detector::Detector() : G4VSensitiveDetector("MATHUSLA/MU/Prototype") {
  collectionName.insert("Prototype_HC");
  for (auto scintillator : _scintillators) {
    scintillator->Register(this);
    const auto name = scintillator->GetFullName();
    const auto id = _decoding.size();
    _scintillator_map.insert({name, scintillator});
    _encoding.insert({name, id});
    _decoding.insert({id, name});
  }
  for (auto rpc : _rpcs) {
    rpc->Register(this);
    for (const auto& pad : rpc->GetPadList()) {
      for (const auto& volume : pad->pvolume_strips) {
        const auto& name = volume->GetName();
        const auto id = std::stoi(name);
        _encoding.insert({name, id});
        _decoding.insert({id, name});
      }
    }
  }
}
//----------------------------------------------------------------------------------------------

//__Initalize Event_____________________________________________________________________________
void Detector::Initialize(G4HCofThisEvent* event) {
  _hit_collection = Tracking::GenerateHitCollection(this, event);
}
//----------------------------------------------------------------------------------------------

//__Hit Processing______________________________________________________________________________
G4bool Detector::ProcessHits(G4Step* step, G4TouchableHistory*) {
  const auto deposit = step->GetTotalEnergyDeposit();

  const auto min_deposit = Scintillator::MinDeposit < RPC::MinDeposit ?
                           Scintillator::MinDeposit : RPC::MinDeposit;

  if (!deposit || deposit < min_deposit)
    return false;

  const auto track       = step->GetTrack();
  const auto trackID     = track->GetTrackID();
  const auto particle    = track->GetParticleDefinition();
  const auto history     = track->GetTouchable()->GetHistory();
  const auto name        = history->GetTopVolume()->GetName();
  const auto post_step   = step->GetPostStepPoint();

  const auto global_time = post_step->GetGlobalTime()  / Units::Time;
  const auto position    = post_step->GetPosition()    / Units::Length;
  const auto energy      = post_step->GetTotalEnergy() / Units::Energy;
  const auto momentum    = post_step->GetMomentum()    / Units::Momentum;

  _hit_collection->insert(
    new Tracking::Hit(
      particle,
      trackID,
      track->GetParentID(),
      name,
      deposit / Units::Energy,
      G4LorentzVector(global_time, position),
      G4LorentzVector(energy, momentum)));

  /* FIXME: add back to data
  Scintillator::PMTPoint pmt_point{0, 0, 0};
  if (detector_id < 100) {
    const auto sci = _scintillator_map[name];

    const auto& volume1 = history->GetVolume(history->GetDepth() - 1);
    const auto& volume2 = history->GetVolume(history->GetDepth() - 2);
    const auto& volume3 = history->GetVolume(history->GetDepth() - 3);
    const auto& volume4 = history->GetVolume(history->GetDepth() - 4);

    const auto& t1 = volume1->GetObjectTranslation();
    const auto& t2 = volume2->GetObjectTranslation();
    const auto& t3 = volume3->GetObjectTranslation();
    const auto& t4 = volume4->GetObjectTranslation();

    const auto& r2 = volume2->GetObjectRotationValue();
    const auto& r3 = volume3->GetObjectRotationValue();
    const auto& r4 = volume4->GetObjectRotationValue();

    const auto translation = r4 * (r3 * (r2 * (t1) + t2) + t3) + t4;
    const auto rotation = (r4*r3*r2).inverse();

    if (sci)
      pmt_point = Scintillator::PMTDistance(position, sci, translation, rotation);
  }
  */

  return true;
}
//----------------------------------------------------------------------------------------------

//__Post-Event Processing_______________________________________________________________________
void Detector::EndOfEvent(G4HCofThisEvent*) {
  if (_hit_collection->GetSize() == 0 && !SaveAll)
    return;

  const auto collection_data = Tracking::ConvertToAnalysis(_hit_collection, _encoding);

  Analysis::ROOT::DataEntryList root_data;
  root_data.reserve(24);
  root_data.push_back(collection_data[0]);
  root_data.push_back(collection_data[1]);
  root_data.push_back(collection_data[2]);
  root_data.push_back(collection_data[3]);
  root_data.push_back(collection_data[4]);
  root_data.push_back(collection_data[5]);
  root_data.push_back(collection_data[6]);
  root_data.push_back(collection_data[7]);
  root_data.push_back(collection_data[8]);
  root_data.push_back(collection_data[9]);
  root_data.push_back(collection_data[10]);
  root_data.push_back(collection_data[11]);
  root_data.push_back(collection_data[12]);
  root_data.push_back(collection_data[13]);

  const auto gen_particle_data = SaveAll ? Tracking::ConvertToAnalysis(GeneratorAction::GetLastEvent())
                                         : Tracking::ConvertToAnalysis(EventAction::GetEvent());
  const auto extra_gen_data = Tracking::ConvertToAnalysis(GeneratorAction::GetGenerator()->ExtraDetails());
  root_data.insert(root_data.cend(), gen_particle_data.cbegin(), gen_particle_data.cend());
  root_data.insert(root_data.cend(), extra_gen_data.cbegin(), extra_gen_data.cend());

  Analysis::ROOT::DataEntry metadata;
  metadata.reserve(2);
  metadata.push_back(collection_data[0].size());
  metadata.push_back(gen_particle_data[0].size());

  Analysis::ROOT::FillNTuple(DataName, Detector::DataKeyTypes, metadata, root_data);
  if (verboseLevel >= 2 && _hit_collection)
    std::cout << *_hit_collection;
}
//----------------------------------------------------------------------------------------------

//__Detector Encoding___________________________________________________________________________
int Detector::EncodeDetector(const std::string& name) {
  return _encoding[name];
}
//----------------------------------------------------------------------------------------------

//__Detector Decoding___________________________________________________________________________
const std::string Detector::DecodeDetector(int id) {
  return _decoding[id];
}
//----------------------------------------------------------------------------------------------

//__Build Detector______________________________________________________________________________
G4VPhysicalVolume* Detector::Construct(G4LogicalVolume* world) {
  Scintillator::Material::Define();
  RPC::Material::Define();
  _scintillators.clear();
  _rpcs.clear();
  _uchannels.clear();

  constexpr double total_outer_box_height = 6796.2 * mm;
  auto DetectorVolume = Construction::BoxVolume("Prototype",
                                                3500 * mm,
                                                3500 * mm,
                                                total_outer_box_height);

  for (const auto& scintillator_info : Scintillator::InfoArray) {
    auto scintillator = new Scintillator(scintillator_info.name,
                                         scintillator_info.trapezoid_height,
                                         scintillator_info.short_base,
                                         scintillator_info.long_base);
    scintillator->pvolume = Construction::PlaceVolume(scintillator->lvolume, DetectorVolume,
      G4Transform3D(
        G4RotationMatrix(G4ThreeVector(0.0, 0.0, 1.0),
                         scintillator_info.z_rotation_angle) * G4RotationMatrix(G4ThreeVector(1.0, 0.0, 0.0), 90 * deg),
        G4ThreeVector(scintillator_info.x, scintillator_info.y, scintillator_info.z)
      )
    );
    _scintillators.push_back(scintillator);
  }

  for (std::size_t rpc_index{}; rpc_index < RPC::Count; rpc_index++) {
    auto rpc = new RPC(rpc_index);
    const auto& info = RPC::InfoArray[rpc_index];
    rpc->PlaceIn(DetectorVolume, Construction::Transform(
      info.x, info.y, info.z, 0.0, 0.0, 1.0, info.z_rotation_angle));
    _rpcs.push_back(rpc);
  }

  for (const auto &uchannel_info : UChannel::InfoArray) {
    auto uchannel = new UChannel(uchannel_info.name, uchannel_info.length);
    Construction::PlaceVolume(uchannel->getLogicalVolume(), DetectorVolume, Construction::Transform(uchannel_info.x, uchannel_info.y, uchannel_info.z, 0.0, 0.0, 1.0, uchannel_info.z_rotation_angle));
    _uchannels.push_back(uchannel);
  }

  for (std::size_t rpc_sublayer_index = 0; rpc_sublayer_index < RPC::Count / 2; ++rpc_sublayer_index) {
    const auto &rpc_1_info = RPC::InfoArray[2 * rpc_sublayer_index];
    const auto &rpc_2_info = RPC::InfoArray[2 * rpc_sublayer_index + 1];
    const auto average_rpc_x = (rpc_1_info.x + rpc_2_info.x) / 2.0;
    const auto average_rpc_y = (rpc_1_info.y + rpc_2_info.y) / 2.0;
    const auto max_rpc_z = std::max(rpc_1_info.z, rpc_2_info.z);
    const auto u_channel_z = max_rpc_z + RPC::Depth / 2.0 + RPC::OuterCasingCornerDepth + UChannel::Height / 2.0;
    const auto average_rpc_angle = (rpc_1_info.z_rotation_angle + rpc_2_info.z_rotation_angle) / 2.0;
    const auto u_channel_z_rotation_angle = average_rpc_angle + 4.0 * std::atan(1.0) / 2.0;
    const auto u_channel_1_x = average_rpc_x + RPC::Height / 4.0 * -std::sin(average_rpc_angle);
    const auto u_channel_1_y = average_rpc_y + RPC::Height / 4.0 *  std::cos(average_rpc_angle);
    auto uchannel_1 = new UChannel("RPC_UChannel_" + std::to_string(rpc_sublayer_index + 1) + "_1", UChannel::RPCUChannelLength);
    Construction::PlaceVolume(uchannel_1->getLogicalVolume(), DetectorVolume, Construction::Transform(u_channel_1_x, u_channel_1_y, u_channel_z, 0.0, 0.0, 1.0, u_channel_z_rotation_angle));
    _uchannels.push_back(uchannel_1);
    auto uchannel_2 = new UChannel("RPC_UChannel_" + std::to_string(rpc_sublayer_index + 1) + "_2", UChannel::RPCUChannelLength);
    Construction::PlaceVolume(uchannel_2->getLogicalVolume(), DetectorVolume, Construction::Transform(-u_channel_1_x, -u_channel_1_y, u_channel_z, 0.0, 0.0, 1.0, u_channel_z_rotation_angle));
    _uchannels.push_back(uchannel_2);
  }

  return Construction::PlaceVolume(DetectorVolume,
                                   world,
                                   G4Translate3D(-2.386 * m,
                                   0.0 * m,
                                   Earth::TotalShift() + Earth::BufferZoneHigherDepth() + Earth::BufferZoneLowerDepth() - 0.5 * total_outer_box_height));
}
//----------------------------------------------------------------------------------------------

//__Build Earth for Detector____________________________________________________________________
G4VPhysicalVolume* Detector::ConstructEarth(G4LogicalVolume* world) {
  return Cavern::Construct(world);
}
//----------------------------------------------------------------------------------------------

} /* namespace Prototype */ ////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
