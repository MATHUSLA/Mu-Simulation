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
    : _volume(nullptr), _pads(), _id(id), _name("RPC" + std::to_string(id)) {
  _volume = Construction::BoxVolume(_name,
    Width, Height, Depth, Material::Casing);

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
             + '_' + std::to_string(1 + pad_index)
             + '_' + std::to_string(strip_index),
        StripWidth, StripHeight, StripDepth,
        Material::Strip,
        Construction::SensitiveAttributes());
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

G4bool RPC::ProcessHits(G4Step*) {
  return false;
}

void RPC::Register(G4VSensitiveDetector* detector) {
  for (const auto& pad : _pads) {
    for (const auto& strip : pad.strips) {
      strip->GetLogicalVolume()->SetSensitiveDetector(detector);
    }
  }
}

} } /* namespace MATHUSLA::MU */
