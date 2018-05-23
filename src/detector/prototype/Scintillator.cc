/* src/detector/prototype/Scintillator.cc
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

#include "detector/Prototype.hh"

#include <cmath>

#include "Geant4/G4SubtractionSolid.hh"
#include "Geant4/G4UnionSolid.hh"
#include "Geant4/G4Tubs.hh"
#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4NistManager.hh"

#include "detector/Construction.hh"

namespace MATHUSLA { namespace MU {

namespace Prototype { //////////////////////////////////////////////////////////////////////////

//__Scintillator Constructor____________________________________________________________________
Scintillator::Scintillator(const std::string& name,
                           const double height,
                           const double minwidth,
                           const double maxwidth)
    : name(name), height(height), minwidth(minwidth), maxwidth(maxwidth),
      lvolume(nullptr), pvolume(nullptr), sensitive(nullptr) {

  auto solid = Construction::Trap(name, height, minwidth, maxwidth, Depth);

  const double dims[4] = {height   - Thickness, minwidth - Thickness,
                          maxwidth - Thickness, Depth    - Thickness};

  auto casing = new G4SubtractionSolid(name + "_C", solid,
    Construction::Trap("", dims[0], dims[1], dims[2], dims[3]));

  auto sensitive_trap = Construction::Trap(name,
    dims[0] - Spacing, dims[1] - Spacing, dims[2] - Spacing, dims[3] - Spacing);

  auto pmt = new G4Tubs(
    name + "_PMT", 0, PMTRadius, 0.5 * PMTLength, 0, 360.0*deg);

  auto pmtTransform = Construction::Transform(
    0.5 * maxwidth + 0.25 * PMTLength, 0, 0.5 * height + 0.25 * PMTLength,
    0, 1, 0, 45*deg);

  lvolume = Construction::Volume(
    new G4UnionSolid(name,
    new G4UnionSolid("", casing, sensitive_trap), pmt, pmtTransform),
    Construction::BorderAttributes());

  auto casingLV = Construction::Volume(casing,
    Material::Casing,
    Construction::CasingAttributes());

  auto sensitiveLV = Construction::Volume(sensitive_trap,
    Material::Scintillator,
    Construction::SensitiveAttributes());

  auto pmtAttr = G4VisAttributes(G4Colour(0.7, 0.7, 0.7));
  pmtAttr.SetForceSolid(true);
  auto pmtLV = Construction::Volume(pmt, Material::PMT, pmtAttr);

  Construction::PlaceVolume(casingLV, lvolume);
  sensitive = Construction::PlaceVolume(sensitiveLV, lvolume);
  Construction::PlaceVolume(pmtLV, lvolume, pmtTransform);
}
//----------------------------------------------------------------------------------------------

//__Scintillator Material_______________________________________________________________________
G4Material* Scintillator::Material::PMT          = nullptr;
G4Material* Scintillator::Material::Casing       = nullptr;
G4Material* Scintillator::Material::Scintillator = nullptr;
//----------------------------------------------------------------------------------------------

//__Define Scintillator Material________________________________________________________________
void Scintillator::Material::Define() {
  Material::PMT = G4NistManager::Instance()->FindOrBuildMaterial("G4_C");
  Material::Casing = Construction::Material::Aluminum;

  Material::Scintillator = new G4Material("Scintillator", 1.032*g/cm3, 2);
  Material::Scintillator->AddElement(Construction::Material::C, 9);
  Material::Scintillator->AddElement(Construction::Material::H, 10);

  constexpr int_fast32_t nSci = 1;
  double eSci[nSci] = { 3.10*eV };
  double rSci[nSci] = { 1.58    };

  auto sciProp = new G4MaterialPropertiesTable();
  sciProp->AddProperty("RINDEX", eSci, rSci, nSci);
  Material::Scintillator->SetMaterialPropertiesTable(sciProp);
}
//----------------------------------------------------------------------------------------------

//__Scintillator Full Name______________________________________________________________________
const std::string Scintillator::GetFullName() const {
  return sensitive ? sensitive->GetName() : name;
}
//----------------------------------------------------------------------------------------------

//__Calculate Distance to PMT___________________________________________________________________
Scintillator::PMTPoint Scintillator::PMTDistance(const G4ThreeVector position,
                                                 const Scintillator* sci,
                                                 const G4ThreeVector translation,
                                                 const G4RotationMatrix rotation) {

  const auto delta = rotation*(translation - position);

  // Trapezoid coordinates
  const auto x = -delta.x();
  const auto y = -delta.z();

  const auto up_distance = 0.5 * sci->height - y;

  return {
    up_distance,
    std::hypot(y, 0.25 * (sci->maxwidth + sci->minwidth) - x),
    std::hypot(up_distance, 0.5 * sci->maxwidth - x)
  };
}
//----------------------------------------------------------------------------------------------

//__Register Scintillator with Detector_________________________________________________________
void Scintillator::Register(G4VSensitiveDetector* detector) {
  sensitive->GetLogicalVolume()->SetSensitiveDetector(detector);
}
//----------------------------------------------------------------------------------------------

//__Clone Scintillator__________________________________________________________________________
Scintillator* Scintillator::Clone(const Scintillator* other) {
  return new Scintillator(other->name, other->height, other->minwidth, other->maxwidth);
}
//----------------------------------------------------------------------------------------------

} /* namespace Prototype */ ////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
