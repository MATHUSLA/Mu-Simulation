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

//__RPC Info____________________________________________________________________________________
const RPC::Info RPC::InfoArray[RPC::Count] = {
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
//----------------------------------------------------------------------------------------------

//__RPC Pad Constructor_________________________________________________________________________
RPC::Pad::Pad(int input_id) : id(input_id) {}
//----------------------------------------------------------------------------------------------

//__RPC Constructor_____________________________________________________________________________
RPC::RPC(int input_id) : _pads(), _id(input_id), _name("RPC" + std::to_string(1 + input_id)) {
  _volume = Construction::BoxVolume(_name, Width, Height, Depth, Construction::Material::Air, Construction::BorderAttributes());

  const auto id_name = (_id < 9 ? "0" : "") + std::to_string(1 + _id);

  auto aluminum_sheet = Construction::BoxVolume(_name + "_Aluminum", Width, Height, AluminumDepth, Construction::Material::Aluminum, Construction::CasingAttributes());
  auto bakelite_sheet = Construction::BoxVolume(_name + "_Bakelite", Width, Height, BakeliteDepth, Construction::Material::Bakelite);
  auto copper_strips = Construction::BoxVolume(_name + "_Copper", Width, Height, CopperDepth, Construction::Material::Copper);
  auto thick_foam_layer = Construction::BoxVolume(_name + "_ThickFoam", Width, Height, ThickFoamDepth, Construction::Material::PolystyreneFoam);
  auto thin_foam_layer = Construction::BoxVolume(_name + "_ThinFoam", Width, Height, ThinFoamDepth, Construction::Material::PolystyreneFoam);
  auto thick_pet_film = Construction::BoxVolume(_name + "_ThickPET", Width, Height, ThickPETDepth, Material::PET);
  auto medium_pet_film = Construction::BoxVolume(_name + "_MediumPET", Width, Height, MediumPETDepth, Material::PET);
  auto thin_pet_film = Construction::BoxVolume(_name + "_ThinPET", Width, Height, ThinPETDepth, Material::PET);

  auto z_shift = -(StripDepth + BakeliteDepth) / 2.0;
  Construction::PlaceVolume(bakelite_sheet, _volume, G4Translate3D(0.0, 0.0, z_shift));
  z_shift -= (BakeliteDepth + CopperDepth) / 2.0;
  Construction::PlaceVolume(copper_strips, _volume, G4Translate3D(0.0, 0.0, z_shift));
  z_shift -= (CopperDepth + MediumPETDepth) / 2.0;
  Construction::PlaceVolume(medium_pet_film, _volume, G4Translate3D(0.0, 0.0, z_shift));
  z_shift -= (MediumPETDepth + ThinFoamDepth) / 2.0;
  Construction::PlaceVolume(thin_foam_layer, _volume, G4Translate3D(0.0, 0.0, z_shift));
  z_shift -= (ThinFoamDepth + ThinPETDepth) / 2.0;
  Construction::PlaceVolume(thin_pet_film, _volume, G4Translate3D(0.0, 0.0, z_shift));
  z_shift -= (ThinPETDepth + CopperDepth) / 2.0;
  Construction::PlaceVolume(copper_strips, _volume, G4Translate3D(0.0, 0.0, z_shift));
  z_shift -= (CopperDepth + AluminumDepth) / 2.0;
  Construction::PlaceVolume(aluminum_sheet, _volume, G4Translate3D(0.0, 0.0, z_shift));
  z_shift -= (AluminumDepth + ThickFoamDepth) / 2.0;
  Construction::PlaceVolume(thick_foam_layer, _volume, G4Translate3D(0.0, 0.0, z_shift));
  z_shift -= (ThickFoamDepth + AluminumDepth) / 2.0;
  Construction::PlaceVolume(aluminum_sheet, _volume, G4Translate3D(0.0, 0.0, z_shift));

  z_shift = (StripDepth + BakeliteDepth) / 2.0;
  Construction::PlaceVolume(bakelite_sheet, _volume, G4Translate3D(0.0, 0.0, z_shift));
  z_shift += (BakeliteDepth + ThickPETDepth) / 2.0;
  Construction::PlaceVolume(thick_pet_film, _volume, G4Translate3D(0.0, 0.0, z_shift));
  z_shift += (ThickPETDepth + ThinFoamDepth) / 2.0;
  Construction::PlaceVolume(thin_foam_layer, _volume, G4Translate3D(0.0, 0.0, z_shift));
  z_shift += (ThinFoamDepth + CopperDepth) / 2.0;
  Construction::PlaceVolume(copper_strips, _volume, G4Translate3D(0.0, 0.0, z_shift));
  z_shift += (CopperDepth + MediumPETDepth) / 2.0;
  Construction::PlaceVolume(medium_pet_film, _volume, G4Translate3D(0.0, 0.0, z_shift));
  z_shift += (MediumPETDepth + AluminumDepth) / 2.0;
  Construction::PlaceVolume(aluminum_sheet, _volume, G4Translate3D(0.0, 0.0, z_shift));
  z_shift += (AluminumDepth + ThickFoamDepth) / 2.0;
  Construction::PlaceVolume(thick_foam_layer, _volume, G4Translate3D(0.0, 0.0, z_shift));
  z_shift += (ThickFoamDepth + AluminumDepth) / 2.0;
  Construction::PlaceVolume(aluminum_sheet, _volume, G4Translate3D(0.0, 0.0, z_shift));

  for (std::size_t pad_index{}; pad_index < PadsPerRPC; ++pad_index) {
    auto pad = new Pad(pad_index);

    pad->lvolume = Construction::BoxVolume("PAD" + std::to_string(1 + pad_index),
                                           PadWidth, PadHeight, PadDepth);

    const auto pad_name = id_name + (pad_index < 9 ? "0" : "") + std::to_string(1 + pad_index);

    for (std::size_t strip_index{}; strip_index < StripsPerPad; ++strip_index) {
      auto strip = Construction::BoxVolume(
        pad_name + std::to_string(1 + strip_index),
        StripWidth, StripHeight, StripDepth,
        Material::Gas,
        Construction::SensitiveAttributes());
      pad->lvolume_strips.push_back(strip);
      pad->pvolume_strips.push_back(Construction::PlaceVolume(strip, pad->lvolume,
        G4Translate3D(0.0, (strip_index - (StripsPerPad - 1) / 2.0) * StripSpacingY, 0.0)));
    }

    pad->pvolume = Construction::PlaceVolume(pad->lvolume, _volume,
      Construction::Transform(
        (pad_index % PadsPerRow - (PadsPerRow - 1) / 2.0) * PadSpacingX,
        (pad_index / PadsPerRow - (PadsPerColumn - 1) / 2.0) * PadSpacingY,
        0.0,
        0.0, 0.0, 1.0, pad_index % 2 ? 3.14159265358979323846264 : 0.0));

    _pads.push_back(pad);
  }
}
//----------------------------------------------------------------------------------------------

//__RPC Material________________________________________________________________________________
G4Material* RPC::Material::Gas = nullptr;
G4Material* RPC::Material::PET = nullptr;
//----------------------------------------------------------------------------------------------

//__Define RPC Material_________________________________________________________________________
void RPC::Material::Define() {
  auto C2H2F4 = new G4Material("C2H2F4", 4.1684*g/L, 3, G4State::kStateGas, 298.15*kelvin);
  C2H2F4->AddElement(Construction::Material::C, 2);
  C2H2F4->AddElement(Construction::Material::H, 2);
  C2H2F4->AddElement(Construction::Material::F, 4);

  auto isobutane = new G4Material("Isobutane", 2.4403*g/L, 2, G4State::kStateGas, 298.15*kelvin);
  isobutane->AddElement(Construction::Material::C, 4);
  isobutane->AddElement(Construction::Material::H, 10);

  auto SF6 = new G4Material("SF6", 6.0380*g/L, 2, G4State::kStateGas, 298.15*kelvin);
  SF6->AddElement(Construction::Material::S, 1);
  SF6->AddElement(Construction::Material::F, 6);

  auto argon = new G4Material("Argon", 1.6339*g/L, 1, G4State::kStateGas, 298.15*kelvin);
  argon->AddElement(Construction::Material::Ar, 1);

  const auto atlas_gas_fraction = 0.85;

  const auto c2h2f4_partial_density    = atlas_gas_fraction * 0.952 * C2H2F4->GetDensity();
  const auto isobutane_partial_density = atlas_gas_fraction * 0.045 * isobutane->GetDensity();
  const auto sf6_partial_density       = atlas_gas_fraction * 0.003 * SF6->GetDensity();
  const auto argon_partial_density     = (1.0 - atlas_gas_fraction) * argon->GetDensity();

  const auto gas_density = c2h2f4_partial_density + isobutane_partial_density + sf6_partial_density + argon_partial_density;

  Material::Gas = new G4Material("Gas", gas_density, 4, G4State::kStateGas, 298.15*kelvin);
  Material::Gas->AddMaterial(C2H2F4,    c2h2f4_partial_density    / gas_density);
  Material::Gas->AddMaterial(isobutane, isobutane_partial_density / gas_density);
  Material::Gas->AddMaterial(SF6,       sf6_partial_density       / gas_density);
  Material::Gas->AddMaterial(argon,     argon_partial_density     / gas_density);

  Material::PET = new G4Material("PET", 1.397*g/cm3, 3, G4State::kStateSolid);
  Material::PET->AddElement(Construction::Material::C, 10);
  Material::PET->AddElement(Construction::Material::H, 8);
  Material::PET->AddElement(Construction::Material::O, 4);
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
