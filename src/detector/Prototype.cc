#include "detector/Prototype.hh"

#include "Geant4/G4Material.hh"
#include "Geant4/G4NistManager.hh"
#include "Geant4/G4Box.hh"
#include "Geant4/G4HCofThisEvent.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4ios.hh"

namespace MATHUSLA { namespace MU {

auto Prototype::_envelopes = EnvelopeList();
auto Prototype::_rpcs = RPCList();

Prototype::Prototype()
    : G4VSensitiveDetector("MATHUSLA/MU/Prototype"), _hit_collection(nullptr) {
  collectionName.insert("Prototype_HC");
  for (auto& envelope : _envelopes) {
    for (auto& trap : envelope.GetScintillatorList())
      trap.GetSensitiveVolume()->GetLogicalVolume()->SetSensitiveDetector(this);
  }
  for (auto& rpc : _rpcs) {
    for (auto& pad : rpc.GetPads()) {
      for (auto& strip : pad.strips)
        strip->GetLogicalVolume()->SetSensitiveDetector(this);
    }
  }
}

void Prototype::Initialize(G4HCofThisEvent* eventHitsCollection) {
  _hit_collection = new PrototypeHC(this->GetName(), collectionName[0]);
  eventHitsCollection->AddHitsCollection(
    G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]),
    _hit_collection);
}

G4bool Prototype::ProcessHits(G4Step* step, G4TouchableHistory*) {
  auto deposit = step->GetTotalEnergyDeposit();

  if (deposit == 0) return false;

  auto track = step->GetTrack();
  auto post_step = step->GetPostStepPoint();
  auto nonionizing_depoist = step->GetNonIonizingEnergyDeposit();

  if (track->GetParentID() == 0
      && step->GetPreStepPoint()->GetStepStatus() == fGeomBoundary) {
    _hit_collection->insert(
      new PrototypeHit(
        track->GetParticleDefinition()->GetParticleName(),
        track->GetTrackID(),
        track->GetTouchable()->GetHistory()->GetVolume(4)->GetName(),
        deposit - nonionizing_depoist,
        nonionizing_depoist,
        post_step->GetGlobalTime(),
        post_step->GetPosition(),
        post_step->GetTotalEnergy(),
        post_step->GetMomentum()));
    return true;
  }
  return false;
}

void Prototype::EndOfEvent(G4HCofThisEvent*) {
  if (verboseLevel < 2) return;

  G4int eventID = 0;
  auto event = G4RunManager::GetRunManager()->GetCurrentEvent();
  if (event) eventID = event->GetEventID();
  G4int hitCount = _hit_collection->entries();

  if (!hitCount) return;

  auto boxside = std::string(25 + std::to_string(eventID).length()
                                + std::to_string(hitCount).length(), '-');

  G4cout << "\n\n" << boxside << '\n'
         << "| Event: " << eventID << " | Hit Count: " << hitCount << " |\n"
         << boxside << '\n';

  G4int trackID = -1;
  G4String chamberID = "";
  for (G4int i = 0; i < hitCount; ++i) {
    auto hit = static_cast<PrototypeHit*>(_hit_collection->GetHit(i));
    auto newChamberID = hit->GetChamberID();
    auto newTrackID = hit->GetTrackID();
    if (i != 0 && (chamberID != newChamberID || trackID != newTrackID)) {
      const auto barlength = 162
        + hit->GetParticleName().length()
        + std::to_string(newTrackID).length()
        + newChamberID.length();
      G4cout << std::string(barlength, '-') << '\n';
    }
    chamberID = newChamberID;
    trackID = newTrackID;
    hit->Print();
  }
  G4cout << '\n';
}

G4Material* Prototype::Material::Aluminum     = 0;
G4Material* Prototype::Material::Carbon       = 0;
G4Material* Prototype::Material::Scintillator = 0;

void Prototype::Material::Define() {
  Material::Aluminum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
  Material::Carbon = G4NistManager::Instance()->FindOrBuildMaterial("G4_C");

  Material::Scintillator = new G4Material("Scintillator", 1.032*g/cm3, 2);
  Material::Scintillator->AddElement(Construction::Material::C, 9);
  Material::Scintillator->AddElement(Construction::Material::H, 10);

  constexpr G4int nSci = 1;
  G4double eSci[nSci] = { 3.10*eV };
  G4double rSci[nSci] = { 1.58    };

  auto sciProp = new G4MaterialPropertiesTable();
  sciProp->AddProperty("RINDEX", eSci, rSci, nSci);
  Material::Scintillator->SetMaterialPropertiesTable(sciProp);
}

G4VPhysicalVolume* Prototype::Construct(G4LogicalVolume* world) {
  auto Detector = Construction::Volume(
    new G4Box("Prototype", 300.0*cm, 300.0*cm, 300.0*cm));

  constexpr auto TopFirst    = Envelope::LayerType::TopFirst;
  constexpr auto BottomFirst = Envelope::LayerType::BottomFirst;
  constexpr auto Left        = Envelope::Alignment::Left;
  constexpr auto Center      = Envelope::Alignment::Center;
  constexpr auto Right       = Envelope::Alignment::Right;

  constexpr G4double depth     = 2.0*cm;
  constexpr G4double thickness = 0.1*cm;
  constexpr G4double spacing   = 0.1*cm;

  auto A10 = Scintillator("A10",     34.2735559430568*cm, 32.52*cm, 35.78*cm,
    depth, thickness, spacing);
  auto A11 = Scintillator("A11",     39.2697011242604*cm, 35.78*cm, 39.52*cm,
    depth, thickness, spacing);
  auto A12 = Scintillator("A12",                55.68*cm, 39.52*cm, 43.90*cm,
    depth, thickness, spacing);

  auto B6 = Scintillator("B6",       46.9637647033140*cm, 26.31*cm, 30.23*cm,
    depth, thickness, spacing);
  auto B7 = Scintillator("B7",       55.5571344149842*cm, 30.23*cm, 34.79*cm,
    depth, thickness, spacing);
  auto B8 = Scintillator("B8",       66.8484225245044*cm, 34.79*cm, 40.37*cm,
    depth, thickness, spacing);
  auto B9 = Scintillator("B9",       37.8707804735234*cm, 40.37*cm, 43.51*cm,
    depth, thickness, spacing);
  auto B10 = Scintillator("B10",     42.3673111366067*cm, 43.51*cm, 47.11*cm,
    depth, thickness, spacing);
  auto B11 = Scintillator("B11",     55.8569031258564*cm, 47.11*cm, 51.86*cm,
    depth, thickness, spacing);
  auto B12 = Scintillator("B12",     64.8499644520229*cm, 51.86*cm, 57.31*cm,
    depth, thickness, spacing);
  auto B12_1 = Scintillator("B12-1", 64.8499644520229*cm, 43.51*cm, 47.11*cm,  // tentative numbers, definition not found
    depth, thickness, spacing);

  auto C3 = Scintillator("C3",       36.9714743409067*cm, 21.97*cm, 24.94*cm,
    depth, thickness, spacing);
  auto C4 = Scintillator("C4",       42.6670798474789*cm, 24.94*cm, 28.29*cm,
    depth, thickness, spacing);
  auto C5 = Scintillator("C5",       49.5617601975399*cm, 28.29*cm, 32.41*cm,
    depth, thickness, spacing);
  auto C6 = Scintillator("C6",       57.8553611983379*cm, 32.41*cm, 37.05*cm,
    depth, thickness, spacing);
  auto C7 = Scintillator("C7",       68.9468035006099*cm, 37.05*cm, 42.59*cm,
    depth, thickness, spacing);
  auto C8 = Scintillator("C8",       81.9367809717393*cm, 42.59*cm, 49.68*cm,
    depth, thickness, spacing);
  auto C9 = Scintillator("C9",       46.8638417996899*cm, 49.68*cm, 53.55*cm,
    depth, thickness, spacing);
  auto C10 = Scintillator("C10",     52.2596785953898*cm, 53.55*cm, 57.80*cm,
    depth, thickness, spacing);
  auto C11 = Scintillator("C11",     69.2465722114821*cm, 57.80*cm, 63.53*cm,
    depth, thickness, spacing);

  constexpr G4double trap_spacing = 5*cm;  // what is the real value?

  auto A1_L = Envelope("A1-L", trap_spacing, BottomFirst, Center, {C3, C4, C5, C6, C7});
  auto A2_H = Envelope("A2-H", trap_spacing, BottomFirst, Center, {C3, C4, C5, C6, C7});
  auto A3_L = Envelope("A3-L", trap_spacing, BottomFirst, Center, {B7, B8, B9, B10, B11});
  auto A4_H = Envelope("A4-H", trap_spacing, TopFirst,    Center, {B8, B9, B10, B11, B12});
  auto A5_L = Envelope("A5-L", trap_spacing, TopFirst,    Center, {C8, C9, C10, C11});
  auto A6_H = Envelope("A6-H", trap_spacing, TopFirst,    Center, {C8, C9, C10, C11});

  auto B1_L = Envelope("B1-L", trap_spacing, BottomFirst, Right,  {C3, C4, B6, B7, C7});
  auto B2_H = Envelope("B2-H", trap_spacing, BottomFirst, Center, {C3, C4, C5, C6, C7});
  auto B3_L = Envelope("B3-L", trap_spacing, BottomFirst, Right,  {B7, B8, B9, C9, C9});
  auto B4_H = Envelope("B4-H", trap_spacing, TopFirst,    Left,   {A10, A11, A11, B9, B12_1, B11});
  auto B5_L = Envelope("B5-L", trap_spacing, TopFirst,    Left,   {A10, A12, C7, C8, C9});
  auto B6_H = Envelope("B6-H", trap_spacing, TopFirst,    Left,   {B11, B11, C9, B11, B11});

  _envelopes = EnvelopeList({
    A1_L, A2_H, A3_L, A4_H, A5_L, A6_H, B1_L, B2_H, B3_L, B4_H, B5_L, B6_H});

  constexpr G4double env_spacing = 5*cm + 2*depth;  // what is the correct distance between envelopes?
  constexpr G4double layer_spacing = 500*cm;        // what is the correct distance between layers?

  constexpr G4double height_A = 255*cm,
                      width_A = 254*cm,
                      depth_A =  40*cm;
  auto layerA = Construction::Volume(
    new G4Box("A-Layer", 0.5 * width_A, 0.5 * depth_A, 0.5 * height_A),
    Construction::Material::Air);

  G4double shift_A = 0;
  bool flip_A = 0, stack_A = 0;
  for (auto aenv : {A6_H, A5_L, A4_H, A3_L, A2_H, A1_L}) {
    auto env_width = flip_A ? aenv.GetBottomWidth() : aenv.GetTopWidth();
    shift_A += 0.5 * env_width;
    Construction::PlaceVolume(
      aenv.GetVolume(),
      layerA,
      Construction::Transform(
        0.5 * width_A - shift_A,
        (stack_A ? 1 : -1) * env_spacing,
        0.5 * (aenv.GetHeight() - height_A),
        0, 1, 0, flip_A ? 0*deg : 180*deg)
    );
    shift_A += 0.5 * env_width;
    flip_A = !flip_A;
    stack_A = !stack_A;
  }

  Construction::PlaceVolume(layerA, Detector,
    Construction::Transform(0, 0, -0.5 * layer_spacing, 1, 0, 0, 90*deg));


  constexpr G4double rpc_angle = 15*deg;  // what is the real value?
  constexpr G4double rpc_small_spacing = 10*cm;  // what is the real value?
  constexpr G4double rpc_large_spacing = 140*cm;  // what is the real value?
  constexpr G4double rpc_top_gap = 30*cm;  // what is the real value?

  for (short layer_index = 0; layer_index < 6; ++layer_index) {
    auto layer = Construction::Volume(
      new G4Box("Layer" + std::to_string(1 + layer_index),
        RPC::Width, 0.5 * RPC::Height, 0.5 * RPC::Depth));
    for (short rpc_index = 0; rpc_index < 2; ++rpc_index) {
      auto rpc = RPC(rpc_index + layer_index + 1);
      Construction::PlaceVolume(rpc.GetVolume(), layer,
        G4Translate3D(
          (rpc_index % 2 ? -0.5 : 0.5) * RPC::Width, 0, 0
        ));
      _rpcs.push_back(rpc);
    }
    auto layer_shift = -rpc_top_gap;
    switch (layer_index % 3) {
        case 0: layer_shift = rpc_large_spacing; break;
        case 1: layer_shift = 0; break;
        case 2: layer_shift = -rpc_large_spacing; break;
    }
    Construction::PlaceVolume(layer, Detector,
      Construction::Transform(
        0, 0,
        (layer_index % 2 ? -1 : 1) * rpc_small_spacing + layer_shift,
        0, 0, -1, rpc_angle + (layer_index % 2 ? 90*deg : 0*deg)
      ));
  }


  constexpr G4double height_B = 254*cm,
                      width_B = 236*cm,
                      depth_B =  40*cm;
  auto layerB = Construction::Volume(
    new G4Box("B-Layer", 0.5 * height_B, 0.5 * depth_B, 0.5 * width_B),
    Construction::Material::Air);

  G4double shift_B = 0;
  bool flip_B = 1, stack_B = 1;
  for (auto benv : {B1_L, B2_H, B3_L}) {
    auto env_width = flip_B ? benv.GetBottomWidth() : benv.GetTopWidth();
    shift_B += 0.5 * env_width;
    Construction::PlaceVolume(
      benv.GetVolume(),
      layerB,
      Construction::Transform(
        0.5 * (benv.GetHeight() - height_B),
        (stack_B ? 1 : -1) * env_spacing,
        0.5 * width_B - shift_B,
        0, 1, 0, flip_B ? 90*deg : -90*deg)
    );
    shift_B += 0.5 * env_width;
    flip_B = !flip_B;
    stack_B = !stack_B;
  }

  shift_B += 0.5 * (B3_L.GetTopWidth() - B3_L.GetBottomWidth());

  for (auto benv : {B4_H, B5_L, B6_H}) {
    auto env_width = !flip_B ? benv.GetBottomWidth() : benv.GetTopWidth();
    shift_B += 0.5 * env_width;
    Construction::PlaceVolume(
      benv.GetVolume(),
      layerB,
      Construction::Transform(
        0.5 * (2 * B4_H.GetHeight() - benv.GetHeight() - height_B),
        (stack_B ? 1 : -1) * env_spacing,
        0.5 * width_B - shift_B,
        0, 1, 0, flip_B ? 90*deg : -90*deg)
    );
    shift_B += 0.5 * env_width;
    flip_B = !flip_B;
    stack_B = !stack_B;
  }

  Construction::PlaceVolume(layerB, Detector,
    Construction::Transform(0, 0, 0.5 * layer_spacing, 1, 0, 0, 90*deg));

  return Construction::PlaceVolume(Detector, world, G4Translate3D(50*m, 50*m, -3*m));
}

} } /* namespace MATHUSLA::MU */
