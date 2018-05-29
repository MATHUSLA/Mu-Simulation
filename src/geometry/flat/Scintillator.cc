/* src/geometry/flat/Scintillator.cc
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

#include "geometry/Flat.hh"

#include "Geant4/G4SubtractionSolid.hh"
#include "Geant4/G4UnionSolid.hh"

namespace MATHUSLA { namespace MU {

namespace Flat { ///////////////////////////////////////////////////////////////////////////////

//__Scintillator Constructor____________________________________________________________________
Scintillator::Scintillator(const std::string& name,
                           const double length,
                           const double height,
                           const double base_width,
                           const double overlap)
    : name(name), length(length), height(height), base_width(base_width), overlap(overlap),
      lvolume(nullptr), pvolume(nullptr), sensitive(nullptr) {

  const auto&& double_thickness = 2 * CasingThickness;
  const auto&& inner_length = length - double_thickness;
  const auto&& inner_height = height - double_thickness;
  const auto&& inner_base_width = base_width - double_thickness;
  const auto&& inner_overlap = overlap - double_thickness;

  const auto&& quarter_height = 0.25 * height;

  auto point = Construction::Box("", 0.01, 0.01, 0.01);

  auto outer_slab = new G4SubtractionSolid("",
    Construction::Box("", base_width, 0.5 * height, length),
    Construction::Box("", inner_base_width, 0.5 * inner_height - CasingThickness, inner_length));

  auto outer = new G4UnionSolid(name + "_C",
    new G4UnionSolid("",
      point,
      outer_slab,
      Construction::Transform(0.5 * (overlap - base_width), -quarter_height, 0)),
    outer_slab,
    Construction::Transform(0.5 * (base_width - overlap), quarter_height, 0));

  auto inner_sliver = Construction::Box("", inner_overlap, double_thickness, inner_length);
  auto inner_slab = Construction::Box("", inner_base_width, 0.5 * inner_height - CasingThickness, inner_length);

  auto inner = new G4UnionSolid(name,
    new G4UnionSolid("",
      inner_sliver,
      inner_slab,
      Construction::Transform(0.5 * (inner_overlap - inner_base_width), -quarter_height, 0)),
    inner_slab,
    Construction::Transform(0.5 * (inner_base_width - inner_overlap), quarter_height, 0));

  auto casing = outer;  // FIXME: there is an alternative way of doing this

  auto casingLV = Construction::Volume(casing,
    Material::Casing,
    Construction::CasingAttributes());

  auto sensitiveLV = Construction::Volume(inner,
    Material::Scintillator,
    Construction::SensitiveAttributes());

  lvolume = Construction::Volume(
    new G4UnionSolid(name, outer, inner),
    Construction::BorderAttributes());

  Construction::PlaceVolume(casingLV, lvolume);
  sensitive = Construction::PlaceVolume(sensitiveLV, lvolume);
}
//----------------------------------------------------------------------------------------------

//__Scintillator Material_______________________________________________________________________
G4Material* Scintillator::Material::Casing       = nullptr;
G4Material* Scintillator::Material::Scintillator = nullptr;
//----------------------------------------------------------------------------------------------

//__Define Scintillator Material________________________________________________________________
void Scintillator::Material::Define() {
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

//__Scintillator Clone__________________________________________________________________________
Scintillator* Scintillator::Clone(const Scintillator* other,
                                  const std::string& new_name) {
  return new Scintillator(new_name, other->length, other->height, other->base_width, other->overlap);
}
//----------------------------------------------------------------------------------------------

} /* namespace Flat */ /////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
