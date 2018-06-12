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

#include <Geant4/G4HCofThisEvent.hh>
#include <Geant4/G4Step.hh>

#include "action.hh"
#include "analysis.hh"
#include "geometry/Earth.hh"
#include "physics/Units.hh"
#include "tracking.hh"

namespace MATHUSLA { namespace MU {

namespace Prototype { //////////////////////////////////////////////////////////////////////////

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Envelopes and RPCs for Prototype____________________________________________________________
std::vector<Envelope*> _envelopes;
std::vector<RPC*> _rpcs;
//----------------------------------------------------------------------------------------------

//__Prototype Hit Collection____________________________________________________________________
G4ThreadLocal Tracking::HitCollection* _hit_collection;
//----------------------------------------------------------------------------------------------

//__Encoding/Decoding Maps______________________________________________________________________
G4ThreadLocal std::unordered_map<std::string, Scintillator*> _sci_map;
G4ThreadLocal std::unordered_map<std::string, int>           _encoding;
G4ThreadLocal std::unordered_map<int, std::string>           _decoding;
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Prototype Data Variables____________________________________________________________________
const std::string& Detector::DataPrefix = "event";
const std::vector<std::string>& Detector::DataKeys = {
  "Deposit", "Time", "Detector",
  "PDG", "Track", "X", "Y", "Z", "E", "PX", "PY", "PZ", "D_PMT"};
//----------------------------------------------------------------------------------------------

//__Prototype Constructor_______________________________________________________________________
/*! \brief Creates Detector and Assigns sensitive volumes
*/
Detector::Detector() : G4VSensitiveDetector("MATHUSLA/MU/Prototype") {
  collectionName.insert("Prototype_HC");
  for (auto envelope : _envelopes) {
    for (auto sci : envelope->GetScintillatorList()) {
      sci->Register(this);
      const auto name = sci->GetFullName();
      const auto id = _decoding.size();
      _sci_map.insert({name, sci});
      _encoding.insert({name, id});
      _decoding.insert({id, name});
    }
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
/*! \brief Initialzes Prototype Hit Collection
*/
void Detector::Initialize(G4HCofThisEvent* event) {
  _hit_collection = Tracking::GenerateHitCollection(this, event);
}
//----------------------------------------------------------------------------------------------

//__Hit Processing______________________________________________________________________________
/*! \brief Collects Data from Hit and Stores in ROOT Ntuple and Geant4 Hit Collection
*/
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
  const auto global_time = post_step->GetGlobalTime();
  const auto position    = post_step->GetPosition();
  const auto energy      = post_step->GetTotalEnergy();
  const auto momentum    = post_step->GetMomentum();

  _hit_collection->insert(
    new Tracking::Hit(
      particle->GetParticleName(),
      trackID,
      track->GetParentID(),
      name,
      deposit,
      G4LorentzVector(global_time, position),
      G4LorentzVector(energy, momentum)));

  const auto detector_id = static_cast<double>(EncodeDetector(name));

  Scintillator::PMTPoint pmt_point = {0, 0, 0};
  if (detector_id < 100) {
    const auto sci = _sci_map[name];

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

    const auto& translation = r4 * (r3 * (r2 * (t1) + t2) + t3) + t4;
    const auto& rotation = (r4*r3*r2).inverse();

    if (sci)
      pmt_point = Scintillator::PMTDistance(position, sci, translation, rotation);
  }

  Analysis::FillNTuple(DataPrefix, EventAction::EventID(), {
    deposit      / Units::Energy,
    global_time  / Units::Time,
    detector_id,
    static_cast<double>(particle->GetPDGEncoding()),
    static_cast<double>(trackID),
    position.x() / Units::Length,
    position.y() / Units::Length,
    position.z() / Units::Length,
    energy       / Units::Energy,
    momentum.x() / Units::Momentum,
    momentum.y() / Units::Momentum,
    momentum.z() / Units::Momentum,
    pmt_point.r  / Units::Length});

  return true;
}
//----------------------------------------------------------------------------------------------

//__Post-Event Processing_______________________________________________________________________
/*! \brief Print data from Hit Collection
*/
void Detector::EndOfEvent(G4HCofThisEvent*) {
  if (verboseLevel >= 2 && _hit_collection)
    std::cout << *_hit_collection;
}
//----------------------------------------------------------------------------------------------

//__Detector Encoding___________________________________________________________________________
/*! \brief Returns encoding generated at construction
*/
int Detector::EncodeDetector(const std::string& name) {
  return _encoding[name];
}
//----------------------------------------------------------------------------------------------

//__Detector Decoding___________________________________________________________________________
/*! \brief Returns decoding generated at construction
*/
const std::string Detector::DecodeDetector(int id) {
  return _decoding[id];
}
//----------------------------------------------------------------------------------------------

//__Build Detector______________________________________________________________________________
/*! \brief Builds Scintillator, Envelope, RPC, and Layers for the Prototype Sensitive Detector
*/
G4VPhysicalVolume* Detector::Construct(G4LogicalVolume* world) {
  Scintillator::Material::Define();
  RPC::Material::Define();
  _envelopes.clear();
  _rpcs.clear();

  constexpr double total_height = 6649*mm;

  constexpr double total_outer_box_height = total_height + 170*mm;
  auto DetectorVolume = Construction::BoxVolume(
    "Prototype", 2800*mm, 2800*mm, total_outer_box_height);

  constexpr auto Top    = Envelope::LayerType::TopFirst;
  constexpr auto Bottom = Envelope::LayerType::BottomFirst;
  constexpr auto Left   = Envelope::Alignment::Left;
  constexpr auto Center = Envelope::Alignment::Center;
  constexpr auto Right  = Envelope::Alignment::Right;
  constexpr auto Flip   = Envelope::Rotation::Flip;
  constexpr auto NoFlip = Envelope::Rotation::NoFlip;

  auto A10 = new Scintillator("A10", 34.2735559430568*cm, 32.52*cm, 35.78*cm);
  auto A11 = new Scintillator("A11", 39.2697011242604*cm, 35.78*cm, 39.52*cm);
  auto A12 = new Scintillator("A12", 55.6800000000000*cm, 39.52*cm, 43.90*cm);
  // UNUSED: auto B6  = new Scintillator("B6",  46.9637647033140*cm, 26.31*cm, 30.23*cm);
  auto B7  = new Scintillator("B7",  55.5571344149842*cm, 30.23*cm, 34.79*cm);
  auto B8  = new Scintillator("B8",  66.8484225245044*cm, 34.79*cm, 40.37*cm);
  auto B9  = new Scintillator("B9",  37.8707804735234*cm, 40.37*cm, 43.51*cm);
  auto B10 = new Scintillator("B10", 42.3673111366067*cm, 43.51*cm, 47.11*cm);
  auto B11 = new Scintillator("B11", 55.8569031258564*cm, 47.11*cm, 51.86*cm);
  auto B12 = new Scintillator("B12", 64.8499644520229*cm, 51.86*cm, 57.31*cm);

  // tentative numbers, definition not found
  auto B12_1 = new Scintillator("B12_1", 64.8499644520229*cm, 43.51*cm, 47.11*cm);

  auto C3  = new Scintillator("C3",  36.9714743409067*cm, 21.97*cm, 24.94*cm);
  auto C4  = new Scintillator("C4",  42.6670798474789*cm, 24.94*cm, 28.29*cm);
  auto C5  = new Scintillator("C5",  49.5617601975399*cm, 28.29*cm, 32.41*cm);
  auto C6  = new Scintillator("C6",  57.8553611983379*cm, 32.41*cm, 37.05*cm);
  auto C7  = new Scintillator("C7",  68.9468035006099*cm, 37.05*cm, 42.59*cm);
  auto C8  = new Scintillator("C8",  81.9367809717393*cm, 42.59*cm, 49.68*cm);
  auto C9  = new Scintillator("C9",  46.8638417996899*cm, 49.68*cm, 53.55*cm);
  auto C10 = new Scintillator("C10", 52.2596785953898*cm, 53.55*cm, 57.80*cm);
  auto C11 = new Scintillator("C11", 69.2465722114821*cm, 57.80*cm, 63.53*cm);

  auto A1_L = new Envelope("A1_L", Bottom, Center, Flip,   {C3, C4, C5, C6, C7});
  auto A2_H = new Envelope("A2_H", Bottom, Center, NoFlip, {C3, C4, C5, C6, C7});
  auto A3_L = new Envelope("A3_L", Bottom, Center, Flip,   {B7, B8, B9, B10, B11});
  auto A4_H = new Envelope("A4_H", Top, Center, NoFlip, {B8, B9, B10, B11, B12});
  auto A5_L = new Envelope("A5_L", Top, Center, Flip,   {C8, C9, C10, C11});
  auto A6_H = new Envelope("A6_H", Top, Center, NoFlip, {C8, C9, C10, C11});

  auto B1_L = new Envelope("B1_L", Bottom, Right, Flip,   {C3, C4, C5, B7, C7});
  auto B2_H = new Envelope("B2_H", Bottom, Right, NoFlip, {C3, C4, C5, C6, C7});
  auto B3_L = new Envelope("B3_L", Bottom, Right, Flip,   {B7, B8, B9, C9, B9});
  auto B4_H = new Envelope("B4_H", Top, Left, NoFlip, {A10, A11, A11, B9, B12_1, B11});
  auto B5_L = new Envelope("B5_L", Top, Left, Flip,   {A10, A12, C7, C8, C9});
  auto B6_H = new Envelope("B6_H", Top, Left, NoFlip, {B11, B11, C9, B11, B11});

  _envelopes = {A1_L, A2_H, A3_L, A4_H, A5_L, A6_H, B1_L, B2_H, B3_L, B4_H, B5_L, B6_H};

  constexpr double envelope_spacing = 24*cm;
  constexpr double outer_layer_spacing = total_height - 50*cm;


  constexpr double height_A = 255*cm,
                    width_A = 254*cm,
                    depth_A =  40*cm;
  auto layerA = Construction::BoxVolume("A-Layer", width_A, depth_A, height_A);

  double shift_A = 0;
  bool flip_A = 0, stack_A = 0;
  for (auto aenv : {A6_H, A5_L, A4_H, A3_L, A2_H, A1_L}) {
    const auto env_width = flip_A ? aenv->GetBottomWidth() : aenv->GetTopWidth();
    shift_A += 0.5 * env_width;
    aenv->PlaceIn(layerA, Construction::Transform(
      0.5 * width_A - shift_A,
      (stack_A ? 0.5 : -0.5) * envelope_spacing,
      0.5 * (aenv->GetHeight() - height_A),
      0, 1, 0, flip_A ? 0*deg : 180*deg));
    shift_A += 0.5 * env_width;
    flip_A = !flip_A;
    stack_A = !stack_A;
  }

  Construction::PlaceVolume(layerA, DetectorVolume,
    Construction::Transform(0, 0, -0.5 * outer_layer_spacing, 1, 0, 0, 90*deg));


  constexpr double rpc_top_gap          =  900*mm;
  constexpr double rpc_sublayer_spacing =   97*mm;
  constexpr double rpc_small_spacing    =  344*mm;
  constexpr double rpc_large_spacing    = 1738*mm;

  for (short layer_i = 0; layer_i < 6; ++layer_i) {
    const auto&& layer_mod2 = layer_i % 2;
    auto layer = Construction::BoxVolume(
      "RPCLayer" + std::to_string(1 + layer_i),
      2 * RPC::Width, RPC::Height, RPC::Depth);
    for (short rpc_i = 1; rpc_i <= 2; ++rpc_i) {
      auto rpc = new RPC(2 * layer_i + rpc_i);
      const auto&& rpc_shift_direction = (rpc_i % 2) ? 0.5 : -0.5;
      rpc->PlaceIn(layer, G4Translate3D(
        (layer_mod2 ? -1 : 1) * rpc_shift_direction * RPC::Width,
        0,
        rpc_shift_direction * rpc_sublayer_spacing));
      _rpcs.push_back(rpc);
    }
    Construction::PlaceVolume(layer, DetectorVolume,
      Construction::Transform(
        0, 0,
        rpc_top_gap - 0.5 * outer_layer_spacing
          + (layer_mod2 ? 0.5 : -0.5) * rpc_small_spacing
          + (layer_i / 2) * rpc_large_spacing,
        0, 0, -1, RPC::Angle + (layer_mod2 ? 90*deg : 0*deg)));
  }


  constexpr double height_B = 254*cm,
                    width_B = 236*cm,
                    depth_B =  40*cm;
  auto layerB = Construction::BoxVolume("B-Layer", height_B, depth_B, width_B);

  double shift_B = 0;
  bool flip_B = 1, stack_B = 1;
  for (auto benv : {B1_L, B2_H, B3_L}) {
    const auto env_width = flip_B ? benv->GetBottomWidth() : benv->GetTopWidth();
    shift_B += 0.5 * env_width;
    benv->PlaceIn(layerB, Construction::Transform(
      0.5 * (benv->GetHeight() - height_B),
      (stack_B ? 0.5 : -0.5) * envelope_spacing,
      0.5 * width_B - shift_B,
      0, 1, 0, flip_B ? 90*deg : -90*deg));
    shift_B += 0.5 * env_width;
    flip_B = !flip_B;
    stack_B = !stack_B;
  }

  shift_B += 0.5 * (B3_L->GetTopWidth() - B3_L->GetBottomWidth());

  for (auto benv : {B4_H, B5_L, B6_H}) {
    const auto env_width = !flip_B ? benv->GetBottomWidth() : benv->GetTopWidth();
    shift_B += 0.5 * env_width;
    benv->PlaceIn(layerB, Construction::Transform(
      0.5 * (2 * B4_H->GetHeight() - benv->GetHeight() - height_B),
      (stack_B ? 0.5 : -0.5) * envelope_spacing,
      0.5 * width_B - shift_B,
      0, 1, 0, flip_B ? 90*deg : -90*deg));
    shift_B += 0.5 * env_width;
    flip_B = !flip_B;
    stack_B = !stack_B;
  }

  Construction::PlaceVolume(layerB, DetectorVolume,
    Construction::Transform(0, 0, 0.5 * outer_layer_spacing, 1, 0, 0, 90*deg));

  return Construction::PlaceVolume(DetectorVolume, world,
    G4Translate3D(0, 0, -0.5*total_outer_box_height));
}
//----------------------------------------------------------------------------------------------

//__Build Earth for Detector____________________________________________________________________
G4VPhysicalVolume* Detector::ConstructEarth(G4LogicalVolume* world) {
  return Earth::Construct(world);
}
//----------------------------------------------------------------------------------------------

} /* namespace Prototype */ ////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
