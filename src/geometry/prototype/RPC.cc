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

#include <Geant4/tls.hh>

namespace MATHUSLA { namespace MU {

namespace Prototype { //////////////////////////////////////////////////////////////////////////

const RPC::RPCInfo RPC::rpc_infos[n_rpcs] = {
  { 616.85*mm,   87.79*mm, -2486.38*mm,  0.14719*rad},
  {-618.50*mm, -108.63*mm, -2383.38*mm,  0.14753*rad},
  { -85.73*mm,  616.81*mm, -2049.38*mm, -1.42123*rad},
  {  98.09*mm, -615.71*mm, -2148.38*mm, -1.42069*rad},
  { 616.12*mm,  -94.51*mm,  -651.13*mm,  2.98398*rad},
  {-614.31*mm,  107.15*mm,  -751.13*mm,  2.98463*rad},
  {  94.86*mm,  620.81*mm,  -408.13*mm,  1.41717*rad},
  { -93.55*mm, -614.15*mm,  -311.13*mm,  1.41976*rad},
  { 615.11*mm, -118.18*mm,   983.12*mm,  2.94831*rad},
  {-610.33*mm,  122.71*mm,  1081.12*mm,  2.95236*rad},
  { 120.28*mm,  616.35*mm,  1423.12*mm,  1.37958*rad},
  {-147.19*mm, -601.52*mm,  1323.12*mm,  1.38013*rad}
};

//__RPC Pad Constructor_________________________________________________________________________
RPC::Pad::Pad(int input_id) : id(input_id) {}
//----------------------------------------------------------------------------------------------

//__RPC Constructor_____________________________________________________________________________
RPC::RPC(int input_id) : _pads(), _id(input_id), _name("RPC" + std::to_string(1 + input_id)) {
  _volume = Construction::OpenBoxVolume(
    _name,
    Width, Height, Depth, Thickness,
    Material::Casing);

  const auto id_name = (_id < 9 ? "0" : "") + std::to_string(1 + _id);

  for (unsigned pad_index = 0; pad_index < n_pads_per_rpc; ++pad_index) {
    auto pad = new Pad(pad_index);

    pad->lvolume = Construction::OpenBoxVolume(
      "PAD" + std::to_string(1 + pad_index),
      PadWidth, PadHeight, PadDepth, PadThickness,
      Material::Pad, Construction::CasingAttributes());

    const auto pad_name = id_name
                        + (pad_index < 9 ? "0" : "")
                        + std::to_string(1 + pad_index);

    for (unsigned strip_index = 0; strip_index < n_strips_per_pad; ++strip_index) {
      auto strip = Construction::BoxVolume(
        pad_name + std::to_string(1 + strip_index),
        StripWidth, StripHeight, StripDepth,
        Material::Gas,
        Construction::SensitiveAttributes());
      pad->lvolume_strips.push_back(strip);
      pad->pvolume_strips.push_back(Construction::PlaceVolume(strip, pad->lvolume,
        G4Translate3D(0, (strip_index - (n_strips_per_pad - 1) / 2.0) * StripSpacingY, 0)));
    }

    constexpr auto pi = 4.0 * atan(1);
    pad->pvolume = Construction::PlaceVolume(pad->lvolume, _volume,
      Construction::Transform(
        (pad_index % n_pads_per_row - (n_pads_per_row - 1) / 2.0) * PadSpacingX,
        (pad_index / n_pads_per_row - (n_pads_per_column - 1) / 2.0) * PadSpacingY,
        0,
        0, 0, 1, pad_index % 2 ? pi : 0));

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
