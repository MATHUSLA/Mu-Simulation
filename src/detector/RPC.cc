#include "detector/Prototype.hh"

#include "Geant4/G4Box.hh"

namespace MATHUSLA { namespace MU {

RPC::Pad::Pad(G4int id) : id(id), volume(nullptr), strips() {}

G4Material* RPC::Material::Casing = nullptr;
G4Material* RPC::Material::Pad    = nullptr;
G4Material* RPC::Material::Strip  = nullptr;

void RPC::Material::Define() {
  // give real definitions
  RPC::Material::Casing = Construction::Material::Air;
  RPC::Material::Pad    = Construction::Material::Air;
  RPC::Material::Strip  = Construction::Material::Air;
}

RPC::RPC(G4int id) : _id(id), _pads(), _volume(nullptr) {
  if (!RPC::Material::Casing || !RPC::Material::Pad || !RPC::Material::Strip)
    RPC::Material::Define();

  _volume = Construction::Volume(
    new G4Box("RPC" + std::to_string(id),
    0.5 * RPC::Width, 0.5 * RPC::Height, 0.5 * RPC::Depth),
    RPC::Material::Casing);

  for (int pad_index = 0; pad_index < 10; ++pad_index) {
    RPC::Pad pad{1 + pad_index};

    auto padLV = Construction::Volume(
      new G4Box("PAD" + std::to_string(1 + pad_index),
        0.5 * RPC::PadWidth, 0.5 * RPC::PadHeight, 0.5 * RPC::PadDepth),
      RPC::Material::Pad,
      Construction::CasingAttributes());

    auto strip_stack = 0.5 * RPC::StripHeight;
    for (int strip = 1; strip <= 8; ++strip) {
      pad.strips.push_back(Construction::PlaceVolume(
        new G4Box(
          G4String() + std::to_string(id) + "/"
                     + std::to_string(1 + pad_index) + "/"
                     + std::to_string(strip),
          0.5 * RPC::StripWidth, 0.5 * RPC::StripHeight, 0.5 * RPC::StripDepth),
        RPC::Material::Strip,
        Construction::SensitiveAttributes(),
        padLV,
        G4Translate3D(
          0,
          0.5 * RPC::PadHeight - RPC::StripTopGap - strip_stack,
          0
        )));
      strip_stack += RPC::StripHeight + RPC::StripYGap;
    }

    pad.volume = Construction::PlaceVolume(padLV, _volume,
      G4Translate3D(
        0.5 * RPC::Width  - RPC::PadStartX
                          - RPC::PadSpacingX * (pad_index % 2),
        0.5 * RPC::Height - RPC::PadStartY
                          - RPC::PadSpacingY * (pad_index / 2 % 5),
        0));

    _pads.push_back(pad);
  }
}

} } /* namespace MATHUSLA::MU */