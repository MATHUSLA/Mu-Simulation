#include "detector/Prototype.hh"

#include "Geant4/G4SDManager.hh"
#include "Geant4/G4RunManager.hh"

namespace MATHUSLA { namespace MU {

RPC::Pad::Pad(int id) : volume(nullptr), strips(), id(id) {}

G4Material* RPC::Material::Casing = nullptr;
G4Material* RPC::Material::Pad    = nullptr;
G4Material* RPC::Material::Strip  = nullptr;

void RPC::Material::Define() {
  // give real definitions
  Material::Casing = Construction::Material::Air;
  Material::Pad    = Construction::Material::Air;
  Material::Strip  = Construction::Material::Air;
}

RPC::RPC(int id)
    : G4VSensitiveDetector("MATHUSLA/MU/Prototype/RPC" + std::to_string(id)),
      _hit_collection(nullptr), _volume(nullptr), _pads(), _id(id) {
  collectionName.insert("Prototype_RPC" + std::to_string(id) + "_HC");
  _volume = Construction::BoxVolume("RPC" + std::to_string(id),
    Width, Height, Depth,
    Material::Casing);

  for (int pad_index = 0; pad_index < 10; ++pad_index) {
    Pad pad(1 + pad_index);

    auto padLV = Construction::BoxVolume("PAD" + std::to_string(1 + pad_index),
      PadWidth, PadHeight, PadDepth,
      Material::Pad,
      Construction::CasingAttributes());

    auto strip_stack = 0.5 * StripHeight;
    for (int strip_index = 1; strip_index <= 8; ++strip_index) {
      auto strip = Construction::BoxVolume(
        G4String() + std::to_string(id)
             + ':' + std::to_string(1 + pad_index)
             + ':' + std::to_string(strip_index),
        StripWidth, StripHeight, StripDepth,
        Material::Strip,
        Construction::SensitiveAttributes());
      // strip->SetSensitiveDetector(this);
      pad.strips.push_back(Construction::PlaceVolume(strip, padLV,
        G4Translate3D(0, 0.5 * PadHeight - StripTopGap - strip_stack, 0)));
      strip_stack += StripHeight + StripYGap;
    }

    pad.volume = Construction::PlaceVolume(padLV, _volume,
      G4Translate3D(
        0.5 * Width  - PadStartX - PadSpacingX * (pad_index % 2),
        0.5 * Height - PadStartY - PadSpacingY * (pad_index / 2 % 5),
        0));

    _pads.push_back(pad);
  }
}

void RPC::Initialize(G4HCofThisEvent* eventHC) {
  _hit_collection = new PrototypeHC(GetName(), collectionName[0]);
  eventHC->AddHitsCollection(
    G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]),
    _hit_collection);
}

G4bool RPC::ProcessHits(G4Step* step, G4TouchableHistory*) {
  auto deposit = step->GetTotalEnergyDeposit();

  if (deposit == 0) return false;

  auto track = step->GetTrack();
  auto post_step = step->GetPostStepPoint();
  auto nonionizing_deposit = step->GetNonIonizingEnergyDeposit();

  auto volume = track->GetTouchable()->GetHistory()->GetTopVolume();

  _hit_collection->insert(
    new PrototypeHit(
      track->GetParticleDefinition()->GetParticleName(),
      track->GetTrackID(),
      volume->GetName(),
      deposit - nonionizing_deposit,
      nonionizing_deposit,
      G4LorentzVector(post_step->GetGlobalTime(), post_step->GetPosition()),
      G4LorentzVector(post_step->GetTotalEnergy(), post_step->GetMomentum())
    ));
  return true;
}

void RPC::EndOfEvent(G4HCofThisEvent*) {
  if (verboseLevel < 2) return;

  G4int eventID = 0;
  auto event = G4RunManager::GetRunManager()->GetCurrentEvent();
  if (event) eventID = event->GetEventID();
  auto hitCount = _hit_collection->entries();

  if (!hitCount) return;

  auto boxside = std::string(25 + std::to_string(eventID).length()
                                + std::to_string(hitCount).length(), '-');

  G4cout << "\n\n" << boxside << '\n'
         << "| Event: " << eventID << " | Hit Count: " << hitCount << " |\n"
         << boxside << '\n';

  G4int trackID = -1;
  G4String chamberID = "";
  for (G4int i = 0; i < hitCount; ++i) {
    auto hit = dynamic_cast<PrototypeHit*>(_hit_collection->GetHit(i));

    auto new_chamberID = hit->GetChamberID();
    auto new_trackID = hit->GetTrackID();
    G4bool hit_type = chamberID[0] == 'A' || chamberID[0] == 'B';
    G4bool new_hit_type = new_chamberID[0] == 'A' || new_chamberID[0] == 'B';

    if (i != 0 && (hit_type != new_hit_type || trackID != new_trackID)) {
      const auto barlength = 162
        + hit->GetParticleName().length()
        + std::to_string(new_trackID).length()
        + new_chamberID.length();
      G4cout << std::string(barlength, '-') << '\n';
    }

    chamberID = new_chamberID;
    trackID = new_trackID;
    hit->Print();
  }
  G4cout << '\n';
}

} } /* namespace MATHUSLA::MU */