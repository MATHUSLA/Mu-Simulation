/* src/geometry/Earth.cc
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

#include "geometry/Earth.hh"

#include "geometry/Construction.hh"

namespace MATHUSLA { namespace MU {

namespace Earth { //////////////////////////////////////////////////////////////////////////////

//__Earth Materials_____________________________________________________________________________
G4Material* Material::CaCO3     = nullptr;
G4Material* Material::Kaolinite = nullptr;
G4Material* Material::SiO2      = nullptr;
G4Material* Material::Marl      = nullptr;
G4Material* Material::Mix       = nullptr;
//----------------------------------------------------------------------------------------------

//__Define Earth Materials______________________________________________________________________
void Material::Define() {
  auto Al = new G4Element("Aluminum", "Al", 13.,  26.981*g/mole);
  auto Si = new G4Element("Silicon",  "Si", 14., 28.0855*g/mole);
  auto Ca = new G4Element("Calcium",  "Ca", 20.,  40.078*g/mole);

  Material::CaCO3 = new G4Material("CaCO3", 2.71*g/cm3, 3);
  Material::CaCO3->AddElement(Construction::Material::C,  1);
  Material::CaCO3->AddElement(Ca, 1);
  Material::CaCO3->AddElement(Construction::Material::O,  3);

  Material::Kaolinite = new G4Material("Clay", 2.65*g/cm3, 4);
  Material::Kaolinite->AddElement(Al, 2);
  Material::Kaolinite->AddElement(Si, 2);
  Material::Kaolinite->AddElement(Construction::Material::O,  9);
  Material::Kaolinite->AddElement(Construction::Material::H,  4);

  Material::SiO2 = new G4Material("Quartz", 2.445*g/cm3, 2);
  Material::SiO2->AddElement(Si, 46.743*perCent);
  Material::SiO2->AddElement(Construction::Material::O, 53.257*perCent);

  Material::Marl = new G4Material("Marl", 2.46*g/cm3, 2);
  Material::Marl->AddMaterial(Material::Kaolinite, 35*perCent);
  Material::Marl->AddMaterial(Material::CaCO3,     65*perCent);

  Material::Mix = new G4Material("Mix", 2.54*g/cm3, 2);
  Material::Mix->AddMaterial(Material::Marl, 50*perCent);
  Material::Mix->AddMaterial(Material::SiO2, 50*perCent);
}
//----------------------------------------------------------------------------------------------

//__Earth Logical Volumes_______________________________________________________________________
G4LogicalVolume* Volume() {
  return Construction::BoxVolume("Earth",
    LayerWidthX, LayerWidthY, TotalDepth);
}
G4LogicalVolume* SandstoneVolume() {
  return Construction::BoxVolume("Sandstone",
    LayerWidthX, LayerWidthY, SandstoneDepth, Material::SiO2);
}
G4LogicalVolume* MarlVolume() {
  return Construction::BoxVolume("Marl",
    LayerWidthX, LayerWidthY, MarlDepth, Material::Marl);
}
G4LogicalVolume* MixVolume() {
  return Construction::BoxVolume("Mix",
    LayerWidthX, LayerWidthY, MixDepth, Material::Mix);
}
//----------------------------------------------------------------------------------------------

//__Earth Transformations_______________________________________________________________________
const G4Translate3D Transform() {
  return G4Translate3D(0, 0, 0.5 * TotalDepth);
}
const G4Translate3D SandstoneTransform() {
  return G4Translate3D(0, 0, 0.5 * (SandstoneDepth - TotalDepth));
}
const G4Translate3D MarlTransform() {
  return G4Translate3D(0, 0, SandstoneDepth + 0.5 * (MarlDepth - TotalDepth));
}
const G4Translate3D MixTransform() {
  return G4Translate3D(0, 0, SandstoneDepth + MarlDepth + 0.5 * (MixDepth - TotalDepth));
}
//----------------------------------------------------------------------------------------------

//__Construct Earth_____________________________________________________________________________
G4VPhysicalVolume* Construct(G4LogicalVolume* world) {
  Material::Define();
  auto earth = Volume();
  Construction::PlaceVolume(SandstoneVolume(), earth, SandstoneTransform());
  Construction::PlaceVolume(MarlVolume(), earth, MarlTransform());
  Construction::PlaceVolume(MixVolume(), earth, MixTransform());
  return Construction::PlaceVolume(earth, world, Transform());
}
//----------------------------------------------------------------------------------------------

} /* namespace Earth */ ////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
