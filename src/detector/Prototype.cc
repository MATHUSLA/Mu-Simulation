#include "detector/Prototype.hh"

#include "Geant4/G4Material.hh"
#include "Geant4/G4HCofThisEvent.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4ios.hh"

#include "analysis/Analysis.hh"
#include "action/RunAction.hh"

namespace MATHUSLA { namespace MU {

auto Prototype::_envelopes = EnvelopeList();
auto Prototype::_rpcs = RPCList();

Prototype::Prototype()
    : G4VSensitiveDetector("MATHUSLA/MU/Prototype"), _hit_collection(nullptr) {
  collectionName.insert("Prototype_HC");

  Analysis::CreateH1("Prototype", 100, 0, 0.5*MeV);

  for (auto envelope : _envelopes) {
    for (auto sci : envelope->GetScintillatorList()) {
      sci->Register(this);
      Analysis::CreateH1(sci->GetFullName(),
      60,
      Scintillator::MinDeposit,
      Scintillator::MaxDeposit);
    }
  }

  for (auto rpc : _rpcs) {
    rpc->Register(this);
    Analysis::CreateH1(rpc->GetName(),
    60,
    RPC::MinDeposit,
    RPC::MaxDeposit);
  }
}

void Prototype::Material::Define() {
  Scintillator::Material::Define();
  RPC::Material::Define();
}

void Prototype::Initialize(G4HCofThisEvent* eventHC) {
  _hit_collection = new PrototypeHC(GetName(), collectionName[0]);
  eventHC->AddHitsCollection(
    G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]),
    _hit_collection);
}

G4bool Prototype::ProcessHits(G4Step* step, G4TouchableHistory*) {
  auto deposit = step->GetTotalEnergyDeposit();

  auto min_deposit = Scintillator::MinDeposit < RPC::MinDeposit ?
                     Scintillator::MinDeposit : RPC::MinDeposit;

  if (deposit < min_deposit)
    return false;

  auto track = step->GetTrack();
  auto volume = track->GetTouchable()->GetHistory()->GetTopVolume();
  auto name = volume->GetName();

  G4String histname;

  if (name[0] == 'A' || name[0] == 'B') {
    if (deposit > Scintillator::MaxDeposit)
      return false;
    histname = name;
  } else {
    if (deposit > RPC::MaxDeposit)
      return false;
    histname = G4String() + "RPC" + name[0];
    if (name[2] == '_')
      histname += name[1];
  }

  auto post_step = step->GetPostStepPoint();
  auto nonionizing_deposit = step->GetNonIonizingEnergyDeposit();

  _hit_collection->insert(
    new PrototypeHit(
      track->GetParticleDefinition()->GetParticleName(),
      track->GetTrackID(),
      track->GetParentID(),
      volume->GetName(),
      deposit - nonionizing_deposit,
      nonionizing_deposit,
      G4LorentzVector(post_step->GetGlobalTime(), post_step->GetPosition()),
      G4LorentzVector(post_step->GetTotalEnergy(), post_step->GetMomentum())
    ));

  Analysis::FillH1(histname, deposit);

  return true;
}

void Prototype::EndOfEvent(G4HCofThisEvent*) {
  G4int eventID = 0;
  auto event = G4RunManager::GetRunManager()->GetCurrentEvent();
  if (event)
    eventID = event->GetEventID();
  auto hitCount = _hit_collection->entries();

  if (!hitCount) return;

  const auto boxside = std::string(25 + std::to_string(eventID).length()
                                      + std::to_string(hitCount).length(), '-');

  const auto box = boxside
                 + "\n| Event: "    + std::to_string(eventID)
                 + " | Hit Count: " + std::to_string(hitCount)
                 + " |\n" + boxside + '\n';

  if (verboseLevel > 2) G4cout << "\n\n" << box;
  RunAction::GetStream() << box;

  G4int trackID = -1;
  G4String chamberID = "";
  for (G4int i = 0; i < hitCount; ++i) {
    auto hit = dynamic_cast<PrototypeHit*>(_hit_collection->GetHit(i));

    auto new_chamberID = hit->GetChamberID();
    auto new_trackID   = hit->GetTrackID();

    G4bool hit_type     =     chamberID[0] == 'A' ||     chamberID[0] == 'B';
    G4bool new_hit_type = new_chamberID[0] == 'A' || new_chamberID[0] == 'B';

    if (i != 0 && (hit_type != new_hit_type || trackID != new_trackID)) {
      const auto barlength = 162
        + hit->GetParticleName().length()
        + std::to_string(new_trackID).length()
        + std::to_string(hit->GetParentID()).length()
        + new_chamberID.length();
      const auto bar = std::string(barlength, '-') + '\n';

      if (verboseLevel > 2) G4cout << bar;
      RunAction::GetStream() << bar;
    }

    chamberID = new_chamberID;
    trackID = new_trackID;

    if (verboseLevel > 2) hit->Print();
    hit->Print(RunAction::GetStream());
  }
  if (verboseLevel > 2) G4cout << '\n';
  RunAction::GetStream() << '\n';
}

G4VPhysicalVolume* Prototype::Construct(G4LogicalVolume* world) {
  constexpr G4double total_height = 6649*mm;

  constexpr G4double total_outer_box_height = total_height + 170*mm;
  auto Detector = Construction::BoxVolume(
    "Prototype", 2800*mm, 2800*mm, total_outer_box_height);

  constexpr auto TopFirst    = Envelope::LayerType::TopFirst;
  constexpr auto BottomFirst = Envelope::LayerType::BottomFirst;
  constexpr auto Left        = Envelope::Alignment::Left;
  constexpr auto Center      = Envelope::Alignment::Center;
  constexpr auto Right       = Envelope::Alignment::Right;

  auto A10 = new Scintillator("A10", 34.2735559430568*cm, 32.52*cm, 35.78*cm);
  auto A11 = new Scintillator("A11", 39.2697011242604*cm, 35.78*cm, 39.52*cm);
  auto A12 = new Scintillator("A12",            55.68*cm, 39.52*cm, 43.90*cm);
  auto B6  = new Scintillator("B6",  46.9637647033140*cm, 26.31*cm, 30.23*cm);
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

  constexpr G4double sci_spacing = 5*cm;

  auto A1_L = new Envelope("A1_L", sci_spacing, BottomFirst, Center, {C3, C4, C5, C6, C7});
  auto A2_H = new Envelope("A2_H", sci_spacing, BottomFirst, Center, {C3, C4, C5, C6, C7});
  auto A3_L = new Envelope("A3_L", sci_spacing, BottomFirst, Center, {B7, B8, B9, B10, B11});
  auto A4_H = new Envelope("A4_H", sci_spacing, TopFirst,    Center, {B8, B9, B10, B11, B12});
  auto A5_L = new Envelope("A5_L", sci_spacing, TopFirst,    Center, {C8, C9, C10, C11});
  auto A6_H = new Envelope("A6_H", sci_spacing, TopFirst,    Center, {C8, C9, C10, C11});

  auto B1_L = new Envelope("B1_L", sci_spacing, BottomFirst, Right,  {C3, C4, B6, B7, C7});
  auto B2_H = new Envelope("B2_H", sci_spacing, BottomFirst, Center, {C3, C4, C5, C6, C7});
  auto B3_L = new Envelope("B3_L", sci_spacing, BottomFirst, Right,  {B7, B8, B9, C9, C9});
  auto B4_H = new Envelope("B4_H", sci_spacing, TopFirst,    Left,   {A10, A11, A11, B9, B12_1, B11});
  auto B5_L = new Envelope("B5_L", sci_spacing, TopFirst,    Left,   {A10, A12, C7, C8, C9});
  auto B6_H = new Envelope("B6_H", sci_spacing, TopFirst,    Left,   {B11, B11, C9, B11, B11});

  _envelopes = EnvelopeList({
    A1_L, A2_H, A3_L, A4_H, A5_L, A6_H, B1_L, B2_H, B3_L, B4_H, B5_L, B6_H});

  constexpr G4double envelope_spacing = 24*cm;

  constexpr G4double outer_layer_spacing = total_height - 50*cm;

  constexpr G4double height_A = 255*cm,
                      width_A = 254*cm,
                      depth_A =  40*cm;
  auto layerA = Construction::BoxVolume("A-Layer", width_A, depth_A, height_A);

  G4double shift_A = 0;
  bool flip_A = 0, stack_A = 0;
  for (auto aenv : {A6_H, A5_L, A4_H, A3_L, A2_H, A1_L}) {
    auto env_width = flip_A ? aenv->GetBottomWidth() : aenv->GetTopWidth();
    shift_A += 0.5 * env_width;
    Construction::PlaceVolume(
      aenv->GetVolume(),
      layerA,
      Construction::Transform(
        0.5 * width_A - shift_A,
        (stack_A ? 0.5 : -0.5) * envelope_spacing,
        0.5 * (aenv->GetHeight() - height_A),
        0, 1, 0, flip_A ? 0*deg : 180*deg));
    shift_A += 0.5 * env_width;
    flip_A = !flip_A;
    stack_A = !stack_A;
  }

  Construction::PlaceVolume(layerA, Detector,
    Construction::Transform(0, 0, -0.5 * outer_layer_spacing, 1, 0, 0, 90*deg));


  constexpr G4double rpc_top_gap       =  900*mm;
  constexpr G4double rpc_small_spacing =  344*mm;
  constexpr G4double rpc_large_spacing = 1738*mm;

  for (short layer_i = 0; layer_i < 6; ++layer_i) {
    auto layer_mod2 = layer_i % 2;
    auto layer = Construction::BoxVolume(
      "RPCLayer" + std::to_string(1 + layer_i),
      2 * RPC::Width, RPC::Height, RPC::Depth);
    for (short rpc_i = 1; rpc_i <= 2; ++rpc_i) {
      auto rpc = new RPC(2 * layer_i + rpc_i);
      Construction::PlaceVolume(rpc->GetVolume(), layer,
        G4Translate3D(
          (layer_mod2 ? -1 : 1) * (rpc_i % 2 ? 0.5 : -0.5) * RPC::Width, 0, 0));
      _rpcs.push_back(rpc);
    }
    Construction::PlaceVolume(layer, Detector,
      Construction::Transform(
        0, 0,
        rpc_top_gap - 0.5 * outer_layer_spacing
          + (layer_mod2 ? 0.5 : -0.5) * rpc_small_spacing
          + (layer_i / 2) * rpc_large_spacing,
        0, 0, -1, RPC::Angle + (layer_mod2 ? 90*deg : 0*deg)));
  }


  constexpr G4double height_B = 254*cm,
                      width_B = 236*cm,
                      depth_B =  40*cm;
  auto layerB = Construction::BoxVolume("B-Layer", height_B, depth_B, width_B);

  G4double shift_B = 0;
  bool flip_B = 1, stack_B = 1;
  for (auto benv : {B1_L, B2_H, B3_L}) {
    auto env_width = flip_B ? benv->GetBottomWidth() : benv->GetTopWidth();
    shift_B += 0.5 * env_width;
    Construction::PlaceVolume(
      benv->GetVolume(),
      layerB,
      Construction::Transform(
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
    auto env_width = !flip_B ? benv->GetBottomWidth() : benv->GetTopWidth();
    shift_B += 0.5 * env_width;
    Construction::PlaceVolume(
      benv->GetVolume(),
      layerB,
      Construction::Transform(
        0.5 * (2 * B4_H->GetHeight() - benv->GetHeight() - height_B),
        (stack_B ? 0.5 : -0.5) * envelope_spacing,
        0.5 * width_B - shift_B,
        0, 1, 0, flip_B ? 90*deg : -90*deg));
    shift_B += 0.5 * env_width;
    flip_B = !flip_B;
    stack_B = !stack_B;
  }

  Construction::PlaceVolume(layerB, Detector,
    Construction::Transform(0, 0, 0.5 * outer_layer_spacing, 1, 0, 0, 90*deg));

  return Construction::PlaceVolume(Detector, world,
    G4Translate3D(0, 0, -0.5*total_outer_box_height));
}

} } /* namespace MATHUSLA::MU */
