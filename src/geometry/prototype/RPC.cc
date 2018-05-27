/* src/geometry/prototype/RPC.cc
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

namespace MATHUSLA { namespace MU {

namespace Prototype { //////////////////////////////////////////////////////////////////////////

//__RPC Pad Constructor_________________________________________________________________________
RPC::Pad::Pad(int id) : id(id) {}
//----------------------------------------------------------------------------------------------

//__RPC Constructor_____________________________________________________________________________
RPC::RPC(int id) : _pads(), _id(id), _name("RPC" + std::to_string(id)) {
  _volume = Construction::OpenBoxVolume(
    _name,
    Width, Height, Depth, Thickness,
    Material::Casing);

  const auto id_name = std::to_string(id);

  for (int pad_index = 0; pad_index < 10; ++pad_index) {
    auto pad = new Pad(1 + pad_index);

    pad->lvolume = Construction::OpenBoxVolume(
      "PAD" + std::to_string(1 + pad_index),
      PadWidth, PadHeight, PadDepth, PadThickness,
      Material::Pad, Construction::CasingAttributes());

    const auto pad_name = id_name
                        + (pad_index < 9 ? std::string("0") : "")
                        + std::to_string(1 + pad_index);

    auto strip_stack = 0.5 * StripHeight;
    for (int strip_index = 1; strip_index <= 8; ++strip_index) {
      auto strip = Construction::BoxVolume(
        pad_name + std::to_string(strip_index),
        StripWidth, StripHeight, StripDepth,
        Material::Gas,
        Construction::SensitiveAttributes());
      pad->lvolume_strips.push_back(strip);
      pad->pvolume_strips.push_back(Construction::PlaceVolume(strip, pad->lvolume,
        G4Translate3D(0, 0.5 * PadHeight - StripTopGap - strip_stack, 0)));
      strip_stack += StripHeight + StripYGap;
    }

    pad->pvolume = Construction::PlaceVolume(pad->lvolume, _volume,
      G4Translate3D(
        0.5 * Width  - PadStartX - PadSpacingX * (pad_index % 2),
        0.5 * Height - PadStartY - PadSpacingY * (pad_index / 2 % 5),
        0));

    _pads.push_back(pad);
  }
}
//----------------------------------------------------------------------------------------------

//__RPC Material________________________________________________________________________________
G4Material* RPC::Material::Casing = nullptr;
G4Material* RPC::Material::Pad    = nullptr;
G4Material* RPC::Material::Gas    = nullptr;
//----------------------------------------------------------------------------------------------

//__Define RPC Material_________________________________________________________________________
void RPC::Material::Define() {
  Material::Casing = Construction::Material::Aluminum;
  Material::Pad    = Construction::Material::Air;

  auto C2H2F4 = new G4Material("C2H2F4", 4.1684*g/L, 3);
  C2H2F4->AddElement(Construction::Material::C, 2);
  C2H2F4->AddElement(Construction::Material::H, 2);
  C2H2F4->AddElement(Construction::Material::F, 4);

  /* NOTE: In case it becomes necessary:

  auto iso_C4H10 = new G4Material("iso-C4H10", 0*g/L, 2);
  iso_C4H10->AddElement(Construction::Material::C, 4);
  iso_C4H10->AddElement(Construction::Material::H, 10);

  auto SF6 = new G4Material("SF6", 0*g/L, 2);
  SF6->AddElement(Construction::Material::S, 1);
  SF6->AddElement(Construction::Material::F, 6);
  */

  auto argon = new G4Material("argon", 1.635*g/L, 1);
  argon->AddElement(Construction::Material::Ar, 1);

  Material::Gas = new G4Material("gas", 3.773*g/L, 2, G4State::kStateGas);
  Material::Gas->AddMaterial(C2H2F4, 0.93);
  Material::Gas->AddMaterial(argon,  0.07);
}
//----------------------------------------------------------------------------------------------

//__Register RPC with Detector__________________________________________________________________
void RPC::Register(G4VSensitiveDetector* detector) {
  for (const auto& pad : _pads)
    for (const auto& volume : pad->lvolume_strips)
      volume->SetSensitiveDetector(detector);
}
//----------------------------------------------------------------------------------------------

//__Place RPC in RPC Layer______________________________________________________________________
G4VPhysicalVolume* RPC::PlaceIn(G4LogicalVolume* parent,
                                const G4Transform3D& transform) {
  return (_placement = Construction::PlaceVolume(_volume, parent, transform));
}
//----------------------------------------------------------------------------------------------

} /* namespace Prototype */ ////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
